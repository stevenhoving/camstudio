// Copyright 2001 - 2003 RenderSoft Software & Web Publishing
//
// camstudio_cl: Screen recording from the command line.
//
// Based on the open source version of CamStudio:
//    http://camstudio.org/
//
// Source code retrieved from:
//    http://camstudio.org/dev/CamStudio.2.5.b1.src.zip
//
// License: GPL
//
// Command line author: dimator(AT)google(DOT)com
// Multi screen support: karol(dot)toth(at)gmail(dot)com

#define CAMSTUDIO_CL_VERSION "0.2"

#include <algorithm>
#include <string>
#include <vector>
#include <iostream>
#include <iterator>
#include <objbase.h>

#include <windows.h>
#include <windowsx.h>
#include <windef.h>
#include <vfw.h>
#include <atltypes.h>
#include <atlstr.h>

#include <time.h>
#include "Commandline.h"
#include <CamLib/CamImage.h>
#include <CamLib/CamFile.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////
// Function prototypes
//////////////////////////////////////

// Screen Detection Functions
BOOL CALLBACK MonitorEnumProc(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData);

// Mouse Capture functions
HCURSOR FetchCursorHandle();
HCURSOR hSavedCursor = nullptr;

// AVI functions  and #defines
#define AVIIF_KEYFRAME 0x00000010L // this frame is a key frame.
#define BUFSIZE 260
#define LPLPBI LPBITMAPINFOHEADER *
#define N_FRAMES 50
#define TEXT_HEIGHT 20

int RecordVideo(int top, int left, int width, int height, int numframes, const char *szFileName /*, screen* pscreen*/);
UINT RecordAVIThread(LPVOID pParam);

// Use these 2 functions to create frames and free frames
LPBITMAPINFOHEADER captureScreenFrame(int left, int top, int width, int height);
void FreeFrame(LPBITMAPINFOHEADER);

void PrintUsage(bool);
void PrintRecordInformation();

//////////////////
// State variables

// Vars used for selecting fixed /variableregion
RECT rcUse;

int maxxScreen;
int maxyScreen;

// Misc Vars
int gRecordState = 0;
DWORD initialtime = 0;
int initcapture = 0;


// Messaging
HWND g_hWndGlobal = nullptr;
/*
static UINT WM_USER_RECORDINTERRUPTED = ::RegisterWindowMessage(WM_USER_RECORDINTERRUPTED_MSG);
*/

/////////////////////////////////////////////////////////
// Variables/Options requiring interface
/////////////////////////////////////////////////////////
int static g_mon_current = -1;
int g_mon_count = 0; // number of physical screens, not virtual
int g_bits = 24;

// Video Options and Compressions
int g_timelapse = 40;
int g_frames_per_second = 25;
int g_key_frames_every = 25;

int g_comp_quality = 7000;
DWORD g_comp_fcc_handler = 0;
ICINFO *g_compressor_info = nullptr;
int g_num_compressor = 0;

// User options:
int g_seconds_to_record = -1;
int g_selected_compressor = -1;
std::string g_output_file;
//int offset_left = 0;
//int offset_right;
//int offset_bottom;
//int offset_top = 0;
// Ver 1.2
// Video Compress Parameters
LPVOID g_pVideoCompressParams = nullptr;
DWORD g_CompressorStateIsFor = 0;
DWORD g_CompressorStateSize = 0;

LPVOID g_pParamsUse = nullptr;
void FreeParamsUse();
BOOL MakeCompressParamsCopy(DWORD paramsSize, LPVOID pOrg);

// Report variables
int g_nActualFrame = 0;
DWORD g_nCurrFrame = 0;
float g_fRate = 0.0;
float g_fActualRate = 0.0;
float g_fTimeLength = 0.0;
int g_nColors = 24;

std::wstring g_strCodec(L"MS Video 1");
int g_actualwidth = 0;
int g_actualheight = 0;

// Cursor variables
int g_customsel = 0;
// TODO(dimator): add command line flag for g_recordcursor
int g_recordcursor = 1;
int g_cursortype = 0;
int g_highlightsize = 64;
int g_highlightshape = 0;
COLORREF g_highlightcolor = RGB(255, 255, 125);
int g_highlightclick = 0;
COLORREF g_highlightclickcolorleft = RGB(255, 0, 0);
COLORREF g_highlightclickcolorright = RGB(0, 0, 255);

// Autopan
int autopan = 0;
int maxpan = 20;
RECT panrect_current;
RECT panrect_dest;

// Path to temporary wav file
TCHAR tempaudiopath[MAX_PATH];
int recordaudio = 0;

// Audio Recording Variables
UINT AudioDeviceID = WAVE_MAPPER;

HWAVEIN m_hRecord;
WAVEFORMATEX m_Format;
DWORD m_ThreadID;
int m_QueuedBuffers = 0;
int m_BufferSize = 1000; // number of samples
// CSoundFile *m_pFile = nullptr;

// Audio Options Dialog
LPWAVEFORMATEX g_pwfx = nullptr;
DWORD cbwfx;

// Audio Formats Dialog
DWORD waveinselected = WAVE_FORMAT_2S16;
WORD audio_bits_per_sample = 16;
WORD audio_num_channels = 2;
int audio_samples_per_seconds = 22050;
BOOL bAudioCompression = TRUE;

#define MILLISECONDS 0
#define FRAMES 1
BOOL interleaveFrames = TRUE;
int interleaveFactor = 100;
int interleaveUnit = MILLISECONDS;

// state vars

// Tray Icon
#define WM_TRAY_ICON_NOTIFY_MESSAGE (WM_USER + 50)

std::string GetCodecDescription(long fccHandler);

dir_access tempPath_Access = dir_access::windows_temp_dir;
std::string specifieddir;

