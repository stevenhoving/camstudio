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
#include <cstring>
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
#include "Commandline.hpp"

using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////
//Function prototypes
//////////////////////////////////////

//Screen Detection Functions
BOOL CALLBACK MonitorEnumProc(HMONITOR hMonitor,HDC hdcMonitor,LPRECT lprcMonitor,LPARAM dwData);

//Mouse Capture functions
HCURSOR FetchCursorHandle();
HCURSOR hSavedCursor = NULL;

//AVI functions  and #defines
#define AVIIF_KEYFRAME  0x00000010L // this frame is a key frame.
#define BUFSIZE 260
#define LPLPBI  LPBITMAPINFOHEADER *
#define N_FRAMES  50
#define TEXT_HEIGHT 20

HANDLE Bitmap2Dib(HBITMAP, UINT);
int RecordVideo(int top,int left,int width,int height,int numframes,const char *szFileName/*, screen* pscreen*/);
UINT RecordAVIThread(LPVOID pParam);

//Use these 2 functions to create frames and free frames
LPBITMAPINFOHEADER captureScreenFrame(int left,int top,int width, int height);
void FreeFrame(LPBITMAPINFOHEADER) ;

void PrintUsage(bool);
void PrintRecordInformation();

//////////////////
//State variables

//Vars used for selecting fixed /variableregion
RECT   rcUse;

int maxxScreen;
int maxyScreen;

//Misc Vars
int gRecordState=0;
DWORD initialtime=0;
int initcapture = 0;
int irsmallcount=0;

// Messaging
HWND hWndGlobal = NULL;
/*
static UINT WM_USER_RECORDINTERRUPTED = ::RegisterWindowMessage(WM_USER_RECORDINTERRUPTED_MSG);
*/

/////////////////////////////////////////////////////////
//Variables/Options requiring interface
/////////////////////////////////////////////////////////
int static mon_current = -1;
int mon_count = 0; // number of physical screens, not virtual
int bits = 24;

//Video Options and Compressions
int timelapse=40;
int frames_per_second = 25;
int keyFramesEvery = 25;


int compquality = 7000;
DWORD compfccHandler = 0;
ICINFO *compressor_info = NULL;
int num_compressor = 0;

// User options:
int seconds_to_record = -1;
int selected_compressor = -1;
string output_file;
int offset_left = 0;
int offset_right;
int offset_bottom;
int offset_top = 0;
//Ver 1.2
//Video Compress Parameters
LPVOID pVideoCompressParams = NULL;
DWORD CompressorStateIsFor = 0;
DWORD CompressorStateSize = 0;

LPVOID pParamsUse = NULL;
void FreeParamsUse();
BOOL MakeCompressParamsCopy(DWORD paramsSize, LPVOID pOrg);

//Report variables
int nActualFrame=0;
int nCurrFrame=0;
float fRate=0.0;
float fActualRate=0.0;
float fTimeLength=0.0;
int nColors=24;

CString strCodec("MS Video 1");
int actualwidth=0;
int actualheight=0;

//Cursor variables
int g_customsel = 0;
//TODO(dimator): add command line flag for g_recordcursor
int g_recordcursor = 1;
int g_cursortype = 0;
int g_highlightsize=64;
int g_highlightshape=0;
COLORREF g_highlightcolor = RGB(255,255,125);
int g_highlightclick=0;
COLORREF g_highlightclickcolorleft = RGB(255,0,0);
COLORREF g_highlightclickcolorright = RGB(0,0,255);

//Autopan
int autopan=0;
int maxpan = 20;
RECT panrect_current;
RECT panrect_dest;

//Path to temporary wav file
char tempaudiopath[MAX_PATH];
int recordaudio=0;

//Audio Recording Variables
UINT AudioDeviceID = WAVE_MAPPER;

HWAVEIN m_hRecord;
WAVEFORMATEX m_Format;
DWORD m_ThreadID;
int m_QueuedBuffers=0;
int  m_BufferSize = 1000; // number of samples
//CSoundFile *m_pFile = NULL;

//Audio Options Dialog
LPWAVEFORMATEX      pwfx = NULL;
DWORD               cbwfx;

//Audio Formats Dialog
DWORD waveinselected = WAVE_FORMAT_2S16;
int audio_bits_per_sample = 16;
int audio_num_channels = 2;
int audio_samples_per_seconds = 22050 ;
BOOL bAudioCompression = TRUE;

#define  MILLISECONDS 0
#define  FRAMES 1
BOOL interleaveFrames = TRUE;
int  interleaveFactor = 100;
int  interleaveUnit = MILLISECONDS;

//state vars

//Tray Icon
#define WM_TRAY_ICON_NOTIFY_MESSAGE (WM_USER + 50)

string GetCodecDescription(long fccHandler);

//version 1.6
#define USE_WINDOWS_TEMP_DIR 0
#define USE_INSTALLED_DIR 1
#define USE_USER_SPECIFIED_DIR 2

int tempPath_Access  = USE_WINDOWS_TEMP_DIR;
CString specifieddir;

int captureTrans=1;
int versionOp = 0;

