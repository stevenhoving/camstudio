// playplusView.cpp : implementation of the CPlayplusView class
//
// Warning : conversion of audio files does not delete the orginal and also possibly the converted file! So it is best
// to keep these files in temp directroy
/////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "playplus.h"

#include "playplusDoc.h"
#include "playplusView.h"
#include "MainFrm.h"

#include "MessageWnd.h"
#include "ProgressDlg.h"

#include <windowsx.h>
#include "muldiv32.h"
#include <vfw.h>

#include "fister/soundfile.h"
#include "AudioFormat.h"

//#include "./Conversion/WAVEIO.H"
#include "Conversion.h"
#include "Picture.h"

#include <fstream>
#include <sstream>
#include <vector>
#include <afxtempl.h>
#include <shlobj.h>
#include <cstdint>
#include <stdexcept>



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// OBSOLETE: not referenced
// CTransparentWnd transWnd;
// int transCreated  = 0;
// CProgressDlg progDlg;
// int progressCreated  = 0;

// Program Mode -- to be set at compile time
#define PLAYER 0
#define DUBBER 1
int pmode = DUBBER;
// int pmode = PLAYER;

// Ver 1.0
#define GlobalSizePtr(lp) GlobalSize(GlobalPtrHandle(lp))
#define LPPAVIFILE PAVIFILE *
typedef BYTE *HPBYTE;
typedef UNALIGNED short *HPSHORT;

extern BOOL CALLBACK aviaudioPlay(HWND hwnd, PAVISTREAM pavi, LONG lStart, LONG lEnd, BOOL fWait);
extern void CALLBACK aviaudioMessage(HWND, UINT, WPARAM, LPARAM);
extern void CALLBACK aviaudioStop(void);
extern LONG CALLBACK aviaudioTime(void);

BOOL CALLBACK DlgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
static BOOL gfDefDlgEx = FALSE; // the recursion flag for message crackers

#define BUFSIZE 260
static char gszBuffer[BUFSIZE];
static char gszExtBuffer[BUFSIZE];
static char gszFileName[BUFSIZE];
static char gszSaveFileName[BUFSIZE];
static char gszFileTitle[BUFSIZE];

#define AVI_EDIT_CLASS "edit"
static LPAVISTREAMINFO glpavisi;
static int gnSel;
int gSelectedStream = -1; // Highlight this text area when painting
RECT grcSelectedStream;   // where to highlight

char gszFilter[512]; // for AVIBuildFilter - more than one string!
HINSTANCE ghInstApp;
static HACCEL ghAccel;
static WNDPROC gOldEditProc;
static HWND ghwndEdit;

#define MAXTIMERANGE 30000
#define MAXNUMSTREAMS 25

int gcpavi;                                     // # of streams
PAVISTREAM gapavi[MAXNUMSTREAMS];               // the current streams
int gcpaviSel;                                  // num of edit streams
PAVISTREAM gapaviSel[MAXNUMSTREAMS];            // edit streams to put on clipbd
int gStreamTop[MAXNUMSTREAMS + 1];              // y position of each stream
AVICOMPRESSOPTIONS gaAVIOptions[MAXNUMSTREAMS]; // compression options

LPAVICOMPRESSOPTIONS galpAVIOptions[MAXNUMSTREAMS];
PGETFRAME gapgf[MAXNUMSTREAMS]; // data for decompressing video
HDRAWDIB ghdd[MAXNUMSTREAMS];   // drawdib handles
LONG galSelStart[MAXNUMSTREAMS];
LONG galSelLen[MAXNUMSTREAMS];
int giFirstAudio = -1; // 1st audio stream found
int giFirstVideo = -1; // 1st video stream found

#define gfVideoFound (giFirstVideo >= 0)
#define gfAudioFound (giFirstAudio >= 0)

BOOL gfPlaying = FALSE; // are we currently playing?
LONG glPlayStartTime;   // When did we start playing?
LONG glPlayStartPos;    // Where were we on the scrollbar?
LONG timeStart;         // cached start, end, length
LONG timeEnd;
LONG timeLength;
LONG timehscroll; // how much arrows scroll HORZ bar
int nVertSBLen;   // vertical scroll bar
int nHorzSBLen;   // horizontal scroll bar
int nVertHeight;
int nHorzWidth;
DWORD gdwMicroSecPerPixel = 1000L; // scale for video
WORD gwZoom = 4;                   // one-half zoom (divide by 4)
HWND ghwndMCI;

// buffer for wave data
//LPVOID lpAudio;

// constants for painting
#define VSPACE 8       // some vertical spacing
#define SELECTVSPACE 4 // height of selection line
#define HSPACE 0       // space between frames for video stream
#define TSPACE 10      // space for text area about each stream
#define AUDIOVSPACE 64 // height of an audio stream at X1 zoom
#define HIGHLIGHT (GetSysColor(COLOR_HIGHLIGHT) ? GetSysColor(COLOR_HIGHLIGHT) : GetSysColor(COLOR_ACTIVECAPTION))

void FrameVideo(HDC hdc, RECT *rcFrame, HBRUSH hbr);
int gfWait = 0;

// ver 1.1
long playtime = 0;

long GetScrollTime();
void SetScrollTime(long time);

// ver 1.1
#define WM_USER_PLAY 0x00401
#define BUFFER_LENGTH 256

void GetImageDimension(RECT &rcFrame);
char playfiledir[300];
char seps[] = "*";
void OpenMovieFileInit(char *filename);
void ResizeToMovie(BOOL useDefault = FALSE);
void SetTitleBar(CString title);

BOOL playerAlive = TRUE;
UINT PlayAVIThread(LPVOID pParam);

#define MENU_OPEN 11
#define MENU_MERGE 17

int ErrMsg(LPSTR sz, ...);
void FreeDrawStuff();
void FileStop(int mode);
void FixWindowTitle();
void FixScrollbars();
void InitStreams();
void InitAvi(LPSTR szFile, int nMenu);
void InitAvi(LPSTR szFile, int nMenu, long starttime, int keepcounter, int overwriteaudio, int resetslider);
void FreeAvi();
void PlayMovie(int mode);
void StopPlayingRecording();

int maxxScreen = 800;
int maxyScreen = 600;

int doneOnce = 0;

HWND viewWnd = NULL;
HWND mainWnd = NULL;

HBITMAP hLogoBM = NULL;

int nColors = 24;
int gfRecording = 0;
#define SOUND_MODE 0
#define SILENT_MODE 1

CSliderCtrl *sliderCtrlPtr;
CStatusBar *statusbarCtrl;

void SetTimeIndicator(CString timestr);
int infoLineHeight = 20;

/////////////////////////////////////////////
// Merging Module
/////////////////////////////////////////////

void NukeAVIStream(int i);
int Merge_Video_And_Sound_File(CString input_video_path, CString input_audio_path, CString output_avi_path,
                               BOOL recompress_audio, LPWAVEFORMATEX audio_recompress_format, DWORD audio_format_size,
                               BOOL bInterleave, int interleave_factor);
BOOL CALLBACK SaveCallback(int iProgress);
BOOL WinYield(void);
CString GetProgPath();

#define NUMSTREAMS 2

CString savedir("");
CString cursordir("");
CString videofilepath("");

// Path to temporary wav file
CString tempaudiopath;
CString tempsilencepath;
int recordaudio = 1;

// Audio Recording Variables
UINT AudioDeviceID = WAVE_MAPPER;

HWAVEIN m_hRecord;
WAVEFORMATEX m_Format;
DWORD m_ThreadID;
int m_QueuedBuffers = 0;
int m_BufferSize = 1000; // number of samples

#define WM_USER_GENERIC 0x00401

// Audio Options Dialog
LPWAVEFORMATEX pwfx = NULL;
DWORD cbwfx;

// Audio Formats Dialog
DWORD waveinselected = WAVE_FORMAT_2S16;
int audio_bits_per_sample = 16;
int audio_num_channels = 2;
int audio_samples_per_seconds = 22050;
BOOL bAudioCompression = TRUE;

#define MILLISECONDS 0
#define FRAMES 1
BOOL interleaveFrames = TRUE;
int interleaveFactor = 100;
int interleaveUnit = MILLISECONDS;

/////////////////////////////////////////////
// Audio Recording Module
/////////////////////////////////////////////

#if !defined(WAVE_FORMAT_MPEGLAYER3)
#define WAVE_FORMAT_MPEGLAYER3 0x0055
#endif

void InsertAVIFile(PAVIFILE pfile, LPSTR lpszFile, long starttime, int keepcounter, int overwriteaudio,
                   int resetslider);
void InsertAVIFile(PAVIFILE pfile, LPSTR lpszFile, long starttime, int keepcounter, int overwriteaudio);

CSoundFile *m_pFile = NULL;
CSoundFile *m_pSilenceFile = NULL;
BOOL silenceFileValid = FALSE;

BOOL StartAudioRecording(WAVEFORMATEX *format);
void waveInErrorMsg(MMRESULT result, const char *);
int AddInputBufferToQueue();
void SetBufferSize(int NumberOfSamples);
void CALLBACK OnMM_WIM_DATA(UINT parm1, LONG parm2);
void CALLBACK OnMM_WOM_DATA(UINT parm1, LONG parm2);
void DataFromSoundIn(CBuffer *buffer);
void StopAudioRecording();
BOOL InitAudioRecording();
void ClearAudioFile();
void GetTempWavePath();
CString GetTempPath();

#define BasicBufSize 32768
BOOL CreateSilenceFile();
void ClearAudioSilenceFile();
void GetSilenceWavePath();
void WriteSilenceFile(CBuffer *buffer);

void RemoveExistingAudioTracks();
void AddAudioWaveFile(char *FileName);

void BuildRecordingFormat();
void SuggestCompressFormat();
void SuggestRecordingFormat();
void AllocCompressFormat();

void TimeDependentInsert(CString filename, long shiftTime);

int recordstate = 0;
int recordpaused = 0;

int audioRecordable = 0;
int audioPlayable = 0;

BOOL endAudioRecording = FALSE;
BOOL initAudioRecording = FALSE;

long glRecordStartTimeValue = 0;

#define RESETCOUNTER 0
#define KEEPCOUNTER 1

#define NEW_AUDIO_TRACK 0
#define OVERWRITE_AUDIO 1

#define NO_RESET_SLIDER 0
#define RESET_TO_START 1
#define RESET_TO_CURRENT 1

int EditStreamReplace(PAVISTREAM pavi, LONG *plPos, LONG *plLength, PAVISTREAM pstream, LONG lStart, LONG lLength);
int EditStreamPadSilence(PAVISTREAM pavi, LONG *plPos, LONG *plLength);
long ExAVIStreamTimeToSample(PAVISTREAM pavi, LONG lTime);
int EditStreamSilenceShift(PAVISTREAM pavi, LONG *plPos, LONG *plLength);
long SafeStreamTimeToSample(PAVISTREAM pavi, LONG starttime);
void RecomputeStreamsTime(int resetslider, long timeCurrent);

void TestConvertFirstAudio();
void CloneAudioStream_ReplaceStreamPool(int i, PAVISTREAM pavi);
void ReInitAudioStream(int i);

void SetAdditionalCompressSettings(BOOL recompress_audio, LPWAVEFORMATEX audio_recompress_format,
                                   DWORD audio_format_size, BOOL bInterleave, int interleave_factor,
                                   int interleave_unit);
int TestConvert(PAVISTREAM pavi, PAVISTREAM *paviConverted, PAVISTREAM paviDstFormat);
LPWAVEFORMATEX allocRetrieveAudioFormat(PAVISTREAM pavi);

void SetDurationLine();
void SetRPSLine();

int allowRecordExtension = 1;

int fileModified = 0;

void Msg(const char fmt[], ...);
void DumpFormat(WAVEFORMATEX *pwfx, const char *str);

CString GetTempPathEx(CString fileName, CString fxstr, CString exstr);
CString ConvertFileToWorkingPCM(CString filename);

////////////////////////////////////
// Ver 2.0 Flash Converter
////////////////////////////////////
//
// Terminology
// -----------
// Keyframe -- a Full Frame is saved ;  all intermediate layers, and halfkey layers on top are cleared
// Intermediate Frame - a frame at layer 100, on top of Keyframe and halfkeys. This frame is never used as a basis for
// comparison with the next frame HalfKey  -- Frames that are saved as a difference to the previous frame, these frames
// are used as a comparison for the next frame, the difference are layered on top of the keyframe. Multiple layers are
// stacked ... up to a depth of Max_HalfkeyDepth
//
// Usage :
// 1) useHalfKey = 1, usePercent = 1, set HalfKeyThreshold to decide when  a frame is a keyframe and when it is a
// halfkey
//    in both cases, the frame returned is saved for comparison with the next frame
//    The  HalfKeyThreshold is the percentage of change in the new frame that cause the program to decide the frame
//    should be saved as a keyframe instead of halfkey No intermediate frames are used
//
// 2) useHalfKey = 1, usePercent = 0, all frames are halfkey...until the Max_HalfKeyDepth is reached...then a key frame
// is stored
//    the frame returned is saved for comparison with the next frame
//    No intermediate frames are used
//
// 3) useHalfKey = 0, usePercent = 0, set sampleFPS
//    No halfkey are used.  Key frames occurs at a fixed frequency ... e.g every 20 fps
//    Otherwise, the frames are intermediate, i.e a single layer on top of the key...this layer is removed and a new
//    layer added when the next intermediate frame is shown
//
// 4) useHalfKey = 0, usePercent = 1,  set PercentThreshold, sampleFPS
//    Key frames usually occurs at a fixed frequency, but if the difference is too big in the current frame  (compared
//    with the keyframe), the keyframe is updated
//
// I) The  expandArea switch affects all three modes above
//    If expandArea==1, the difference blocks will be slightly enlarged (by expandThickness, measured in no. of pixels)
//    This cuase the zooming of the flash movie to appears slightly better
//
// Remarks
// Method 1) buggy ?
// Method 2) gives the best compression, but zooming it cause problems
// Method 4) is good for zooming (when used with expandArea)-- although not perfect. and gives decent compression
// Method 3) is good for zooming (when used with expandArea)-- although not perfect. but gives lousy compression
// Te expandArea switch seems to increase the file size only slightly
//
// ********************

void ConvertToFlash(long currentTime, HDC hdc, HBITMAP hbm, std::ostringstream &f);
void PaintSwfFrame(HDC hdc, HBITMAP hbm, RECT rcFrame, LPBITMAPINFOHEADER lpbi, int iStream, std::ostringstream &f);
LPBYTE makeReverse16(int width, int height, int bitPlanes, LPBITMAPINFOHEADER alpbi);
LPBYTE makeReverse32(int width, int height, int bitPlanes, LPBITMAPINFOHEADER alpbi);
LPBITMAPINFOHEADER GetFrame(LPBYTE *bits, int &BITMAP_X, int &BITMAP_Y, int numbits);

HANDLE Bitmap2Dib(HBITMAP hbitmap, UINT bits);
LPBITMAPINFOHEADER GetFrame(HBITMAP hbm, LPBYTE *bits, int &BITMAP_X, int &BITMAP_Y, int numbits);
void FreeFrame(LPBITMAPINFOHEADER alpbi);
void WriteSwfFrame(LPBITMAPINFOHEADER alpbi, std::ostringstream &f, LPBYTE bitmap, int BITMAP_X, int BITMAP_Y,
                   int format);
LPBYTE swapPixelBytes16(int width, int height, int bitPlanes, LPBYTE bits16);

LPBITMAPINFOHEADER currentKey_lpbi = NULL;
int ProcessSwfFrame(LPBITMAPINFOHEADER alpbi, std::ostringstream &f, LPBYTE bitmap, int BITMAP_X, int BITMAP_Y,
                    int format);
void finishTemporalCompress();
void initTemporalCompress(int bmWidth, int bmHeight);
void cleanChangeArray();

#define FRAME_KEYFRAME 0
#define FRAME_INTERMEDIATE 1
#define FRAME_HALFKEY 2

int CopyStream(PAVIFILE pavi, PAVISTREAM pstm);
PAVISTREAM ConvertFirstAudioStream(LPWAVEFORMATEX lpFormat);
void cleanTempFile();
#define WAVBUFFER 4096

class CChangeRectSwf
{
public:
    CChangeRectSwf();
    virtual ~CChangeRectSwf();

    int initialize(int blockx, int blocky, int blocksizex, int blocksizey, int blockwidth, int pixelwidth,
                   int pixelheight, int x, int y);

public:
    int blockx;     // measured in blocks
    int blocky;     // measured in blocks
    int blockwidth; // measured in blocks

    int pixelwidth;     // measured in pixels
    int pixelheight;    // measured in pixels
    int blocksizex;     // measured in pixels
    int blocksizey;     // measured in pixels
    CRect blockbounds;  // measured in pixels
    int greatestLeft;   // measured in pixels
    int greatestTop;    // measured in pixels
    int smallestRight;  // measured in pixels
    int smallestBottom; // measured in pixels
};

CChangeRectSwf::CChangeRectSwf()
{
    blockx = -1;
    blocky = -1;
}

CChangeRectSwf::~CChangeRectSwf()
{
}

// *********************************************************
// Code Derived From Chris Losinger's JpegFile Class
// *********************************************************
BOOL VertFlipBuf(BYTE *inbuf, UINT widthBytes, UINT height);

BOOL ARGBFromBGRA(BYTE *buf, UINT widthPix, UINT height);

// ver 2.21
int sampletimeInc = 1;
int sampleFPS = 40;
int convertBits = 16;

int noLoop = 0;
int noUrl = 0;
int noAutoPlay = 0;
int addControls = 1;
int controlsWidth = 100;
int controlsHeight = 25;
int firstvideoFrame = 1;

// ver 2.22
// Temporal Compression
int framecount = 0;
int framei = 0;
int keyframerate = 20;

int blocksize_x = 64;
int blocksize_y = 64;
int numblocks_x = 0;
int numblocks_y = 0;

int FrameOffsetX = 0;
int FrameOffsetY = 0;

// Using thse seems to give lesser errors..
int MatrixOffsetX = 0;
int MatrixOffsetY = 0;
int MoveOffsetX = 1;
int MoveOffsetY = 1;

int hasKeyFrame = 0;
int hasIntermediateFrame = 0;
int KeyFrameDepth = 100;
int HalfKeyDepthBase = 200;
int HalfKeyDepthInc = 0;
int IFrameDepth = 300;
int ObjectDepth = 0;

// Some predefined depths
// int LoadingDepth =  ObjectDepth + 6;
// int ProgressbarDepth =  ObjectDepth + 5;
// int playButtonDepth = ObjectDepth + 2;
// int pauseButtonDepth = ObjectDepth + 2;
// int stopButtonDepth = ObjectDepth + 2;
// int BitmapplayButtonDepth = ObjectDepth + 2;
// int BitmappauseButtonDepth = ObjectDepth + 2;
// int BitmapstopButtonDepth = ObjectDepth + 2;
// int BackgroundBarDepth = ObjectDepth + 1;
// int writeTextDepth = ObjectDepth;

int expandArea = 0; // does not seems to expand the size too much
int expandThickness = 2;

double PercentThreshold = 33;
double HalfKeyThreshold = 45;
int Max_HalfKeyDepth = 15; // maximum 90 .... playback will be SSSSSLOW for large values
int usePercent = 0;
int useHalfKey = 1;

// ver 2.23
// MP3 Background music
CString mp3File;
int mp3FileLoaded = 0;
int mp3volume = 26000; // 0 to 32768

// ver 2.24
// **************************************
// Conversion to PCM
// **************************************

int swf_bits_per_sample = 16;
int swf_samples_per_seconds = 22050;
int swf_num_channels = 2;
int usePCMConvertedStream = 0;

PAVISTREAM PCMConvertedStream = NULL;
PAVIFILE PCMConvertedFile = NULL;

CString tempfile1("");
CString tempfile2("");

// ver 2.24
// produce HTML preview
void produceFlashHTML(CString htmlfilename, CString flashfilename, CString flashfilepath, int onlyflashtag, int width,
                      int height, int bk_red, int bk_green, int bk_blue);

// *****************************
// Audio Compression
// *****************************
int useMP3 = 0;
int useAudio = 1;
int noAudioStream = 0;
int useAudioCompression = 1; // 0 : not compression, 1 : ADPCM ,  2 : MP3 (not yet)
// ADPCM conversion seems to work only on 16 bit PCM wave data .. 8 bit ....don't work

char play_rate;
bool play_16bit;
bool play_stereo;
char compression;
char stream_rate;
bool stream_16bit;
bool stream_stereo;
uint16_t samplecountavg;

long slCurrentSwf = 0;

int adpcmBPS = 5; // 2 to 5
void MakeSoundStreamBlockADPCM(void *buffer, int buffersize, int numsamples, std::ostringstream &f);
void *MakeFullBuffer(void *buffer, long &buffersize, long &numsamples, int avgsamplespersecond, bool streamstereo,
                     bool stream16bit);

int launchPropPrompt = 1;
int launchHTMLPlayer = 1;
int deleteAVIAfterUse = 0;

CString swfname;
CString swfhtmlname;
CString swfbasename;
int onlyflashtag;
CString avifilename;
CString urlRedirect;

int swfbk_red = 255;
int swfbk_green = 255;
int swfbk_blue = 255;

int swfbar_red = 0;
int swfbar_green = 0;
int swfbar_blue = 255;

int runmode = 0; // 0 -- window mode, 1 -- CamStudio recorder internal mode, 2 --  batch mode, silence
int oldPercent = 0;

void LoadSettings();
void SaveSettings();
void LoadCommand();

// ver 2.25  Interfaces
int convertMode = 0;

int MessageOutINT(HWND hWnd, long strMsg, long strTitle, UINT mbstatus, long val);
int MessageOutINT2(HWND hWnd, long strMsg, long strTitle, UINT mbstatus, long val1, long val2);
int MessageOut(HWND hWnd, long strMsg, long strTitle, UINT mbstatus);

void AdjustOutName(CString avioutpath);

int CreateFlashBitmapPlayButton(std::ostringstream &f, int imagewidth, int imageheight, CString subdir,
                                int imageoffset);
int CreateFlashBitmapPauseButton(std::ostringstream &f, int imagewidth, int imageheight, CString subdir,
                                 int imageoffset);
int CreateFlashBitmapStopButton(std::ostringstream &f, int imagewidth, int imageheight, CString subdir,
                                int imageoffset);
int DrawRightPiece(std::ostringstream &f, int imagewidth, int imageheight, CString subdir, int imageoffset,
                   int yoffset);
int DrawLeftPiece(std::ostringstream &f, int imagewidth, int imageheight, CString subdir, int imageoffset, int yoffset);
int DrawLoading(std::ostringstream &f, int imagewidth, int imageheight, CString subdir, int imageoffset, int yoffset);

int ButtonSpaceX = 0;
int ButtonSpaceY = 7;
int PieceOffsetY = 2;
int ProgressOffsetY = 2;
int yes_drawLeftPiece = 1;
int yes_drawRightPiece = 1;
int yes_drawStopButton = 1;
int ControllerWidth = 220;
int ControllerAlignment = 1;

int controlsType = 2;
int bitmapBarHeight = 19;

// ver 2.27 free character
CArray<int, int> freeCharacterArray;
void gcFlash(std::ostringstream &f);

#define MAXFLASHLIMIT 15600

// intenral var
int needbreakapart = 0;
int breakcycle = 0;
int moreSWFsneeded = 0;
int filesAreSplit = 0;
int preloadFrames = 2;

// external var
int allowChaining = 1;
int freecharacter = 0;
double percentLoadedThreshold = 0.7;
int addPreloader = 1;
int applyPreloaderToSplitFiles = 1;
int produceRaw = 0;

// ver 2.28 preloader / actionscripts
#define _X 0
#define _Y 1
#define _xscale 2
#define _yscale 3
#define _currentframe 4
#define _totalframes 5
#define _alpha 6
#define _visible 7
#define _width 8
#define _height 9
#define _rotation 10
#define _target 11
#define _framesloaded 12
#define _name 13
#define _droptarget 14
#define _url 15
#define _highquality 16
#define _focusrect 17
#define _soundbuftime 18
#define _quality 19
#define _xmouse 20
#define _ymouse 21

int CreateProgressBar(std::ostringstream &f, int controlsWidth, int controlsHeight, int FrameOffsetX, int FrameOffsetY,
                      int BITMAP_X, int BITMAP_Y, int additonalOffsetX, int additionalOffsetY);
void FlashActionGetPropertyVar(std::ostringstream &f, CString SpriteTarget, int index, CString varname);
void FlashActionSetPropertyFloat(std::ostringstream &f, CString SpriteTarget, int index, CString valuestr);
void FlashActionSetPropertyFloatVar(std::ostringstream &f, CString SpriteTarget, int index, CString varstr);
void WriteTextOut(std::ostringstream &f, int width, int height, CString Loadstr, CString fontstr, int red, int green,
                  int blue, int pointsize, bool bold, bool italic, bool uLine);
void Preloader(std::ostringstream &f, int widthBar, int bmWidth, int bmHeight, int progressOffset);
void actionLoadBaseMovie(std::ostringstream &f);
void GetBounds(const char *fontname, CString textstr, int pointsize, CSize &retExtent, bool bold = false,
               bool italic = false, bool uLine = false);
int DrawNodes(std::ostringstream &f, int widthBar, int imagewidth, int imageheight, CString subdir, int imageoffset,
              int yoffset, int additionalOffsetX, int additionalOffsetY);

// CString fontname("Times New Roman");
CString fontname("Arial");
int font_red = 40;
int font_green = 135;
int font_blue = 175;
int font_pointsize = 16;
bool font_bold = false;
bool font_italic = false;
bool font_uLine = false;

CString loadingPath;

void LoadController();
void SaveController();

/////////////////////////////////////////////////////////////////////////////
// CPlayplusView

IMPLEMENT_DYNCREATE(CPlayplusView, CView)