int captureTrans = 1;
int versionOp = 0;

// ver 2.26 Vscap Interface
#define ModeAVI 0
int RecordingMode = 0;

// TODO(dimator): remove restrictVideoCodecs, or possibly fix the codec to only
// allow camstudio lossless.
int restrictVideoCodecs = 0;

// TODO(dimator): Add command line option for timestampAnnotation
int timestampAnnotation = 0;

// TODO(dimator): Remove these two:
int presettime = 60;
int recordpreset = 0;

#ifndef CAPTUREBLT
#define CAPTUREBLT (DWORD)0x40000000
#endif

///////////////////////// //////////////////
/////////////// Functions //////////////////
///////////////////////// //////////////////
BOOL CALLBACK MonitorEnumProc(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData)
{
    (void)lprcMonitor;
    (void)hdcMonitor;
    screen *obr;
    obr = (screen *)dwData;
    g_mon_current = g_mon_current + 1;
    MONITORINFOEXA mo;
    mo.cbSize = sizeof(mo);
    if (GetMonitorInfoA(hMonitor, &mo))
    {
    }
    else
    {
         std::cout << "Error in detecting monitors" << std::endl;
        return false;
    }
    obr[g_mon_current].SetDimensions(mo.rcMonitor.left, mo.rcMonitor.right, mo.rcMonitor.top, mo.rcMonitor.bottom);

    strcpy_s(obr[g_mon_current].dispName, sizeof(obr[g_mon_current].dispName), mo.szDevice);
     std::cout << "Screen: " << obr[g_mon_current].dispName;
     std::cout << " resolution:" << obr[g_mon_current].width << " x " << obr[g_mon_current].height << std::endl;

    return true;
}
CHAR wide_to_narrow(WCHAR w)
{
    // simple typecast
    // works because UNICODE incorporates ASCII into itself
    return CHAR(w);
}

UINT RecordAVIThread(LPVOID lParam)
{
    CoInitialize(nullptr);
    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_NORMAL);
    screen *pscreen = (screen *)lParam; // pointer to current screen
                                        // const char *filepath = pscreen->outFile;

    int top = pscreen->top;
    int left = pscreen->left;
    int width = pscreen->width;
    int height = pscreen->height;
    int fps = g_frames_per_second;

    const auto *filepath = g_output_file.c_str();

    // Test the validity of writing to the file
    // Make sure the file to be created is currently not used by another application
    int fileverified = 0;
    while (!fileverified)
    {
        OFSTRUCT ofstruct;
        HFILE fhandle = OpenFile(filepath, &ofstruct, OF_SHARE_EXCLUSIVE | OF_WRITE | OF_CREATE);
        if (fhandle != HFILE_ERROR)
        {
            fileverified = 1;
            CloseHandle((HANDLE)fhandle);
            DeleteFileA(filepath);
        }
        else
        {
             std::cout << "Error: Could not open file for creation: " << filepath << std::endl;
            ExitThread(1);
        }
    }
    RecordVideo(top, left, width, height, fps, filepath);

    ExitThread(0);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// RecordVideo