//ver 2.26 Vscap Interface
#define ModeAVI 0
#define ModeFlash 1
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
BOOL CALLBACK MonitorEnumProc(HMONITOR hMonitor,HDC hdcMonitor,LPRECT lprcMonitor,LPARAM dwData)
{
			
			lprcMonitor;hdcMonitor;
			screen *obr;
			obr = (screen*) dwData;
			mon_current = mon_current + 1;
			MONITORINFOEX mo;
			mo.cbSize = sizeof(MONITORINFOEX);
			if(GetMonitorInfo(hMonitor,&mo)){
			}
			else{
				cout << "Error in detecting monitors" << endl;
				return false;
			}
			obr[mon_current].SetDimensions(mo.rcMonitor.left, mo.rcMonitor.right, mo.rcMonitor.top, mo.rcMonitor.bottom);
			strcpy_s(obr[mon_current].dispName, sizeof(obr[mon_current].dispName),mo.szDevice);
			cout << "Screen: " << obr[mon_current].dispName;
			cout << " resolution:" << obr[mon_current].width << " x " << obr[mon_current].height << endl;
			
			return true;
}
CHAR wide_to_narrow(WCHAR w)
{
	// simple typecast
	// works because UNICODE incorporates ASCII into itself
	return CHAR(w);
}

HANDLE  Bitmap2Dib( HBITMAP hbitmap, UINT bits )
{
  HANDLE               hdib ;
  HDC                 hdc ;
  BITMAP              bitmap ;
  UINT                wLineLen ;
  DWORD               dwSize ;
  DWORD               wColSize ;
  LPBITMAPINFOHEADER  lpbi ;
  LPBYTE              lpBits ;

  GetObject(hbitmap,sizeof(BITMAP),&bitmap) ;

  //
  // DWORD align the width of the DIB
  // Figure out the size of the colour table
  // Calculate the size of the DIB
  //
  wLineLen = (bitmap.bmWidth*bits+31)/32 * 4;
  wColSize = sizeof(RGBQUAD)*((bits <= 8) ? 1<<bits : 0);
  dwSize = sizeof(BITMAPINFOHEADER) + wColSize +
    (DWORD)(UINT)wLineLen*(DWORD)(UINT)bitmap.bmHeight;

  //
  // Allocate room for a DIB and set the LPBI fields
  //
  hdib = GlobalAlloc(GHND,dwSize);
  if (!hdib)
    return hdib ;

  lpbi = (LPBITMAPINFOHEADER)GlobalLock(hdib) ;

  lpbi->biSize = sizeof(BITMAPINFOHEADER) ;
  lpbi->biWidth = bitmap.bmWidth ;
  lpbi->biHeight = bitmap.bmHeight ;
  lpbi->biPlanes = 1 ;
  lpbi->biBitCount = (WORD) bits ;
  lpbi->biCompression = BI_RGB ;
  lpbi->biSizeImage = dwSize - sizeof(BITMAPINFOHEADER) - wColSize ;
  lpbi->biXPelsPerMeter = 0 ;
  lpbi->biYPelsPerMeter = 0 ;
  lpbi->biClrUsed = (bits <= 8) ? 1<<bits : 0;
  lpbi->biClrImportant = 0 ;

  //
  // Get the bits from the bitmap and stuff them after the LPBI
  //
  lpBits = (LPBYTE)(lpbi+1)+wColSize ;

  hdc = CreateCompatibleDC(NULL) ;

  GetDIBits(hdc,hbitmap,0,bitmap.bmHeight,lpBits,(LPBITMAPINFO)lpbi, DIB_RGB_COLORS);

  lpbi->biClrUsed = (bits <= 8) ? 1<<bits : 0;

  DeleteDC(hdc) ;
  GlobalUnlock(hdib);

  return hdib ;
}