BEGIN_MESSAGE_MAP(CPlayplusView, CView)
//{{AFX_MSG_MAP(CPlayplusView)
ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
ON_COMMAND(ID_FILE_CLOSE, OnFileClose)
ON_COMMAND(ID_FILE_PLAY, OnFilePlay)
ON_COMMAND(ID_FILE_REWIND, OnFileRewind)
ON_COMMAND(ID_FILE_STOP, OnFileStop)
ON_WM_CREATE()
ON_WM_DESTROY()
ON_WM_PAINT()
ON_WM_SIZE()
ON_COMMAND(ID_FILE_LASTFRAME, OnFileLastframe)
ON_WM_ERASEBKGND()
ON_WM_HSCROLL()
ON_WM_VSCROLL()
ON_COMMAND(ID_ZOOM_RESIZETOMOVIESIZE, OnZoomResizetomoviesize)
ON_COMMAND(ID_ZOOM_QUARTER, OnZoomQuarter)
ON_COMMAND(ID_ZOOM_HALF, OnZoomHalf)
ON_COMMAND(ID_ZOOM_1, OnZoom1)
ON_COMMAND(ID_ZOOM_2, OnZoom2)
ON_COMMAND(ID_ZOOM_4, OnZoom4)
ON_UPDATE_COMMAND_UI(ID_ZOOM_1, OnUpdateZoom1)
ON_UPDATE_COMMAND_UI(ID_ZOOM_2, OnUpdateZoom2)
ON_UPDATE_COMMAND_UI(ID_ZOOM_4, OnUpdateZoom4)
ON_UPDATE_COMMAND_UI(ID_ZOOM_HALF, OnUpdateZoomHalf)
ON_UPDATE_COMMAND_UI(ID_ZOOM_QUARTER, OnUpdateZoomQuarter)
ON_UPDATE_COMMAND_UI(ID_ZOOM_RESIZETOMOVIESIZE, OnUpdateZoomResizetomoviesize)
ON_UPDATE_COMMAND_UI(ID_FILE_REWIND, OnUpdateFileRewind)
ON_UPDATE_COMMAND_UI(ID_FILE_LASTFRAME, OnUpdateFileLastframe)
ON_COMMAND(ID_BUTTON32785, OnButtonRecord)
ON_UPDATE_COMMAND_UI(ID_BUTTON32785, OnUpdateRecord)
ON_COMMAND(ID_AUDIO_REMOVEEXISTINGAUDIOTRACKS, OnAudioRemoveexistingaudiotracks)
ON_COMMAND(ID_AUDIO_ADDAUDIOFROMWAVEFILE, OnAudioAddaudiofromwavefile)
ON_UPDATE_COMMAND_UI(ID_AUDIO_REMOVEEXISTINGAUDIOTRACKS, OnUpdateAudioRemoveexistingaudiotracks)
ON_UPDATE_COMMAND_UI(ID_AUDIO_ADDAUDIOFROMWAVEFILE, OnUpdateAudioAddaudiofromwavefile)
ON_COMMAND(ID_FILE_SAVEAS, OnFileSaveas)
ON_COMMAND(ID_AUDIO_AUDIOOPTIONS, OnAudioAudiooptions)
ON_COMMAND(ID_ZOOM_TESTAUDIO, OnZoomTestaudio)
ON_UPDATE_COMMAND_UI(ID_FILE_SAVEAS, OnUpdateFileSaveas)
ON_UPDATE_COMMAND_UI(ID_FILE_PLAY, OnUpdateFilePlay)
ON_COMMAND(ID_AUDIO_EXTENSION, OnAudioExtension)
ON_UPDATE_COMMAND_UI(ID_AUDIO_EXTENSION, OnUpdateAudioExtension)
ON_COMMAND(ID_CONVERT, OnConvert)
ON_COMMAND(ID_HELP_HELP, OnHelpHelp)
//}}AFX_MSG_MAP
// Standard printing commands
ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
// ON_MESSAGE (WM_USER_GENERIC, OnUserGeneric) C2440 error
ON_MESSAGE(MM_WIM_DATA, OnMM_WIM_DATA)
ON_MESSAGE(MM_WOM_DONE, OnMM_WOM_DONE)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPlayplusView construction/destruction

CPlayplusView::CPlayplusView()
{
    // TODO: add construction code here
}

CPlayplusView::~CPlayplusView()
{
}

BOOL CPlayplusView::PreCreateWindow(CREATESTRUCT &cs)
{
    // TODO: Modify the Window class or styles here by modifying
    //  the CREATESTRUCT cs

    return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CPlayplusView drawing

void CPlayplusView::OnDraw(CDC * /*pDC*/)
{
    CPlayplusDoc *pDoc = GetDocument();
    ASSERT_VALID(pDoc);
    // TODO: add draw code for native data here
}

/////////////////////////////////////////////////////////////////////////////
// CPlayplusView printing

BOOL CPlayplusView::OnPreparePrinting(CPrintInfo *pInfo)
{
    // default preparation
    return DoPreparePrinting(pInfo);
}

void CPlayplusView::OnBeginPrinting(CDC * /*pDC*/, CPrintInfo * /*pInfo*/)
{
    // TODO: add extra initialization before printing
}

void CPlayplusView::OnEndPrinting(CDC * /*pDC*/, CPrintInfo * /*pInfo*/)
{
    // TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// CPlayplusView diagnostics

#ifdef _DEBUG
void CPlayplusView::AssertValid() const
{
    CView::AssertValid();
}

void CPlayplusView::Dump(CDumpContext &dc) const
{
    CView::Dump(dc);
}

CPlayplusDoc *CPlayplusView::GetDocument() // non-debug version is inline
{
    ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CPlayplusDoc)));
    return (CPlayplusDoc *)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CPlayplusView message handlers

void CPlayplusView::OnFileOpen()
{
    OPENFILENAME ofn;
    gszFileName[0] = 0;
    gszFileTitle[0] = 0;

    if (convertMode == 1)
        LoadString(ghInstApp, IDS_OPENTITLE2, gszBuffer, BUFSIZE);
    else
        LoadString(ghInstApp, IDS_OPENTITLE, gszBuffer, BUFSIZE);

    memset(&ofn, 0, sizeof(ofn));
    ofn.lpstrTitle = gszBuffer;
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = m_hWnd;
    ofn.lpstrFilter = "AVI Movie Files (*.avi)\0*.avi\0\0";
    ofn.lpstrFile = gszFileName;
    ofn.nMaxFile = sizeof(gszFileName);
    ofn.lpstrFileTitle = gszFileTitle;
    ofn.nMaxFileTitle = sizeof(gszFileTitle);
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    // If filename exists
    if (GetOpenFileName(&ofn))
    {
        OnFileStop();
        gwZoom = 4;
        InitAvi(gszFileName, MENU_OPEN);
        ResizeToMovie();
        AdjustOutName(gszFileName);
    }
    //...and not recording...
    // CString msx;
    // msx.Format("!gfPlaying %d  giFirstAudio %d  giFirstVideo %d",!gfPlaying,giFirstAudio,giFirstVideo);
    //::MessageBox(NULL,msx,"note", MB_OK|MB_ICONEXCLAMATION|MB_TASKMODAL);

    Invalidate();
}

void ResizeToMovie(BOOL useDefault)
{
    RECT rc;
    if (useDefault)
    {
        rc.left = 100;
        rc.top = 100;
        rc.right = rc.left + 308 - 1;
        rc.bottom = rc.top + 222 - 1;
    }
    else
    {
        GetImageDimension(rc);
    }
    if (((rc.bottom - rc.top) < 20) && ((rc.right - rc.left) < 10))
        return;

    CWnd *mainWindow = AfxGetMainWnd();
    ((CMainFrame *)mainWindow)->ResizeToMovie(rc);
}

void CPlayplusView::OnFileClose()
{
    if ((fileModified == 1) && (pmode == DUBBER))
    {
        // int ret = MessageBox("Do you want to save changes ?","Note",MB_YESNOCANCEL | MB_ICONQUESTION);
        int ret = MessageOut(this->m_hWnd, IDS_DOYOUWSC, IDS_NOTE, MB_YESNOCANCEL | MB_ICONQUESTION);

        if (ret == IDYES)
        {
            SendMessage(WM_COMMAND, ID_FILE_SAVEAS, 0);
            return;
        }
        else if (ret == IDCANCEL)
        {
            return;
        }
    }
    FreeAvi();
    fileModified = 0;

    gszFileName[0] = '\0';
    gszFileTitle[0] = '\0';
    FixWindowTitle();

    ResizeToMovie(TRUE);

    // Invalidate();
}

void CPlayplusView::OnFilePlay()
{
    PlayMovie(SOUND_MODE);
}

void PlayMovie(int mode)
{
    glPlayStartTime = timeGetTime();
    glPlayStartPos = GetScrollTime();

    // Recording sessions do not reset time
    if ((allowRecordExtension) && (pmode == DUBBER) && (mode == SILENT_MODE))
    {
        // do not reset time if this is a recording session (mode == silent) at the end of the longest stream and
        // extension is allowed
    }
    else if ((!allowRecordExtension) && (pmode == DUBBER) && (mode == SILENT_MODE))
    {
        // do not reset time also if recording sesson even if  not allowRecordExtension
    }
    else if (glPlayStartPos >= timeEnd)
    {
        // resetting time for playing sessions
        SetScrollTime(timeStart);
        glPlayStartPos = GetScrollTime();
    }
    if (mode == SOUND_MODE)
    {
        // should this be put here since there is adjustment to the scroll time
        if (gfAudioFound)
        {
            aviaudioPlay(viewWnd, gapavi[giFirstAudio], AVIStreamTimeToSample(gapavi[giFirstAudio], GetScrollTime()),
                         AVIStreamEnd(gapavi[giFirstAudio]), FALSE);
        }
    }
    // else {
    // silent mode
    //}
    gfPlaying = TRUE;
}

void CPlayplusView::OnFileRewind()
{
    // TODO: Add your command handler code here
    SetScrollTime(timeStart);
    ::InvalidateRect(viewWnd, NULL, FALSE);
    ::UpdateWindow(viewWnd);
}

void CPlayplusView::OnFileStop()
{
    StopPlayingRecording();

    SetRPSLine();
}

void StopPlayingRecording()
{
    if (gfRecording)
    {
        endAudioRecording = TRUE;
        FileStop(SILENT_MODE);
    }
    else
    {
        FileStop(SOUND_MODE);
    }
}

void PaintVideo(HDC hdc, RECT rcFrame, int iStream, LPBITMAPINFOHEADER lpbi, LONG lCurSamp, LONG lPos)
{
    if (!lpbi)
    {
        // Before or after the movie (or read error) draw GRAY
        // do nothing
        return;
    }
    // if lbpi is present, draw it
    // Added code for centering
    CRect rect;
    ::GetClientRect(viewWnd, &rect);

    int offsetx = 0;
    int offsety = 0;
    int bitmapwidth = rcFrame.right - rcFrame.left + 1;
    int bitmapheight = rcFrame.bottom - rcFrame.top + 1;

    if (rect.right > bitmapwidth)
    {
        offsetx = (rect.right - bitmapwidth) / 2;
    }
    if (rect.bottom > bitmapheight)
    {
        offsety = (rect.bottom - bitmapheight) / 2;
    }
    DrawDibDraw(ghdd[iStream], hdc, rcFrame.left + offsetx, rcFrame.top + offsety, rcFrame.right - rcFrame.left - 1,
                rcFrame.bottom - rcFrame.top - 1, lpbi, NULL, 0, 0, -1, -1,
                (iStream == giFirstVideo) ? 0 : DDF_BACKGROUNDPAL);

    // char szText[BUFSIZE];
    // int iLen = wsprintf(szText, "%ld %ld.%03lds", lCurSamp, lPos / 1000, lPos % 1000);
    TRACE("%ld %ld.%03lds\n", lCurSamp, lPos / 1000, lPos % 1000);
}

int PaintStuff(HDC hdc, HWND hwnd, BOOL fDrawEverything)
{
    int xStreamLeft;
    int yStreamTop;
    int iFrameWidth;
    LONG lSamp, lCurSamp;
    int n;
    int nFrames;
    LPBITMAPINFOHEADER lpbi = NULL;
    LONG l;
    LONG lTime;
    LONG lSize = 0;
    RECT rcFrame = {'\0'}; // Init whole struct to prevent Warning C4701: potentially uninitialized local variable
    RECT rcC;

    GetClientRect(hwnd, &rcC);

    lTime = GetScrollTime();
    yStreamTop = -GetScrollPos(hwnd, SB_VERT);
    xStreamLeft = -GetScrollPos(hwnd, SB_HORZ);

    // for all streams
    for (int i = 0; i < gcpavi; i++)
    {
        AVISTREAMINFO avis;
        LONG lEndTime;

        gStreamTop[i] = yStreamTop + GetScrollPos(hwnd, SB_VERT);
        AVIStreamInfo(gapavi[i], &avis, sizeof(avis));

        if (avis.fccType == streamtypeVIDEO)
        {
            if (gapgf[i] == NULL)
                continue;

            // Which frame belongs at this time?
            lEndTime = AVIStreamEndTime(gapavi[i]);
            if (lTime <= lEndTime)
            {
                lSamp = AVIStreamTimeToSample(gapavi[i], lTime);
            }
            else
            {
                // we've scrolled past the end of this stream
                // lSamp = AVIStreamTimeToSample(gapavi[i], AVIStreamStartTime(gapavi[i]));
                lSamp = AVIStreamTimeToSample(gapavi[i], AVIStreamStartTime(gapavi[i]) + timeLength - 1);
            }
            // ver 1.1
            iFrameWidth = (avis.rcFrame.right - avis.rcFrame.left) + HSPACE;
            nFrames = 0;

            for (n = -nFrames; n <= nFrames; n++)
            {
                if (i == giFirstVideo)
                {
                    lCurSamp = lSamp + n;
                    l = AVIStreamSampleToTime(gapavi[i], lCurSamp);
                }
                else
                { // NOT the first video stream
                    l = lTime + MulDiv32(n * (iFrameWidth + HSPACE), gdwMicroSecPerPixel, 1000);
                    lCurSamp = AVIStreamTimeToSample(gapavi[i], l);
                    l = AVIStreamSampleToTime(gapavi[i], lCurSamp);
                }
                if (gapgf[i] && lCurSamp >= AVIStreamStart(gapavi[i]))
                {
                    lpbi = (LPBITMAPINFOHEADER)AVIStreamGetFrame(gapgf[i], lCurSamp);
                }
                else
                {
                    lpbi = NULL;
                }
                // Figure out where to draw this frame
                rcFrame.left = xStreamLeft;
                rcFrame.top = yStreamTop;
                rcFrame.right = rcFrame.left + (avis.rcFrame.right - avis.rcFrame.left) * gwZoom / 4;
                rcFrame.bottom = rcFrame.top + (avis.rcFrame.bottom - avis.rcFrame.top) * gwZoom / 4;

                // Patch to prevent blank screen
                if (lpbi == NULL)
                {
                    lpbi = (LPBITMAPINFOHEADER)AVIStreamGetFrame(gapgf[i], AVIStreamEnd(gapavi[i]) - 1);
                }
                PaintVideo(hdc, rcFrame, i, lpbi, lCurSamp, l);
            }
            if (lpbi)
                AVIStreamSampleSize(gapavi[i], lSamp, &lSize);

            // ver 1.1
            yStreamTop += (rcFrame.bottom - rcFrame.top);
        } // If Video Stream

        // Give up once we're painting below the bottom of the window
        if (!fDrawEverything && yStreamTop >= rcC.bottom)
            break;
    } // for all streams

    // The bottom of all the streams;
    gStreamTop[gcpavi] = yStreamTop + GetScrollPos(hwnd, SB_VERT);

    // Return number of lines drawn
    return yStreamTop + GetScrollPos(hwnd, SB_VERT);
}

// Separation of init streams into 1)avioptions preparation 2)timestart, timeend, time length finding  3)reseting
// counter components Pre-condition : Init streams is called only in InserAVIfile..if it needed to be called in other
// places, make sure the relevant and only the revlant components are included
void InsertAVIFile(PAVIFILE pfile, LPSTR lpszFile, long starttime, int /*keepcounter*/, int overwriteaudio,
                   int resetslider)
{
    // Note: the keepcounter variable is not used
    // It's function is taken by the resetslider

    // Currently the resetslider is used only if the mode is not OVERWRITEAUDIO
    // if it is FALSE, then we force the counter not to reset
    // by default, for most cases, the reset silder is set to TRUE, especially when insertAviFile  is used to insert new
    // streams into the current stream pool

    int i;
    PAVISTREAM pavi;
    int needshift = 0;

    long timeCurrent = GetScrollTime();

    // ErrMsg("overover %d",overwriteaudio);
    if (overwriteaudio)
    {
        // Assume this case is only 1 audio track
        if (giFirstAudio >= 0)
        {
            if (AVIFileGetStream(pfile, &pavi, streamtypeAUDIO, 0) != AVIERR_OK)
            {
                ErrMsg("Unable to load audio file");
                return;
            }
            // if not PCM, if not at same audio rate, then covert to working PCM format
            // TestConvert(pavi);

            // editreplace stream

            long startsample = SafeStreamTimeToSample(gapavi[giFirstAudio], starttime);
            // startsample = AVIStreamTimeToSample( gapavi[giFirstAudio], starttime );
            // startsample=10000;

            PAVISTREAM paviConverted = NULL;
            if (startsample > -1)
            {
                int retval = TestConvert(pavi, &paviConverted, gapavi[giFirstAudio]);
                if (retval > 0)
                {
                    // Conversion is performed
                    AVIStreamRelease(pavi);
                    pavi = paviConverted;
                }
                else if (retval == 0)
                {
                    // No conversion is needed..proceed with the display
                }
                else if (retval < 0)
                {
                    // Error in the conversion, abort the Stream Replace
                    AVIStreamRelease(pavi);
                    AVIFileRelease(pfile);
                }
                long lx = AVIStreamLength(pavi);
                // if (EditStreamPaste(gapavi[giFirstAudio], &startsample, &lx, pavi, AVIStreamStart(pavi),
                // AVIStreamLength(pavi)) !=  AVIERR_OK) ErrMsg("overwrite  %d",retval); ErrMsg("EditStreamReplace
                // startsample %ld, lx %ld, pavi %ld, AVIStreamStart(pavi) %ld, AVIStreamLength(pavi) %ld",startsample,
                // lx, pavi, AVIStreamStart(pavi), AVIStreamLength(pavi));
                if (EditStreamReplace(gapavi[giFirstAudio], &startsample, &lx, pavi, AVIStreamStart(pavi),
                                      AVIStreamLength(pavi)) != AVIERR_OK)
                {
                    ErrMsg("Unable to add audio at insertion point");
                }
            }
            AVIStreamRelease(pavi);
            AVIFileRelease(pfile);

            ErrMsg("resetslider %d,timeCurrent %d", resetslider, timeCurrent);
            RecomputeStreamsTime(resetslider, timeCurrent);

            return;
        }
    }
    else
    { // if no overwrite
        // ErrMsg("giFirstAudio = %d, starttime = %d",giFirstAudio, starttime);
        if ((giFirstAudio < 0) && (starttime > 0))
        {
            // Do nothing...consider this as normal insert with no overwrite  and startime == 0
            // However need to shift time after insertion

            // Pass on
            needshift = 1;
        }
        else
        {
            // Do nothing...consider this as normal insert with no overwrite  and startime == 0
            // Pass on
        }
    }
    for (i = gcpavi; i <= MAXNUMSTREAMS; i++)
    {
        if (AVIFileGetStream(pfile, &pavi, 0L, i - gcpavi) != AVIERR_OK)
            break;

        if (i == MAXNUMSTREAMS)
        {
            AVIStreamRelease(pavi);
            LoadString(ghInstApp, IDS_MAXSTREAMS, gszBuffer, BUFSIZE);
            ErrMsg(gszBuffer);
            break;
        }
        if (CreateEditableStream(&gapavi[i], pavi) != AVIERR_OK)
        {
            AVIStreamRelease(pavi);
            break;
        }
        AVIStreamRelease(pavi);
        galSelStart[i] = galSelLen[i] = -1;
    }
    AVIFileRelease(pfile);

    if (gcpavi == i && i != MAXNUMSTREAMS)
    {
        LoadString(ghInstApp, IDS_NOOPEN, gszBuffer, BUFSIZE);

        ErrMsg(gszBuffer, lpszFile);
        return;
    }
    FreeDrawStuff();
    gcpavi = i;
    InitStreams(); // This function inherent has a RecomputeStreamsTime component

    TestConvertFirstAudio();

    // At this point, the gifirstaudio should be succesfully set
    if ((needshift) && (giFirstAudio >= 0))
    {
        // int value = 0;
        long startsample = 0;

        startsample = SafeStreamTimeToSample(gapavi[giFirstAudio], starttime);
        ErrMsg("giFirstAudio = %d,startsample = %d, starttime = %d", giFirstAudio, startsample, starttime);

        if (startsample > 0)
        {
            long startPos = startsample;
            long silenceLengthPasted = 0;
            EditStreamSilenceShift(gapavi[giFirstAudio], &startPos, &silenceLengthPasted);
        }
    }
    // This function is here mainly for the benefit of  needshift ==1
    // however, for thecase where needshift == 0, the recomputation of Streams Time does not logically affect the
    // original computation by InitStreams in this case, the recomputStreamsTime is basically for (a) setting the slider
    // value or (b) for recomputing streamstime after a conversion

    RecomputeStreamsTime(resetslider, timeCurrent);
    FixScrollbars();
    FixWindowTitle();
}

void InsertAVIFile(PAVIFILE pfile, LPSTR lpszFile, long starttime, int keepcounter, int overwriteaudio)
{
    // By default, if the resetslider is not specified, it is assumed to be TRUE
    // I.e, the slider will be reset after each InsertAVIFile
    InsertAVIFile(pfile, lpszFile, starttime, keepcounter, overwriteaudio, TRUE);
}

void FreeAvi()
{
    FreeDrawStuff();

    AVISaveOptionsFree(gcpavi, galpAVIOptions);

    for (int i = 0; i < gcpavi; i++)
    {
        AVIStreamRelease(gapavi[i]);
    }
    // Good a place as any to make sure audio data gets freed
    //if (lpAudio)
      //  GlobalFreePtr(lpAudio);
    //lpAudio = NULL;

    gcpavi = 0;
}

void InitAvi(LPSTR szFile, int nMenu)
{
    InitAvi(szFile, nMenu, -1, KEEPCOUNTER, NEW_AUDIO_TRACK, RESET_TO_START);
}

void InitAvi(LPSTR szFile, int nMenu, long starttime, int keepcounter, int overwriteaudio, int resetslider)
{
    HRESULT hr;
    PAVIFILE pfile;

    hr = AVIFileOpen(&pfile, szFile, 0, 0L);

    if (hr != 0)
    {
        ErrMsg("Unable to open file %s", szFile);
        return;
    }
    // If we're opening something new, close other open files, otherwise
    // just close the draw stuff so we'll merge streams with the new file
    if (nMenu == MENU_OPEN)
    {
        FreeAvi();
        fileModified = 0;
    }
    // ErrMsg("overwite in initavi %d",overwriteaudio);
    InsertAVIFile(pfile, szFile, starttime, keepcounter, overwriteaudio, resetslider);
}

UINT PlayAVIThread(LPVOID /*pParam*/)
{
    while (playerAlive)
    {
        if (!gfRecording && initAudioRecording)
        {
            initAudioRecording = FALSE;
            gfRecording = TRUE;

            InitAudioRecording();
            StartAudioRecording(&m_Format);
        }
        if (gfRecording && endAudioRecording)
        {
            endAudioRecording = FALSE;
            gfRecording = FALSE;
            // gfPlaying = FALSE;

            StopAudioRecording();
            ClearAudioFile();
            CreateSilenceFile();

            ::PostMessage(viewWnd, WM_USER_GENERIC, 0, 0);
        }
        if (gfPlaying)
        {
            LONG l = aviaudioTime(); // returns -1 if no audio playing

            // If we can't use the audio clock to tell us how long we've been
            // playing, calculate it ourself
            if (l == -1)
                l = timeGetTime() - glPlayStartTime + glPlayStartPos;

            if (l != (LONG)GetScrollTime())
            {
                if (l < timeStart)
                {
                    // make sure number isn't out of bounds
                    l = timeStart;
                }
                if (l > timeEnd)
                {
                    // looks like we're all done!
                    if (pmode == PLAYER)
                        PostMessage(viewWnd, WM_COMMAND, ID_FILE_STOP, 0);
                    else if (!allowRecordExtension)
                    {
                        // StopPlayingRecording();
                        PostMessage(viewWnd, WM_COMMAND, ID_FILE_STOP, 0);
                    }
                    else
                    {
                        // Allowing Recording Extension even if even if end of current file is reached
                        if (gfRecording)
                        {
                            // Stop only the playing but not the recording
                            gfPlaying = FALSE;
                            // l = timeEnd;
                        }
                        else
                        {
                            // playing mode...stop the playing
                            PostMessage(viewWnd, WM_COMMAND, ID_FILE_STOP, 0);
                        }
                    }
                }
                SetScrollTime(l);

                InvalidateRect(viewWnd, NULL, FALSE);
                UpdateWindow(viewWnd);

                continue;
            }
        }
        ::Sleep(50);
    }
    return 0;
}

int ErrMsg(LPSTR sz, ...)
{
    static char szOutput[4 * BUFSIZE];

    va_list va;

    va_start(va, sz);
    wvsprintf(szOutput, sz, va); /* Format the string */
    va_end(va);

    if ((runmode == 0) || (runmode == 1))
        MessageBox(NULL, szOutput, NULL, MB_OK | MB_ICONEXCLAMATION | MB_TASKMODAL);
    return FALSE;
}

void FreeDrawStuff()
{
    // Make sure we're not playing!
    aviaudioStop();

    for (int i = 0; i < gcpavi; i++)
    {
        if (gapgf[i])
        {
            AVIStreamGetFrameClose(gapgf[i]);
            gapgf[i] = NULL;
        }
        if (ghdd[i])
        {
            DrawDibClose(ghdd[i]);
            ghdd[i] = 0;
        }
    }
    ::SetScrollRange(viewWnd, SB_HORZ, 0, 0, TRUE);
    giFirstVideo = giFirstAudio = -1;
}

void FileStop(int mode)
{
    if (mode == SOUND_MODE)
    {
        if (gfAudioFound)
            aviaudioStop();
    }
    // else {//silent mode
    //}
    gfPlaying = FALSE;
}