//
// The main function used in the recording of video
// Includes opening/closing avi file, initializing avi settings, capturing frames, applying cursor effects etc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int RecordVideo(int top, int left, int width, int height, int fps, const char *szFileName /*screen *pscreen*/)
{
    LPBITMAPINFOHEADER alpbi;
    AVISTREAMINFO strhdr;
    PAVIFILE pfile = nullptr;
    PAVISTREAM ps = nullptr, psCompressed = nullptr;
    AVICOMPRESSOPTIONS opts;
    AVICOMPRESSOPTIONS FAR *aopts[1] = {&opts};
    HRESULT hr = 0;
    WORD wVer = 0;

    DWORD nextFrameAt = 0;
    DWORD nextFrameNumber = 0;

    long divx = 0;
    long oldsec = 0;

    DWORD timeexpended = 0;
    DWORD frametime = 0;
    DWORD oldframetime = 0;

    g_actualwidth = width;
    g_actualheight = height;

    wVer = HIWORD(VideoForWindowsVersion());
    if (wVer < 0x010a)
    {
        return false;
    }

    alpbi = nullptr;

    ////////////////////////////////////////////////
    // CAPTURE FIRST FRAME
    ////////////////////////////////////////////////
    alpbi = captureScreenFrame(left, top, width, height);

    ////////////////////////////////////////////////
    // TEST VALIDITY OF COMPRESSOR
    //////////////////////////////////////////////////

    if (g_selected_compressor > 0)
    {

        HIC hic = nullptr;
        hic = ICOpen(g_compressor_info[g_selected_compressor].fccType, g_compressor_info[g_selected_compressor].fccHandler,
                     ICMODE_QUERY);
        if (hic)
        {

            int newleft = 0;
            int newtop = 0;
            int newwidth = 0;
            int newheight = 0;
            int align = 1;
            while (ICERR_OK != ICCompressQuery(hic, alpbi, nullptr))
            {
                // Try adjusting width/height a little bit
                align = align * 2;
                if (align > 8)
                    break;
                newleft = left;
                newtop = top;
                int wm = (width % align);
                if (wm > 0)
                {
                    newwidth = width + (align - wm);
                    if (newwidth > maxxScreen)
                        newwidth = width - wm;
                }
                int hm = (height % align);
                if (hm > 0)
                {
                    newheight = height + (align - hm);
                    if (newheight > maxyScreen)
                        newwidth = height - hm;
                }
                if (alpbi)
                    FreeFrame(alpbi);
                alpbi = captureScreenFrame(newleft, newtop, newwidth, newheight);
            }

            // if succeed with new width/height, use the new width and height
            // else if still fails ==> default to MS Video 1 (MSVC)
            if (align == 1)
            {
                // Compressor has no problem with the current dimensions...so proceed
                // do nothing here
            }
            else if (align <= 8)
            {
                // Compressor can work if the dimensions is adjusted slightly
                left = newleft;
                top = newtop;
                width = newwidth;
                height = newheight;

                g_actualwidth = newwidth;
                g_actualheight = newheight;
            }
            else
            {
                g_comp_fcc_handler = mmioFOURCC('M', 'S', 'V', 'C');
                g_strCodec = L"MS Video 1";
            }

            ICClose(hic);
        }
        else
        {
            g_comp_fcc_handler = mmioFOURCC('M', 'S', 'V', 'C');
            g_strCodec = L"MS Video 1";
            // MessageBox(nullptr,"hic default","note",MB_OK);
        }

    } // selected_compressor

    // IV50
    if (g_comp_fcc_handler == mmioFOURCC('I', 'V', '5', '0'))
    {
        // Still Can't Handle Indeo 5.04
        g_comp_fcc_handler = mmioFOURCC('M', 'S', 'V', 'C');
        g_strCodec = L"MS Video 1";
    }

    ////////////////////////////////////////////////
    // INIT AVI USING FIRST FRAME
    ////////////////////////////////////////////////
    AVIFileInit();

    //
    // Open the movie file for writing....
    //
    hr = AVIFileOpenA(&pfile, szFileName, OF_WRITE | OF_CREATE, nullptr);
    if (hr != AVIERR_OK)
        goto error;

    // Fill in the header for the video stream....
    // The video stream will run in 15ths of a second....
    _fmemset(&strhdr, 0, sizeof(strhdr));
    strhdr.fccType = streamtypeVIDEO; // stream type

    // strhdr.fccHandler             = compfccHandler;
    strhdr.fccHandler = 0;

    strhdr.dwScale = 1;
    strhdr.dwRate = fps;
    strhdr.dwSuggestedBufferSize = alpbi->biSizeImage;
    SetRect(&strhdr.rcFrame, 0, 0, // rectangle for stream
            (int)alpbi->biWidth, (int)alpbi->biHeight);

    // And create the stream;
    hr = AVIFileCreateStream(pfile, &ps, &strhdr);
    if (hr != AVIERR_OK)
        goto error;

    memset(&opts, 0, sizeof(opts));
    aopts[0]->fccType = streamtypeVIDEO;
    // aopts[0]->fccHandler     = mmioFOURCC('M', 'S', 'V', 'C');
    aopts[0]->fccHandler = g_comp_fcc_handler;
    aopts[0]->dwKeyFrameEvery = g_key_frames_every;                      // keyframe rate
    aopts[0]->dwQuality = g_comp_quality;                               // compress quality 0-10,000
    aopts[0]->dwBytesPerSecond = 0;                                  // bytes per second
    aopts[0]->dwFlags = AVICOMPRESSF_VALID | AVICOMPRESSF_KEYFRAMES; // flags
    aopts[0]->lpFormat = 0x0;                                        // save format
    aopts[0]->cbFormat = 0;
    aopts[0]->dwInterleaveEvery = 0; // for non-video streams only


    // Ver 1.2
    //
    if ((g_comp_fcc_handler == g_CompressorStateIsFor) && (g_comp_fcc_handler != 0))
    {

        // make a copy of the g_pVideoCompressParams just in case after compression, this variable become messed up
        if (MakeCompressParamsCopy(g_CompressorStateSize, g_pVideoCompressParams))
        {

            aopts[0]->lpParms = g_pParamsUse;
            aopts[0]->cbParms = g_CompressorStateSize;
        }
    }

    // The 1 here indicates only 1 stream
    // if (!AVISaveOptions(nullptr, 0, 1, &ps, (LPAVICOMPRESSOPTIONS *) &aopts))
    //        goto error;

    hr = AVIMakeCompressedStream(&psCompressed, ps, &opts, nullptr);
    if (hr != AVIERR_OK)
        goto error;

    hr = AVIStreamSetFormat(psCompressed, 0,
                            alpbi,          // stream format
                            alpbi->biSize + // format size
                                alpbi->biClrUsed * sizeof(RGBQUAD));
    if (hr != AVIERR_OK)
        goto error;

    FreeFrame(alpbi);
    alpbi = nullptr;

    if (autopan)
    {
        panrect_current.left = left;
        panrect_current.top = top;
        panrect_current.right = left + width - 1;
        panrect_current.bottom = top + height - 1;
    }

    timeexpended = 0;
    frametime = 0;
    oldframetime = 0;

    initialtime = timeGetTime();
    initcapture = 1;
    oldframetime = 0;
    g_nCurrFrame = 0;
    g_nActualFrame = 0;

    //////////////////////////////////////////////
    // WRITING FRAMES
    //////////////////////////////////////////////

    divx = 0;
    oldsec = 0;

    // Time when the next frame will be taken
    nextFrameAt = timeGetTime();
    nextFrameNumber = 0;

    while (gRecordState)
    { // repeatedly loop
        // TODO(dimator): Enable this via a command line argument:
        // PrintRecordInformation();

        if (initcapture == 0)
        {
            timeexpended = timeGetTime() - initialtime;
        }
        else
        {
            frametime = 0;
            timeexpended = 0;
        }

        // Wait for next frame
        int toNextFrame = nextFrameAt - timeGetTime();
        printf("nextFrameAt: %d, timeGetTime: %d, toNextFrame: %d\n", nextFrameAt, timeGetTime(), toNextFrame);
        if (toNextFrame > 0)
        {
            printf("Sleeping for %d msec (%d - ", toNextFrame, timeGetTime());
            Sleep(toNextFrame);
            printf("%d)\n", timeGetTime());
        }
        // todo add dropped frame handling

        DWORD now = timeGetTime();
        // nextFrameAt will be incremented at least once to account for
        // sleep inaccuracy.
        do
        {
            nextFrameAt += g_timelapse;
            nextFrameNumber++;
        } while ((int)nextFrameAt - (int)now < 0);

        // Record frame
        // Autopan
        if ((autopan) && (width < maxxScreen) && (height < maxyScreen))
        {
            POINT xPoint;
            GetCursorPos(&xPoint);

            int extleft = ((panrect_current.right - panrect_current.left) * 1) / 4 + panrect_current.left;
            int extright = ((panrect_current.right - panrect_current.left) * 3) / 4 + panrect_current.left;
            int exttop = ((panrect_current.bottom - panrect_current.top) * 1) / 4 + panrect_current.top;
            int extbottom = ((panrect_current.bottom - panrect_current.top) * 3) / 4 + panrect_current.top;

            if (xPoint.x < extleft)
            { // need to pan left

                panrect_dest.left = xPoint.x - width / 2;
                panrect_dest.right = panrect_dest.left + width - 1;
                if (panrect_dest.left < 0)
                {
                    panrect_dest.left = 0;
                    panrect_dest.right = panrect_dest.left + width - 1;
                }
            }
            else if (xPoint.x > extright)
            { // need to pan right

                panrect_dest.left = xPoint.x - width / 2;
                panrect_dest.right = panrect_dest.left + width - 1;
                if (panrect_dest.right >= maxxScreen)
                {
                    panrect_dest.right = maxxScreen - 1;
                    panrect_dest.left = panrect_dest.right - width + 1;
                }
            }
            else
            {

                panrect_dest.right = panrect_current.right;
                panrect_dest.left = panrect_current.left;
            }

            if (xPoint.y < exttop)
            { // need to pan up

                panrect_dest.top = xPoint.y - height / 2;
                panrect_dest.bottom = panrect_dest.top + height - 1;
                if (panrect_dest.top < 0)
                {
                    panrect_dest.top = 0;
                    panrect_dest.bottom = panrect_dest.top + height - 1;
                }
            }
            else if (xPoint.y > extbottom)
            { // need to pan down

                panrect_dest.top = xPoint.y - height / 2;
                panrect_dest.bottom = panrect_dest.top + height - 1;
                if (panrect_dest.bottom >= maxyScreen)
                {
                    panrect_dest.bottom = maxyScreen - 1;
                    panrect_dest.top = panrect_dest.bottom - height + 1;
                }
            }
            else
            {

                panrect_dest.top = panrect_current.top;
                panrect_dest.bottom = panrect_current.bottom;
            }

            // Determine Pan Values
            int xdiff, ydiff;
            xdiff = panrect_dest.left - panrect_current.left;
            ydiff = panrect_dest.top - panrect_current.top;

            if (abs(xdiff) < maxpan)
            {
                panrect_current.left += xdiff;
            }
            else
            {
                if (xdiff < 0)
                    panrect_current.left -= maxpan;
                else
                    panrect_current.left += maxpan;
            }

            if (abs(ydiff) < maxpan)
            {
                panrect_current.top += ydiff;
            }
            else
            {

                if (ydiff < 0)
                    panrect_current.top -= maxpan;
                else
                    panrect_current.top += maxpan;
            }

            panrect_current.right = panrect_current.left + width - 1;
            panrect_current.bottom = panrect_current.top + height - 1;

            alpbi = captureScreenFrame(panrect_current.left, panrect_current.top, width, height);
        }
        else
        {
            alpbi = captureScreenFrame(left, top, width, height);
        }

        if (initcapture == 0)
        {

            if (g_timelapse > 1000)
                frametime++;
            else
                frametime = (DWORD)(((double)timeexpended / 1000.0) * (double)(1000.0 / g_timelapse));
        }
        else
        {
            initcapture = 0;
        }

        g_fTimeLength = ((float)timeexpended) / ((float)1000.0);

        if (recordpreset)
        {
            // if (int(g_fTimeLength) >= presettime)
            // ;
            // gRecordState = 0;
            // PostMessage(g_hWndGlobal,WM_USER_RECORDINTERRUPTED,0,0);

            // std::string msgStr;
            // msgStr.Format("%.2f %d",g_fTimeLength,presettime);
            // MessageBox(nullptr,msgStr,"N",MB_OK);

            // or should we post messages
        }

        if ((frametime == 0) || (frametime > oldframetime))
        {
            // if frametime repeats (frametime == oldframetime) ...the avistreamwrite will cause an error

            hr = AVIStreamWrite(psCompressed,   // stream pointer
                                frametime,      // time of this frame
                                1,              // number to write
                                (LPBYTE)alpbi + // pointer to data
                                    alpbi->biSize + alpbi->biClrUsed * sizeof(RGBQUAD),
                                alpbi->biSizeImage, // size of this frame
                                // AVIIF_KEYFRAME,      // flags....
                                0, // Dependent n previous frame, not key frame
                                nullptr, nullptr);

            if (hr != AVIERR_OK)
                break;

            g_nActualFrame++;
            g_nCurrFrame = frametime;
            g_fRate = ((float)g_nCurrFrame) / g_fTimeLength;
            g_fActualRate = ((float)g_nActualFrame) / g_fTimeLength;

            // Update recording stats every half a second
            divx = timeexpended / 500;

            if (divx != oldsec)
            {
                oldsec = divx;
                // TODO(dimator): enabling the following causes bad flickering.  Don't
                // know why it ever existed.
                // InvalidateRect(g_hWndGlobal,nullptr, FALSE);
            }

            // free memory
            FreeFrame(alpbi);
            alpbi = nullptr;
            oldframetime = frametime;
        } // if frametime is different

        frametime = nextFrameNumber;
    } // for loop

error:

    //
    // Now close the file
    //

    // Ver 1.2
    //
    if ((g_comp_fcc_handler == g_CompressorStateIsFor) && (g_comp_fcc_handler != 0))
    {

        // Detach g_pParamsUse from AVICOMPRESSOPTIONS so AVISaveOptionsFree will not free it
        //(we will free it ourselves)

        // Setting this is no harm even if g_pParamsUse is not attached to lpParams
        aopts[0]->lpParms = 0;
        aopts[0]->cbParms = 0;
    }

    AVISaveOptionsFree(1, (LPAVICOMPRESSOPTIONS FAR *)&aopts);

    if (pfile)
        AVIFileClose(pfile);

    if (ps)
        AVIStreamClose(ps);

    if (psCompressed)
        AVIStreamClose(psCompressed);

    AVIFileExit();

    if (hr != NOERROR)
    {
        // PostMessage(g_hWndGlobal,WM_USER_RECORDINTERRUPTED,0,0);
        /*
        char *ErrorBuffer; // This really is a pointer - not reserved space!
        FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |  FORMAT_MESSAGE_FROM_SYSTEM, nullptr, GetLastError(),
        MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US), (LPTSTR)&ErrorBuffer, 0, nullptr);

        std::string reasonstr(ErrorBuffer);
        std::string errorstr("File Creation Error. Unable to rename file.\n\n");
        std::string reportstr;

        reportstr = errorstr + reasonstr;
        //MessageBox(nullptr,reportstr,"Note",MB_OK | MB_ICONEXCLAMATION);
        */

        if (g_comp_fcc_handler != mmioFOURCC('M', 'S', 'V', 'C'))
        {
            // if (IDYES == MessageBox(nullptr, "Error recording AVI file using current compressor. Use default compressor
            // ? ", "Note", MB_YESNO | MB_ICONEXCLAMATION)) {
            /*
            if (IDYES == MessageOut(nullptr, IDS_STRING_ERRAVIDEFAULT, IDS_STRING_NOTE, MB_YESNO | MB_ICONQUESTION )) {

              compfccHandler = mmioFOURCC('M', 'S', 'V', 'C');
              strCodec = "MS Video 1";
            }
            */
        }
        else
        {
            // MessageBox(nullptr, "Error Creating AVI File", "Error", MB_OK | MB_ICONEXCLAMATION);
            // MessageOut(nullptr,IDS_STRING_ERRCREATEAVI ,IDS_STRING_NOTE,MB_OK | MB_ICONEXCLAMATION);
        }

        return 0;
    }

    // Save the file on success

    std::cout << "Recording finished" <<std::endl;
    return 0;
}