UINT RecordAVIThread(LPVOID lParam) {
  CoInitialize(NULL);
  SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_NORMAL);
  screen* pscreen = (screen*) lParam; // pointer to current screen
 // const char *filepath = pscreen->outFile;

  int top=pscreen->top;
  int left=pscreen->left;
  int width=pscreen->width;
  int height=pscreen->height;
  int fps = frames_per_second;

  const char *filepath = output_file.c_str();

  //Test the validity of writing to the file
  //Make sure the file to be created is currently not used by another application
  int fileverified = 0;
  while (!fileverified)
  {
    OFSTRUCT ofstruct;
    HFILE fhandle = OpenFile(filepath, &ofstruct,
        OF_SHARE_EXCLUSIVE | OF_WRITE  | OF_CREATE );
    if (fhandle != HFILE_ERROR) {
      fileverified = 1;
      CloseHandle( (HANDLE) fhandle );
      DeleteFile(filepath);
    }
    else {
      cout << "Error: Could not open file for creation: " << filepath << endl;
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
int RecordVideo(int top,int left,int width,int height,int fps,
    const char *szFileName /*screen *pscreen*/){

  LPBITMAPINFOHEADER alpbi;
  AVISTREAMINFO strhdr;
  PAVIFILE pfile = NULL;
  PAVISTREAM ps = NULL, psCompressed = NULL;
  AVICOMPRESSOPTIONS opts;
  AVICOMPRESSOPTIONS FAR * aopts[1] = {&opts};
  HRESULT hr;
  WORD wVer;

  actualwidth=width;
  actualheight=height;

  wVer = HIWORD(VideoForWindowsVersion());
  if (wVer < 0x010a)
  {
    return FALSE;
  }

  alpbi = NULL;

  ////////////////////////////////////////////////
  // CAPTURE FIRST FRAME
  ////////////////////////////////////////////////
  alpbi = captureScreenFrame(left,top,width, height);

  ////////////////////////////////////////////////
  // TEST VALIDITY OF COMPRESSOR
  //////////////////////////////////////////////////

  if (selected_compressor > 0) {

    HIC hic = NULL;
    hic = ICOpen(compressor_info[selected_compressor].fccType,
        compressor_info[selected_compressor].fccHandler, ICMODE_QUERY);
    if (hic) {

      int left =0;
	  int top=0;
	  int width=0;
	  int height=0;
      int align = 1;
      while   (ICERR_OK!=ICCompressQuery(hic, alpbi, NULL))
      {
        //Try adjusting width/height a little bit
        align = align * 2 ;
        if (align>8) break;
        int wm = (width % align);
        if (wm > 0) {
          width = width + (align - wm);
          if (width>maxxScreen)
            width = width - wm;
        }
        int hm = (height % align);
        if (hm > 0) {
          height = height + (align - hm);
          if (height>maxyScreen)
            width = height - hm;
        }
        if (alpbi)
          FreeFrame(alpbi);
        alpbi = captureScreenFrame(left,top,width, height);
      }

      //if succeed with new width/height, use the new width and height
      //else if still fails ==> default to MS Video 1 (MSVC)
      if (align == 1){
        //Compressor has no problem with the current dimensions...so proceed
        //do nothing here
      }
      else if (align <= 8) {
          //Compressor can work if the dimensions is adjusted slightly
          width=width;
          height=height;

          actualwidth=width;
          actualheight=height;
      }
      else {
          compfccHandler = mmioFOURCC('M', 'S', 'V', 'C');
		  strCodec = CString("MS Video 1");
      }

      ICClose(hic);
    }
    else {
      compfccHandler = mmioFOURCC('M', 'S', 'V', 'C');
	  strCodec = CString("MS Video 1");
      //MessageBox(NULL,"hic default","note",MB_OK);
    }

  }//selected_compressor

  //IV50
  if (compfccHandler==mmioFOURCC('I', 'V', '5', '0'))
  {
    //Still Can't Handle Indeo 5.04
    compfccHandler = mmioFOURCC('M', 'S', 'V', 'C');
	strCodec = CString("MS Video 1");
  }

  ////////////////////////////////////////////////
  // INIT AVI USING FIRST FRAME
  ////////////////////////////////////////////////
  AVIFileInit();

  //
  // Open the movie file for writing....
  //
  hr = AVIFileOpen(&pfile, szFileName, OF_WRITE | OF_CREATE, NULL);
  if (hr != AVIERR_OK)
    goto error;

  // Fill in the header for the video stream....
  // The video stream will run in 15ths of a second....
  _fmemset(&strhdr, 0, sizeof(strhdr));
  strhdr.fccType                = streamtypeVIDEO;// stream type

  //strhdr.fccHandler             = compfccHandler;
  strhdr.fccHandler             = 0;

  strhdr.dwScale                = 1;
  strhdr.dwRate                 = fps;
  strhdr.dwSuggestedBufferSize  = alpbi->biSizeImage;
  SetRect(&strhdr.rcFrame, 0, 0,        // rectangle for stream
      (int) alpbi->biWidth,
      (int) alpbi->biHeight);

  // And create the stream;
  hr = AVIFileCreateStream(pfile, &ps, &strhdr);
  if (hr != AVIERR_OK)  goto error;

  memset(&opts, 0, sizeof(opts));
  aopts[0]->fccType      = streamtypeVIDEO;
  //aopts[0]->fccHandler     = mmioFOURCC('M', 'S', 'V', 'C');
  aopts[0]->fccHandler     = compfccHandler;
  aopts[0]->dwKeyFrameEvery    = keyFramesEvery;    // keyframe rate
  aopts[0]->dwQuality    = compquality;        // compress quality 0-10,000
  aopts[0]->dwBytesPerSecond           = 0;   // bytes per second
  aopts[0]->dwFlags      = AVICOMPRESSF_VALID | AVICOMPRESSF_KEYFRAMES;    // flags
  aopts[0]->lpFormat       = 0x0;                         // save format
  aopts[0]->cbFormat       = 0;
  aopts[0]->dwInterleaveEvery = 0;      // for non-video streams only

  //ver 2.26
  if (RecordingMode == ModeFlash)
  {
    //Internally adjust codec to MSVC 100 Quality
    aopts[0]->fccHandler = mmioFOURCC('M', 'S', 'V', 'C');   //msvc
	strCodec = CString("MS Video 1");
    aopts[0]->dwQuality = 10000;

  }
  else
  {
    //Ver 1.2
    //
    if ((compfccHandler == CompressorStateIsFor) && (compfccHandler != 0)) {

      //make a copy of the pVideoCompressParams just in case after compression, this variable become messed up
      if (MakeCompressParamsCopy(CompressorStateSize, pVideoCompressParams)) {

        aopts[0]->lpParms      = pParamsUse;
        aopts[0]->cbParms      = CompressorStateSize;

      }

    }

  }

  //The 1 here indicates only 1 stream
  //if (!AVISaveOptions(NULL, 0, 1, &ps, (LPAVICOMPRESSOPTIONS *) &aopts))
  //        goto error;

  hr = AVIMakeCompressedStream(&psCompressed, ps, &opts, NULL);
  if (hr != AVIERR_OK)    goto error;

  hr = AVIStreamSetFormat(psCompressed, 0,
             alpbi,     // stream format
             alpbi->biSize +   // format size
             alpbi->biClrUsed * sizeof(RGBQUAD));
  if (hr != AVIERR_OK) goto error;

  FreeFrame(alpbi);
  alpbi=NULL;

  if (autopan) {
    panrect_current.left = left;
    panrect_current.top = top;
    panrect_current.right = left + width - 1;
    panrect_current.bottom = top + height - 1;
  }

  DWORD timeexpended, frametime, oldframetime;

  initialtime = timeGetTime();
  initcapture = 1;
  oldframetime = 0;
  nCurrFrame = 0;
  nActualFrame = 0;

  //////////////////////////////////////////////
  // WRITING FRAMES
  //////////////////////////////////////////////

  long divx, oldsec;
  divx=0;
  oldsec=0;
  
  // Time when the next frame will be taken
  DWORD nextFrameAt = timeGetTime();
  DWORD nextFrameNumber = 0;

  while (gRecordState) {  //repeatedly loop
	// TODO(dimator): Enable this via a command line argument:
	//PrintRecordInformation();

    if (initcapture==0) {
      timeexpended = timeGetTime() - initialtime;
    }
    else {
      frametime = 0;
      timeexpended = 0;
    }

	// Wait for next frame
	int toNextFrame = nextFrameAt - timeGetTime();
	printf("nextFrameAt: %d, timeGetTime: %d, toNextFrame: %d\n", nextFrameAt, timeGetTime(), toNextFrame);
	if(toNextFrame > 0) {
		printf("Sleeping for %d msec (%d - ", toNextFrame, timeGetTime());
		Sleep(toNextFrame);
		printf("%d)\n", timeGetTime());
	}
	// todo add dropped frame handling
	
	DWORD now = timeGetTime();
	// nextFrameAt will be incremented at least once to account for
	// sleep inaccuracy.
	do {
		nextFrameAt += timelapse;
		nextFrameNumber++;
	} while((int)nextFrameAt - (int)now < 0);

	// Record frame
    //Autopan
    if ((autopan) && (width < maxxScreen) && (height < maxyScreen)) {
          POINT xPoint;
          GetCursorPos(&xPoint);

          int extleft = ((panrect_current.right - panrect_current.left)*1)/4 + panrect_current.left;
          int extright = ((panrect_current.right - panrect_current.left)*3)/4 + panrect_current.left;
          int exttop = ((panrect_current.bottom - panrect_current.top)*1)/4 + panrect_current.top;
          int extbottom = ((panrect_current.bottom - panrect_current.top)*3)/4 + panrect_current.top;

          if (xPoint.x  < extleft ) { //need to pan left

            panrect_dest.left = xPoint.x - width/2;
            panrect_dest.right = panrect_dest.left +  width - 1;
            if (panrect_dest.left < 0)  {
                panrect_dest.left = 0;
                panrect_dest.right = panrect_dest.left +  width - 1;
            }

          }
          else if (xPoint.x  > extright ) { //need to pan right

            panrect_dest.left = xPoint.x - width/2;
            panrect_dest.right = panrect_dest.left +  width - 1;
            if (panrect_dest.right >= maxxScreen) {
                panrect_dest.right = maxxScreen - 1;
                panrect_dest.left  = panrect_dest.right - width + 1;
            }

          }
          else {

            panrect_dest.right = panrect_current.right;
            panrect_dest.left  = panrect_current.left;

          }

          if (xPoint.y  < exttop ) { //need to pan up

            panrect_dest.top = xPoint.y - height/2;
            panrect_dest.bottom = panrect_dest.top +  height - 1;
            if (panrect_dest.top < 0)  {
                panrect_dest.top = 0;
                panrect_dest.bottom = panrect_dest.top +  height - 1;
            }
          }
          else if (xPoint.y  > extbottom ) { //need to pan down

            panrect_dest.top = xPoint.y - height/2;
            panrect_dest.bottom = panrect_dest.top +  height - 1;
            if (panrect_dest.bottom >= maxyScreen) {
                panrect_dest.bottom = maxyScreen - 1;
                panrect_dest.top  = panrect_dest.bottom - height + 1;
            }

          }
          else {

            panrect_dest.top = panrect_current.top;
            panrect_dest.bottom  = panrect_current.bottom;

          }

          //Determine Pan Values
          int xdiff,ydiff;
          xdiff = panrect_dest.left - panrect_current.left;
          ydiff = panrect_dest.top - panrect_current.top;

          if (abs(xdiff) < maxpan) {
            panrect_current.left += xdiff;
          }
          else {

            if (xdiff<0)
              panrect_current.left -= maxpan;
            else
              panrect_current.left += maxpan;

          }

          if (abs(ydiff) < maxpan) {
            panrect_current.top += ydiff;
          }
          else {

            if (ydiff<0)
              panrect_current.top -= maxpan;
            else
              panrect_current.top += maxpan;

          }

          panrect_current.right = panrect_current.left + width - 1;
          panrect_current.bottom =  panrect_current.top + height - 1;

          alpbi = captureScreenFrame(panrect_current.left,
              panrect_current.top,width, height);

    }
    else {

      alpbi = captureScreenFrame(left,top,width, height);

    }

    if (initcapture==0) {

      if (timelapse>1000)
        frametime++;
      else
        frametime = (DWORD) (((double) timeexpended /1000.0 ) * (double) (1000.0/timelapse));

    }
    else  {
      initcapture = 0;

    }

    fTimeLength = ((float) timeexpended) /((float) 1000.0);

    if (recordpreset)
    {
      //if (int(fTimeLength) >= presettime)
       // ;
        //gRecordState = 0;
        //PostMessage(hWndGlobal,WM_USER_RECORDINTERRUPTED,0,0);

      //CString msgStr;
      //msgStr.Format("%.2f %d",fTimeLength,presettime);
      //MessageBox(NULL,msgStr,"N",MB_OK);

        //or should we post messages
    }

    if ((frametime==0) || (frametime>oldframetime)) {
      //if frametime repeats (frametime == oldframetime) ...the avistreamwrite will cause an error

      hr = AVIStreamWrite(psCompressed, // stream pointer
        frametime,        // time of this frame
        1,        // number to write
        (LPBYTE) alpbi +    // pointer to data
          alpbi->biSize +
          alpbi->biClrUsed * sizeof(RGBQUAD),
          alpbi->biSizeImage, // size of this frame
        //AVIIF_KEYFRAME,      // flags....
        0,    //Dependent n previous frame, not key frame
        NULL,
        NULL);

      if (hr != AVIERR_OK)
        break;

      nActualFrame ++ ;
      nCurrFrame = frametime;
      fRate = ((float) nCurrFrame)/fTimeLength;
      fActualRate = ((float) nActualFrame)/fTimeLength;

      //Update recording stats every half a second
      divx = timeexpended / 500;

      if (divx != oldsec) {
        oldsec=divx;
        // TODO(dimator): enabling the following causes bad flickering.  Don't
        // know why it ever existed.
        //InvalidateRect(hWndGlobal,NULL, FALSE);
      }

      //free memory
      FreeFrame(alpbi);
      alpbi=NULL;
      oldframetime = frametime;
    } // if frametime is different

	frametime = nextFrameNumber;
  } //for loop

error:

  //
  // Now close the file
  //

  //Ver 1.2
  //
  if ((compfccHandler == CompressorStateIsFor) && (compfccHandler != 0)) {

      //Detach pParamsUse from AVICOMPRESSOPTIONS so AVISaveOptionsFree will not free it
      //(we will free it ourselves)

      //Setting this is no harm even if pParamsUse is not attached to lpParams
      aopts[0]->lpParms      = 0;
      aopts[0]->cbParms      = 0;

  }

  AVISaveOptionsFree(1,(LPAVICOMPRESSOPTIONS FAR *) &aopts);

  if (pfile)
    AVIFileClose(pfile);

  if (ps)
    AVIStreamClose(ps);

  if (psCompressed)
    AVIStreamClose(psCompressed);

  AVIFileExit();

  if (hr != NOERROR)  {
    //PostMessage(hWndGlobal,WM_USER_RECORDINTERRUPTED,0,0);
    /*
    char *ErrorBuffer; // This really is a pointer - not reserved space!
    FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |  FORMAT_MESSAGE_FROM_SYSTEM, NULL, GetLastError(), MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US), (LPTSTR)&ErrorBuffer, 0, NULL);

    CString reasonstr(ErrorBuffer);
    CString errorstr("File Creation Error. Unable to rename file.\n\n");
    CString reportstr;

    reportstr = errorstr + reasonstr;
    //MessageBox(NULL,reportstr,"Note",MB_OK | MB_ICONEXCLAMATION);
    */

    if (compfccHandler != mmioFOURCC('M', 'S', 'V', 'C')) {
      //if (IDYES == MessageBox(NULL, "Error recording AVI file using current compressor. Use default compressor ? ", "Note", MB_YESNO | MB_ICONEXCLAMATION)) {
      /*
      if (IDYES == MessageOut(NULL, IDS_STRING_ERRAVIDEFAULT, IDS_STRING_NOTE, MB_YESNO | MB_ICONQUESTION )) {

        compfccHandler = mmioFOURCC('M', 'S', 'V', 'C');
        strCodec = "MS Video 1";
      }
      */
    }
    else {
      //MessageBox(NULL, "Error Creating AVI File", "Error", MB_OK | MB_ICONEXCLAMATION);
      //MessageOut(NULL,IDS_STRING_ERRCREATEAVI ,IDS_STRING_NOTE,MB_OK | MB_ICONEXCLAMATION);
    }

    return 0;
  }

  //Save the file on success

  cout << "Recording finished" << endl;
  return 0;
}

LPBITMAPINFOHEADER captureScreenFrame(int left,int top,int width, int height)
{
//EnumDisplayMonitors(NULL, NULL, MyInfoEnumProc, 0);
  HDC hScreenDC = ::GetDC(NULL);

  HDC hMemDC = ::CreateCompatibleDC(hScreenDC);
  HBITMAP hbm;

  hbm = CreateCompatibleBitmap(hScreenDC, width, height);
  HBITMAP oldbm = (HBITMAP) SelectObject(hMemDC, hbm);

  //BitBlt(hMemDC, 0, 0, width, height, hScreenDC, left, top, SRCCOPY);

  //ver 1.6
  DWORD bltFlags = SRCCOPY;
  if ((captureTrans) && (versionOp>4))
  //if (captureTrans)
    bltFlags |= CAPTUREBLT;
  BitBlt(hMemDC, 0, 0, width, height, hScreenDC, left, top, bltFlags);

  RECT rect;
  rect.left = 0;
  rect.top = 0;
  rect.right = rect.left + width;
  rect.bottom = rect.top + height;
  // TODO(dimator): Enable timestamp annotation
  if(timestampAnnotation){
    SYSTEMTIME systime;
    ::GetLocalTime(&systime);
    //::GetSystemTime(&systime);
    TCHAR msg[0x100];
    ::sprintf_s(msg, "%s %02d:%02d:%02d:%03d", "Recording", systime.wHour, systime.wMinute, systime.wSecond, systime.wMilliseconds);
  }

  //Get Cursor Pos
  POINT xPoint;
  GetCursorPos( &xPoint );
  HCURSOR hcur = FetchCursorHandle();
  xPoint.x -= left;
  xPoint.y -= top;

  //Draw the Cursor
  if (g_recordcursor == 1) {
    ICONINFO iconinfo ;
    BOOL ret;
    ret = GetIconInfo( hcur,  &iconinfo );
    if (ret) {
      xPoint.x -= iconinfo.xHotspot;
      xPoint.y -= iconinfo.yHotspot;

      //need to delete the hbmMask and hbmColor bitmaps
      //otherwise the program will crash after a while after running out of resource
      if (iconinfo.hbmMask)
        DeleteObject(iconinfo.hbmMask);
      if (iconinfo.hbmColor)
        DeleteObject(iconinfo.hbmColor);
    }
	
	::DrawIconEx( hMemDC, xPoint.x, xPoint.y, hcur, 0, 0, 0, NULL, DI_NORMAL);
  }

  SelectObject(hMemDC,oldbm);
  LPBITMAPINFOHEADER pBM_HEADER = (LPBITMAPINFOHEADER)GlobalLock(Bitmap2Dib(hbm, bits));
  //LPBITMAPINFOHEADER pBM_HEADER = (LPBITMAPINFOHEADER)GlobalLock(Bitmap2Dib(hbm, 24));
  if (pBM_HEADER == NULL) {
    //TODO(dimator): Some kind of error message here, and a saner exit.
    //MessageBox(NULL,"Error reading a frame!","Error",MB_OK | MB_ICONEXCLAMATION);
    //MessageOut(NULL,IDS_STRING_ERRFRAME ,IDS_STRING_NOTE,MB_OK | MB_ICONEXCLAMATION);
    ExitThread(1);
  }

  DeleteObject(hbm);
  DeleteDC(hMemDC);

  ReleaseDC(NULL,hScreenDC) ;

  return pBM_HEADER;
}

void FreeFrame(LPBITMAPINFOHEADER alpbi)
{
  if (!alpbi)
    return ;

  GlobalFreePtr(alpbi);
  //GlobalFree(alpbi);
  alpbi = NULL;
}

HCURSOR FetchCursorHandle() {
  if (hSavedCursor == NULL)
    hSavedCursor = GetCursor();
  return hSavedCursor;
}

//ver 1.6
#define MAXCOMPRESSORS 50

//===============================================
// AUDIO CODE
//===============================================
// Ver 1.1
//===============================================

/*
MMRESULT IsFormatSupported(LPWAVEFORMATEX pwfx, UINT uDeviceID);

MMRESULT IsFormatSupported(LPWAVEFORMATEX pwfx, UINT uDeviceID)
{
  return (waveInOpen(
    NULL,                 // ptr can be NULL for query
    uDeviceID,            // the device identifier
    pwfx,                 // defines requested format
    NULL,                 // no callback
    NULL,                 // no instance data
    WAVE_FORMAT_QUERY));  // query only, do not open device
}
*/

void FreeParamsUse() {
  if (pParamsUse) {
    GlobalFreePtr(pParamsUse);
    pParamsUse = NULL;
  }
}

BOOL MakeCompressParamsCopy(DWORD paramsSize, LPVOID pOrg) {
  if (pParamsUse) {
    //Free Existing
    FreeParamsUse();
  }

  pParamsUse = (LPVOID) GlobalAllocPtr(GHND, paramsSize);
  if (NULL == pParamsUse)
  {
        //::MessageBox(NULL,"Failure allocating Video Params or compression","Note", MB_OK | MB_ICONEXCLAMATION);
    //MessageOut(NULL,IDS_STRING_FAILALLOCVCM ,IDS_STRING_NOTE,MB_OK | MB_ICONEXCLAMATION);

    return FALSE;
  }

  memcpy(pParamsUse,pOrg,paramsSize);
  return TRUE;
}

string GetCodecDescription(long fccHandler) {
  ICINFO compinfo;

  HIC hic;
  hic = ICOpen(ICTYPE_VIDEO, fccHandler, ICMODE_QUERY);
  if (hic) {
    ICGetInfo(hic, &compinfo, sizeof(ICINFO));
    ICClose(hic);
  }

  char tmp[128];
  std::transform(compinfo.szDescription, compinfo.szDescription + 128, tmp, wide_to_narrow);
  return string(tmp);
}

void VideoCodecOptions()
{
  //Capture a frame and use it to determine compatitble compressors for user to select

  LPBITMAPINFOHEADER first_alpbi = NULL;

  COMPVARS compVars;
  compVars.cbSize = sizeof(COMPVARS);     // validate it
    compVars.dwFlags = 0;

  int top=0;
  int left=0;
  int width=320;
  int height=200;

  first_alpbi = captureScreenFrame(left,top,width, height);

  num_compressor = 0;
  if  (compressor_info == NULL) {
    compressor_info = (ICINFO *) calloc(MAXCOMPRESSORS,sizeof(ICINFO));
  }
  else {

    free(compressor_info);
    compressor_info = (ICINFO *) calloc(MAXCOMPRESSORS,sizeof(ICINFO));

  }

  for(int i=0; ICInfo(ICTYPE_VIDEO, i, &compressor_info[num_compressor]); i++) {
    if (num_compressor >= MAXCOMPRESSORS)
      break;

    HIC hic;

    if (restrictVideoCodecs)
    {
      //allow only a few
      if (
        (compressor_info[num_compressor].fccHandler==mmioFOURCC('m', 's', 'v', 'c')) ||
        (compressor_info[num_compressor].fccHandler==mmioFOURCC('m', 'r', 'l', 'e')) ||
        (compressor_info[num_compressor].fccHandler==mmioFOURCC('c', 'v', 'i', 'd')) ||
        (compressor_info[num_compressor].fccHandler==mmioFOURCC('d', 'i', 'v', 'x')) )
      {
        hic = ICOpen(compressor_info[num_compressor].fccType, compressor_info[num_compressor].fccHandler, ICMODE_QUERY);
        if (hic) {
          if   (ICERR_OK==ICCompressQuery(hic, first_alpbi, NULL)) {
            ICGetInfo(hic, &compressor_info[num_compressor], sizeof(ICINFO));
            num_compressor++;
          }
          ICClose(hic);
        }
      }
    }
    else
    {
      //CamStudio still cannot handle VIFP
      if (compressor_info[num_compressor].fccHandler!=mmioFOURCC('v', 'i', 'f', 'p'))
      {
        hic = ICOpen(compressor_info[num_compressor].fccType,
            compressor_info[num_compressor].fccHandler, ICMODE_QUERY);
        if (hic) {
          if   (ICERR_OK==ICCompressQuery(hic, first_alpbi, NULL)) {
            ICGetInfo(hic, &compressor_info[num_compressor], sizeof(ICINFO));
            num_compressor ++ ;
          }
          ICClose(hic);
        }
      }
    }
  }
  FreeFrame(first_alpbi);
}

void PrintRecordInformation(){
  //Display the record information when recording
  if (gRecordState==1) {
    printf("=================================\n");
    printf("Current Frame : %d\n", nCurrFrame);
    printf("Theoretical Frame Rate  : %.2f fps\n", fRate);
    printf("Time Elasped  : %.2f sec\n", fTimeLength);
    printf("Number of Colors  : %d bits\n", nColors);
    printf("Codec  : %s\n", LPCTSTR(strCodec));
    printf("Actual Input Rate  : %.2f fps\n", fActualRate);
    printf("Dimension  : %d X %d\n", actualwidth,actualheight);
  }
}

int OptionNameEqual(string option_name, string parsed_name){
  // Remove '/' and '-' characters at the beginning of parsed_name, convert to
  // lower case, and compare with option_name.
  string::iterator start_iter = parsed_name.begin();

  if( (*start_iter) == '/'){
    parsed_name.erase(start_iter);
  } else if( (*start_iter) == '-'){
    parsed_name.erase(start_iter);
    if( (*start_iter) == '-')
      parsed_name.erase(start_iter);
  }
  transform(start_iter, parsed_name.end(), start_iter, ::tolower);

  return option_name.compare(parsed_name);
}

int ParseOptions(int argc, char *argv[]){
  vector<string> args;

  for(int i = 0; i < argc; ++i)
    args.push_back(argv[i]);

  for(vector<string>::iterator it = args.begin();
      it != args.end();
      ++it){

    if(!OptionNameEqual("codec", *it)){
      if(++it == args.end()){
        cout << "Expected integer ID of codec" << endl;
        return 0;
      }
      selected_compressor = atoi((*it).c_str());

      // Make sure we get a valid codec value. Since atoi() returns 0 on
      // failure, we need to add a check for literal "0", because 0 could in
      // fact be a valid codec.
      if(!selected_compressor && *it != "0"){
        cout << "Expected integer ID of codec: " << *it << endl;
        return 0;
      }
    } else if(!OptionNameEqual("outfile", *it)){
      if(++it == args.end()){
        cout << "Expected filename to write to (.avi)" << endl;
        return 0;
      }
      output_file = *it;
    } else if(!OptionNameEqual("seconds", *it)){
      if(++it == args.end()){
        cout << "Expected valid number of seconds to record" << endl;
        return 0;
      }
      seconds_to_record = atoi((*it).c_str());

      // 0 is not a valid number of seconds to record
      if(!seconds_to_record){
        cout << "Expected valid number of seconds to record: " << *it << endl;
        return 0;
      }
    } else if(!OptionNameEqual("fps", *it)){
	  if(++it == args.end()){
        cout << "Expected framerate" << endl;
        return 0;
      }
      frames_per_second = atoi((*it).c_str());
	  // Set to one key frame per second
	  keyFramesEvery = frames_per_second;
	  timelapse = 1000 / frames_per_second;

      // framerate should be positive
	  if(frames_per_second <= 0){
        cout << "Framerate should be positive: " << *it << endl;
        return 0;
      }
	} else if(!OptionNameEqual("help", *it)){
      PrintUsage(TRUE);
      return 0;
    }
  }

  return 1;
}

void PrintUsage(bool showCodecs = 1){
  cout << "Usage:" << endl << endl;
  cout << "-codec: which codec to use" << endl
       << "-outfile: .avi file to write to" << endl
       << "-seconds: how many seconds to record for ('0' means to record until "
       << "a key is pressed)" << endl
	   << "-fps: framerate to record with" << endl
       << "-help: this screen" << endl;

  if(!showCodecs)
    return;

  cout << endl << "Valid codecs on this machine:" << endl;
  for(int i = 0; i < num_compressor; ++i){
    string s = GetCodecDescription(compressor_info[i].fccHandler);

    cout << i << ": " << s << endl;
  }
}

int ChooseBestCodec(){
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

  vector<string> best_compressors;
  best_compressors.push_back("camstudio lossless");
  best_compressors.push_back("microsoft video 1");
  best_compressors.push_back("cinepak codec");
  best_compressors.push_back("indeo");

  vector<string>::iterator best = best_compressors.begin();
  for(; best != best_compressors.end(); ++best){
    for(int i = 0; i < num_compressor; ++i){
      string s = GetCodecDescription(compressor_info[i].fccHandler);
      string lower = s;
      transform(lower.begin(), lower.end(), lower.begin(), ::tolower);

      if(lower.find(*best) != string::npos){
        return i;
      }
    }
  }

  // None of the best codecs were found
  return -1;
}

int main(int argc, char* argv[])
{
  DWORD tid = 0;
  DWORD exitcode = 0;
  HDC hScreenDC;
  int mon_count = GetSystemMetrics(SM_CMONITORS); // get nubmer of monitors

  HANDLE* th = (HANDLE *) malloc(sizeof(HANDLE) * mon_count);
  //PAVIFILE* pfile = (PAVIFILE *) malloc(sizeof(PAVIFILE) * mon_count);

  cout << "camstudio_cl: Command line screen recording" << endl;
  cout << "v" << CAMSTUDIO_CL_VERSION << endl << endl;

  VideoCodecOptions();

  if(!ParseOptions(argc, argv)){
    return 1;
  }
  
  if(!output_file.length()){
    cout << "Error: Need a valid output file, -outfile" << endl;
    PrintUsage();
    return 1;
  }
  if(selected_compressor >= num_compressor){
    cout << "Error: Need a valid codec, -codec" << endl;
    PrintUsage();
    return 1;
  }

  if(selected_compressor == -1){
    selected_compressor = ChooseBestCodec();

    if(selected_compressor == -1){
      cout << "Error: No useable codec was found on this machine" << endl;
      return 1;
    }
  }

  compfccHandler = compressor_info[selected_compressor].fccHandler;
  strCodec = CString(compressor_info[selected_compressor].szDescription);

  cout << "Using codec: " << strCodec.GetBuffer() << endl;

  // Screen metrics:
  hScreenDC = ::GetDC(NULL);
  maxxScreen = GetDeviceCaps(hScreenDC,HORZRES);
  maxyScreen = GetDeviceCaps(hScreenDC,VERTRES);
  
  screen* obr = (screen *) malloc(sizeof(screen) * mon_count);// new screen[mon_count];//array of screens(objects)
  screen* pscreen = obr;

  //Detection of screens
  cout << "Detected displays:" << endl;
  EnumDisplayMonitors(NULL, NULL, MonitorEnumProc,(LPARAM) pscreen);

  int i=0;
  char buffer[2];
  string recordHere = output_file;

	for(i=0; i<=mon_count-1; i++){
		
		// whole recording stuff goes BELOW this line.
		maxxScreen = pscreen[i].width;
		maxyScreen = pscreen[i].height;
		rcUse.left = pscreen[i].left;
		rcUse.top = pscreen[i].top;
		rcUse.right = pscreen[i].right - 1;
		rcUse.bottom = pscreen[i].bottom - 1;
		// Write AVI file.
		output_file = recordHere + "_" + (_itoa_s(i,buffer,2,10) + ".avi");// _itoa(i,buffer,10)+".avi";
		strcpy_s(pscreen[i].outFile, output_file.c_str());
		pscreen[i].index = i;

		gRecordState = 1;

		cout << "Creating recording thread for screen no.:" << i <<"..." << endl;
		cout << "Recording to: " << output_file << endl;


		th[i] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)RecordAVIThread, (LPVOID) &pscreen[i], 0, &tid);

  

		// whole recording stuff goes ABOVE this line.
		//cout<<"Sirka:" << p_obr[i].width << endl; // just some line for testing.
	}
	if(seconds_to_record > 0){
		cout << "Recording for " << seconds_to_record << " seconds" << endl;
		::Sleep(seconds_to_record * 1000);
	} else {
		cout << "Enter any key to stop recording..." << endl;
		cout.flush();
		getchar();
	}
	// Set gRecordState to 0, to end the while loop inside the recording thread.
	// TODO(dimator): Maybe some better IPC, other than a global variable???
	gRecordState = 0;
	
	free(obr);
	Sleep(1000);
 

   
  //rcUse.left = offset_left;
  //rcUse.top = offset_top;
  //rcUse.right = 300;
  //rcUse.bottom = 400;
  //if(offset_bottom){
	 // rcUse.bottom = offset_bottom - 1;
  //}
  //else{
	 // rcUse.bottom = maxyScreen - 1;
  //}
  //if(offset_right){
	 // rcUse.right = offset_right - 1;
  //}
  //else{
	 // rcUse.right = maxxScreen - 1;
  //}
  ////cout << "Offset L:" << rcUse.left << endl;
  ////cout << "Offset T:" << rcUse.top << endl;
  ////cout << "Offset R:" << rcUse.right << endl; 
  ////cout << "Offset B:" << rcUse.bottom << endl;
  //
  //gRecordState = 1;

  //cout << "Creating recording thread..." << endl;
  //cout << "Recording to: " << output_file << endl;

  //if(seconds_to_record > 0){
  //   cout << "Recording for " << seconds_to_record << " seconds" << endl;
  //  ::Sleep(seconds_to_record * 1000);
  //} else {
  //  cout << "Enter any key to stop recording..." << endl;
  //  cout.flush();
  //  getchar();
  //}

  //// Set gRecordState to 0, to end the while loop inside the recording thread.
  //// TODO(dimator): Maybe some better IPC, other than a global variable???
  //gRecordState = 0;

  cout << "Waiting for recording thread to exit..." << endl;
  if(WaitForSingleObject(th[0], 5000) == WAIT_TIMEOUT){
    cout << "Error: Timed out!  The recorded video might have errors." << endl;
    cout << "Killing thread..." << endl;
    TerminateThread(th[0], exitcode);
  }

  // Make sure the thread ended gracefully
  GetExitCodeThread(th[0], &exitcode);
  if(exitcode){
    cout << "Error: Recording thread ended abnormally" << endl;
    return exitcode;
  }

  return 0;
}