void RecomputeStreamsTime(int resetslider, long timeCurrent)
{
    timeStart = 0x7FFFFFFF;
    timeEnd = 0;

    // Walk through and init all streams loaded
    for (int i = 0; i < gcpavi; i++)
    {
        timeStart = min(timeStart, AVIStreamStartTime(gapavi[i]));
        timeEnd = max(timeEnd, AVIStreamEndTime(gapavi[i]));
    }
    timeLength = timeEnd - timeStart;

    if (timeLength == 0)
        timeLength = 1;

    // Make sure HSCROLL scrolls enough to be noticeable.
    timehscroll = max(timehscroll, timeLength / MAXTIMERANGE + 2);

    // Set the slider positiom after the timeLenght has been adjusted
    if (resetslider == NO_RESET_SLIDER)
    {
        // Do nothing
    }
    else if (resetslider == RESET_TO_START)
        SetScrollTime(timeStart);
    else if (resetslider == RESET_TO_CURRENT) // This setting has not been tested, it is set to the position of the
                                              // scrolltime where no stream functions has been previously altered
        SetScrollTime(timeCurrent);

    return;
}

void InitStreams()
{
    AVISTREAMINFO avis = {0};
    LONG lTemp = -1;
    int needTestConvert = 0;

    //
    // Start with bogus times
    //
    timeStart = 0x7FFFFFFF;
    timeEnd = 0;

    //
    // Walk through and init all streams loaded
    //
    for (int i = 0; i < gcpavi; i++)
    {
        AVIStreamInfo(gapavi[i], &avis, sizeof(avis));

        //
        // Save and SaveOptions code takes a pointer to our compression opts
        //
        galpAVIOptions[i] = &gaAVIOptions[i];

        //
        // clear options structure to zeroes
        //
        memset(galpAVIOptions[i], 0, sizeof(AVICOMPRESSOPTIONS));

        //
        // Initialize the compression options to some default stuff
        // !!! Pick something better
        //
        galpAVIOptions[i]->fccType = avis.fccType;

        switch (avis.fccType)
        {
            case streamtypeVIDEO:
                galpAVIOptions[i]->dwFlags = AVICOMPRESSF_VALID | AVICOMPRESSF_KEYFRAMES | AVICOMPRESSF_DATARATE;
                galpAVIOptions[i]->fccHandler = 0;
                galpAVIOptions[i]->dwQuality = (DWORD)ICQUALITY_DEFAULT;
                galpAVIOptions[i]->dwKeyFrameEvery = (DWORD)-1; // Default
                galpAVIOptions[i]->dwBytesPerSecond = 0;
                galpAVIOptions[i]->dwInterleaveEvery = 1;
                break;

            case streamtypeAUDIO:
                galpAVIOptions[i]->dwFlags |= AVICOMPRESSF_VALID;
                galpAVIOptions[i]->dwInterleaveEvery = 1;
                AVIStreamReadFormat(gapavi[i], AVIStreamStart(gapavi[i]), NULL, &lTemp);
                galpAVIOptions[i]->cbFormat = lTemp;
                if (lTemp)
                {
                    auto audio_format = GlobalAllocPtr(GHND, lTemp);
                    if (audio_format == nullptr)
                        throw std::runtime_error("unable to allocate global ptr");
                    galpAVIOptions[i]->lpFormat = audio_format;
                }
                // Use current format as default format
                if (galpAVIOptions[i]->lpFormat)
                    AVIStreamReadFormat(gapavi[i], AVIStreamStart(gapavi[i]), galpAVIOptions[i]->lpFormat, &lTemp);
                break;

            default:
                break;
        }
        //
        // We're finding the earliest and latest start and end points for
        // our scrollbar.
        //
        timeStart = min(timeStart, AVIStreamStartTime(gapavi[i]));
        timeEnd = max(timeEnd, AVIStreamEndTime(gapavi[i]));

        //
        // Initialize video streams for getting decompressed frames to display
        //
        if (avis.fccType == streamtypeVIDEO)
        {
            gapgf[i] = AVIStreamGetFrameOpen(gapavi[i], NULL);

            if (gapgf[i] == NULL)
                continue;

            ghdd[i] = DrawDibOpen();
            // !!! DrawDibBegin?

            if (!gfVideoFound)
            {
                DWORD dw;

                //
                // Remember the first video stream --- treat it specially
                //
                giFirstVideo = i;

                //
                // Set the horizontal scrollbar scale to show every frame
                // of the first video stream exactly once
                //
                dw = (avis.rcFrame.right - avis.rcFrame.left) * gwZoom / 4 + HSPACE;

                gdwMicroSecPerPixel = muldiv32(1000000, avis.dwScale, dw * avis.dwRate);
                // Move one frame on the top video screen for each HSCROLL
                timehscroll = muldiv32(1000, avis.dwScale, avis.dwRate);
            }
        }
        else if (avis.fccType == streamtypeAUDIO)
        {
            // These aren't used and better be NULL!
            ghdd[i] = NULL;
            gapgf[i] = NULL;
            // gapgf[i] =  ghdd[i] =   NULL;

            //
            // If there are no video streams, we base everything on this
            // audio stream.
            //
            if (!gfAudioFound && !gfVideoFound)
            {
                // Show one sample per pixel
                gdwMicroSecPerPixel = muldiv32(1000000, avis.dwScale, avis.dwRate);
                // Move one sample per HSCROLL
                // Move at least enough to show movement
                timehscroll = muldiv32(1000, avis.dwScale, avis.dwRate);
            }
            //
            // Remember the first audio stream --- treat it specially
            //
            if (!gfAudioFound)
            {
                // ErrMsg("reset");
                giFirstAudio = i;
                needTestConvert = 1;
            }
        }
    }
    timeLength = timeEnd - timeStart;

    if (timeLength == 0)
        timeLength = 1;

    // Make sure HSCROLL scrolls enough to be noticeable.
    timehscroll = max(timehscroll, timeLength / MAXTIMERANGE + 2);
}

// if ret -1, pavi still valid, paviConverted if not null, is not valid
// if ret 0, pavi, which is unconverted, remains the valid stream, paviConverted if not null, is not valid
// if ret 1,2 or 3 , and paviConverted* is not null, both pavi and paviConverted is valid after returned
// if ret 1,2 or 3 , and paviConverted* is null, only pavi is valid, paviConverted remains null
// if paviDstFormat is NULL, convert to defaultRecordingFormat
// pavi : stream to convert
// paviConverted  :ptr to converted straam
// paviDstFormat  :if not null and in PCM, convert to this format, esle convert to recoring format
int TestConvert(PAVISTREAM pavi, PAVISTREAM *paviConverted, PAVISTREAM paviDstFormat)
{
    // take _THIS_GIVES_OTHERWISE_WARNING_C4702_IS_UNREACHABLE_CODE_ in account when preventing warnings
    pavi = pavi;
    paviConverted = paviConverted; // Eliminates C4100 warning
    paviDstFormat = paviDstFormat; // Eliminates C4100 warning
    // restore
    return 0;

// #define _THIS_GIVES_OTHERWISE_WARNING_C4702_IS_UNREACHABLE_CODE_
#ifdef _THIS_GIVES_OTHERWISE_WARNING_C4702_IS_UNREACHABLE_CODE_
    if (pmode == PLAYER)
        return 0;

    // 0 :No Error, No Conversion Needed
    //-1 :Error
    // 1 : Conversion Succeed, Compressed to PCM Conversion performed
    // 2 : Conversion Succeed, PCM to PCM Rate Conversion performed
    // 3 : Conversion Succeed, Compressed to PCM *and* PCM Rate Conversion performed
    int ret = 0;

    AVISTREAMINFO avis;
    AVIStreamInfo(pavi, &avis, sizeof(avis));

    if (avis.fccType == streamtypeAUDIO)
    {
        WAVEFORMATEX PCMFormat;
        PAVISTREAM intermediatePCMStream = NULL;
        PAVISTREAM finalPCMStream = NULL;

        int nonPCM = 0;
        LPWAVEFORMATEX srcWaveFormat;
        srcWaveFormat = allocRetrieveAudioFormat(pavi);
        if (srcWaveFormat == NULL)
            return -1;

        if (srcWaveFormat->wFormatTag != WAVE_FORMAT_PCM)
        {
            nonPCM = 1;
            // *****************************
            // need conversion to PCM format
            // *****************************

            PCMFormat.wFormatTag = WAVE_FORMAT_PCM;
            MMRESULT mmr =
                acmFormatSuggest(NULL, srcWaveFormat, &PCMFormat, sizeof(PCMFormat), ACM_FORMATSUGGESTF_WFORMATTAG);

            if (mmr != 0)
            {
                ErrMsg("Error suggesting PCM format for conversion !");
            }
            AVICOMPRESSOPTIONS compressOptions;
            _fmemset(&compressOptions, 0, sizeof(AVICOMPRESSOPTIONS));

            compressOptions.dwFlags |= AVICOMPRESSF_VALID;
            compressOptions.dwInterleaveEvery = 1;
            compressOptions.cbFormat = sizeof(PCMFormat);
            compressOptions.lpFormat = &PCMFormat;

            // ErrMsg("Here is fine 0!");
            if (AVIMakeCompressedStream(&intermediatePCMStream, pavi, &compressOptions, NULL) != AVIERR_OK)
            {
                ErrMsg("Error converting to PCM format!");
                return -1;
            }
            ret += 1;
        }
        if (nonPCM)
        {
            // ReRead the audio format
            if (srcWaveFormat)
                free(srcWaveFormat);

            srcWaveFormat = allocRetrieveAudioFormat(intermediatePCMStream);

            if (srcWaveFormat == NULL)
            {
                ErrMsg("Here is read 5!");

                // release the intermediate stream only if the original stream for conversion is non PCM
                if ((nonPCM) && (intermediatePCMStream))
                    AVIStreamRelease(intermediatePCMStream);
                return -1;
            }
        }
        else
            intermediatePCMStream = pavi;

        BuildRecordingFormat();

        LPWAVEFORMATEX dstFormat = NULL;
        int needPhaseTwoConversion = 0;
        if (paviDstFormat)
        {
            dstFormat = allocRetrieveAudioFormat(paviDstFormat);
            needPhaseTwoConversion = 1;
        }
        else
        { // convert to recording format

            if ((srcWaveFormat->nChannels != m_Format.nChannels) ||
                (srcWaveFormat->nSamplesPerSec != m_Format.nSamplesPerSec) ||
                (srcWaveFormat->nAvgBytesPerSec != m_Format.nAvgBytesPerSec) ||
                (srcWaveFormat->nBlockAlign != m_Format.nBlockAlign) ||
                (srcWaveFormat->wBitsPerSample != m_Format.wBitsPerSample) ||
                (srcWaveFormat->cbSize != m_Format.cbSize))
            {
                needPhaseTwoConversion = 1;
            }
        }
        if (needPhaseTwoConversion)
        {
            // *******************************************
            // need conversion of PCM to compatibile rates
            // *******************************************

            if (m_Format.wFormatTag != WAVE_FORMAT_PCM)
            {
                ErrMsg("Invalid Working Format!Working Format is non PCM !");
                free(srcWaveFormat);

                // release the intermediate stream only if the original stream for conversion is non PCM
                if (nonPCM)
                    AVIStreamRelease(intermediatePCMStream);
                return -1;
            }
            if (srcWaveFormat->wFormatTag != WAVE_FORMAT_PCM)
            {
                ErrMsg("Invalid Intermediate Format! Intermediate Format is non PCM !");
                free(srcWaveFormat);

                // release the intermediate stream only if the original stream for conversion is non PCM
                if (nonPCM)
                    AVIStreamRelease(intermediatePCMStream);
                return -1;
            }
            if (dstFormat)
            {
                if (dstFormat->wFormatTag != WAVE_FORMAT_PCM)
                {
                    ErrMsg("Invalid Destination Format! Destination Format is non PCM !");
                    free(srcWaveFormat);
                    free(dstFormat);

                    // release the intermediate stream only if the original stream for conversion is non PCM
                    if (nonPCM)
                        AVIStreamRelease(intermediatePCMStream);

                    return -1;
                }
            }
            // Setting Dest Format
            PCMFormat.wFormatTag = WAVE_FORMAT_PCM;
            PCMFormat.nChannels = m_Format.nChannels;
            PCMFormat.nSamplesPerSec = m_Format.nSamplesPerSec;
            PCMFormat.nAvgBytesPerSec = m_Format.nAvgBytesPerSec;
            PCMFormat.nBlockAlign = m_Format.nBlockAlign;
            PCMFormat.wBitsPerSample = m_Format.wBitsPerSample;
            PCMFormat.cbSize = 0;

            // ErrMsg("Here is fine 1!");
            MMRESULT mmr;
            if (dstFormat)
            {
                // By not sugessting, but using convert directly...we increase the chance of no crashing..
                // mmr = acmFormatSuggest(NULL, srcWaveFormat, dstFormat, sizeof(PCMFormat),
                // ACM_FORMATSUGGESTF_NCHANNELS  | ACM_FORMATSUGGESTF_NSAMPLESPERSEC | ACM_FORMATSUGGESTF_WBITSPERSAMPLE
                // | ACM_FORMATSUGGESTF_WFORMATTAG);
            }
            else
            {
                mmr = acmFormatSuggest(NULL, srcWaveFormat, &PCMFormat, sizeof(PCMFormat),
                                       ACM_FORMATSUGGESTF_NCHANNELS | ACM_FORMATSUGGESTF_NSAMPLESPERSEC |
                                           ACM_FORMATSUGGESTF_WBITSPERSAMPLE | ACM_FORMATSUGGESTF_WFORMATTAG);
            }
            if (mmr != 0)
            {
                ErrMsg("Error suggesting compatible PCM working format for conversion !");

                free(srcWaveFormat);

                // release the intermediate stream only if the original stream for conversion is non PCM
                if (nonPCM)
                    AVIStreamRelease(intermediatePCMStream);

                return -1;
            }
            if ((PCMFormat.nAvgBytesPerSec != m_Format.nAvgBytesPerSec) ||
                (PCMFormat.nBlockAlign != m_Format.nBlockAlign))
            {
                ErrMsg("Note: The format suggested for conversion is not compatible with the working format !");

                // free(srcWaveFormat);
                // release the intermediate stream only if the intermediate stream is not a pointer to the original
                // stream if (nonPCM) AVIStreamRelease(intermediatePCMStream); return -1;
            }
            AVICOMPRESSOPTIONS compressOptionsFinal;
            _fmemset(&compressOptionsFinal, 0, sizeof(AVICOMPRESSOPTIONS));

            compressOptionsFinal.dwFlags |= AVICOMPRESSF_VALID;
            compressOptionsFinal.dwInterleaveEvery = 1;

            // LPWAVEFORMATEX xformat = allocRetrieveAudioFormat(gapavi[giFirstAudio]);

            if (dstFormat)
            {
                // ErrMsg("1");
                compressOptionsFinal.cbFormat = sizeof(*dstFormat);
                compressOptionsFinal.lpFormat = dstFormat;
            }
            else
            {
                compressOptionsFinal.cbFormat = sizeof(PCMFormat);
                compressOptionsFinal.lpFormat = &PCMFormat;
            }
            if (AVIMakeCompressedStream(&finalPCMStream, intermediatePCMStream, &compressOptionsFinal, NULL) !=
                AVIERR_OK)
            {
                ErrMsg("Error converting PCM attributes !");
                free(srcWaveFormat);

                // release the intermediate stream only if the original stream for conversion is non PCM
                if (nonPCM)
                    AVIStreamRelease(intermediatePCMStream);
                return -1;
            }
            ret += 2;
        }
        if (srcWaveFormat)
            free(srcWaveFormat);

        if (ret == 0)
        { // no conversion necessary

            if (paviConverted) // if paviConverted is not null
                *paviConverted = 0;
        }
        else if (ret == 1)
        { // only Compressed to PCM Conversion performed

            if (paviConverted) // if paviConverted is not null
                *paviConverted = intermediatePCMStream;
            else
            {
                if (intermediatePCMStream)
                    AVIStreamRelease(intermediatePCMStream);
            }
        }
        else if (ret == 2)
        { // only PCM Rate Conversion performed

            if (paviConverted) // if paviConverted is not null
                *paviConverted = finalPCMStream;
            else
            {
                if (finalPCMStream)
                    AVIStreamRelease(finalPCMStream);
            }
        }
        else if (ret == 3)
        { // both Conversion performed

            if (paviConverted) // if paviConverted is not null
                *paviConverted = finalPCMStream;
            else
            {
                if (finalPCMStream)
                    AVIStreamRelease(finalPCMStream);
            }
            if (intermediatePCMStream)
                AVIStreamRelease(intermediatePCMStream);
        }
    }
    else
    {
        ErrMsg("Error converting audio, (Streamtype not Audio)");
        return -1;
    }
    return ret;
#endif _THIS_GIVES_OTHERWISE_WARNING_C4702_IS_UNREACHABLE_CODE_
}

// Assume Stream is an audio stream
// Need to free the waveformat after using this function
LPWAVEFORMATEX allocRetrieveAudioFormat(PAVISTREAM pavi)
{
    LONG lStart, lLength;
    LPVOID srcFormat;

    lStart = AVIStreamStart(pavi);

    if (AVIStreamReadFormat(pavi, lStart, NULL, &lLength) != 0)
    {
        ErrMsg("Error converting audio (1)");
        return 0;
    }
    if ((srcFormat = malloc(lLength)) == NULL)
    {
        ErrMsg("Error converting audio (2)");
        return 0;
    }
    if (AVIStreamReadFormat(pavi, lStart, srcFormat, &lLength) != 0)
    {
        ErrMsg("Error converting audio (3)");
        free(srcFormat);
        return 0;
    }
    return (LPWAVEFORMATEX)srcFormat;
}

void FixWindowTitle()
{
    char szTitle[512];

    if (pmode == PLAYER)
    {
        if (gszFileTitle[0])
            // wsprintf(szTitle, "Player - %s", (LPSTR)gszFileTitle);
            wsprintf(szTitle, "Player :- %s", (LPSTR)gszFileTitle);
        else
            wsprintf(szTitle, "Player");
    }
    else
    {
        if (gszFileTitle[0])
            // wsprintf(szTitle, "Player - %s", (LPSTR)gszFileTitle);
            wsprintf(szTitle, "SWF Producer :- %s", (LPSTR)gszFileTitle);
        else
            wsprintf(szTitle, "SWF Producer");
    }
    SetTitleBar(szTitle);
}

void SetTitleBar(CString title)
{
    CWinApp *app = NULL;
    app = AfxGetApp();
    if (app)
    {
        HWND mainwnd = NULL;
        mainwnd = app->m_pMainWnd->m_hWnd;
        if (mainwnd)
            ::SetWindowText(mainwnd, LPCTSTR(title));
    }
}

void FixScrollbars()
{
    int nHeight = 0;
    int nWidth = 0;
    RECT rc;
    HDC hdc;
    HWND hwnd;

    hwnd = viewWnd;

    //
    // Determine how tall our window needs to be to display everything.
    //
    hdc = GetDC(NULL);
    ExcludeClipRect(hdc, 0, 0, 32767, 32767); // don't actually draw
    nHeight = PaintStuff(hdc, hwnd, TRUE);
    ReleaseDC(NULL, hdc);

    //
    // Set vertical scrollbar for scrolling the visible area
    //
    GetClientRect(hwnd, &rc);
    nVertHeight = nHeight; // total height in pixels of entire display

    RECT rcImage;
    GetImageDimension(rcImage);

    if (gwZoom >= 4)
        nHorzWidth = ((rcImage.right - rcImage.left + 1) * gwZoom) / 4;
    else
        nHorzWidth = rcImage.right - rcImage.left + 1;

    //
    // We won't fit in the window... need scrollbars
    //
    if (nHeight > rc.bottom)
    {
        nVertSBLen = nHeight - rc.bottom;
        SetScrollRange(hwnd, SB_VERT, 0, nVertSBLen, TRUE);
        SetScrollPos(hwnd, SB_VERT, 0, TRUE);

        //
        // We will fit in the window!  No scrollbars necessary
        //
    }
    else
    {
        nVertSBLen = 0;
        SetScrollRange(hwnd, SB_VERT, 0, 0, TRUE);
    }
    //
    // Horzizontal Scrollbar
    //
    if (nWidth > rc.right)
    {
        nHorzSBLen = nWidth - rc.right;
        SetScrollRange(hwnd, SB_HORZ, 0, nHorzSBLen, TRUE);
        SetScrollPos(hwnd, SB_HORZ, 0, TRUE);

        //
        // We will fit in the window!  No scrollbars necessary
        //
    }
    else
    {
        nHorzSBLen = 0;
        SetScrollRange(hwnd, SB_HORZ, 0, 0, TRUE);
    }
    return;
}

// Using this function assume that gcpavi, the number of streams
// and gapavi[], the stream array is properly initialized
// Return the Dimension of the first video stream
void GetImageDimension(RECT &rcFrame)
{
    AVISTREAMINFO avis;

    avis.rcFrame.left = 0;
    avis.rcFrame.right = 0;
    avis.rcFrame.top = 0;
    avis.rcFrame.bottom = 0;

    int j;
    for (j = 0; j < gcpavi; j++)
    {
        AVIStreamInfo(gapavi[j], &avis, sizeof(avis));

        if (avis.fccType == streamtypeVIDEO)
        {
            if (gapgf[j] != NULL)
                break; // break from for loop
        }
    }
    rcFrame = avis.rcFrame;
}

int CPlayplusView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CView::OnCreate(lpCreateStruct) == -1)
        return -1;

    // TODO: Add your specialized creation code here
    WORD wVer = HIWORD(VideoForWindowsVersion());
    if (wVer < 0x010a)
    {
        if ((0 == runmode) || (1 == runmode))
            MessageOut(NULL, IDS_TPRVFW11, IDS_NOTE, MB_OK | MB_ICONEXCLAMATION);
        return -1;
    }
    AVIFileInit();

    HDC hScreenDC = ::GetDC(NULL);
    nColors = ::GetDeviceCaps(hScreenDC, BITSPIXEL);
    maxxScreen = GetDeviceCaps(hScreenDC, HORZRES);
    maxyScreen = GetDeviceCaps(hScreenDC, VERTRES);
    ::ReleaseDC(NULL, hScreenDC);

    if (pmode == PLAYER)
        hLogoBM = LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BITMAP_LOGO_PLAYER));
    else if (pmode == DUBBER)
        hLogoBM = LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BITMAP_LOGO_DUBBER));

    int tdata = 0;
    /*CWinThread * pThread = */ (void)AfxBeginThread(PlayAVIThread, &tdata);

    audioPlayable = waveOutGetNumDevs();

    audioRecordable = waveInGetNumDevs();
    if (audioRecordable)
    {
        SuggestRecordingFormat();
        SuggestCompressFormat();
    }
    if (PLAYER == pmode)
        allowRecordExtension = 0;
    else if (DUBBER == pmode)
        allowRecordExtension = 1;

    // ver 2.25
    //CreatePropertySheet();

    return 0;
}

void CPlayplusView::OnDestroy()
{
    CView::OnDestroy();

    AVIClearClipboard();
    FreeAvi();     // close all open streams
    AVIFileExit(); // shuts down the AVIFile system

    if (hLogoBM)
    {
        DeleteObject(hLogoBM);
        hLogoBM = NULL;
    }
    if (pwfx)
    {
        GlobalFreePtr(pwfx);
        pwfx = NULL;
    }
    if (runmode == 0 || runmode == 1)
        SaveSettings();

    // SaveController();

    // Multilanguage
    if (CurLangID != STANDARD_LANGID)
        FreeLibrary(AfxGetResourceHandle());
}

void CPlayplusView::OnPaint()
{
    if (!doneOnce)
    {
        if (strlen(playfiledir) != 0)
        {
            OpenMovieFileInit(playfiledir);
            doneOnce = 1;

            if (0 < runmode)
            {
                // Assign default output name
                // swfname = playfiledir;
                AdjustOutName(playfiledir);

                // silence mode
                if (runmode == 2)
                {
                    // these settings will override those in CamStudio.Producer.param file
                    // becuase loadsettings is called first
                    launchHTMLPlayer = 0;
                    launchPropPrompt = 0;
                    deleteAVIAfterUse = 0;
                }
                // launchHTMLPlayer = 0;
                // OnFileConverttoswf();
                //PostMessage(WM_COMMAND, ID_FILE_CONVERTTOSWF, 0);
            }
        }
        doneOnce = 1;
    }
    CPaintDC dc(this); // device context for painting

    // TODO: Add your message handler code here
    if (gcpavi <= 0)
    {
        if (nColors >= 8)
        {
            HDC hdcBits = CreateCompatibleDC(dc.m_hDC);

            CRect rect;
            GetClientRect(&rect);

            int offsetx = 0;
            int offsety = 0;
            int bitmapwidth = 300;
            int bitmapheight = 220;

            if (rect.right > bitmapwidth)
                offsetx = (rect.right - bitmapwidth) / 2;

            if (rect.bottom > bitmapheight)
                offsety = (rect.bottom - bitmapheight) / 2;

            HBITMAP old_bitmap = (HBITMAP)::SelectObject(hdcBits, hLogoBM);

            // OffScreen Buffer
            ::BitBlt(dc.m_hDC, offsetx, offsety, rect.right - rect.left + 1, rect.bottom - rect.top + 1, hdcBits, 0, 0,
                     SRCCOPY);

            ::SelectObject(hdcBits, old_bitmap);

            DeleteDC(hdcBits);
        }
    }
    else
        PaintStuff(dc.m_hDC, viewWnd, FALSE);

    SetDurationLine();
    SetRPSLine();

    // Do not call CView::OnPaint() for painting messages
}

void SetDurationLine()
{
    CString durationStr(" 0.00 / 0.00 sec");
    if (timeLength > 0)
    {
        long timenow = GetScrollTime();
        float durationPlayed = (float)((timenow - timeStart) / 1000.0);
        float durationLength = (float)((timeEnd - timeStart) / 1000.0);
        durationStr.Format(" %6.1f /%6.1f sec", durationPlayed, durationLength);
    }
    if (statusbarCtrl)
        statusbarCtrl->SetPaneText(2, durationStr);
}

void SetRPSLine()
{
    CString istr;
    if (gfRecording)
        istr = "Recording";
    else if (gfPlaying)
        istr = "Playing";
    else
        istr = "Stopped";

    // istr += durationStr;

    if (statusbarCtrl)
        statusbarCtrl->SetPaneText(1, istr);
}