LPBITMAPINFOHEADER captureScreenFrame(int left, int top, int width, int height)
{
    // EnumDisplayMonitors(nullptr, nullptr, MyInfoEnumProc, 0);
    HDC hScreenDC = ::GetDC(nullptr);

    HDC hMemDC = ::CreateCompatibleDC(hScreenDC);
    HBITMAP hbm;

    hbm = CreateCompatibleBitmap(hScreenDC, width, height);
    HBITMAP oldbm = (HBITMAP)SelectObject(hMemDC, hbm);

    // BitBlt(hMemDC, 0, 0, width, height, hScreenDC, left, top, SRCCOPY);

    // ver 1.6
    DWORD bltFlags = SRCCOPY;
    if ((captureTrans) && (versionOp > 4))
        // if (captureTrans)
        bltFlags |= CAPTUREBLT;
    BitBlt(hMemDC, 0, 0, width, height, hScreenDC, left, top, bltFlags);

    RECT rect;
    rect.left = 0;
    rect.top = 0;
    rect.right = rect.left + width;
    rect.bottom = rect.top + height;
    // TODO(dimator): Enable timestamp annotation
    if (timestampAnnotation)
    {
        SYSTEMTIME systime;
        ::GetLocalTime(&systime);
        //::GetSystemTime(&systime);
        TCHAR msg[0x100];
        ::_stprintf_s(msg, _T("%s %02d:%02d:%02d:%03d"), _T("Recording"), systime.wHour, systime.wMinute, systime.wSecond,
                    systime.wMilliseconds);
    }

    // Get Cursor Pos
    POINT xPoint;
    GetCursorPos(&xPoint);
    HCURSOR hcur = FetchCursorHandle();
    xPoint.x -= left;
    xPoint.y -= top;

    // Draw the Cursor
    if (g_recordcursor == 1)
    {
        ICONINFO iconinfo;
        BOOL ret;
        ret = GetIconInfo(hcur, &iconinfo);
        if (ret)
        {
            xPoint.x -= iconinfo.xHotspot;
            xPoint.y -= iconinfo.yHotspot;

            // need to delete the hbmMask and hbmColor bitmaps
            // otherwise the program will crash after a while after running out of resource
            if (iconinfo.hbmMask)
                DeleteObject(iconinfo.hbmMask);
            if (iconinfo.hbmColor)
                DeleteObject(iconinfo.hbmColor);
        }

        ::DrawIconEx(hMemDC, xPoint.x, xPoint.y, hcur, 0, 0, 0, nullptr, DI_NORMAL);
    }

    SelectObject(hMemDC, oldbm);
    LPBITMAPINFOHEADER pBM_HEADER = (LPBITMAPINFOHEADER)GlobalLock(Bitmap2Dib(hbm, g_bits));
    // LPBITMAPINFOHEADER pBM_HEADER = (LPBITMAPINFOHEADER)GlobalLock(Bitmap2Dib(hbm, 24));
    if (pBM_HEADER == nullptr)
    {
        // TODO(dimator): Some kind of error message here, and a saner exit.
        // MessageBox(nullptr,"Error reading a frame!","Error",MB_OK | MB_ICONEXCLAMATION);
        // MessageOut(nullptr,IDS_STRING_ERRFRAME ,IDS_STRING_NOTE,MB_OK | MB_ICONEXCLAMATION);
        ExitThread(1);
    }

    DeleteObject(hbm);
    DeleteDC(hMemDC);

    ReleaseDC(nullptr, hScreenDC);

    return pBM_HEADER;
}