BOOL CPlayplusView::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT &rect,
                           CWnd *pParentWnd, UINT nID, CCreateContext *pContext)
{
    // TODO: Add your specialized code here and/or call the base class
    BOOL retval = CWnd::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);

    viewWnd = m_hWnd;

    swfname = GetProgPath() + "\\untitled.swf";
    swfhtmlname = GetProgPath() + "\\untitled.swf.html";
    swfbasename = "untitled.swf";

    if (1 == runmode)
        LoadCommand(); // used internally by CamStudio Recorder

    // else if (runmode == 0 || runmode == 2)
    LoadSettings();

    loadingPath = GetProgPath() + "\\controller\\loadpiece.bmp";
    return retval;
}

void CPlayplusView::OnSize(UINT nType, int cx, int cy)
{
    CView::OnSize(nType, cx, cy);

    // TODO: Add your message handler code here
    HWND hwnd;
    RECT rc;
    RECT rcWnd;

    hwnd = viewWnd;

    if (gcpavi <= 0)
    {
        if (hwnd)
        {
            ::SetScrollRange(hwnd, SB_VERT, 0, 0, TRUE);
            ::SetScrollRange(hwnd, SB_HORZ, 0, 0, TRUE);
        }
        return;
    }
    // AdjustToolBar (hwnd);

    ::GetClientRect(hwnd, &rc);

    ::GetWindowRect(hwnd, &rcWnd);
    //
    // There is not enough vertical room to show all streams. Scrollbars
    // are required.
    //

    // Added ver 1.0
    int ScrollVertWidth = GetSystemMetrics(SM_CXVSCROLL);
    int ScrollHorzHeight = GetSystemMetrics(SM_CYHSCROLL);

    int MinPos;
    int MaxPos;
    // int IncYVal = 0;

    int origbottom = 0;
    int origright = 0;
    origbottom = rc.bottom;
    origright = rc.right;

    // Obtaining true client area including the scroll bars

    MaxPos = 0;
    ::GetScrollRange(hwnd, SB_HORZ, &MinPos, &MaxPos);
    if (MaxPos > 0)
        rc.bottom += ScrollHorzHeight;

    MaxPos = 0;
    ::GetScrollRange(hwnd, SB_VERT, &MinPos, &MaxPos);
    if (MaxPos > 0)
    {
        rc.right += ScrollVertWidth;

        // This is a hack, as the above code GetScrollRange may sometimes detect the presence of a scroll bar even if it
        // is absent
        if (rc.right > rcWnd.right - rcWnd.left)
            rc.right -= ScrollVertWidth; // deduct back
    }
    // rc.right and rc.bottom now refers to the true client area
    if ((nVertHeight > rc.bottom) && (nHorzWidth > rc.right))
    {
        // Both Scroll Bars

        // Adjust the client client to reflect both scroll bars
        rc.bottom -= ScrollHorzHeight;
        rc.right -= ScrollVertWidth;

        nVertSBLen = nVertHeight - rc.bottom;
        ::SetScrollRange(hwnd, SB_VERT, 0, nVertSBLen, TRUE);

        nHorzSBLen = nHorzWidth - rc.right;
        ::SetScrollRange(hwnd, SB_HORZ, 0, nHorzSBLen, TRUE);
    }
    else if ((nVertHeight <= rc.bottom) && (nHorzWidth <= rc.right))
    {
        // No Scroll Bars
        nVertSBLen = 0;
        ::SetScrollRange(hwnd, SB_VERT, 0, 0, TRUE);
        nHorzSBLen = 0;
        ::SetScrollRange(hwnd, SB_HORZ, 0, 0, TRUE);
    }
    else if ((nVertHeight > rc.bottom) && (nHorzWidth <= rc.right))
    {
        if (nHorzWidth <= rc.right - ScrollVertWidth)
        { // if after the addition of vertical SCROLLBAR, the horizontal width is still larger than image

            // Vert Scroll Bar Only
            nHorzSBLen = 0;
            ::SetScrollRange(hwnd, SB_HORZ, 0, 0, TRUE);

            nVertSBLen = nVertHeight - rc.bottom;
            ::SetScrollRange(hwnd, SB_VERT, 0, nVertSBLen, TRUE);
        }
        else
        {
            // Activate Both ScrollBars

            // Adjust the client client to reflect both scroll bars
            rc.bottom -= ScrollHorzHeight;
            rc.right -= ScrollVertWidth;

            nVertSBLen = nVertHeight - rc.bottom;
            ::SetScrollRange(hwnd, SB_VERT, 0, nVertSBLen, TRUE);

            nHorzSBLen = nHorzWidth - rc.right;
            ::SetScrollRange(hwnd, SB_HORZ, 0, nHorzSBLen, TRUE);
        }
    }
    else if ((nVertHeight <= rc.bottom) && (nHorzWidth > rc.right))
    {
        if (nVertHeight <= rc.bottom - ScrollHorzHeight)
        { // if after the addition of horiz SCROLLBAR, the vertical height is still larger than image

            // Horiz Scroll Bar Only
            nVertSBLen = 0;
            ::SetScrollRange(hwnd, SB_VERT, 0, 0, TRUE);

            nHorzSBLen = nHorzWidth - rc.right;
            ::SetScrollRange(hwnd, SB_HORZ, 0, nHorzSBLen, TRUE);
        }
        else
        {
            // Activate Both ScrollBars

            // Adjust the client client to reflect both scroll bars
            rc.bottom -= ScrollHorzHeight;
            rc.right -= ScrollVertWidth;

            nVertSBLen = nVertHeight - rc.bottom;
            ::SetScrollRange(hwnd, SB_VERT, 0, nVertSBLen, TRUE);

            nHorzSBLen = nHorzWidth - rc.right;
            ::SetScrollRange(hwnd, SB_HORZ, 0, nHorzSBLen, TRUE);
        }
    }
}

void CPlayplusView::OnFileLastframe()
{
    // TODO: Add your command handler code here
    SetScrollTime(timeStart + timeLength - 1);
    ::InvalidateRect(viewWnd, NULL, FALSE);
    ::UpdateWindow(viewWnd);
}

BOOL CPlayplusView::OnEraseBkgnd(CDC *pDC)
{
    // Set brush to desired background color
    CBrush backBrush(RGB(0, 0, 0));

    // Save old brush
    CBrush *pOldBrush = pDC->SelectObject(&backBrush);

    CRect rect;
    pDC->GetClipBox(&rect); // Erase the area needed

    pDC->PatBlt(rect.left, rect.top, rect.Width(), rect.Height(), PATCOPY);
    pDC->SelectObject(pOldBrush);
    return TRUE;
}

void CPlayplusView::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar *pScrollBar)
{
    // TODO: Add your message handler code here and/or call default

    HWND hwnd;
    int nScrollPos;
    RECT rc;

    hwnd = viewWnd;

    nScrollPos = ::GetScrollPos(hwnd, SB_HORZ);

    ::GetClientRect(hwnd, &rc);

    switch (nSBCode)
    {
        case SB_LINEDOWN:
            nScrollPos += 10;
            break;
        case SB_LINEUP:
            nScrollPos -= 10;
            break;
        case SB_PAGEDOWN:
            nScrollPos += rc.right;
            break;
        case SB_PAGEUP:
            nScrollPos -= rc.right;
            break;
        case SB_THUMBTRACK:
        case SB_THUMBPOSITION:
            nScrollPos = nPos;
            break;
    }
    if (nScrollPos < 0)
        nScrollPos = 0;

    if (nScrollPos > nHorzSBLen)
        nScrollPos = nHorzSBLen;

    if (nScrollPos == ::GetScrollPos(hwnd, SB_HORZ))
        return;

    ::SetScrollPos(hwnd, SB_HORZ, nScrollPos, TRUE);
    // InvalidateRect(hwnd, NULL, TRUE);
    ::InvalidateRect(hwnd, NULL, FALSE);
    ::UpdateWindow(hwnd);

    CView::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CPlayplusView::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar *pScrollBar)
{
    // TODO: Add your message handler code here and/or call default
    HWND hwnd;

    int nScrollPos;
    RECT rc;

    hwnd = viewWnd;

    nScrollPos = ::GetScrollPos(hwnd, SB_VERT);

    ::GetClientRect(hwnd, &rc);

    switch (nSBCode)
    {
        case SB_LINEDOWN:
            nScrollPos += 10;
            break;
        case SB_LINEUP:
            nScrollPos -= 10;
            break;
        case SB_PAGEDOWN:
            nScrollPos += rc.bottom;
            break;
        case SB_PAGEUP:
            nScrollPos -= rc.bottom;
            break;
        case SB_THUMBTRACK:
        case SB_THUMBPOSITION:
            nScrollPos = nPos;
            break;
    }
    if (nScrollPos < 0)
        nScrollPos = 0;

    if (nScrollPos > nVertSBLen)
        nScrollPos = nVertSBLen;

    if (nScrollPos == ::GetScrollPos(hwnd, SB_VERT))
        return;

    ::SetScrollPos(hwnd, SB_VERT, nScrollPos, TRUE);
    // InvalidateRect(hwnd, NULL, TRUE);
    ::InvalidateRect(hwnd, NULL, FALSE);
    ::UpdateWindow(hwnd);

    CView::OnVScroll(nSBCode, nPos, pScrollBar);
}

void OpenMovieFileInit(char *gzFileName)
{
    InitAvi(gzFileName, MENU_OPEN);
    ResizeToMovie();
}

void CPlayplusView::OnZoomResizetomoviesize()
{
    // TODO: Add your command handler code here
    ResizeToMovie();
}

void CPlayplusView::OnZoomQuarter()
{
    gwZoom = 1;
    FixScrollbars();
    ::InvalidateRect(viewWnd, NULL, TRUE);
}

void CPlayplusView::OnZoomHalf()
{
    gwZoom = 2;
    FixScrollbars();
    ::InvalidateRect(viewWnd, NULL, TRUE);
}

void CPlayplusView::OnZoom1()
{
    gwZoom = 4;
    FixScrollbars();
    ::InvalidateRect(viewWnd, NULL, TRUE);
}

void CPlayplusView::OnZoom2()
{
    gwZoom = 8;
    FixScrollbars();
    ::InvalidateRect(viewWnd, NULL, TRUE);
}

void CPlayplusView::OnZoom4()
{
    gwZoom = 16;
    FixScrollbars();
    ::InvalidateRect(viewWnd, NULL, TRUE);
}

void CPlayplusView::OnUpdateZoom1(CCmdUI *pCmdUI)
{
    pCmdUI->SetCheck(gwZoom == 4);
}

void CPlayplusView::OnUpdateZoom2(CCmdUI *pCmdUI)
{
    pCmdUI->SetCheck(gwZoom == 8);
}

void CPlayplusView::OnUpdateZoom4(CCmdUI *pCmdUI)
{
    pCmdUI->SetCheck(gwZoom == 16);
}

void CPlayplusView::OnUpdateZoomHalf(CCmdUI *pCmdUI)
{
    pCmdUI->SetCheck(gwZoom == 2);
}

void CPlayplusView::OnUpdateZoomQuarter(CCmdUI *pCmdUI)
{
    pCmdUI->SetCheck(gwZoom == 1);
}

void CPlayplusView::OnUpdateZoomResizetomoviesize(CCmdUI *pCmdUI)
{
    pCmdUI->Enable(gwZoom == 4);
}

long GetScrollTime()
{
    return playtime;
}

void SetScrollTime(long time)
{
    playtime = time;

    if (timeLength > 0)
    {
        float durationPlayed = (float)((time - timeStart) / 1000.0);
        CString durationStr;
        durationStr.Format("%8.1f sec", durationPlayed);
        SetTimeIndicator(durationStr);
    }
    int x = 0;
    if (timeLength > 0)
        x = ((time - timeStart) * MAXTIMERANGE) / timeLength;
    else
        x = 0;

    if (x < 0)
        x = 0;

    if (x > MAXTIMERANGE)
        x = MAXTIMERANGE;

    if (sliderCtrlPtr)
        sliderCtrlPtr->SetPos(x);
}

void CPlayplusView::OnUpdateFileRewind(CCmdUI *pCmdUI)
{
    BOOL enablebutton = (!gfPlaying) && (!gfRecording);
    pCmdUI->Enable(enablebutton);
}

void CPlayplusView::OnUpdateFileLastframe(CCmdUI *pCmdUI)
{
    // TODO: Add your command update UI handler code here
    BOOL enablebutton = (!gfPlaying) && (!gfRecording);
    pCmdUI->Enable(enablebutton);
}

void CPlayplusView::OnButtonRecord()
{
    if (!gfRecording)
    {
        initAudioRecording = TRUE;

        glRecordStartTimeValue = GetScrollTime();

        PlayMovie(SILENT_MODE);

        fileModified = 1;
    }
}

void CPlayplusView::OnUpdateRecord(CCmdUI *pCmdUI)
{
    // TODO: Add your command update UI handler code here
    // BOOL enablebutton = (!gfPlaying) && (!gfRecording) && (giFirstAudio<0) && (giFirstVideo>=0) && (audioRecordable);
    BOOL enablebutton = (!gfPlaying) && (!gfRecording) && (giFirstVideo >= 0) && (audioRecordable);
    pCmdUI->Enable(enablebutton);
}

/////////////////////////////////////////////
// Code from Recorder
/////////////////////////////////////////////

/////////////////////////////////////////////
// Merging Module
/////////////////////////////////////////////

/////////////////////////////////////////////
// Merge_Video_And_Sound_File
/////////////////////////////////////////////
int Merge_Video_And_Sound_File(CString input_video_path, CString input_audio_path, CString output_avi_path,
                               BOOL recompress_audio, LPWAVEFORMATEX audio_recompress_format, DWORD audio_format_size,
                               BOOL bInterleave, int interleave_factor)
{
    PAVISTREAM AviStream[NUMSTREAMS];            // the editable streams
    AVICOMPRESSOPTIONS aAVIOptions[NUMSTREAMS]; // compression options
    LPAVICOMPRESSOPTIONS alpAVIOptions[NUMSTREAMS];

    memset(AviStream, 0, sizeof(AviStream));
    memset(aAVIOptions, 0, sizeof(aAVIOptions));
    memset(alpAVIOptions, 0, sizeof(alpAVIOptions));

    PAVIFILE pfileVideo = NULL;

    AVIFileInit();

    // Open Video and Audio Files
    HRESULT hr = AVIFileOpen(&pfileVideo, LPCTSTR(input_video_path), OF_READ | OF_SHARE_DENY_NONE, 0L);
    if (hr != 0)
    {
        if ((runmode == 0) || (runmode == 1))
            // MessageBox(NULL,"Unable to open video file.","Note",MB_OK | MB_ICONEXCLAMATION);
            MessageOut(NULL, IDS_UTOVF, IDS_NOTE, MB_OK | MB_ICONEXCLAMATION);
        return 1;
    }
    // Get Video Stream from Video File and Audio Stream from Audio File

    // ==========================================================
    // Important Assumption
    // Assume stream 0 is the correct stream in the files
    // ==========================================================
    if (pfileVideo)
    {
        PAVISTREAM pavi;
        if (AVIFileGetStream(pfileVideo, &pavi, streamtypeVIDEO, 0) != AVIERR_OK)
        {
            AVIFileRelease(pfileVideo);
            if ((runmode == 0) || (runmode == 1))
                MessageOut(NULL, IDS_UTOVS, IDS_NOTE, MB_OK | MB_ICONEXCLAMATION);
            // MessageBox(NULL,"Unable to open video stream.","Note",MB_OK | MB_ICONEXCLAMATION);
            return 1;
        }
        // Set editable stream number as 0
        if (CreateEditableStream(&AviStream[0], pavi) != AVIERR_OK)
        {
            AVIStreamRelease(pavi);
            AVIFileRelease(pfileVideo);
            if ((runmode == 0) || (runmode == 1))
                MessageOut(NULL, IDS_UTCEVS, IDS_NOTE, MB_OK | MB_ICONEXCLAMATION);
            // MessageBox(NULL,"Unable to create editable video stream.","Note",MB_OK | MB_ICONEXCLAMATION);
            return 1;
        }
        AVIStreamRelease(pavi);

        AVIFileRelease(pfileVideo);
        pfileVideo = NULL;
    }
    // =============================
    // Getting Audio Stream
    // =============================
    {
        PAVISTREAM pavi;
        if (AVIStreamOpenFromFile(&pavi, input_audio_path, streamtypeAUDIO, 0, OF_READ | OF_SHARE_DENY_NONE, NULL) !=
            AVIERR_OK)
        {
            AVIStreamRelease(AviStream[0]);
            if ((runmode == 0) || (runmode == 1))
                MessageOut(NULL, IDS_UTOAS, IDS_NOTE, MB_OK | MB_ICONEXCLAMATION);
            // MessageBox(NULL,"Unable to open audio stream.","Note",MB_OK | MB_ICONEXCLAMATION);
            return 2;
        }
        // Set editable stream number as 1
        if (CreateEditableStream(&AviStream[1], pavi) != AVIERR_OK)
        {
            AVIStreamRelease(pavi);
            AVIStreamRelease(AviStream[0]);
            if ((runmode == 0) || (runmode == 1))
                MessageOut(NULL, IDS_TOCEAS, IDS_NOTE, MB_OK | MB_ICONEXCLAMATION);
            // MessageBox(NULL,"Unable to create editable audio stream.","Note",MB_OK | MB_ICONEXCLAMATION);
            return 2;
        }
        AVIStreamRelease(pavi);
    }
    // Verifying streams are of the correct type
    AVISTREAMINFO avis[NUMSTREAMS];

    AVIStreamInfo(AviStream[0], &avis[0], sizeof(avis[0]));
    AVIStreamInfo(AviStream[1], &avis[1], sizeof(avis[1]));

    // Assert that the streams we are going to work with are correct in our assumption
    // such that stream 0 is video and stream 1 is audio

    if (avis[0].fccType != streamtypeVIDEO)
    {
        if ((runmode == 0) || (runmode == 1))
            MessageOut(NULL, IDS_UTVVS, IDS_NOTE, MB_OK | MB_ICONEXCLAMATION);
        // MessageBox(NULL,"Unable to verify video stream.","Note",MB_OK | MB_ICONEXCLAMATION);
        AVIStreamRelease(AviStream[0]);
        AVIStreamRelease(AviStream[1]);
        return 3;
    }
    if (avis[1].fccType != streamtypeAUDIO)
    {
        if ((runmode == 0) || (runmode == 1))
            MessageOut(NULL, IDS_UTVAS, IDS_NOTE, MB_OK | MB_ICONEXCLAMATION);
        // MessageBox(NULL,"Unable to verify audio stream.","Note",MB_OK | MB_ICONEXCLAMATION);
        AVIStreamRelease(AviStream[0]);
        AVIStreamRelease(AviStream[1]);
        return 4;
    }
    //
    // AVISaveV code takes a pointer to compression opts
    //
    alpAVIOptions[0] = &aAVIOptions[0];
    alpAVIOptions[1] = &aAVIOptions[1];

    //
    // clear options structure to zeroes
    //
    _fmemset(alpAVIOptions[0], 0, sizeof(AVICOMPRESSOPTIONS));
    _fmemset(alpAVIOptions[1], 0, sizeof(AVICOMPRESSOPTIONS));

    //=========================================
    // Set Video Stream Compress Options
    //=========================================
    // No Recompression
    alpAVIOptions[0]->fccType = streamtypeVIDEO;
    alpAVIOptions[0]->fccHandler = 0;
    alpAVIOptions[0]->dwFlags = AVICOMPRESSF_VALID | AVICOMPRESSF_KEYFRAMES | AVICOMPRESSF_DATARATE;
    if (bInterleave)
        alpAVIOptions[0]->dwFlags = alpAVIOptions[0]->dwFlags | AVICOMPRESSF_INTERLEAVE;
    alpAVIOptions[0]->dwKeyFrameEvery = (DWORD)-1;
    alpAVIOptions[0]->dwQuality = (DWORD)ICQUALITY_DEFAULT;
    alpAVIOptions[0]->dwBytesPerSecond = 0;
    alpAVIOptions[0]->dwInterleaveEvery = interleave_factor;
    // galpAVIOptions[0]->cbParms = 0;
    // galpAVIOptions[0]->cbFormat = 0;

    //=========================================
    // Set Audio Stream Compress Options
    //=========================================
    // Recompression may be applied
    //
    // Audio Compress Options seems to be specified by the audio format in avicompressoptions
    alpAVIOptions[1]->fccType = streamtypeAUDIO;
    alpAVIOptions[1]->fccHandler = 0;
    alpAVIOptions[1]->dwFlags = AVICOMPRESSF_VALID;
    if (bInterleave)
        alpAVIOptions[1]->dwFlags = alpAVIOptions[1]->dwFlags | AVICOMPRESSF_INTERLEAVE;
    alpAVIOptions[1]->dwKeyFrameEvery = 0;
    alpAVIOptions[1]->dwQuality = 0;
    alpAVIOptions[1]->dwBytesPerSecond = 0;
    alpAVIOptions[1]->dwInterleaveEvery = interleave_factor;
    // galpAVIOptions[1]->cbParms = 0;

    if (recompress_audio)
    {
        alpAVIOptions[1]->cbFormat = audio_format_size;
        alpAVIOptions[1]->lpFormat = GlobalAllocPtr(GHND, audio_format_size);
        memcpy((void *)alpAVIOptions[1]->lpFormat, (void *)audio_recompress_format, audio_format_size);
    }
    else
    {
        LONG lTemp;
        AVIStreamReadFormat(AviStream[1], AVIStreamStart(AviStream[1]), NULL, &lTemp);
        alpAVIOptions[1]->cbFormat = lTemp;

        if (lTemp)
            alpAVIOptions[1]->lpFormat = GlobalAllocPtr(GHND, lTemp);
        // Use existing format as compress format
        if (alpAVIOptions[1]->lpFormat)
            AVIStreamReadFormat(AviStream[1], AVIStreamStart(AviStream[1]), alpAVIOptions[1]->lpFormat, &lTemp);
    }
    // ============================
    // Do the work! Merging
    // ============================

    // Save     fccHandlers
    DWORD fccHandler[NUMSTREAMS];
    fccHandler[0] = alpAVIOptions[0]->fccHandler;
    fccHandler[1] = alpAVIOptions[1]->fccHandler;

    hr = AVISaveV(LPCTSTR(output_avi_path), NULL, (AVISAVECALLBACK)SaveCallback, NUMSTREAMS, AviStream, alpAVIOptions);
    // hr = AVISaveV(LPCTSTR(output_avi_path),  NULL, (AVISAVECALLBACK) NULL, NUMSTREAMS, AviStream, galpAVIOptions);
    if (hr != AVIERR_OK)
    {
        // Error merging with audio compress options, retry merging with default audio options (no recompression)
        if (recompress_audio)
        {
            AVISaveOptionsFree(NUMSTREAMS, alpAVIOptions);

            alpAVIOptions[0] = &aAVIOptions[0];
            alpAVIOptions[1] = &aAVIOptions[1];

            // Resetting Compress Options
            _fmemset(alpAVIOptions[0], 0, sizeof(AVICOMPRESSOPTIONS));
            _fmemset(alpAVIOptions[1], 0, sizeof(AVICOMPRESSOPTIONS));

            alpAVIOptions[0]->fccType = streamtypeVIDEO;
            alpAVIOptions[0]->fccHandler = 0;
            alpAVIOptions[0]->dwFlags = AVICOMPRESSF_VALID | AVICOMPRESSF_KEYFRAMES | AVICOMPRESSF_DATARATE;
            if (bInterleave)
                alpAVIOptions[0]->dwFlags = alpAVIOptions[0]->dwFlags | AVICOMPRESSF_INTERLEAVE;
            alpAVIOptions[0]->dwKeyFrameEvery = (DWORD)-1;
            alpAVIOptions[0]->dwQuality = (DWORD)ICQUALITY_DEFAULT;
            alpAVIOptions[0]->dwBytesPerSecond = 0;
            alpAVIOptions[0]->dwInterleaveEvery = interleave_factor;

            alpAVIOptions[1]->fccType = streamtypeAUDIO;
            alpAVIOptions[1]->fccHandler = 0;
            alpAVIOptions[1]->dwFlags = AVICOMPRESSF_VALID;
            if (bInterleave)
                alpAVIOptions[1]->dwFlags = alpAVIOptions[1]->dwFlags | AVICOMPRESSF_INTERLEAVE;
            alpAVIOptions[1]->dwKeyFrameEvery = 0;
            alpAVIOptions[1]->dwQuality = 0;
            alpAVIOptions[1]->dwBytesPerSecond = 0;
            alpAVIOptions[1]->dwInterleaveEvery = interleave_factor;

            // Use default audio format
            LONG lTemp;
            AVIStreamReadFormat(AviStream[1], AVIStreamStart(AviStream[1]), NULL, &lTemp);
            alpAVIOptions[1]->cbFormat = lTemp;
            if (lTemp)
                alpAVIOptions[1]->lpFormat = GlobalAllocPtr(GHND, lTemp);
            // Use existing format as compress format
            if (alpAVIOptions[1]->lpFormat)
                AVIStreamReadFormat(AviStream[1], AVIStreamStart(AviStream[1]), alpAVIOptions[1]->lpFormat, &lTemp);

            // Do the Work .... Merging
            hr = AVISaveV(LPCTSTR(output_avi_path), NULL, (AVISAVECALLBACK)NULL, NUMSTREAMS, AviStream, alpAVIOptions);

            if (hr != AVIERR_OK)
            {
                AVISaveOptionsFree(NUMSTREAMS, alpAVIOptions);
                AVIStreamRelease(AviStream[0]);
                AVIStreamRelease(AviStream[1]);

                if ((runmode == 0) || (runmode == 1))
                    MessageOut(NULL, IDS_UTMAVS, IDS_NOTE, MB_OK | MB_ICONEXCLAMATION);
                // MessageBox(NULL,"Unable to merge audio and video streams (1).","Note",MB_OK | MB_ICONEXCLAMATION);
                return 5;
            }
            // Succesful Merging, but with no audio recompression
            if ((runmode == 0) || (runmode == 1))
                MessageOut(NULL, IDS_UAACSOYMIAC, IDS_NOTE, MB_OK | MB_ICONEXCLAMATION);
            // MessageBox(NULL,"Unable to apply audio compression with the selected options. Your movie is saved without
            // audio compression.","Note",MB_OK | MB_ICONEXCLAMATION);

        } // if recompress audio retry
        else
        {
            AVISaveOptionsFree(NUMSTREAMS, alpAVIOptions);
            AVIStreamRelease(AviStream[0]);
            AVIStreamRelease(AviStream[1]);
            if ((runmode == 0) || (runmode == 1))
                MessageOut(NULL, IDS_UTMAVS2, IDS_NOTE, MB_OK | MB_ICONEXCLAMATION);
            // MessageBox(NULL,"Unable to audio and video merge streams (2).","Note",MB_OK | MB_ICONEXCLAMATION);
            return 5;
        }
    }
    // Restore fccHandlers
    alpAVIOptions[0]->fccHandler = fccHandler[0];
    alpAVIOptions[1]->fccHandler = fccHandler[1];

    // Set Title Bar
    HWND mainwnd = NULL;
    mainwnd = AfxGetApp()->m_pMainWnd->m_hWnd;
    if (mainwnd)
        ::SetWindowText(mainwnd, "CamStudio");

    AVISaveOptionsFree(NUMSTREAMS, alpAVIOptions);

    // Free Editable Avi Streams
    for (int i = 0; i < NUMSTREAMS; i++)
    {
        if (AviStream[i])
        {
            AVIStreamRelease(AviStream[i]);
            AviStream[i] = NULL;
        }
    }
    AVIFileExit();

    return 0;
}

BOOL CALLBACK SaveCallback(int iProgress)
{
    // Set Progress in Title Bar

    char szText[300];

    wsprintf(szText, "Compressing Audio %d%%", iProgress);

    HWND mainwnd = NULL;
    mainwnd = AfxGetApp()->m_pMainWnd->m_hWnd;
    if (mainwnd)
        ::SetWindowText(mainwnd, szText);

    return WinYield();
    // return FALSE;
}

BOOL WinYield(void)
{
    // Process 3 messages, then return false
    MSG msg;
    for (int i = 0; i < 3; i++)
    {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
    return FALSE;
}

//////////////////////////////////////////////////////////////////
// Output File Module
// This function is called when the avi saving is completed
//////////////////////////////////////////////////////////////////
LRESULT CPlayplusView::OnUserGeneric(UINT /*wParam*/, LONG /*lParam*/)
{
    // AddAudioWaveFile((char *) LPCTSTR(tempaudiopath));
    //::MessageBox(NULL,tempaudiopath,"User Generic",MB_OK | MB_ICONEXCLAMATION);
    TimeDependentInsert(tempaudiopath, glRecordStartTimeValue);

    SetRPSLine();

    return 0;
}

void TimeDependentInsert(CString filename, long shiftTime)
{
    int overwriteaudio = NEW_AUDIO_TRACK;
    if (giFirstAudio >= 0)
        overwriteaudio = OVERWRITE_AUDIO;
    else
        overwriteaudio = NEW_AUDIO_TRACK;

    CString convertedPath;
    convertedPath = ConvertFileToWorkingPCM(filename);

    // Only the NEW_AUDIO_TRACK selection will cause the resetslider var (FALSE) to have any effect
    if (shiftTime > timeStart)
        InitAvi((char *)LPCTSTR(convertedPath), MENU_MERGE, shiftTime, KEEPCOUNTER, overwriteaudio, RESET_TO_CURRENT);
    else
        InitAvi((char *)LPCTSTR(convertedPath), MENU_MERGE, -1, KEEPCOUNTER, overwriteaudio, RESET_TO_START);
}

CString ConvertFileToWorkingPCM(CString filename)
{
    CString convertedPath = filename;

    // ErrMsg("overwriteaudio %d",overwriteaudio);
    // Only the NEW_AUDIO_TRACK selection will cause the resetslider var (FALSE) to have any effect

    HCURSOR hcur;
    hcur = SetCursor(LoadCursor(NULL, IDC_WAIT));
    ShowCursor(TRUE);

    int retval = 0;
    BuildRecordingFormat();
    convertedPath = GetTempPathEx("\\~converted001.wav", "\\~converted", ".wav");
    retval = MultiStepConvertToPCM(filename, convertedPath, &m_Format, sizeof(m_Format));
    if (retval <= 0)
        convertedPath = filename;

    ShowCursor(FALSE);
    SetCursor(hcur);

    return convertedPath;
}

CString GetProgPath()
{
    // locals
    TCHAR szTemp[300];
    CFile converter;
    CString result;

    // get root
    GetModuleFileName(NULL, szTemp, 300);

    CString path = (CString)szTemp;
    path = path.Left(path.ReverseFind('\\'));
    return path;
}

CString GetAppDataPath()
{
    int folder = CSIDL_APPDATA;

    char szPath[MAX_PATH + 100];
    szPath[0] = 0;
    CString path = szPath;

    if (SUCCEEDED(SHGetFolderPath(NULL, folder, 0, 0, szPath)))
    {
        path = szPath;
    }
    else if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_PERSONAL, 0, 0, szPath)))
    {
        path = szPath;
    }
    return path;
}

CString GetTempPath()
{
    char dirx[300];
    GetWindowsDirectory(dirx, 300);
    CString tempdir(dirx);
    tempdir = tempdir + "\\temp";

    // Verify the chosen temp path is valid

    WIN32_FIND_DATA wfd;
    memset(&wfd, 0, sizeof(wfd));
    HANDLE hdir = FindFirstFile(LPCTSTR(tempdir), &wfd);
    if (!hdir)
    {
        return GetProgPath();
    }
    FindClose(hdir);

    // If valid directory, return Windows\temp as temp directory
    if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        return tempdir;

    // else return program path as temp directory
    return GetProgPath();
}

//===============================================
// AUDIO CODE
//===============================================
// Ver 1.1
//===============================================

void waveInErrorMsg(MMRESULT result, const char *addstr)
{
    // say error message
    char errorbuffer[300];
    waveInGetErrorText(result, errorbuffer, 300);
    // ErrorMsg("WAVEIN:%x:%s %s", result, errorbuffer, addstr);
    CString msgstr;
    msgstr.Format("%s %s", errorbuffer, addstr);
    if ((runmode == 0) || (runmode == 1))
    {
        CString title;
        title.LoadString(IDS_WIERROR);
        MessageBox(NULL, msgstr, title, MB_OK | MB_ICONEXCLAMATION);
    }
}

BOOL InitAudioRecording()
{
    m_ThreadID = ::GetCurrentThreadId();
    m_QueuedBuffers = 0;
    m_hRecord = NULL;

    m_BufferSize = 1000; // samples per callback

    BuildRecordingFormat();

    ClearAudioFile();

    // Create temporary wav file for audio recording
    GetTempWavePath();

    // look up here
    m_pFile = new CSoundFile(tempaudiopath, &m_Format);

    // even if we recording with firstaudio's format, we still get the unable to insert problem!
    // m_pFile = new CSoundFile(tempaudiopath, (LPWAVEFORMATEX) galpAVIOptions[giFirstAudio]->lpFormat);

    if (!(m_pFile && m_pFile->IsOK()))
    {
        if ((runmode == 0) || (runmode == 1))
            MessageOut(NULL, IDS_ECSF, IDS_NOTE, MB_OK | MB_ICONEXCLAMATION);
    }
    return TRUE;
}

// Initialize the tempaudiopath variable with a valid temporary path
void GetTempWavePath()
{
    CString fileName("\\~twav001.wav");
    tempaudiopath = GetTempPath() + fileName;

    // Test the validity of writing to the file
    int fileverified = 0;
    while (!fileverified)
    {
        OFSTRUCT ofstruct;
        HFILE fhandle = OpenFile(tempaudiopath, &ofstruct, OF_SHARE_EXCLUSIVE | OF_WRITE | OF_CREATE);
        if (fhandle != HFILE_ERROR)
        {
            fileverified = 1;
            CloseHandle((HANDLE)fhandle);
            DeleteFile(tempaudiopath);
        }
        else
        {
            srand((unsigned)time(NULL));
            int randnum = rand();
            char numstr[50];
            sprintf(numstr, "%d", randnum);

            CString cnumstr(numstr);
            CString fxstr("\\~temp");
            CString exstr(".wav");
            tempaudiopath = GetTempPath() + fxstr + cnumstr + exstr;
        }
    }
}

// Using this function affects the silenceFileValid variable
// which may be set in a variety of places

// The silenceFileValid, when TRUE, imply the tempsilencepath is pointing to a compatible silence sound file
BOOL CreateSilenceFile()
{
    silenceFileValid = FALSE;

    BuildRecordingFormat();

    ClearAudioSilenceFile();

    // Create temporary wav file for audio recording
    GetSilenceWavePath();
    m_pSilenceFile = new CSoundFile(tempsilencepath, &m_Format);

    if (!(m_pSilenceFile && m_pSilenceFile->IsOK()))
    {
        if ((runmode == 0) || (runmode == 1))
            MessageOut(NULL, IDS_ECSSF, IDS_NOTE, MB_OK | MB_ICONEXCLAMATION);
        // MessageBox(NULL,"Error Creating Silence Sound File","Note",MB_OK | MB_ICONEXCLAMATION);
        silenceFileValid = FALSE;
        return silenceFileValid;
    }
    CBuffer buf(BasicBufSize, TRUE);
    buf.Erase();
    WriteSilenceFile(&buf);

    ClearAudioSilenceFile();

    silenceFileValid = TRUE;

    return silenceFileValid;
}

void ClearAudioSilenceFile()
{
    if (m_pSilenceFile)
    {
        // will close output file
        delete m_pSilenceFile;
        m_pSilenceFile = NULL;
    }
}

void GetSilenceWavePath()
{
    CString fileName("\\~tsil001.wav");
    tempsilencepath = GetTempPath() + fileName;

    // Test the validity of writing to the file
    int fileverified = 0;
    while (!fileverified)
    {
        OFSTRUCT ofstruct;
        HFILE fhandle = OpenFile(tempsilencepath, &ofstruct, OF_SHARE_EXCLUSIVE | OF_WRITE | OF_CREATE);
        if (fhandle != HFILE_ERROR)
        {
            fileverified = 1;
            CloseHandle((HANDLE)fhandle);
            DeleteFile(tempsilencepath);
        }
        else
        {
            srand((unsigned)time(NULL));
            int randnum = rand();
            char numstr[50];
            sprintf(numstr, "%d", randnum);

            CString cnumstr(numstr);
            CString fxstr("\\~tsil");
            CString exstr(".wav");
            tempsilencepath = GetTempPath() + fxstr + cnumstr + exstr;
        }
    }
}

// Delete the m_pFile variable and close existing audio file
void ClearAudioFile()
{
    if (m_pFile)
    {
        // will close output file
        delete m_pFile;
        m_pFile = NULL;
    }
}

void SetBufferSize(int NumberOfSamples)
{
    m_BufferSize = NumberOfSamples;
}

BOOL StartAudioRecording(WAVEFORMATEX *format)
{
    MMRESULT mmReturn = 0;

    if (format != NULL)
        m_Format = *format;

    // open wavein device
    mmReturn = ::waveInOpen(&m_hRecord, AudioDeviceID, &m_Format, (DWORD_PTR)viewWnd, NULL,
                            CALLBACK_WINDOW); // use on message to map.....

    if (mmReturn)
    {
        waveInErrorMsg(mmReturn, "Error in StartAudioRecording()");
        return FALSE;
    }
    else
    {
        // make several input buffers and add them to the input queue
        for (int i = 0; i < 3; i++)
        {
            AddInputBufferToQueue();
        }
        // start recording
        mmReturn = ::waveInStart(m_hRecord);
        if (mmReturn)
        {
            waveInErrorMsg(mmReturn, "Error in StartAudioRecording()");
            return FALSE;
        }
    }
    return TRUE;
}

int AddInputBufferToQueue()
{
    MMRESULT mmReturn = 0;

    // create the header
    LPWAVEHDR pHdr = new WAVEHDR;
    if (pHdr == NULL)
        return NULL;
    ZeroMemory(pHdr, sizeof(WAVEHDR));

    // new a buffer
    CBuffer buf(m_Format.nBlockAlign * m_BufferSize, false);
    pHdr->lpData = buf.ptr.c;
    pHdr->dwBufferLength = buf.ByteLen;

    // prepare it
    mmReturn = ::waveInPrepareHeader(m_hRecord, pHdr, sizeof(WAVEHDR));
    if (mmReturn)
    {
        waveInErrorMsg(mmReturn, "in AddInputBufferToQueue()");
        return m_QueuedBuffers;
    }
    // add the input buffer to the queue
    mmReturn = ::waveInAddBuffer(m_hRecord, pHdr, sizeof(WAVEHDR));
    if (mmReturn)
    {
        waveInErrorMsg(mmReturn, "Error in AddInputBufferToQueue()");
        return m_QueuedBuffers;
    }
    // no error
    // increment the number of waiting buffers
    return m_QueuedBuffers++;
}

void StopAudioRecording()
{
    MMRESULT mmReturn = MMSYSERR_NOERROR;

    mmReturn = ::waveInReset(m_hRecord);
    if (mmReturn)
    {
        waveInErrorMsg(mmReturn, "in Stop()");
        return;
    }
    else
    {
        // Sleep(100);

        mmReturn = ::waveInStop(m_hRecord);
        if (mmReturn)
            waveInErrorMsg(mmReturn, "Error in StopAudioRecording() (WaveinStop)");

        // while (m_QueuedBuffers>0)
        //    Sleep(50);

        mmReturn = ::waveInClose(m_hRecord);
        if (mmReturn)
            waveInErrorMsg(mmReturn, "Error in StopAudioRecording() (WaveinClose)");
    }
    // if (m_QueuedBuffers != 0) ErrorMsg("Still %d buffers in waveIn queue!", m_QueuedBuffers);
    if (m_QueuedBuffers != 0)
    {
        if ((runmode == 0) || (runmode == 1))
            MessageOut(NULL, IDS_ABSIQ, IDS_NOTE, MB_OK | MB_ICONEXCLAMATION);
        // MessageBox(NULL,"Audio buffers still in queue!","note", MB_OK);
    }
}

LRESULT CPlayplusView::OnMM_WIM_DATA(WPARAM /*parm1*/, LPARAM parm2)
{
    MMRESULT mmReturn = 0;

    LPWAVEHDR pHdr = (LPWAVEHDR)parm2;

    mmReturn = ::waveInUnprepareHeader(m_hRecord, pHdr, sizeof(WAVEHDR));
    if (mmReturn)
    {
        waveInErrorMsg(mmReturn, "in OnWIM_DATA() Unprepare Header");
        return 0;
    }
    TRACE("WIM_DATA %4d\n", pHdr->dwBytesRecorded);

    if (gfRecording)
    {
        CBuffer buf(pHdr->lpData, pHdr->dwBufferLength);

        if (!recordpaused)
        { // write only if not paused

            // Write Data to file
            DataFromSoundIn(&buf);
        }
        // reuse the buffer:
        // prepare it again
        mmReturn = ::waveInPrepareHeader(m_hRecord, pHdr, sizeof(WAVEHDR));
        if (mmReturn)
        {
            waveInErrorMsg(mmReturn, "in OnWIM_DATA() Prepare Header");
        }
        else // no error
        {
            // add the input buffer to the queue again
            mmReturn = ::waveInAddBuffer(m_hRecord, pHdr, sizeof(WAVEHDR));
            if (mmReturn)
                waveInErrorMsg(mmReturn, "in OnWIM_DATA() Add Buffer");
            else
                return 0; // no error
        }
    }
    // we are closing the waveIn handle,
    // all data must be deleted
    // this buffer was allocated in Start()
    delete pHdr->lpData;
    delete pHdr;
    m_QueuedBuffers--;

    return 0;
}

void WriteSilenceFile(CBuffer *buffer)
{
    if (m_pSilenceFile)
    {
        if (!m_pSilenceFile->Write(buffer))
        {
            ClearAudioSilenceFile();
            if ((runmode == 0) || (runmode == 1))
                MessageOut(NULL, IDS_EWSSF, IDS_NOTE, MB_OK | MB_ICONEXCLAMATION);
            // MessageBox(NULL,"Error Writing Silence Sound File","Note",MB_OK | MB_ICONEXCLAMATION);
            silenceFileValid = FALSE;
        }
    }
}

void DataFromSoundIn(CBuffer *buffer)
{
    if (m_pFile)
    {
        if (!m_pFile->Write(buffer))
        {
            // m_SoundIn.Stop();
            StopAudioRecording();
            ClearAudioFile();
            if ((runmode == 0) || (runmode == 1))
                MessageOut(NULL, IDS_EWSF, IDS_NOTE, MB_OK | MB_ICONEXCLAMATION);
            // MessageBox(NULL,"Error Writing Sound File","Note",MB_OK | MB_ICONEXCLAMATION);
        }
    }
}

void BuildRecordingFormat()
{
    m_Format.wFormatTag = WAVE_FORMAT_PCM;
    m_Format.wBitsPerSample = static_cast<WORD>(audio_bits_per_sample);
    m_Format.nSamplesPerSec = audio_samples_per_seconds;
    m_Format.nChannels = static_cast<WORD>(audio_num_channels);
    m_Format.nBlockAlign = m_Format.nChannels * (m_Format.wBitsPerSample / 8);
    m_Format.nAvgBytesPerSec = m_Format.nSamplesPerSec * m_Format.nBlockAlign;
    m_Format.cbSize = 0;
}

// Suggest Save/Compress Format to pwfx
void SuggestRecordingFormat()
{
    WAVEINCAPS pwic;
    /* MMRESULT mmr = */ (void)waveInGetDevCaps(AudioDeviceID, &pwic, sizeof(pwic));

    // Ordered in preference of choice
    if ((pwic.dwFormats) & WAVE_FORMAT_2S16)
    {
        audio_bits_per_sample = 16;
        audio_num_channels = 2;
        audio_samples_per_seconds = 22050;
        waveinselected = WAVE_FORMAT_2S16;
    }
    else if ((pwic.dwFormats) & WAVE_FORMAT_2M08)
    {
        audio_bits_per_sample = 8;
        audio_num_channels = 1;
        audio_samples_per_seconds = 22050;
        waveinselected = WAVE_FORMAT_2M08;
    }
    else if ((pwic.dwFormats) & WAVE_FORMAT_2S08)
    {
        audio_bits_per_sample = 8;
        audio_num_channels = 2;
        audio_samples_per_seconds = 22050;
        waveinselected = WAVE_FORMAT_2S08;
    }
    else if ((pwic.dwFormats) & WAVE_FORMAT_2M16)
    {
        audio_bits_per_sample = 16;
        audio_num_channels = 1;
        audio_samples_per_seconds = 22050;
        waveinselected = WAVE_FORMAT_2M16;
    }
    else if ((pwic.dwFormats) & WAVE_FORMAT_1M08)
    {
        audio_bits_per_sample = 8;
        audio_num_channels = 1;
        audio_samples_per_seconds = 11025;
        waveinselected = WAVE_FORMAT_1M08;
    }
    else if ((pwic.dwFormats) & WAVE_FORMAT_1M16)
    {
        audio_bits_per_sample = 16;
        audio_num_channels = 1;
        audio_samples_per_seconds = 11025;
        waveinselected = WAVE_FORMAT_1M16;
    }
    else if ((pwic.dwFormats) & WAVE_FORMAT_1S08)
    {
        audio_bits_per_sample = 8;
        audio_num_channels = 2;
        audio_samples_per_seconds = 11025;
        waveinselected = WAVE_FORMAT_1S08;
    }
    else if ((pwic.dwFormats) & WAVE_FORMAT_1S16)
    {
        audio_bits_per_sample = 16;
        audio_num_channels = 2;
        audio_samples_per_seconds = 11025;
        waveinselected = WAVE_FORMAT_1S16;
    }
    else if ((pwic.dwFormats) & WAVE_FORMAT_4M08)
    {
        audio_bits_per_sample = 8;
        audio_num_channels = 1;
        audio_samples_per_seconds = 44100;
        waveinselected = WAVE_FORMAT_4M08;
    }
    else if ((pwic.dwFormats) & WAVE_FORMAT_4M16)
    {
        audio_bits_per_sample = 16;
        audio_num_channels = 1;
        audio_samples_per_seconds = 44100;
        waveinselected = WAVE_FORMAT_4M16;
    }
    else if ((pwic.dwFormats) & WAVE_FORMAT_4S08)
    {
        audio_bits_per_sample = 8;
        audio_num_channels = 2;
        audio_samples_per_seconds = 44100;
        waveinselected = WAVE_FORMAT_4S08;
    }
    else if ((pwic.dwFormats) & WAVE_FORMAT_4S16)
    {
        audio_bits_per_sample = 16;
        audio_num_channels = 2;
        audio_samples_per_seconds = 44100;
        waveinselected = WAVE_FORMAT_4S16;
    }
    else
    {
        // Arbitrarily choose one
        audio_bits_per_sample = 8;
        audio_num_channels = 1;
        audio_samples_per_seconds = 22050;
        waveinselected = WAVE_FORMAT_2M08;
    }
    // Build m_Format
    BuildRecordingFormat();
}

void SuggestCompressFormat()
{
    bAudioCompression = TRUE;

    AllocCompressFormat();
    MMRESULT mmr = (MMRESULT)0;

    // 1st try MPEGLAYER3
    BuildRecordingFormat();
    if ((m_Format.nSamplesPerSec == 22050) && (m_Format.nChannels == 2) && (m_Format.wBitsPerSample <= 16))
    {
        pwfx->wFormatTag = WAVE_FORMAT_MPEGLAYER3;
        mmr = acmFormatSuggest(NULL, &m_Format, pwfx, cbwfx, ACM_FORMATSUGGESTF_WFORMATTAG);
    }
    if (mmr != 0)
    {
        // Use PCM in order to handle most cases
        BuildRecordingFormat();
        pwfx->wFormatTag = WAVE_FORMAT_PCM;
        mmr = acmFormatSuggest(NULL, &m_Format, pwfx, cbwfx, ACM_FORMATSUGGESTF_WFORMATTAG);

        if (mmr != 0)
        {
            bAudioCompression = FALSE;
        }
    }
}

void AllocCompressFormat()
{
    int initial_audiosetup = 1;

    if (pwfx)
    {
        initial_audiosetup = 0;
        // Do nothing....already allocated
    }
    else
    {
        MMRESULT mmresult = acmMetrics(NULL, ACM_METRIC_MAX_SIZE_FORMAT, &cbwfx);
        if (MMSYSERR_NOERROR != mmresult)
        {
            CString msgstr;
            msgstr.Format("Metrics failed mmresult=%u!", mmresult);
            if ((runmode == 0) || (runmode == 1))
            {
                CString title;
                title.LoadString(IDS_NOTE);
                ::MessageBox(NULL, msgstr, title, MB_OK | MB_ICONEXCLAMATION);
            }
            return;
        }
        pwfx = (LPWAVEFORMATEX)GlobalAllocPtr(GHND, cbwfx);
        if (NULL == pwfx)
        {
            CString msgstr;
            msgstr.Format("GlobalAllocPtr(%lu) failed!", cbwfx);

            if ((runmode == 0) || (runmode == 1))
            {
                CString title;
                title.LoadString(IDS_NOTE);
                ::MessageBox(NULL, msgstr, title, MB_OK | MB_ICONEXCLAMATION);
            }
            //::MessageBox(NULL,msgstr,"Note", MB_OK | MB_ICONEXCLAMATION);
            return;
        }
        initial_audiosetup = 1;
    }
}

void SetTimeIndicator(CString timestr)
{
    // restore
    // if (statusbarCtrl)
    //    statusbarCtrl->SetPaneText(1,timestr);

    // CWnd* mainWindow = AfxGetMainWnd( );
    //((CMainFrame*) mainWindow)->SetTimeIndicator(timestr);
}

void NukeAVIStream(int i)
{
    int j;

    //
    // Make sure it's a real stream number
    //
    if (i < 0 || i >= gcpavi)
        return;

    //
    // Free all the resources associated with this stream
    //
    AVIStreamRelease(gapavi[i]);
    if (galpAVIOptions[i]->lpFormat)
    {
        GlobalFreePtr(galpAVIOptions[i]->lpFormat);
    }
    if (gapgf[i])
    {
        AVIStreamGetFrameClose(gapgf[i]);
        gapgf[i] = NULL;
    }
    if (ghdd[i])
    {
        DrawDibClose(ghdd[i]);
        ghdd[i] = 0;
    }
    //
    // Compact the arrays of junk
    //
    for (j = i; j < gcpavi - 1; j++)
    {
        gapavi[j] = gapavi[j + 1];
        galpAVIOptions[j] = galpAVIOptions[j + 1];
        gapgf[j] = gapgf[j + 1];
        ghdd[j] = ghdd[j + 1];
    }
    gcpavi--;
}
LRESULT CPlayplusView::OnMM_WOM_DONE(WPARAM /*parm1*/, LPARAM /*parm2*/)
{
    aviaudioMessage(viewWnd, MM_WOM_DONE, 0, 0);

    return 0;
}

void CPlayplusView::OnAudioRemoveexistingaudiotracks()
{
    RemoveExistingAudioTracks();
    fileModified = 1;
}

void CPlayplusView::OnAudioAddaudiofromwavefile()
{
    char Buffer[BUFSIZE];
    char FileName[BUFSIZE];
    char FileTitle[BUFSIZE];

    OPENFILENAME ofn;
    FileName[0] = 0;
    FileTitle[0] = 0;

    LoadString(ghInstApp, IDS_OPENWAVE, Buffer, BUFSIZE);

    memset(&ofn, 0, sizeof(ofn));
    ofn.lpstrTitle = Buffer;
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = m_hWnd;
    ofn.lpstrFilter = "Audio Wave Files\0*.wav\0\0";
    ofn.lpstrFile = FileName;
    ofn.nMaxFile = sizeof(FileName);
    ofn.lpstrFileTitle = FileTitle;
    ofn.nMaxFileTitle = sizeof(FileTitle);
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    // If filename exists
    if (GetOpenFileName(&ofn))
    {
        // AddAudioWaveFile(FileName);
        // TimeDependentInsert(FileName);

        if (giFirstAudio >= 0)
        {
            // this case has no need to padd silence (except at the end)
            // so the following code should be used with confidence

            // The silence file is still unable to work...unless it is converted...??
            CreateSilenceFile();

            long shiftTime = GetScrollTime();
            if (shiftTime < 0)
                shiftTime = 0;
            if (shiftTime > timeEnd)
                shiftTime = timeEnd;
            TimeDependentInsert(FileName, shiftTime);
        }
        else
        {
            // Unable to measure startsample
            TimeDependentInsert(FileName, -1);
        }
        // Unable to insert silence for wave files becuase no slience file has been created
        // TimeDependentInsert(FileName,-1);

        fileModified = 1;
    }
}