void FreeFrame(LPBITMAPINFOHEADER alpbi)
{
    if (!alpbi)
        return;

    GlobalFreePtr(alpbi);
    // GlobalFree(alpbi);
    alpbi = nullptr;
}

HCURSOR FetchCursorHandle()
{
    if (hSavedCursor == nullptr)
        hSavedCursor = GetCursor();
    return hSavedCursor;
}

// ver 1.6
#define MAXCOMPRESSORS 50

//===============================================
// AUDIO CODE
//===============================================
// Ver 1.1
//===============================================

/*
MMRESULT IsFormatSupported(LPWAVEFORMATEX g_pwfx, UINT uDeviceID);

MMRESULT IsFormatSupported(LPWAVEFORMATEX g_pwfx, UINT uDeviceID)
{
  return (waveInOpen(
    nullptr,                 // ptr can be nullptr for query
    uDeviceID,            // the device identifier
    g_pwfx,                 // defines requested format
    nullptr,                 // no callback
    nullptr,                 // no instance data
    WAVE_FORMAT_QUERY));  // query only, do not open device
}
*/

void FreeParamsUse()
{
    if (g_pParamsUse)
    {
        GlobalFreePtr(g_pParamsUse);
        g_pParamsUse = nullptr;
    }
}

BOOL MakeCompressParamsCopy(DWORD paramsSize, LPVOID pOrg)
{
    if (g_pParamsUse)
    {
        // Free Existing
        FreeParamsUse();
    }

    g_pParamsUse = (LPVOID)GlobalAllocPtr(GHND, paramsSize);
    if (nullptr == g_pParamsUse)
    {
        //::MessageBox(nullptr,"Failure allocating Video Params or compression","Note", MB_OK | MB_ICONEXCLAMATION);
        // MessageOut(nullptr,IDS_STRING_FAILALLOCVCM ,IDS_STRING_NOTE,MB_OK | MB_ICONEXCLAMATION);

        return false;
    }

    memcpy(g_pParamsUse, pOrg, paramsSize);
    return TRUE;
}

std::string GetCodecDescription(long fccHandler)
{
    ICINFO compinfo;

    HIC hic;
    hic = ICOpen(ICTYPE_VIDEO, fccHandler, ICMODE_QUERY);
    if (hic)
    {
        ICGetInfo(hic, &compinfo, sizeof(ICINFO));
        ICClose(hic);
    }

    char tmp[128];
    std::transform(compinfo.szDescription, compinfo.szDescription + 128, tmp, wide_to_narrow);
    return std::string(tmp);
}

void VideoCodecOptions()
{
    // Capture a frame and use it to determine compatible compressors for user to select

    LPBITMAPINFOHEADER first_alpbi = nullptr;

    COMPVARS compVars;
    compVars.cbSize = sizeof(COMPVARS); // validate it
    compVars.dwFlags = 0;

    int top = 0;
    int left = 0;
    int width = 320;
    int height = 200;

    first_alpbi = captureScreenFrame(left, top, width, height);

    g_num_compressor = 0;
    if (g_compressor_info == nullptr)
    {
        g_compressor_info = (ICINFO *)calloc(MAXCOMPRESSORS, sizeof(ICINFO));
    }
    else
    {

        free(g_compressor_info);
        g_compressor_info = (ICINFO *)calloc(MAXCOMPRESSORS, sizeof(ICINFO));
    }

    for (int i = 0; ICInfo(ICTYPE_VIDEO, i, &g_compressor_info[g_num_compressor]); i++)
    {
        if (g_num_compressor >= MAXCOMPRESSORS)
            break;

        HIC hic;

        if (restrictVideoCodecs)
        {
            // allow only a few
            if ((g_compressor_info[g_num_compressor].fccHandler == mmioFOURCC('m', 's', 'v', 'c')) ||
                (g_compressor_info[g_num_compressor].fccHandler == mmioFOURCC('m', 'r', 'l', 'e')) ||
                (g_compressor_info[g_num_compressor].fccHandler == mmioFOURCC('c', 'v', 'i', 'd')) ||
                (g_compressor_info[g_num_compressor].fccHandler == mmioFOURCC('d', 'i', 'v', 'x')))
            {
                hic = ICOpen(g_compressor_info[g_num_compressor].fccType, g_compressor_info[g_num_compressor].fccHandler,
                             ICMODE_QUERY);
                if (hic)
                {
                    if (ICERR_OK == ICCompressQuery(hic, first_alpbi, nullptr))
                    {
                        ICGetInfo(hic, &g_compressor_info[g_num_compressor], sizeof(ICINFO));
                        g_num_compressor++;
                    }
                    ICClose(hic);
                }
            }
        }
        else
        {
            // CamStudio still cannot handle VIFP
            if (g_compressor_info[g_num_compressor].fccHandler != mmioFOURCC('v', 'i', 'f', 'p'))
            {
                hic = ICOpen(g_compressor_info[g_num_compressor].fccType, g_compressor_info[g_num_compressor].fccHandler,
                             ICMODE_QUERY);
                if (hic)
                {
                    if (ICERR_OK == ICCompressQuery(hic, first_alpbi, nullptr))
                    {
                        ICGetInfo(hic, &g_compressor_info[g_num_compressor], sizeof(ICINFO));
                        g_num_compressor++;
                    }
                    ICClose(hic);
                }
            }
        }
    }
    FreeFrame(first_alpbi);
}