void RemoveExistingAudioTracks()
{
    AVISTREAMINFO avis;
    int i;
    for (i = gcpavi - 1; i >= 0; i--)
    {
        AVIStreamInfo(gapavi[i], &avis, sizeof(avis));
        if (avis.fccType == streamtypeAUDIO)
        {
            NukeAVIStream(i);
        }
    }
    giFirstAudio = -1;

    RecomputeStreamsTime(RESET_TO_START, 0);
}

void AddAudioWaveFile(char *FileName)
{
    // if merging (ie audio exists).. then we do not use initavi..but overwrite existing...

    InitAvi(FileName, MENU_MERGE);
}

void CPlayplusView::OnUpdateAudioRemoveexistingaudiotracks(CCmdUI *pCmdUI)
{
    // TODO: Add your command update UI handler code here
    BOOL enablebutton = (!gfPlaying) && (giFirstAudio >= 0) && (giFirstVideo >= 0) && (!gfRecording);
    pCmdUI->Enable(enablebutton);
}

void CPlayplusView::OnUpdateAudioAddaudiofromwavefile(CCmdUI *pCmdUI)
{
    // TODO: Add your command update UI handler code here
    BOOL enablebutton;
    enablebutton = (!gfPlaying) && (giFirstVideo >= 0) && (!gfRecording);

    pCmdUI->Enable(enablebutton);
}

void CPlayplusView::OnFileSaveas()
{
    OPENFILENAME openfilename;
    gszSaveFileName[0] = 0;

    LoadString(ghInstApp, IDS_SAVETITLE, gszBuffer, BUFSIZE);
    openfilename.lStructSize = sizeof(OPENFILENAME);
    openfilename.lpstrTitle = gszBuffer;
    openfilename.hwndOwner = viewWnd;
    openfilename.hInstance = NULL;

    // AVIBuildFilter(gszFilter, sizeof(gszFilter), TRUE);
    // openfilename.lpstrFilter = gszFilter;
    openfilename.lpstrFilter = "AVI Movie Files (*.avi)\0*.avi\0\0";
    openfilename.lpstrCustomFilter = NULL;
    openfilename.nMaxCustFilter = 0;
    openfilename.nFilterIndex = 0;
    openfilename.lpstrFile = gszSaveFileName;
    openfilename.nMaxFile = sizeof(gszSaveFileName);
    openfilename.lpstrFileTitle = NULL;
    openfilename.nMaxFileTitle = 0;
    openfilename.lpstrInitialDir = NULL;
    openfilename.Flags = OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
    openfilename.nFileOffset = 0;
    openfilename.nFileExtension = 0;

    LoadString(ghInstApp, IDS_DEFEXT, gszExtBuffer, BUFSIZE);
    openfilename.lpstrDefExt = gszExtBuffer;
    openfilename.lCustData = 0;
    openfilename.lpfnHook = NULL;
    openfilename.lpTemplateName = NULL;

    // If we get a filename, save it
    if (GetSaveFileName(&openfilename))
    {
        DWORD fccHandler[MAXNUMSTREAMS];
        HRESULT hr;

        // StartWait();

        for (int i = 0; i < gcpavi; i++)
            fccHandler[i] = galpAVIOptions[i]->fccHandler;

        SetAdditionalCompressSettings(bAudioCompression, pwfx, cbwfx, interleaveFrames, interleaveFactor,
                                      interleaveUnit);

        hr = AVISaveV(gszSaveFileName, NULL, (AVISAVECALLBACK)SaveCallback, gcpavi, gapavi, galpAVIOptions);

        if (hr != AVIERR_OK)
        {
            switch (hr)
            {
                case AVIERR_FILEOPEN:
                    LoadString(ghInstApp, IDS_ERROVERWRITE, gszBuffer, BUFSIZE);
                    ErrMsg(gszBuffer);
                    break;
                default:
                    LoadString(ghInstApp, IDS_SAVEERROR, gszBuffer, BUFSIZE);
                    ErrMsg(gszBuffer);
            }
        }
        else
            fileModified = 0;

        // Now put the video compressors back that we stole
        for (int i = 0; i < gcpavi; i++)
            galpAVIOptions[i]->fccHandler = fccHandler[i];

        // EndWait();
        FixWindowTitle();
    }
}

void CPlayplusView::OnAudioAudiooptions()
{
    AudioFormat aod;
    aod.DoModal();
}

// proven: after edit paste, the start playing at non zero can easiy crash!
// this is due to the AVISTREAMREAD !!
#define AUDIO_BUFFER_SIZE 16384
void AuditAudio(PAVISTREAM pavi, long startsample)
{
    LONG lRead;
    LONG lSamplesToPlay;
    long slSampleSize;
    long slEnd, slBegin, slCurrent;

    slBegin = AVIStreamStart(pavi);
    slEnd = AVIStreamEnd(pavi);

    if (startsample >= 0)
        slBegin = startsample;

    slCurrent = slBegin;

    void *buffer = malloc(AUDIO_BUFFER_SIZE);
    long retlen = 0;

    AVISTREAMINFO strhdr;
    AVIStreamInfo(pavi, &strhdr, sizeof(strhdr));
    slSampleSize = (LONG)strhdr.dwSampleSize;

    if (slSampleSize <= 0 || slSampleSize > AUDIO_BUFFER_SIZE)
    {
        free(buffer);
        ErrMsg("Not Pass");
        return;
    }
    lSamplesToPlay = slEnd - slCurrent;
    if (lSamplesToPlay > AUDIO_BUFFER_SIZE / slSampleSize)
        lSamplesToPlay = AUDIO_BUFFER_SIZE / slSampleSize;

    ErrMsg("slCurrent %ld, slEnd %ld, lSamplesToPlay %ld", slCurrent, slEnd, lSamplesToPlay);

    while (slCurrent < slEnd)
    {
        // ErrMsg("slCurrent %ld, lSamplesToPlay %ld, toplay %ld",slCurrent, lSamplesToPlay, slEnd - slCurrent);
        /*long retval = */ (void)AVIStreamRead(pavi, slCurrent, lSamplesToPlay, buffer, AUDIO_BUFFER_SIZE, &retlen,
                                               &lRead);

        slCurrent += lRead;
    }
    if (buffer)
        free(buffer);

    ErrMsg("Ok");
}

void CPlayplusView::OnZoomTestaudio()
{
    long sPos = GetScrollTime();
    long samplepos = SafeStreamTimeToSample(gapavi[giFirstAudio], sPos);
    AuditAudio(gapavi[giFirstAudio], samplepos);
}

void Msg(const char fmt[], ...)
{
    static int debug = GetPrivateProfileInt("debug", "log", 0, "huffyuv.ini");
    if (!debug)
        return;

    DWORD written;
    char buf[2000];
    va_list val;

    va_start(val, fmt);
    wvsprintf(buf, fmt, val);
    va_end(val);

    const COORD _80x50 = {80, 50};
    static BOOL startup = (AllocConsole(), SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), _80x50));
    WriteConsole(GetStdHandle(STD_OUTPUT_HANDLE), buf, lstrlen(buf), &written, 0);
}

void MsgC(const char fmt[], ...)
{
    DWORD written;
    char buf[2000];
    va_list val;

    va_start(val, fmt);
    wvsprintf(buf, fmt, val);
    va_end(val);

    const COORD _80x50 = {80, 50};
    static BOOL startup = (AllocConsole(), SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), _80x50));
    WriteConsole(GetStdHandle(STD_OUTPUT_HANDLE), buf, lstrlen(buf), &written, 0);
}

void DumpFormat(WAVEFORMATEX *p_wfx, const char *str)
{
    Msg("\n Str %s: wFormatTag %d channels %d nSamplesPerSec %d abytesPerSec %d blockalign %d wBitsPerSample %d cbSize "
        " %d",
        str, p_wfx->wFormatTag, p_wfx->nChannels, p_wfx->nSamplesPerSec, p_wfx->nAvgBytesPerSec, p_wfx->nBlockAlign,
        p_wfx->wBitsPerSample, p_wfx->cbSize);
}

/////////////////////////////////////////////////////////////////////////////
// Some of the follwing functions are not used because they are unstable
/////////////////////////////////////////////////////////////////////////////

//////////////////////////////

// 1)function fails (in streamcut) if stream length is smaller then stream start
// 2)editpaste : if pasting at point much further than the end of the stream, the pasting point is set to the end
// instead rathr than the specified point that is beyond this may cause the cutting to fails...

// The plLength returns the number of bytes *removed* from the original streams
// This value may be *different* from the number of bytes inserted into the stream

// This function allows the lStart to be larger than the existing stream length
// In this case, the stream will be padded with silence
int EditStreamReplace(PAVISTREAM pavi, LONG *plPos, LONG *plLength, PAVISTREAM pstream, LONG lStart, LONG lLength)
{
    // dumping of format indicates they are the same format
    long cbFormat;
    WAVEFORMATEX *lpFormat, *orgFormat;

    AVIStreamFormatSize(pstream, 0, &cbFormat);
    lpFormat = (WAVEFORMATEX *)GlobalAllocPtr(GHND, cbFormat);
    AVIStreamReadFormat(pstream, 0, lpFormat, &cbFormat);

    AVIStreamFormatSize(pavi, 0, &cbFormat);
    orgFormat = (WAVEFORMATEX *)GlobalAllocPtr(GHND, cbFormat);
    AVIStreamReadFormat(pavi, 0, orgFormat, &cbFormat);

    DumpFormat((WAVEFORMATEX *)galpAVIOptions[giFirstAudio]->lpFormat, "1stAud (Existing)");
    DumpFormat((WAVEFORMATEX *)&m_Format, "Working    Format");
    DumpFormat((WAVEFORMATEX *)lpFormat, "To  replace  with");
    DumpFormat((WAVEFORMATEX *)orgFormat, "pavi(same as 1st)");

    int value = 0;

    // Add Silence if start point is larger than existing stream length
    long startPos = *plPos;
    long silenceLengthPasted = *plLength;
    value = EditStreamPadSilence(pavi, &startPos, &silenceLengthPasted);
    if (value)
        return value;

    // ErrMsg("pavi %ld, *plPos %ld, *plLength %ld, pstream %ld, lStart %ld,lLength %ld",pavi, *plPos, *plLength,
    // pstream, lStart,lLength);
    /* ??? if (value = EditStreamPaste(pavi, plPos, plLength, pstream, lStart,lLength) !=  AVIERR_OK) */
    value = EditStreamPaste(pavi, plPos, plLength, pstream, lStart, lLength);
    if (value != AVIERR_OK)
    {
        ErrMsg("Unable to add audio at insertion point (Stream Replace)");
    }
    // ErrMsg("pavi %ld, *plPos %ld, *plLength %ld, pstream %ld, lStart %ld,lLength %ld",pavi, *plPos, *plLength,
    // pstream, lStart,lLength);

    PAVISTREAM tempStream;
    long cutStartPoint;
    cutStartPoint = *plPos + *plLength;

    long lx = AVIStreamLength(pavi);
    if (lx >= (cutStartPoint + *plLength))
    {
        // if pavi is long enough to be cut
        // do nothing...
    }
    else
    {
        // if not, try to cut from startpoint to
        *plLength = lx - cutStartPoint - 1;
    }
    // ErrMsg("toCut_length plength %ld cutStartPoint %ld AVIStreamLength %ld", (*plLength) ,cutStartPoint,lx);

    // restore

    if (*plLength > 0)
    {
        /* ??? if (value = EditStreamCut(pavi, &cutStartPoint, plLength,  &tempStream ) !=  AVIERR_OK) */
        value = EditStreamCut(pavi, &cutStartPoint, plLength, &tempStream);
        if (value != AVIERR_OK)
        {
            ErrMsg("Unable to remove audio at replace point (Stream Replace)");
        }
        AVIStreamRelease(tempStream);
    }
    return value;
}

// Will pad silence from end of stream to plPos if plPos>length(pavi)
//  before pad> |---org stream-----|                         plPos
//  after  pad> |---org stream-----|_____silence_padded_____|plPos
int EditStreamPadSilence(PAVISTREAM pavi, LONG *plPos, LONG *plLength)
{
    HRESULT hr;

    long lz = AVIStreamLength(pavi);
    long startPasteLocation = *plPos;

    // CString msgstr;
    // msgstr.Format("lz %ld startPasteLocation %ld silenceFileValid %d",lz, startPasteLocation, silenceFileValid);
    // MessageBox(NULL,msgstr,"Note",MB_OK | MB_ICONEXCLAMATION);

    if (startPasteLocation >= lz)
    {
        if (silenceFileValid)
        {
            long SilencePasteLength = startPasteLocation - (lz - 1);

            PAVISTREAM paviSilence;
            PAVIFILE pfileSilence;

            hr = AVIFileOpen(&pfileSilence, tempsilencepath, 0, 0L);

            if (hr != 0)
            {
                ErrMsg("Unable to open silence file");
                return hr;
            }
            hr = AVIFileGetStream(pfileSilence, &paviSilence, streamtypeAUDIO, 0);
            if (hr != AVIERR_OK)
            {
                ErrMsg("Unable to load silence stream");
                return hr;
            }
            long SilenceStreamLength = AVIStreamLength(paviSilence);
            // long initialStreamEnd = AVIStreamEnd(pavi)+1;
            long initialStreamEnd = AVIStreamEnd(pavi);
            long totalPastedLength = 0;
            while (SilencePasteLength > 0)
            {
                LONG PastedLength;
                LONG lengthToPaste;

                if (SilencePasteLength >= SilenceStreamLength)
                    lengthToPaste = SilenceStreamLength; // restricting to ceiling of SilenceStreamLength
                else
                    lengthToPaste = SilencePasteLength;

                // CString msgstr;
                // msgstr.Format("initialStreamEnd %ld lengthToPaste %ld SilenceStreamLength %ld totalPastedLength
                // %ld,SilencePasteLength %ld",initialStreamEnd, lengthToPaste,
                // SilenceStreamLength,totalPastedLength,SilencePasteLength); MessageBox(NULL,msgstr,"Note",MB_OK |
                // MB_ICONEXCLAMATION);

                hr = EditStreamPaste(pavi, &initialStreamEnd, &PastedLength, paviSilence, 0, lengthToPaste);
                if (hr != AVIERR_OK)
                {
                    ErrMsg("Unable to pad silence to existing stream at position %ld (Stream Replace)",
                           initialStreamEnd);
                    return hr;
                }
                totalPastedLength += PastedLength;
                lz = AVIStreamLength(pavi);
                SilencePasteLength = startPasteLocation - (lz - 1);
            }
            AVIStreamRelease(paviSilence);
            AVIFileRelease(pfileSilence);

            *plLength = totalPastedLength;
            *plPos = initialStreamEnd;
        }
        else
        {
            // no silence added
            ErrMsg("Invalid Silence File! No audio [silence] added");

            // return -1;
        }
    }
    return 0;
}

// Will insert silence at begining of stream to shift the original stream to start sounding at a later time
int EditStreamSilenceShift(PAVISTREAM pavi, LONG *plPos, LONG *plLength)
{
    HRESULT hr;

    /* long lz = */ (void)AVIStreamLength(pavi);
    long ShiftToLocation = *plPos;
    long ShiftFromLocation = AVIStreamStart(pavi);

    if (ShiftToLocation > ShiftFromLocation)
    {
        if (silenceFileValid)
        {
            long SilencePasteLength = ShiftToLocation - ShiftFromLocation;

            PAVISTREAM paviSilence;
            PAVIFILE pfileSilence;

            hr = AVIFileOpen(&pfileSilence, tempsilencepath, 0, 0L);

            if (hr != 0)
            {
                ErrMsg("Unable to open silence file (2) ");
                return hr;
            }
            hr = AVIFileGetStream(pfileSilence, &paviSilence, streamtypeAUDIO, 0);
            if (hr != AVIERR_OK)
            {
                ErrMsg("Unable to load silence stream (2)");
                return hr;
            }
            long SilenceStreamLength = AVIStreamLength(paviSilence);
            long StreamPastePoint = ShiftFromLocation;
            long totalPastedLength = 0;
            while (SilencePasteLength > 0)
            {
                LONG PastedLength;
                LONG lengthToPaste;

                if (SilencePasteLength >= SilenceStreamLength)
                    lengthToPaste = SilenceStreamLength; // restricting to ceiling of SilenceStreamLength
                else
                    lengthToPaste = SilencePasteLength;

                // CString msgstr;
                // msgstr.Format("initialStreamEnd %ld lengthToPaste %ld SilenceStreamLength %ld totalPastedLength
                // %ld,SilencePasteLength %ld",initialStreamEnd, lengthToPaste,
                // SilenceStreamLength,totalPastedLength,SilencePasteLength); MessageBox(NULL,msgstr,"Note",MB_OK |
                // MB_ICONEXCLAMATION);

                PastedLength = 0;
                hr = EditStreamPaste(pavi, &StreamPastePoint, &PastedLength, paviSilence, 0, lengthToPaste);
                if (hr != AVIERR_OK)
                {
                    ErrMsg("Unable to pad silence to existing stream at position %ld (2)", StreamPastePoint);
                    return hr;
                }
                if (PastedLength <= 0)
                {
                    ErrMsg("Unable to pad silence ! Pad Length <= 0 ! (2)");
                    return -1;
                }
                totalPastedLength += PastedLength;
                SilencePasteLength = SilencePasteLength - PastedLength;
            }
            AVIStreamRelease(paviSilence);
            AVIFileRelease(pfileSilence);

            *plLength = totalPastedLength;
            *plPos = StreamPastePoint;
        }
        else
        {
            // no silence added
            ErrMsg("Invalid Silence File! No audio [silence] added (2)");

            // return -1;
        }
    }
    return 0;
}

// Returns a sample that correspond to the lTime of the stream
// The lTime can be *larger* than  the length of the original pavi
long ExAVIStreamTimeToSample(PAVISTREAM pavi, LONG lTime)
{
    AVISTREAMINFO avis;
    AVIStreamInfo(pavi, &avis, sizeof(avis));

    long sampleStreamStart;
    long sampleStreamEnd;
    long timeStreamStart;
    long timeStreamEnd;

    sampleStreamStart = AVIStreamStart(pavi);
    sampleStreamEnd = AVIStreamEnd(pavi);
    timeStreamStart = AVIStreamStartTime(pavi);
    timeStreamEnd = AVIStreamEndTime(pavi);

    // double timefrac = ((double ) lTime-timeStreamStart)/((double) (timeStreamEnd -  timeStreamStart));
    // timefrac =timefrac * ((double) (sampleStreamEnd - sampleStreamStart));
    // long sampleAtTime =  ((long) timefrac) + sampleStreamStart;
    // ErrMsg("lTime %ld sampletime %d sampleStreamStart %ld sampleStreamEnd %ld timeStreamStart %ld timeStreamEnd
    // %ld",lTime,sampletime, sampleStreamStart,sampleStreamEnd,timeStreamStart,timeStreamEnd); return sampleAtTime;

    long sampleByAVIS = (long)(((double)(avis.dwRate / avis.dwScale)) * ((double)(lTime - timeStreamStart) / 1000.0)) +
                        sampleStreamStart;
    // ErrMsg("lTime %ld sampleAttime %ld sampleByAVIS %ld",lTime, sampleAtTime, sampleByAVIS);
    return sampleByAVIS;
}

long SafeStreamTimeToSample(PAVISTREAM pavi, LONG starttime)
{
    long startsample = 0;
    if (starttime > AVIStreamLengthTime(pavi))
        startsample = ExAVIStreamTimeToSample(pavi, starttime);
    else
        startsample = AVIStreamTimeToSample(pavi, starttime);

    if (startsample <= -1)
        startsample = 0;

    return startsample;
}

// The interleave factor is set * correctly * (i.e adjusted with respect to the interleaveUnit of MILLISECONDS or FRAMES
// for saving on other occasations, it is assumed to be 1 (and is not used?)
void SetAdditionalCompressSettings(BOOL recompress_audio, LPWAVEFORMATEX audio_recompress_format,
                                   DWORD audio_format_size, BOOL bInterleave, int interleave_factor,
                                   int interleave_unit)
{
    int frames_per_second = -1;

    for (int i = 0; i < gcpavi; i++)
    {
        // use the firstvideo to calculate the frames per seconds for use in interleave
        if (bInterleave)
        {
            if (i == giFirstVideo)
            {
                AVISTREAMINFO avis;
                AVIStreamInfo(gapavi[giFirstVideo], &avis, sizeof(avis));

                if (avis.dwScale > 0)
                    frames_per_second = avis.dwRate / avis.dwScale;
            }
            else
            {
                // Do nothing
            }
        }
    }
    for (int i = 0; i < gcpavi; i++)
    {
        if (bInterleave)
        {
            galpAVIOptions[i]->dwFlags = galpAVIOptions[i]->dwFlags | AVICOMPRESSF_INTERLEAVE;

            if (interleave_unit == FRAMES)
            {
                galpAVIOptions[i]->dwInterleaveEvery = interleave_factor;
            }
            else
            {
                // Interleave by milliseconds
                if (frames_per_second > 0)
                {
                    double interfloat = (((double)interleaveFactor) * ((double)frames_per_second)) / 1000.0;
                    int interint = (int)interfloat;
                    if (interint <= 0)
                    {
                        interint = 1;
                    }
                    galpAVIOptions[i]->dwInterleaveEvery = interint;
                }
                else
                {
                    galpAVIOptions[i]->dwInterleaveEvery = interleave_factor;
                }
            }
        }
        else
        {
            galpAVIOptions[i]->dwFlags = galpAVIOptions[i]->dwFlags & ~AVICOMPRESSF_INTERLEAVE;
            galpAVIOptions[i]->dwInterleaveEvery = 1;
        }
        if (recompress_audio)
        {
            if (i == giFirstAudio)
            {
                if (galpAVIOptions[i]->lpFormat)
                {
                    GlobalFreePtr(galpAVIOptions[i]->lpFormat);
                }
                galpAVIOptions[i]->cbFormat = audio_format_size;
                galpAVIOptions[i]->lpFormat = GlobalAllocPtr(GHND, audio_format_size);
                memcpy((void *)galpAVIOptions[i]->lpFormat, (void *)audio_recompress_format, audio_format_size);
            }
        }
        else
        {
            // Do nothing
        }
    }
}

void CPlayplusView::OnUpdateFileSaveas(CCmdUI *pCmdUI)
{
    pCmdUI->Enable(giFirstVideo >= 0);
}

// * Only use this function to perform the conversion *
// Put inside insertAVIFILE for NEW_AUDIO_TRACK case
// The rest of the conversion helper functions (except TestConvert) should be protected from use in other functions/code

// Important, need to call RecomputeStreamsTime after this function
// RecomputeStreamsTime not called inside ReiInitStreams becuase thiss allow flexibility in setting the slider
void TestConvertFirstAudio()
{
    // restore
    return;

#ifdef UNUSED_CODE
    // if player mode..returns
    if (pmode == PLAYER)
        return;

    if (giFirstAudio >= 0)
    {
        PAVISTREAM paviConverted = NULL;
        int retval = TestConvert(gapavi[giFirstAudio], &paviConverted, NULL);
        if (retval > 0)
        {
            // Conversion is performed
            CloneAudioStream_ReplaceStreamPool(giFirstAudio, paviConverted);
            // no need to free the streams as ReplaceStreamPool already does that
        }
        else if (retval == 0)
        {
            // no conversion necessary
            ErrMsg("No Conversion Performed");
        }
        else
        {
            // Error
            ErrMsg("Error in Conversion");
            // no need to free resources as gapavi[giFirstAudio] remains valid and paviConverted has not been used
        }
    }
#endif // UNUSED_CODE
}

// Note : Using test convert will affect the behavior of player
// use an option that can turn test convert off

// Creates an Editable copy of pavi and use it to replace the orignal stream i
void CloneAudioStream_ReplaceStreamPool(int i, PAVISTREAM pavi)
{
    if (i < 0 || i >= gcpavi)
        return;

    // Creates an editable clone of pavi and set it to
    PAVISTREAM paviConverted;
    if (CreateEditableStream(&paviConverted, pavi) != AVIERR_OK)
    {
        // error
        ErrMsg("Unable to Create Editable Stream for Converted Audio");

        // will not affect original stream

        return;
    }
    else
    {
        // Free all the resources associated with old stream
        AVIStreamRelease(gapavi[i]);
        if (galpAVIOptions[i]->lpFormat)
        {
            GlobalFreePtr(galpAVIOptions[i]->lpFormat);
        }
        if (gapgf[i])
        {
            AVIStreamGetFrameClose(gapgf[i]);
            gapgf[i] = NULL;
        }
        if (ghdd[i])
        {
            DrawDibClose(ghdd[i]);
            ghdd[i] = 0;
        }
        gapavi[i] = paviConverted;
        galSelStart[i] = galSelLen[i] = -1;

        ReInitAudioStream(i);
    }
}

// This function should be called inside initstreams before the line
// timeLength = timeEnd - timeStart;
void ReInitAudioStream(int i)
{
    // if (giFirstAudio>=0)
    //    i=giFirstAudio;

    AVISTREAMINFO avis;
    AVIStreamInfo(gapavi[i], &avis, sizeof(avis));
    galpAVIOptions[i] = &gaAVIOptions[i];
    _fmemset(galpAVIOptions[i], 0, sizeof(AVICOMPRESSOPTIONS));
    galpAVIOptions[i]->fccType = avis.fccType;

    // case streamtypeAUDIO:
    LONG lTemp;
    galpAVIOptions[i]->dwFlags |= AVICOMPRESSF_VALID;
    galpAVIOptions[i]->dwInterleaveEvery = 1;
    AVIStreamReadFormat(gapavi[i], AVIStreamStart(gapavi[i]), NULL, &lTemp);
    galpAVIOptions[i]->cbFormat = lTemp;
    if (lTemp)
        galpAVIOptions[i]->lpFormat = GlobalAllocPtr(GHND, lTemp);

    // Use current format as default format
    if (galpAVIOptions[i]->lpFormat)
        AVIStreamReadFormat(gapavi[i], AVIStreamStart(gapavi[i]), galpAVIOptions[i]->lpFormat, &lTemp);

    // We're finding the earliest and latest start and end points for
    // our scrollbar.
    // timeStart = min(timeStart, AVIStreamStartTime(gapavi[i]));
    // timeEnd   = max(timeEnd, AVIStreamEndTime(gapavi[i]));

    ghdd[i] = NULL;
    gapgf[i] = NULL;

    // RecomputeStreamsTime(RESET_TO_START,0);
}