void PrintRecordInformation()
{
    // Display the record information when recording
    if (gRecordState == 1)
    {
        printf("=================================\n");
        printf("Current Frame : %d\n", g_nCurrFrame);
        printf("Theoretical Frame Rate  : %.2f fps\n", g_fRate);
        printf("Time Elasped  : %.2f sec\n", g_fTimeLength);
        printf("Number of Colors  : %d bits\n", g_nColors);
        printf("Codec  : %S\n", g_strCodec.c_str());
        printf("Actual Input Rate  : %.2f fps\n", g_fActualRate);
        printf("Dimension  : %d X %d\n", g_actualwidth, g_actualheight);
    }
}

int OptionNameEqual(const std::string &option_name, std::string parsed_name)
{
    // Remove '/' and '-' characters at the beginning of parsed_name, convert to
    // lower case, and compare with option_name.
    auto start_iter = parsed_name.begin();

    if ((*start_iter) == '/')
    {
        parsed_name.erase(start_iter);
    }
    else if ((*start_iter) == '-')
    {
        start_iter = parsed_name.erase(start_iter);
        if ((*start_iter) == '-')
            parsed_name.erase(start_iter);
    }
    transform(start_iter, parsed_name.end(), start_iter, ::tolower);

    return option_name.compare(parsed_name);
}

int ParseOptions(int argc, char *argv[])
{
    std::vector<std::string> args;

    for (int i = 0; i < argc; ++i)
        args.emplace_back(argv[i]);

    for (auto it = args.begin(); it != args.end(); ++it)
    {
        if (!OptionNameEqual("codec", *it))
        {
            if (++it == args.end())
            {
                 std::cout << "Expected integer ID of codec" <<std::endl;
                return 0;
            }
            g_selected_compressor = atoi((*it).c_str());

            // Make sure we get a valid codec value. Since atoi() returns 0 on
            // failure, we need to add a check for literal "0", because 0 could in
            // fact be a valid codec.
            if (!g_selected_compressor && *it != "0")
            {
                std::cout << "Expected integer ID of codec: " << *it <<std::endl;
                return 0;
            }
        }
        else if (!OptionNameEqual("outfile", *it))
        {
            if (++it == args.end())
            {
                std::cout << "Expected filename to write to (.avi)" <<std::endl;
                return 0;
            }
            g_output_file = *it;
        }
        else if (!OptionNameEqual("seconds", *it))
        {
            if (++it == args.end())
            {
                std::cout << "Expected valid number of seconds to record" <<std::endl;
                return 0;
            }
            g_seconds_to_record = atoi((*it).c_str());

            // 0 is not a valid number of seconds to record
            if (!g_seconds_to_record)
            {
                std::cout << "Expected valid number of seconds to record: " << *it <<std::endl;
                return 0;
            }
        }
        else if (!OptionNameEqual("fps", *it))
        {
            if (++it == args.end())
            {
                std::cout << "Expected framerate" <<std::endl;
                return 0;
            }
            g_frames_per_second = atoi((*it).c_str());

            // framerate should be positive
            if (g_frames_per_second <= 0)
            {
                std::cout << "Framerate should be positive: " << *it <<std::endl;
                return 0;
            }

            // Set to one key frame per second
            g_key_frames_every = g_frames_per_second;
            g_timelapse = 1000 / g_frames_per_second;


        }
        else if (!OptionNameEqual("help", *it))
        {
            PrintUsage(TRUE);
            return 0;
        }
    }

    return 1;
}

void PrintUsage(bool showCodecs = 1)
{
    std::cout << "Usage:" <<std::endl <<std::endl;
    std::cout << "-codec: which codec to use" <<std::endl
         << "-outfile: .avi file to write to" <<std::endl
         << "-seconds: how many seconds to record for ('0' means to record until "
         << "a key is pressed)" <<std::endl
         << "-fps: framerate to record with" <<std::endl
         << "-help: this screen" <<std::endl;

    if (!showCodecs)
        return;

    std::cout <<std::endl << "Valid codecs on this machine:" <<std::endl;
    for (int i = 0; i < g_num_compressor; ++i)
    {
        const auto s = GetCodecDescription(g_compressor_info[i].fccHandler);

         std::cout << i << ": " << s <<std::endl;
    }
}

int ChooseBestCodec()
{
    // TODO(dimator): Pick a codec by ourselves.  The best codecs in terms of
    // availability on most windows machines are:
    //
    //    CamStudio Lossless Codec v1.0 (not usually available)
    //    Microsoft Video 1
    //    Cinepak Codec by Radius
    //    Indeo« video 5.10
    //
    // If available, CamStudio Lossless should be used.  Otherwise, use
    // Microsoft Video, Cinepak Codec or Indeo.  If none of these are
    // installed, then just use the first codec in the compressor_info[] list.
    // If the compressor_info list is empty, then we must fail and exit.

    const static std::vector<std::string> best_compressors = {
        "camstudio lossless",
        "microsoft video 1",
        "cinepak codec",
        "indeo"
    };

    auto best = best_compressors.begin();
    for (; best != best_compressors.end(); ++best)
    {
        for (int i = 0; i < g_num_compressor; ++i)
        {
            auto s = GetCodecDescription(g_compressor_info[i].fccHandler);
            auto lower = s;
            transform(lower.begin(), lower.end(), lower.begin(), ::tolower);

            if (lower.find(*best) != std::string::npos)
            {
                return i;
            }
        }
    }

    // None of the best codecs were found
    return -1;
}