void CPlayplusView::OnUpdateFilePlay(CCmdUI *pCmdUI)
{
    // TODO: Add your command update UI handler code here
    BOOL ev = ((!gfPlaying) && (!gfRecording));
    pCmdUI->Enable(ev);
}

void CPlayplusView::OnAudioExtension()
{
    // TODO: Add your command handler code here
    if (allowRecordExtension)
        allowRecordExtension = 0;
    else
        allowRecordExtension = 1;
}

void CPlayplusView::OnUpdateAudioExtension(CCmdUI *pCmdUI)
{
    // TODO: Add your command update UI handler code here
    pCmdUI->SetCheck(!allowRecordExtension);
}

/////////////////////////////////////
// more stable methods / functions //
/////////////////////////////////////

CString GetTempPathEx(CString fileName, CString fxstr, CString exstr)
{
    CString tempPathEx = GetTempPath() + fileName;

    // Test the validity of writing to the file
    int fileverified = 0;
    while (!fileverified)
    {
        OFSTRUCT ofstruct;
        HFILE fhandle = OpenFile(tempPathEx, &ofstruct, OF_SHARE_EXCLUSIVE | OF_WRITE | OF_CREATE);
        if (fhandle != HFILE_ERROR)
        {
            fileverified = 1;
            CloseHandle((HANDLE)fhandle);

            DeleteFile(tempPathEx);
        }
        else
        {
            srand((unsigned)time(NULL));
            int randnum = rand();
            char numstr[50];
            sprintf(numstr, "%d", randnum);

            CString cnumstr(numstr);

            tempPathEx = GetTempPath() + fxstr + cnumstr + exstr;
        }
    }
    return tempPathEx;
}

// Code for open link
BOOL CPlayplusView::Openlink(CString link)
{
    BOOL bSuccess = FALSE;

    // As a last resort try ShellExecuting the URL, may
    // even work on Navigator!
    if (!bSuccess)
        bSuccess = OpenUsingShellExecute(link);

    if (!bSuccess)
        bSuccess = OpenUsingRegisteredClass(link);
    return bSuccess;
}

BOOL CPlayplusView::OpenUsingShellExecute(CString link)
{
    LPCTSTR mode;
    mode = _T ("open");

    // HINSTANCE hRun = ShellExecute (GetParent ()->GetSafeHwnd (), mode, m_sActualLink, NULL, NULL, SW_SHOW);
    HINSTANCE hRun = ShellExecute(GetSafeHwnd(), mode, link, NULL, NULL, SW_SHOW);
    if ((int)hRun <= HINSTANCE_ERROR)
    {
        TRACE("Failed to invoke URL using ShellExecute\n");
        return FALSE;
    }
    return TRUE;
}

BOOL CPlayplusView::OpenUsingRegisteredClass(CString link)
{
    TCHAR key[MAX_PATH + MAX_PATH];
    HINSTANCE result;

    if (GetRegKey(HKEY_CLASSES_ROOT, _T (".htm"), key) == ERROR_SUCCESS)
    {
        LPCTSTR mode;
        mode = _T ("\\shell\\open\\command");
        _tcscat(key, mode);
        if (GetRegKey(HKEY_CLASSES_ROOT, key, key) == ERROR_SUCCESS)
        {
            LPTSTR pos;
            pos = _tcsstr(key, _T ("\"%1\""));
            if (pos == NULL)
            { // No quotes found

                pos = strstr(key, _T ("%1")); // Check for %1, without quotes

                if (pos == NULL) // No parameter at all...

                    pos = key + _tcslen(key) - 1;
                else
                    *pos = _T('\0'); // Remove the parameter
            }
            else
                *pos = _T('\0'); // Remove the parameter

            _tcscat(pos, _T (" "));
            _tcscat(pos, link);
            result = (HINSTANCE)WinExec(key, SW_SHOW);
            if ((int)result <= HINSTANCE_ERROR)
            {
                CString str;
                switch ((int)result)
                {
                    case 0:
                        str = _T ("The operating system is out\nof memory or resources.");
                        break;
                    case SE_ERR_PNF:
                        str = _T ("The specified path was not found.");
                        break;
                    case SE_ERR_FNF:
                        str = _T ("The specified file was not found.");
                        break;
                    case ERROR_BAD_FORMAT:
                        str = _T ("The .EXE file is invalid\n(non-Win32 .EXE or error in .EXE image).");
                        break;
                    case SE_ERR_ACCESSDENIED:
                        str = _T ("The operating system denied\naccess to the specified file.");
                        break;
                    case SE_ERR_ASSOCINCOMPLETE:
                        str = _T ("The filename association is\nincomplete or invalid.");
                        break;
                    case SE_ERR_DDEBUSY:
                        str =
                            _T ("The DDE transaction could not\nbe completed because other DDE transactions\nwere being processed.");
                        break;
                    case SE_ERR_DDEFAIL:
                        str = _T ("The DDE transaction failed.");
                        break;
                    case SE_ERR_DDETIMEOUT:
                        str = _T ("The DDE transaction could not\nbe completed because the request timed out.");
                        break;
                    case SE_ERR_DLLNOTFOUND:
                        str = _T ("The specified dynamic-link library was not found.");
                        break;
                    case SE_ERR_NOASSOC:
                        str = _T ("There is no application associated\nwith the given filename extension.");
                        break;
                    case SE_ERR_OOM:
                        str = _T ("There was not enough memory to complete the operation.");
                        break;
                    case SE_ERR_SHARE:
                        str = _T ("A sharing violation occurred.");
                        break;
                    default:
                        str.Format(_T ("Unknown Error (%d) occurred."), (int)result);
                }
                str = _T ("Unable to open hyperlink:\n\n") + str;

                AfxMessageBox(str, MB_ICONEXCLAMATION | MB_OK);
            }
            else
                return TRUE;
        }
    }
    return FALSE;
}

LONG CPlayplusView::GetRegKey(HKEY key, LPCTSTR subkey, LPTSTR retdata)
{
    HKEY hkey;
    LONG retval = RegOpenKeyEx(key, subkey, 0, KEY_QUERY_VALUE, &hkey);

    if (retval == ERROR_SUCCESS)
    {
        long datasize = MAX_PATH;
        TCHAR data[MAX_PATH];
        RegQueryValue(hkey, NULL, data, &datasize);
        _tcscpy(retdata, data);
        RegCloseKey(hkey);
    }
    return retval;
}

//////////////////////////
// ver 2.2
// Flash Output

int CChangeRectSwf ::initialize(int _blockx, int _blocky, int _blocksizex, int _blocksizey, int _blockwidth,
                                int _pixelwidth, int _pixelheight, int x, int y)
{
    blockx = _blockx;
    blocky = _blocky;
    blocksizex = _blocksizex;
    blocksizey = _blocksizey;
    blockwidth = _blockwidth;

    pixelwidth = _pixelwidth;
    pixelheight = _pixelheight;

    blockbounds.left = blockx * blocksizex;
    blockbounds.right = blockbounds.left + blocksizex - 1;
    blockbounds.top = blocky * blocksizey;
    blockbounds.bottom = blockbounds.top + blocksizey - 1;

    greatestLeft = x;
    greatestTop = y;
    smallestRight = x;
    smallestBottom = y;

    /*
    if (pixelwidth < blockbounds.right)
    {
    if (pixelwidth < blockbounds.left)
    {
    blockx = -1; //to be remained uninitialized
    blocky = -1;
    return 1;
    }
    else
    {
    blockbounds.right = pixelwidth-1;
    if (blockbounds.right < blockbounds.left)
    {
    blockx = -1; //to be remained uninitialized
    blocky = -1;
    return 1;

    }
    }
    }
    */

    return 0;
}

CArray<CChangeRectSwf *, CChangeRectSwf *> changeArray;

int IsDifferent(LPBITMAPINFOHEADER alpbi, int BITMAP_X, int BITMAP_Y, int x, int y, int format);
void AddExpandBlock(int BITMAP_X, int BITMAP_Y, int x, int y, int format);
void AddNewBlock(int BITMAP_X, int BITMAP_Y, int x, int y, int format);
void ExpandBlock(CChangeRectSwf *itemRect, int BITMAP_X, int BITMAP_Y, int x, int y, int format);
LPBYTE MakeFullRect(LPBITMAPINFOHEADER alpbi, LPBYTE bitmap, int BITMAP_X, int BITMAP_Y, int format, int max);
double ComputePercentCovered(int BITMAP_X, int BITMAP_Y);

void CreatePlayButton(std::ostringstream &f, int controlsWidth, int controlsHeight, int FrameOffsetX, int FrameOffsetY,
                      int BITMAP_X, int BITMAP_Y);
void CreatePauseButton(std::ostringstream &f, int controlsWidth, int controlsHeight, int FrameOffsetX, int FrameOffsetY,
                       int BITMAP_X, int BITMAP_Y);
void CreateStopButton(std::ostringstream &f, int controlsWidth, int controlsHeight, int FrameOffsetX, int FrameOffsetY,
                      int BITMAP_X, int BITMAP_Y);
void CreateBackgroundBar(std::ostringstream &f, int controlsWidth, int controlsHeight, int FrameOffsetX,
                         int FrameOffsetY, int BITMAP_X, int BITMAP_Y);

LPBYTE makeReverse32(int width, int height, int /*bitPlanes*/, LPBITMAPINFOHEADER alpbi)
{
    LPBYTE bitsOrg = (LPBYTE)alpbi + // pointer to data
                     alpbi->biSize + alpbi->biClrUsed * sizeof(RGBQUAD);

    UINT widthBytes = (width * 32 + 31) / 32 * 4;
    VertFlipBuf(bitsOrg, widthBytes, height);
    ARGBFromBGRA(bitsOrg, width, height);

    return bitsOrg;
}

LPBYTE makeReverse16(int width, int height, int bitPlanes, LPBITMAPINFOHEADER alpbi)
{
    LPBYTE bitsOrg = (LPBYTE)alpbi + // pointer to data
                     alpbi->biSize + alpbi->biClrUsed * sizeof(RGBQUAD);

    UINT widthBytes = (width * 16 + 31) / 32 * 4;
    VertFlipBuf(bitsOrg, widthBytes, height);
    swapPixelBytes16(width, height, bitPlanes, bitsOrg);

    return bitsOrg;
}

LPBYTE swapPixelBytes16(int width, int height, int /*bitPlanes*/, LPBYTE bits16)
{
    BYTE *tmp;
    tmp = bits16;

    long counter16 = 0;
    long rowcounter16 = 0;
    long widthBytes = (width * 16 + 31) / 32 * 4;
    for (int h = 0; h < height; h++)
    {
        counter16 = rowcounter16;
        for (int w = 0; w < width; w++)
        {
            WORD oldval;
            oldval = *((WORD *)(tmp + counter16));
            *((WORD *)(tmp + counter16)) = (oldval << 8) | (oldval >> 8);
            counter16 += 2;
        }
        rowcounter16 += widthBytes;
    }
    return tmp;
}

BOOL VertFlipBuf(BYTE *inbuf, UINT widthBytes, UINT height)
{
    BYTE *tb1;
    BYTE *tb2;

    if (inbuf == NULL)
        return FALSE;

    UINT bufsize;

    bufsize = widthBytes;

    tb1 = (BYTE *)new BYTE[bufsize];
    if (tb1 == NULL)
    {
        return FALSE;
    }
    tb2 = (BYTE *)new BYTE[bufsize];
    if (tb2 == NULL)
    {
        delete[] tb1;
        return FALSE;
    }
    UINT row_cnt;
    ULONG off1 = 0;
    ULONG off2 = 0;

    for (row_cnt = 0; row_cnt < (height + 1) / 2; row_cnt++)
    {
        off1 = row_cnt * bufsize;
        off2 = ((height - 1) - row_cnt) * bufsize;

        memcpy(tb1, inbuf + off1, bufsize);
        memcpy(tb2, inbuf + off2, bufsize);
        memcpy(inbuf + off1, tb2, bufsize);
        memcpy(inbuf + off2, tb1, bufsize);
    }
    delete[] tb1;
    delete[] tb2;

    return TRUE;
}

BOOL ARGBFromBGRA(BYTE *buf, UINT widthPix, UINT height)
{
    if (buf == NULL)
        return FALSE;

    UINT col, row;
    for (row = 0; row < height; row++)
    {
        for (col = 0; col < widthPix; col++)
        {
            LPBYTE pRed, pGrn, pBlu, pRes;
            pRed = buf + row * widthPix * 4 + col * 4;
            pGrn = buf + row * widthPix * 4 + col * 4 + 1;
            pBlu = buf + row * widthPix * 4 + col * 4 + 2;
            pRes = buf + row * widthPix * 4 + col * 4 + 3;

            // swap red and blue
            BYTE tmp;
            tmp = *pGrn;

            *pRes = *pRed;
            *pGrn = *pBlu;
            *pBlu = tmp;
            *pRed = 255; // alpha works for higer level ...code
        }
    }
    return TRUE;
}

LPBITMAPINFOHEADER GetFrame(HBITMAP hbm, LPBYTE *bits, int &BITMAP_X, int &BITMAP_Y, int numbits)
{
    if ((numbits == 16) || (numbits == 32))
    {
        LPBITMAPINFOHEADER alpbi = (LPBITMAPINFOHEADER)GlobalLock(Bitmap2Dib(hbm, numbits));

        if (alpbi)
        {
            BITMAP_X = alpbi->biWidth;
            BITMAP_Y = alpbi->biHeight;

            if (numbits == 16)
                *bits = makeReverse16(alpbi->biWidth, alpbi->biHeight, 2, alpbi);
            else
                *bits = makeReverse32(alpbi->biWidth, alpbi->biHeight, 4, alpbi);
        }
        return alpbi;
    }
    return NULL;
}

HANDLE Bitmap2Dib(HBITMAP hbitmap, UINT bits)
{
    HANDLE hdib;
    HDC hdc;
    BITMAP bitmap;
    UINT wLineLen;
    DWORD dwSize;
    DWORD wColSize;
    LPBITMAPINFOHEADER lpbi;
    LPBYTE lpBits;

    GetObject(hbitmap, sizeof(BITMAP), &bitmap);

    //
    // DWORD align the width of the DIB
    // Figure out the size of the colour table
    // Calculate the size of the DIB
    //
    wLineLen = (bitmap.bmWidth * bits + 31) / 32 * 4;
    wColSize = sizeof(RGBQUAD) * ((bits <= 8) ? 1 << bits : 0);
    dwSize = sizeof(BITMAPINFOHEADER) + wColSize + (DWORD)(UINT)wLineLen * (DWORD)(UINT)bitmap.bmHeight;

    //
    // Allocate room for a DIB and set the LPBI fields
    //
    hdib = GlobalAlloc(GHND, dwSize);
    if (!hdib)
        return hdib;

    lpbi = (LPBITMAPINFOHEADER)GlobalLock(hdib);

    lpbi->biSize = sizeof(BITMAPINFOHEADER);
    lpbi->biWidth = bitmap.bmWidth;
    lpbi->biHeight = bitmap.bmHeight;
    lpbi->biPlanes = 1;
    lpbi->biBitCount = (WORD)bits;
    lpbi->biCompression = BI_RGB;
    lpbi->biSizeImage = dwSize - sizeof(BITMAPINFOHEADER) - wColSize;
    lpbi->biXPelsPerMeter = 0;
    lpbi->biYPelsPerMeter = 0;
    lpbi->biClrUsed = (bits <= 8) ? 1 << bits : 0;
    lpbi->biClrImportant = 0;

    //
    // Get the bits from the bitmap and stuff them after the LPBI
    //
    lpBits = (LPBYTE)(lpbi + 1) + wColSize;

    hdc = CreateCompatibleDC(NULL);

    GetDIBits(hdc, hbitmap, 0, bitmap.bmHeight, lpBits, (LPBITMAPINFO)lpbi, DIB_RGB_COLORS);

    lpbi->biClrUsed = (bits <= 8) ? 1 << bits : 0;

    DeleteDC(hdc);
    GlobalUnlock(hdib);

    return hdib;
}

void FreeFrame(LPBITMAPINFOHEADER alpbi)
{
    if (!alpbi)
        return;

    GlobalFreePtr(alpbi);
    // GlobalFree(alpbi);
    alpbi = NULL;
}

// Create a new bitmap of the same width and height of alpbi
// Fill it with zeros,
// Then fill up the changeArray areas with alpbi data
LPBYTE MakeFullRect(LPBITMAPINFOHEADER /*alpbi*/, LPBYTE bitmap, int BITMAP_X, int BITMAP_Y, int format, int max)
{
    int imageLineLen, pixelbytes;
    CChangeRectSwf *itemRect = NULL;

    if (format == 4)
    {
        imageLineLen = ((BITMAP_X)*16 + 31) / 32 * 4;
        pixelbytes = 2;
    }
    else if (format == 5)
    {
        imageLineLen = ((BITMAP_X)*32 + 31) / 32 * 4;
        pixelbytes = 4;
    }
    else
        return NULL;

    int dwSize = imageLineLen * BITMAP_Y;
    LPBYTE bitNew = NULL;
    bitNew = (LPBYTE)malloc(dwSize);

    if (bitNew == NULL)
        return NULL;

    memset(bitNew, 0, dwSize);

    for (int i = 0; i < max; i++)
    {
        itemRect = changeArray[i];
        if (itemRect)
        {
            // MsgC(" Count %d",i);

            int top = itemRect->greatestTop;
            int bottom = itemRect->smallestBottom;
            int left = itemRect->greatestLeft;
            int right = itemRect->smallestRight;

            if (expandArea) // try adding more information to allow zooming
            {
                left = left - expandThickness;
                if (left < 0)
                    left = 0;

                right = right + expandThickness;
                if (right > BITMAP_X - 1)
                    right = BITMAP_X - 1;

                top = top - expandThickness;
                if (top < 0)
                    top = 0;

                bottom = bottom + expandThickness;
                if (bottom > BITMAP_Y - 1)
                    bottom = BITMAP_Y - 1;
            }
            int length = right - left + 1;
            // int height = bottom - top + 1;
            int wLineLen;
            if (format == 4)
                wLineLen = ((length)*16 + 31) / 32 * 4;
            else if (format == 5)
                wLineLen = ((length)*32 + 31) / 32 * 4;

            int imageStart = (top * imageLineLen) + left * pixelbytes;
            int imageRow = imageStart;

            // int rectStart = 0;
            for (int y = top; y <= bottom; y++)
            {
                imageStart = imageRow;

                for (int x = left; x <= right; x++)
                {
                    if (format == 4)
                    {
                        *(bitNew + imageStart) = *(bitmap + imageStart);
                        *(bitNew + imageStart + 1) = *(bitmap + imageStart + 1);
                    }
                    else if (format == 5)
                    {
                        *(bitNew + imageStart) = *(bitmap + imageStart);
                        *(bitNew + imageStart + 1) = *(bitmap + imageStart + 1);
                        *(bitNew + imageStart + 2) = *(bitmap + imageStart + 2);
                        *(bitNew + imageStart + 3) = *(bitmap + imageStart + 3);
                    }
                    imageStart += pixelbytes;

                } // for x

                imageRow += imageLineLen;

            } // for y

        } // if rect

    } // for each rect

    return bitNew;
}


int IsDifferent(LPBITMAPINFOHEADER alpbi, int BITMAP_X, int BITMAP_Y, int x, int y, int format)
{
    unsigned long widthBytes, pixbytes;
    if (format == 4)
    {
        pixbytes = 2;
        widthBytes = (BITMAP_X * 16 + 31) / 32 * 4;
    }
    else if (format == 5)
    {
        pixbytes = 4;
        widthBytes = (BITMAP_X * 32 + 31) / 32 * 4;
    }
    else
        return FALSE;

    unsigned long access = y * widthBytes + x * pixbytes;

    LPBYTE bitsFrame = (LPBYTE)alpbi + // pointer to data
                       alpbi->biSize + alpbi->biClrUsed * sizeof(RGBQUAD);

    LPBYTE bitsKey = (LPBYTE)currentKey_lpbi + // pointer to data
                     currentKey_lpbi->biSize + currentKey_lpbi->biClrUsed * sizeof(RGBQUAD);

    int diff = FALSE;

    if (format == 4)
    {
        int val1 = *(bitsFrame + access);
        int val2 = *(bitsFrame + access + 1);

        int valKey1 = *(bitsKey + access);
        int valKey2 = *(bitsKey + access + 1);

        if ((val1 - valKey1) || (val2 - valKey2))
            diff = TRUE;
    }
    else if (format == 5)
    {
        int Blue = *(bitsFrame + access);
        int Green = *(bitsFrame + access + 1);
        int Red = *(bitsFrame + access + 2);
        int Alpha = *(bitsFrame + access + 3);

        int BlueKey = *(bitsKey + access);
        int GreenKey = *(bitsKey + access + 1);
        int RedKey = *(bitsKey + access + 2);
        int AlphaKey = *(bitsKey + access + 3);

        if ((Blue - BlueKey) || (Green - GreenKey) || (Red - RedKey) || (Alpha - AlphaKey))
            diff = TRUE;
    }
    return diff;
}

void AddExpandBlock(int BITMAP_X, int BITMAP_Y, int x, int y, int format)
{
    int blockx = x / blocksize_x;
    int blocky = y / blocksize_y;

    int max = changeArray.GetSize();
    if (max > 0)
    {
        // SearchBlock
        int found = 0;
        CChangeRectSwf *itemRect = NULL;
        for (int i = 0; i < max; i++)
        {
            itemRect = changeArray[i];
            if (itemRect)
            {
                if ((itemRect->blockx == blockx) && (itemRect->blocky == blocky))
                {
                    found = 1;
                    break;
                }
            }
        }
        if (found)
        {
            ExpandBlock(itemRect, BITMAP_X, BITMAP_Y, x, y, format);
        }
        else
            AddNewBlock(BITMAP_X, BITMAP_Y, x, y, format);
    }
    else
        AddNewBlock(BITMAP_X, BITMAP_Y, x, y, format);
}

// Assuming the itemRect is correctly chosen
void ExpandBlock(CChangeRectSwf *itemRect, int /*BITMAP_X*/, int /*BITMAP_Y*/, int x, int y, int /*format*/)
{
    if (itemRect)
    {
        // Expand left bounds
        if (x < itemRect->greatestLeft)
        {
            itemRect->greatestLeft = x;
        }
        // Expand top bounds
        if (y < itemRect->greatestTop)
        {
            itemRect->greatestTop = y;
        }
        // Expand right bounds
        if (x > itemRect->smallestRight)
        {
            itemRect->smallestRight = x;
        }
        // Expand bottom bounds
        if (y > itemRect->smallestBottom)
        {
            itemRect->smallestBottom = y;
        }
    }
}

void AddNewBlock(int BITMAP_X, int BITMAP_Y, int x, int y, int /*format*/)
{
    CChangeRectSwf *itemRect = new CChangeRectSwf;

    int blockx = x / blocksize_x;
    int blocky = y / blocksize_y;
    itemRect->initialize(blockx, blocky, blocksize_x, blocksize_y, numblocks_x, BITMAP_X, BITMAP_Y, x, y);
    changeArray.Add(itemRect);
}

void finishTemporalCompress()
{
    initTemporalCompress(0, 0);
}

void initTemporalCompress(int bmWidth, int bmHeight)
{
    if (currentKey_lpbi)
    {
        free(currentKey_lpbi);
        currentKey_lpbi = NULL;
    }
    cleanChangeArray();
    framei = 0;
    framecount = 0;

    hasKeyFrame = 0;
    hasIntermediateFrame = 0;
    HalfKeyDepthInc = 0;

    numblocks_x = bmWidth / blocksize_x;
    numblocks_y = bmHeight / blocksize_y;

    if ((bmWidth % blocksize_x) > 0)
        numblocks_x++;

    if ((bmHeight % blocksize_y) > 0)
        numblocks_y++;

    freeCharacterArray.RemoveAll();
}

void cleanChangeArray()
{
    int max = changeArray.GetSize();
    CChangeRectSwf *itemRect = NULL;
    for (int i = 0; i < max; i++)
    {
        itemRect = changeArray[i];
        if (itemRect)
            delete itemRect;
    }
    changeArray.RemoveAll();
}

void *MakeFullBuffer(void *buffer, long &buffersize, long &numsamples, int avgsamplespersecond, bool streamstereo,
                     bool stream16bit)
{
    if (buffersize == 0)
        return buffer;

    if (numsamples >= (avgsamplespersecond))
        return buffer;

    if (!stream16bit)
        return buffer;

    int newsize, bytespersample;
    int smul = 1;
    if (streamstereo)
        smul = 2;

    bytespersample = 2; // only handle 16 bits per sample

    newsize = avgsamplespersecond * smul * bytespersample;

    if (newsize <= buffersize)
        return buffer;

    void *newbuffer = realloc(buffer, newsize);
    buffersize = newsize;
    numsamples = avgsamplespersecond;
    return newbuffer;
}


// ver 2/24
// **************************************
// Conversion to PCM
// **************************************
int SaveFirstAudioToFile(CString filename)
{
    PAVIFILE pavi;
    if (AVIFileOpen(&pavi, LPCTSTR(filename), OF_CREATE | OF_WRITE | OF_SHARE_DENY_NONE, NULL) != 0)
        return 0;
    if (CopyStream(pavi, gapavi[giFirstAudio]) != 0)
        return 0;
    AVIFileRelease(pavi);
    return 1;
}

// Code Derived from article "Concerning Video for Windows API"
// See credit links in help file for more info
int CopyStream(PAVIFILE pavi, PAVISTREAM pstm)
{
    AVISTREAMINFO si;
    LONG st, ed, leng;
    BYTE p[20000];
    PAVISTREAM ptmp;

    st = AVIStreamStart(pstm);
    ed = st + AVIStreamLength(pstm) - 1;
    if (AVIStreamInfo(pstm, &si, sizeof(AVISTREAMINFO)) != 0)
        return -1;
    if (AVIFileCreateStream(pavi, &ptmp, &si) != 0)
        return -1;
    if (AVIStreamReadFormat(pstm, st, NULL, &leng) != 0)
        return -1;
    if (AVIStreamReadFormat(pstm, st, p, &leng) != 0)
        return -1;
    if (AVIStreamSetFormat(ptmp, st, p, leng) != 0)
        return -1;

    long sampleread = 4096;
    long byteread = 0;
    long bufferlength = 20000;
    long slCurrentWav = 0;

    slCurrentWav = st;
    while (slCurrentWav <= ed)
    {
        AVIStreamRead(pstm, slCurrentWav, WAVBUFFER, p, bufferlength, &byteread, &sampleread);

        if (sampleread > 0)
        {
            AVIStreamWrite(ptmp, slCurrentWav, sampleread, p, bufferlength, AVIIF_KEYFRAME, NULL, NULL);

            slCurrentWav += sampleread;
        }
        else
            break;
    }
    AVIStreamRelease(ptmp);
    return 0;
}

// return <=0 if fails / no conversion
int ConvertFileToPCM(CString infilename, CString outfilename, LPWAVEFORMATEX lpFormat)
{
    HCURSOR hcur;
    hcur = SetCursor(LoadCursor(NULL, IDC_WAIT));
    ShowCursor(TRUE);

    int retval = 0;
    retval = MultiStepConvertToPCM(infilename, outfilename, lpFormat, sizeof(WAVEFORMATEX));

    ShowCursor(FALSE);
    SetCursor(hcur);

    return retval;
}

// Using this function assumes we have checked giFirstAudio stream is non PCM
PAVISTREAM ConvertFirstAudioStream(LPWAVEFORMATEX lpFormat)
{
    if (giFirstAudio < 0)
        return NULL;

    tempfile1 = "";
    tempfile2 = "";

    CString nonPCMfileName("\\~tnonPCM.wav");
    CString yesPCMfileName("\\~tyesPCM.wav");
    CString tpath = GetProgPath();
    CString infilename = tpath + nonPCMfileName;
    CString outfilename = tpath + yesPCMfileName;

    SaveFirstAudioToFile(infilename);

    int retval = ConvertFileToPCM(infilename, outfilename, lpFormat);
    if (retval <= 0)
        return NULL;

    tempfile1 = infilename;
    tempfile2 = outfilename;

    PAVISTREAM pstm;

    if (AVIFileOpen(&PCMConvertedFile, LPCTSTR(outfilename), OF_READ | OF_SHARE_DENY_NONE, NULL) != 0)
        return NULL;

    if (AVIFileGetStream(PCMConvertedFile, &pstm, 0, 0) != 0)
        return NULL;

    return pstm;
}

void cleanTempFile()
{
    if (PCMConvertedStream)
    {
        AVIStreamRelease(PCMConvertedStream);
        PCMConvertedStream = NULL;
    }
    if (PCMConvertedFile)
    {
        AVIFileRelease(PCMConvertedFile);
        PCMConvertedFile = NULL;
    }
    if (tempfile1 != "")
        DeleteFile(tempfile1);

    if (tempfile2 != "")
        DeleteFile(tempfile2);
}


void LoadSettings()
{
    // Do not load saved settings for now
    //    if (runmode==0)
    //        return;

    // if runmode==2 (batch mode...attempt to load settings)

    FILE *sFile;
    CString setDir, setPath;
    CString fileName;

    //********************************************
    // Loading CamProducer.ini for storing text data
    //********************************************
    if (runmode == 0 || runmode == 1)
        fileName = "\\CamStudio.Producer.ini";

    setDir = GetAppDataPath();
    setPath = setDir + fileName;

    sFile = fopen(LPCTSTR(setPath), "rt");
    if (sFile == NULL)
    {
        setDir = GetProgPath();
        setPath = setDir + fileName;

        sFile = fopen(LPCTSTR(setPath), "rt");
        if (sFile == NULL)
        {
            return;
        }
    }
    // ****************************
    // Read Variables
    // ****************************

    // char sdata[1000];
    float ver = 1.0;

    fscanf(sFile, "[ CamStudio Flash Producer Settings ver%f -- Please do not edit ] \n\n", &ver);

    int swfnameLen = 0;
    int swfhtmlnameLen = 0;
    int swfbasenameLen = 0;
    // int avifilenameLen = 0;

    // Ver 1.0
    if (ver >= 0.99999)
    {
        // Important Variables

        // fscanf(sFile, "avifilenameLen = %d \n",&avifilenameLen);

        fscanf(sFile, "usePercent = %d \n", &usePercent);

        fscanf(sFile, "useHalfKey = %d \n", &useHalfKey);
        fscanf(sFile, "keyframerate = %d \n", &keyframerate);
        fscanf(sFile, "Max_HalfKeyDepth = %d \n", &Max_HalfKeyDepth);

        fscanf(sFile, "FrameOffsetX = %d \n", &FrameOffsetX);
        fscanf(sFile, "FrameOffsetY = %d \n", &FrameOffsetY);

        fscanf(sFile, "useAudio = %d \n", &useAudio);
        fscanf(sFile, "useAudioCompression = %d \n", &useAudioCompression);
        fscanf(sFile, "useMP3 = %d \n", &useMP3);
        fscanf(sFile, "mp3volume = %d \n", &mp3volume);

        fscanf(sFile, "sampleFPS = %d \n", &sampleFPS);
        fscanf(sFile, "convertBits = %d \n", &convertBits);

        fscanf(sFile, "noLoop = %d \n", &noLoop);
        fscanf(sFile, "noAutoPlay = %d \n", &noAutoPlay);
        fscanf(sFile, "addControls = %d \n", &addControls);
        fscanf(sFile, "controlsWidth = %d \n", &controlsWidth);
        fscanf(sFile, "controlsHeight = %d \n", &controlsHeight);

        fscanf(sFile, "adpcmBPS = %d \n", &adpcmBPS);
        fscanf(sFile, "launchPropPrompt = %d \n", &launchPropPrompt);
        fscanf(sFile, "launchHTMLPlayer = %d \n", &launchHTMLPlayer);

        fscanf(sFile, "swfnameLen = %d \n", &swfnameLen);
        fscanf(sFile, "swfhtmlnameLen = %d \n", &swfhtmlnameLen);
        fscanf(sFile, "swfbasenameLen = %d \n", &swfbasenameLen);
        fscanf(sFile, "onlyflashtag = %d \n", &onlyflashtag);

        // Lesser Variables

        fscanf(sFile, "PercentThreshold = %lf \n", &PercentThreshold);
        fscanf(sFile, "HalfKeyThreshold = %lf \n", &HalfKeyThreshold);

        fscanf(sFile, "blocksize_x = %d \n", &blocksize_x);
        fscanf(sFile, "blocksize_y = %d \n", &blocksize_y);
        fscanf(sFile, "numblocks_x = %d \n", &numblocks_x);
        fscanf(sFile, "numblocks_y = %d \n", &numblocks_y);
        fscanf(sFile, "expandArea = %d \n", &expandArea);
        fscanf(sFile, "expandThickness = %d \n", &expandThickness);

        fscanf(sFile, "MatrixOffsetX = %d \n", &MatrixOffsetX);
        fscanf(sFile, "MatrixOffsetY = %d \n", &MatrixOffsetY);
        fscanf(sFile, "MoveOffsetX = %d \n", &MoveOffsetX);
        fscanf(sFile, "MoveOffsetY = %d \n", &MoveOffsetY);

        fscanf(sFile, "KeyFrameDepth = %d \n", &KeyFrameDepth);
        fscanf(sFile, "HalfKeyDepthBase = %d \n", &HalfKeyDepthBase);
        fscanf(sFile, "IFrameDepth = %d \n", &IFrameDepth);
        fscanf(sFile, "ObjectDepth = %d \n", &ObjectDepth);

        fscanf(sFile, "swfbar_red = %d \n", &swfbar_red);
        fscanf(sFile, "swfbar_green = %d \n", &swfbar_green);
        fscanf(sFile, "swfbar_blue = %d \n", &swfbar_blue);

        fscanf(sFile, "swfbk_red = %d \n", &swfbk_red);
        fscanf(sFile, "swfbk_green = %d \n", &swfbk_green);
        fscanf(sFile, "swfbk_blue = %d \n", &swfbk_blue);

        fscanf(sFile, "swf_bits_per_sample = %d \n", &swf_bits_per_sample);
        fscanf(sFile, "swf_samples_per_seconds = %d \n", &swf_samples_per_seconds);
        fscanf(sFile, "swf_num_channels = %d \n", &swf_num_channels);

        fscanf(sFile, "allowChaining = %d \n", &allowChaining);
        fscanf(sFile, "freecharacter = %d \n", &freecharacter);
        fscanf(sFile, "percentLoadedThreshold = %lf \n", &percentLoadedThreshold);

        fscanf(sFile, "addPreloader = %d \n", &addPreloader);
        fscanf(sFile, "applyPreloaderToSplitFiles = %d \n", &applyPreloaderToSplitFiles);
        fscanf(sFile, "produceRaw = %d \n", &produceRaw);
    }
    fclose(sFile);

    //********************************************
    // Loading Camdata.ini  binary data
    //********************************************
    FILE *tFile;
    if (runmode == 0)
        fileName = "\\CamStudio.Producer.Data.ini";
    else
        fileName = "\\CamStudio.Producer.Data.command"; // command line mode
    setDir = GetAppDataPath();
    setPath = setDir + fileName;

    tFile = fopen(LPCTSTR(setPath), "rb");
    if (tFile == NULL)
    {
        return;
    }
    if (ver > 0.999999)
    { // ver 1.0

        // ****************************
        // Load Binary Data
        // ****************************
        /*
        if ((swfnameLen>0) && (swfnameLen<1000)) {
        fread( (void *) sdata, swfnameLen, 1, tFile );
        sdata[swfnameLen]=0;
        swfname=CString(sdata);
        }
        if ((swfhtmlnameLen>0) && (swfhtmlnameLen<1000)) {
        fread( (void *) sdata, swfhtmlnameLen, 1, tFile );
        sdata[swfhtmlnameLen]=0;
        swfhtmlname=CString(sdata);
        }
        if ((swfbasenameLen>0) && (swfbasenameLen<1000)) {
        fread( (void *) sdata, swfbasenameLen, 1, tFile );
        sdata[swfbasenameLen]=0;
        swfbasename=CString(sdata);
        }
        */

        /*
        if ((avifilenameLen>0) && (avifilenameLen<1000)) {
        fread( (void *) sdata, avifilenameLen, 1, tFile );
        sdata[avifilenameLen]=0;
        avifilename=CString(sdata);

        }
        */

        // ver 1.2
        if (ver > 1.199999)
        {
        }
    }
    fclose(tFile);
}

void SaveController()
{
    FILE *sFile;
    CString setDir, setPath;
    CString fileName;

    fileName = "\\controller\\controller.ini";
    setDir = GetAppDataPath();
    setPath = setDir + fileName;

    sFile = fopen(LPCTSTR(setPath), "wt");
    if (sFile == NULL)
    {
        return;
    }
    // ****************************
    // Write Variables
    // ****************************

    float ver = 1.0;

    fprintf(sFile, "[ CamStudio Controller Settings ver%.2f ] \n\n", ver);

    // Ver 1.0
    {
        fprintf(sFile, "Spacing_Between_Buttons_X = %d \n", ButtonSpaceX);
        fprintf(sFile, "Spacing_Between_Controller_And_Movie_Y = %d \n", ButtonSpaceY);
        fprintf(sFile, "Shift_LR_Pieces_Down_By_Y = %d \n", PieceOffsetY);
        fprintf(sFile, "Spacing_Between_ProgressBar_And_Movie_Y = %d \n", ProgressOffsetY);

        fprintf(sFile, "Draw_Left_Piece = %d \n", yes_drawLeftPiece);
        fprintf(sFile, "Draw_Right_Piece = %d \n", yes_drawRightPiece);
        fprintf(sFile, "Draw_Stop_Button = %d \n", yes_drawStopButton);

        fprintf(sFile, "Full_Controller_Width = %d \n", ControllerWidth);
        fprintf(sFile, "Controller_Alignment = %d \n", ControllerAlignment);
    }
    fclose(sFile);
}

void LoadController()
{
    FILE *sFile;
    CString setDir, setPath;
    CString fileName;

    fileName = "\\controller\\controller.ini";
    setDir = GetAppDataPath();
    setPath = setDir + fileName;

    sFile = fopen(LPCTSTR(setPath), "rt");
    if (sFile == NULL)
    {
        return;
    }
    // ****************************
    // Write Variables
    // ****************************

    float ver = 1.0;

    fscanf(sFile, "[ CamStudio Controller Settings ver%f ] \n\n", &ver);

    // Ver 1.0
    if (ver > 0.99)
    {
        fscanf(sFile, "Spacing_Between_Buttons_X = %d \n", &ButtonSpaceX);
        fscanf(sFile, "Spacing_Between_Controller_And_Movie_Y = %d \n", &ButtonSpaceY);
        fscanf(sFile, "Shift_LR_Pieces_Down_By_Y = %d \n", &PieceOffsetY);
        fscanf(sFile, "Spacing_Between_ProgressBar_And_Movie_Y = %d \n", &ProgressOffsetY);

        fscanf(sFile, "Draw_Left_Piece = %d \n", &yes_drawLeftPiece);
        fscanf(sFile, "Draw_Right_Piece = %d \n", &yes_drawRightPiece);
        fscanf(sFile, "Draw_Stop_Button = %d \n", &yes_drawStopButton);

        fscanf(sFile, "Full_Controller_Width = %d \n", &ControllerWidth);
        fscanf(sFile, "Controller_Alignment = %d \n", &ControllerAlignment);
    }
    fclose(sFile);
}

void SaveSettings()
{
    FILE *sFile;
    CString setDir, setPath;
    CString fileName;

    //********************************************
    // Saving CamProducer.ini for storing text data
    //********************************************

    fileName = "\\CamStudio.Producer.ini";
    setDir = GetAppDataPath();
    setPath = setDir + fileName;

    sFile = fopen(LPCTSTR(setPath), "wt");
    if (sFile == NULL)
    {
        return;
    }
    // ****************************
    // Write Variables
    // ****************************

    float ver = 1.0;

    fprintf(sFile, "[ CamStudio Flash Producer Settings ver%.2f -- Please do not edit ] \n\n", ver);

    // int swfnameLen = 0;
    // int swfhtmlnameLen = 0;
    // int swfbasenameLen = 0;

    // Ver 1.0
    // if (ver>=0.99999)
    {
        // Important Variables
        // fprintf(sFile, "avifilenameLen = %d \n",avifilename.GetLength());

        fprintf(sFile, "usePercent = %d \n", usePercent);
        fprintf(sFile, "useHalfKey = %d \n", useHalfKey);
        fprintf(sFile, "keyframerate = %d \n", keyframerate);
        fprintf(sFile, "Max_HalfKeyDepth = %d \n", Max_HalfKeyDepth);

        fprintf(sFile, "FrameOffsetX = %d \n", FrameOffsetX);
        fprintf(sFile, "FrameOffsetY = %d \n", FrameOffsetY);

        fprintf(sFile, "useAudio = %d \n", useAudio);
        fprintf(sFile, "useAudioCompression = %d \n", useAudioCompression);
        fprintf(sFile, "useMP3 = %d \n", useMP3);
        fprintf(sFile, "mp3volume = %d \n", mp3volume);

        fprintf(sFile, "sampleFPS = %d \n", sampleFPS);
        fprintf(sFile, "convertBits = %d \n", convertBits);

        fprintf(sFile, "noLoop = %d \n", noLoop);
        fprintf(sFile, "noAutoPlay = %d \n", noAutoPlay);
        fprintf(sFile, "addControls = %d \n", addControls);
        fprintf(sFile, "controlsWidth = %d \n", controlsWidth);
        fprintf(sFile, "controlsHeight = %d \n", controlsHeight);

        fprintf(sFile, "adpcmBPS = %d \n", adpcmBPS);
        fprintf(sFile, "launchPropPrompt = %d \n", launchPropPrompt);
        fprintf(sFile, "launchHTMLPlayer = %d \n", launchHTMLPlayer);

        fprintf(sFile, "swfnameLen = %d \n", swfname.GetLength());
        fprintf(sFile, "swfhtmlnameLen = %d \n", swfhtmlname.GetLength());
        fprintf(sFile, "swfbasenameLen = %d \n", swfbasename.GetLength());
        fprintf(sFile, "onlyflashtag = %d \n", onlyflashtag);

        // Lesser Variables
        fprintf(sFile, "PercentThreshold = %f \n", PercentThreshold);
        fprintf(sFile, "HalfKeyThreshold = %f \n", HalfKeyThreshold);

        fprintf(sFile, "blocksize_x = %d \n", blocksize_x);
        fprintf(sFile, "blocksize_y = %d \n", blocksize_y);
        fprintf(sFile, "numblocks_x = %d \n", numblocks_x);
        fprintf(sFile, "numblocks_y = %d \n", numblocks_y);
        fprintf(sFile, "expandArea = %d \n", expandArea);
        fprintf(sFile, "expandThickness = %d \n", expandThickness);

        fprintf(sFile, "MatrixOffsetX = %d \n", MatrixOffsetX);
        fprintf(sFile, "MatrixOffsetY = %d \n", MatrixOffsetY);
        fprintf(sFile, "MoveOffsetX = %d \n", MoveOffsetX);
        fprintf(sFile, "MoveOffsetY = %d \n", MoveOffsetY);

        fprintf(sFile, "KeyFrameDepth = %d \n", KeyFrameDepth);
        fprintf(sFile, "HalfKeyDepthBase = %d \n", HalfKeyDepthBase);
        fprintf(sFile, "IFrameDepth = %d \n", IFrameDepth);
        fprintf(sFile, "ObjectDepth = %d \n", ObjectDepth);

        fprintf(sFile, "swfbar_red = %d \n", swfbar_red);
        fprintf(sFile, "swfbar_green = %d \n", swfbar_green);
        fprintf(sFile, "swfbar_blue = %d \n", swfbar_blue);

        fprintf(sFile, "swfbk_red = %d \n", swfbk_red);
        fprintf(sFile, "swfbk_green = %d \n", swfbk_green);
        fprintf(sFile, "swfbk_blue = %d \n", swfbk_blue);

        fprintf(sFile, "swf_bits_per_sample = %d \n", swf_bits_per_sample);
        fprintf(sFile, "swf_samples_per_seconds = %d \n", swf_samples_per_seconds);
        fprintf(sFile, "swf_num_channels = %d \n", swf_num_channels);

        fprintf(sFile, "allowChaining = %d \n", allowChaining);
        fprintf(sFile, "freecharacter = %d \n", freecharacter);
        fprintf(sFile, "percentLoadedThreshold = %.2f \n", percentLoadedThreshold);

        fprintf(sFile, "addPreloader = %d \n", addPreloader);
        fprintf(sFile, "applyPreloaderToSplitFiles = %d \n", applyPreloaderToSplitFiles);
        fprintf(sFile, "produceRaw = %d \n", produceRaw);
    }
    fclose(sFile);

    //********************************************
    // Saving Camdata.ini  binary data
    //********************************************
    FILE *tFile;
    fileName = "\\CamStudio.Producer.Data.ini";

    setDir = GetAppDataPath();
    setPath = setDir + fileName;

    tFile = fopen(LPCTSTR(setPath), "wb");
    if (tFile == NULL)
    {
        return;
    }
    // if (ver> 0.999999)
    { // ver 1.0

        // ****************************
        // Save Binary Data
        // ****************************
        /*
        if (swfname.GetLength()>0)  {
        fwrite( (void *) LPCTSTR(swfname), swfname.GetLength(), 1, tFile );
        }
        if (swfhtmlname.GetLength()>0) {
        fwrite( (void *) LPCTSTR(swfhtmlname), swfhtmlname.GetLength(), 1, tFile );
        }
        if (swfbasename.GetLength()>0) {
        fwrite( (void *) LPCTSTR(swfbasename), swfbasename.GetLength(), 1, tFile );
        }
        if (avifilename.GetLength()>0) {
        fwrite( (void *) LPCTSTR(avifilename), avifilename.GetLength(), 1, tFile );
        }
        */

        // ver 1.2
        if (ver > 1.199999)
        {
        }
    }
    fclose(tFile);
}

// ver 2.25
void CPlayplusView::OnConvert()
{
    // TODO: Add your command handler code here
    convertMode = 1;

    if (giFirstVideo < 0)
    {
        BeginWaitCursor();
        OnFileOpen();
        EndWaitCursor();
    }

    convertMode = 0;
}

int MessageOut(HWND hWnd, long strMsg, long strTitle, UINT mbstatus)
{
    CString tstr("");
    CString mstr("");
    tstr.LoadString(strTitle);
    mstr.LoadString(strMsg);

    return ::MessageBox(hWnd, mstr, tstr, mbstatus);
}

int MessageOutINT(HWND hWnd, long strMsg, long strTitle, UINT mbstatus, long val)
{
    CString tstr("");
    CString mstr("");
    CString fstr("");
    tstr.LoadString(strTitle);
    mstr.LoadString(strMsg);
    fstr.Format(mstr, val);

    return ::MessageBox(hWnd, fstr, tstr, mbstatus);
}

int MessageOutINT2(HWND hWnd, long strMsg, long strTitle, UINT mbstatus, long val1, long val2)
{
    CString tstr("");
    CString mstr("");
    CString fstr("");
    tstr.LoadString(strTitle);
    mstr.LoadString(strMsg);
    fstr.Format(mstr, val1, val2);

    return ::MessageBox(hWnd, fstr, tstr, mbstatus);
}

void CPlayplusView::OnHelpHelp()
{
    CString helpfile("\\helpProducer.htm");
    CString pathx = GetProgPath();
    helpfile = pathx + helpfile;
    Openlink(helpfile);
}

void AdjustOutName(CString avioutpath)
{
    swfname = avioutpath;
    int lenx = swfname.GetLength();
    //::MessageBox(NULL,swfname,"Adjust",MB_OK);
    if ((5 <= lenx) && ((swfname[lenx - 1] == 'i') || (swfname[lenx - 1] == 'I')) &&
        ((swfname[lenx - 2] == 'v') || (swfname[lenx - 2] == 'V')) &&
        ((swfname[lenx - 3] == 'a') || (swfname[lenx - 3] == 'A')) && (swfname[lenx - 4] == '.'))
    {
        swfname.SetAt(lenx - 1, 'f');
        swfname.SetAt(lenx - 2, 'w');
        swfname.SetAt(lenx - 3, 's');
        swfname.SetAt(lenx - 4, '.');

        swfbasename = swfname.Right(lenx - swfname.ReverseFind('\\') - 1);
        swfhtmlname = swfname + ".html";
    }
    else
    {
        swfname += ".swf";
        swfhtmlname = swfname + ".html";
        swfbasename = swfname.Right(lenx - swfname.ReverseFind('\\') - 1);
    }
}

void LoadCommand()
{
    CString fileName = "\\CamStudio.Producer.command"; // command line mode
    CString setDir = GetAppDataPath();
    CString setPath = setDir + fileName;
    FILE *sFile = fopen(LPCTSTR(setPath), "rt");
    if (sFile == NULL)
    {
        setDir = GetProgPath();
        setPath = setDir + fileName;
        sFile = fopen(LPCTSTR(setPath), "rt");
        if (sFile == NULL)
        {
            return;
        }
    }
    // ****************************
    // Read Variables
    // ****************************

//    float ver = 1.0;

    // Debugging info
    // The use of scanf("%.2f") instead of scanf("%f") results in hard-to-detect bugs
    // This is iffy and fails on some OSs. THe profile API is used to write stuff out, it should also
    // be used to read it back.
    // fscanf(sFile,"[CamStudio Flash Producer Commands ver%f]",&ver);
    // fscanf(sFile,"LaunchPropPrompt=%d",&launchPropPrompt);
    // fscanf(sFile,"LaunchHTMLPlayer=%d",&launchHTMLPlayer);
    // fscanf(sFile,"DeleteAVIAfterUse=%d",&deleteAVIAfterUse);
    fclose(sFile);

    CString strSection = _T("CamStudio Flash Producer Commands");
    const int retStringSize = 300;
    _TCHAR retString[retStringSize];
    retString[0] = 0;
    CString strValue;

    CString strKey = _T("LaunchPropPrompt");
    ::GetPrivateProfileString(strSection, strKey, 0, retString, retStringSize, setPath);
    strValue = retString;
    launchPropPrompt = _ttoi(strValue);

    strKey = _T("LaunchHTMLPlayer");
    ::GetPrivateProfileString(strSection, strKey, 0, retString, retStringSize, setPath);
    strValue = retString;
    launchHTMLPlayer = _ttoi(strValue);

    strKey = _T("DeleteAVIAfterUse");
    ::GetPrivateProfileString(strSection, strKey, 0, retString, retStringSize, setPath);
    strValue = retString;
    deleteAVIAfterUse = _ttoi(strValue);
}

LPBITMAPINFOHEADER LoadBitmapFile(CString bitmapFile)
{
    LPBITMAPINFOHEADER alpbi = NULL;
    CPicture picture;

    if (picture.Load(bitmapFile))
    {
        HBITMAP hbitmap = NULL;
        if (picture.m_IPicture->get_Handle((unsigned int *)&hbitmap) == S_OK)
        {
            alpbi = (LPBITMAPINFOHEADER)GlobalLock(Bitmap2Dib(hbitmap, 32));
        }
        else
        {
            alpbi = NULL;
        }
    }
    return alpbi;
}


void GetBounds(const char *font, CString textstr, int pointsize, CSize &retExtent, bool bold, bool italic, bool uLine)
{
    unsigned int fWeight = 400;
    if (bold == true)
        fWeight = 700;

    CSize Extent;
    HDC hdcBits = GetDC(NULL);
    HFONT myfont =
        CreateFont(pointsize, 0, 0, 0, fWeight, italic ? TRUE : FALSE, uLine ? TRUE : FALSE, FALSE, DEFAULT_CHARSET,
                   OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, PROOF_QUALITY, DEFAULT_PITCH, font);
    HFONT oldFont = (HFONT)SelectObject(hdcBits, myfont);

    GetTextExtentPoint32(hdcBits, LPCTSTR(textstr), textstr.GetLength(), &Extent);

    if (oldFont)
        SelectObject(hdcBits, oldFont);
    ::ReleaseDC(NULL, hdcBits);

    DeleteObject(myfont);

    retExtent = Extent;
}