int main(int argc, char *argv[])
{
    DWORD tid = 0;
    DWORD exitcode = 0;
    HDC hScreenDC;
    int mon_count = GetSystemMetrics(SM_CMONITORS); // get nubmer of monitors

    std::vector<HANDLE> th(mon_count);
    // PAVIFILE* pfile = (PAVIFILE *) malloc(sizeof(PAVIFILE) * mon_count);

    std::cout << "camstudio_cl: Command line screen recording" <<std::endl;
    std::cout << "v" << CAMSTUDIO_CL_VERSION <<std::endl <<std::endl;

    VideoCodecOptions();

    if (!ParseOptions(argc, argv))
    {
        return 1;
    }

    if (!g_output_file.length())
    {
        std::cout << "Error: Need a valid output file, -outfile" <<std::endl;
        PrintUsage();
        return 1;
    }
    if (g_selected_compressor >= g_num_compressor)
    {
        std::cout << "Error: Need a valid codec, -codec" <<std::endl;
        PrintUsage();
        return 1;
    }

    if (g_selected_compressor == -1)
    {
        g_selected_compressor = ChooseBestCodec();

        if (g_selected_compressor == -1)
        {
            std::cout << "Error: No useable codec was found on this machine" <<std::endl;
            return 1;
        }
    }

    g_comp_fcc_handler = g_compressor_info[g_selected_compressor].fccHandler;
    g_strCodec = g_compressor_info[g_selected_compressor].szDescription;

    std::wcout << L"Using codec: " << g_strCodec <<std::endl;

    // Screen metrics:
    hScreenDC = ::GetDC(nullptr);
    maxxScreen = GetDeviceCaps(hScreenDC, HORZRES);
    maxyScreen = GetDeviceCaps(hScreenDC, VERTRES);

    screen *obr = (screen *)malloc(sizeof(screen) * mon_count); // new screen[mon_count];//array of screens(objects)
    screen *pscreen = obr;

    // Detection of screens
    std::cout << "Detected displays:" <<std::endl;
    EnumDisplayMonitors(nullptr, nullptr, MonitorEnumProc, (LPARAM)pscreen);

    char buffer[2];
    auto recordHere = g_output_file;

    for (int i = 0; i <= mon_count - 1; i++)
    {
        // whole recording stuff goes BELOW this line.
        maxxScreen = pscreen[i].width;
        maxyScreen = pscreen[i].height;
        rcUse.left = pscreen[i].left;
        rcUse.top = pscreen[i].top;
        rcUse.right = pscreen[i].right - 1;
        rcUse.bottom = pscreen[i].bottom - 1;
        // Write AVI file.
        _itoa_s(i, buffer, 2, 10);
        auto tmpPath = recordHere + "_" + buffer + std::string(".avi"); // _itoa(i,buffer,10)+".avi";
        strcpy_s(pscreen[i].outFile, tmpPath.c_str());
        pscreen[i].index = i;

        gRecordState = 1;

        std::cout << "Creating recording thread for screen no.:" << i << "..." <<std::endl;
        std::cout << "Recording to: " << tmpPath <<std::endl;

        th[i] = CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)RecordAVIThread, (LPVOID)&pscreen[i], 0, &tid);

        // whole recording stuff goes ABOVE this line.
        //std::cout<<"Sirka:" << p_obr[i].width <<std::endl; // just some line for testing.
    }
    if (g_seconds_to_record > 0)
    {
        std::cout << "Recording for " << g_seconds_to_record << " seconds" <<std::endl;
        ::Sleep(g_seconds_to_record * 1000);
    }
    else
    {
        std::cout << "Enter any key to stop recording..." <<std::endl;
        std::cout.flush();
        getchar();
    }
    // Set gRecordState to 0, to end the while loop inside the recording thread.
    // TODO(dimator): Maybe some better IPC, other than a global variable???
    gRecordState = 0;

    free(obr);
    Sleep(1000);

    // rcUse.left = offset_left;
    // rcUse.top = offset_top;
    // rcUse.right = 300;
    // rcUse.bottom = 400;
    // if(offset_bottom){
    // rcUse.bottom = offset_bottom - 1;
    //}
    // else{
    // rcUse.bottom = maxyScreen - 1;
    //}
    // if(offset_right){
    // rcUse.right = offset_right - 1;
    //}
    // else{
    // rcUse.right = maxxScreen - 1;
    //}
    ////cout << "Offset L:" << rcUse.left <<std::endl;
    ////cout << "Offset T:" << rcUse.top <<std::endl;
    ////cout << "Offset R:" << rcUse.right <<std::endl;
    ////cout << "Offset B:" << rcUse.bottom <<std::endl;
    //
    // gRecordState = 1;

    //std::cout << "Creating recording thread..." <<std::endl;
    //std::cout << "Recording to: " << g_output_file <<std::endl;

    // if(seconds_to_record > 0){
    //  std::cout << "Recording for " << seconds_to_record << " seconds" <<std::endl;
    //  ::Sleep(seconds_to_record * 1000);
    //} else {
    // std::cout << "Enter any key to stop recording..." <<std::endl;
    // std::cout.flush();
    //  getchar();
    //}

    //// Set gRecordState to 0, to end the while loop inside the recording thread.
    //// TODO(dimator): Maybe some better IPC, other than a global variable???
    // gRecordState = 0;

    std::cout << "Waiting for recording thread to exit..." <<std::endl;
    if (WaitForSingleObject(th[0], 5000) == WAIT_TIMEOUT)
    {
        std::cout << "Error: Timed out!  The recorded video might have errors." <<std::endl;
        std::cout << "Killing thread..." <<std::endl;
        TerminateThread(th[0], exitcode);
    }

    // Make sure the thread ended gracefully
    GetExitCodeThread(th[0], &exitcode);
    if (exitcode)
    {
        std::cout << "Error: Recording thread ended abnormally" <<std::endl;
        return exitcode;
    }

    return 0;
}
