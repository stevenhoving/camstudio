// playplusView.cpp : implementation of the CPlayplusView class
//

//Warning : conversion of audio files does not delete the orginal and also possibly the converted file! So it is best to keep these files in temp directroy

#include "stdafx.h"
#include "playplus.h"

#include "playplusDoc.h"
#include "playplusView.h"
#include "MainFrm.h"

#include "MessageWnd.h"
#include "ProgressDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include <windowsx.h>
#include "muldiv32.h"
#include <vfw.h>

#include "fister/soundfile.h"
#include "AudioFormat.h"

//#include "./Conversion/WAVEIO.H"
#include "Conversion.h"
#include "FlashConversionDlg.h"
#include "AdvProperty.h"
#include "FlashInterface.h"

CTransparentWnd transWnd;
int transCreated  = 0;

CProgressDlg progDlg;
int progressCreated  = 0;


// Program Mode -- to be set at compile time
#define PLAYER  0
#define DUBBER  1
int pmode = DUBBER;
//int pmode = PLAYER;


//Ver 1.0
#define GlobalSizePtr(lp)   GlobalSize(GlobalPtrHandle(lp))
#define LPPAVIFILE PAVIFILE *
typedef BYTE * HPBYTE;
typedef UNALIGNED short * HPSHORT;


extern BOOL CALLBACK aviaudioPlay(HWND hwnd, PAVISTREAM pavi, LONG lStart, LONG lEnd, BOOL fWait);
extern void CALLBACK aviaudioMessage(HWND, UINT, WPARAM, LPARAM);
extern void CALLBACK aviaudioStop(void);
extern LONG CALLBACK aviaudioTime(void);

BOOL CALLBACK DlgProc(HWND    hwnd,UINT    msg, WPARAM  wParam, LPARAM  lParam);
static BOOL gfDefDlgEx = FALSE;         //the recursion flag for message crackers

#define BUFSIZE 260
static char gszBuffer[BUFSIZE];
static char gszExtBuffer[BUFSIZE];
static char gszFileName[BUFSIZE];
static char gszSaveFileName[BUFSIZE];
static char gszFileTitle[BUFSIZE];

#define AVI_EDIT_CLASS  "edit"
static LPAVISTREAMINFO glpavisi;
static int gnSel;
int     gSelectedStream = -1;   // Highlight this text area when painting
RECT    grcSelectedStream;      // where to highlight

char    gszFilter[512];     // for AVIBuildFilter - more than one string!
HINSTANCE   ghInstApp;
static  HACCEL      ghAccel;
static  WNDPROC     gOldEditProc;
static  HWND        ghwndEdit;

#define MAXTIMERANGE  30000
#define MAXNUMSTREAMS   25

int                 gcpavi;                     // # of streams
PAVISTREAM          gapavi[MAXNUMSTREAMS];      // the current streams
int                 gcpaviSel;                  // num of edit streams
PAVISTREAM          gapaviSel[MAXNUMSTREAMS];   // edit streams to put on clipbd
int                 gStreamTop[MAXNUMSTREAMS+1];// y position of each stream
AVICOMPRESSOPTIONS  gaAVIOptions[MAXNUMSTREAMS];// compression options
LPAVICOMPRESSOPTIONS  galpAVIOptions[MAXNUMSTREAMS];
PGETFRAME           gapgf[MAXNUMSTREAMS];       // data for decompressing video
HDRAWDIB            ghdd[MAXNUMSTREAMS];        // drawdib handles
LONG                galSelStart[MAXNUMSTREAMS];
LONG                galSelLen[MAXNUMSTREAMS];
int                 giFirstAudio = -1;          // 1st audio stream found
int                 giFirstVideo = -1;          // 1st video stream found

#define             gfVideoFound (giFirstVideo >= 0)
#define             gfAudioFound (giFirstAudio >= 0)

BOOL                gfPlaying = FALSE;          // are we currently playing?
LONG                glPlayStartTime;    // When did we start playing?
LONG                glPlayStartPos;     // Where were we on the scrollbar?
LONG                timeStart;          // cached start, end, length
LONG                timeEnd;
LONG                timeLength;
LONG                timehscroll;        // how much arrows scroll HORZ bar
int                 nVertSBLen;         // vertical scroll bar
int                 nHorzSBLen;         // horizontal scroll bar
int                 nVertHeight;
int                 nHorzWidth;
DWORD               gdwMicroSecPerPixel = 1000L;        // scale for video
WORD                gwZoom = 4;         // one-half zoom (divide by 4)
HWND                ghwndMCI;

// buffer for wave data
LPVOID lpAudio;

// constants for painting
#define VSPACE  8       // some vertical spacing
#define SELECTVSPACE 4 // height of selection line
#define HSPACE  0       // space between frames for video stream
#define TSPACE  10      // space for text area about each stream
#define AUDIOVSPACE  64 // height of an audio stream at X1 zoom
#define HIGHLIGHT       (GetSysColor(COLOR_HIGHLIGHT) ? GetSysColor(COLOR_HIGHLIGHT) : GetSysColor(COLOR_ACTIVECAPTION))

void             FrameVideo(HDC hdc, RECT *rcFrame, HBRUSH hbr);
int gfWait = 0;


//ver 1.1
long playtime=0;

long GetScrollTime();
void SetScrollTime(long time);


//ver 1.1
#define WM_USER_PLAY 0x00401
#define BUFFER_LENGTH  256

void GetImageDimension(RECT& rcFrame); 
char playfiledir[300];	
char seps[] = "*";
void OpenMovieFileInit(char *filename);
void ResizeToMovie(BOOL useDefault = FALSE);
void SetTitleBar(CString title);

BOOL playerAlive = TRUE;
UINT PlayAVIThread(LPVOID pParam);

#define MENU_OPEN           11
#define MENU_MERGE          17

int ErrMsg (LPSTR sz,...);
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

int doneOnce=0;

HWND viewWnd = NULL;
HWND mainWnd = NULL;

HBITMAP hLogoBM = NULL;

int nColors=24;
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
int Merge_Video_And_Sound_File(CString input_video_path, CString input_audio_path, CString output_avi_path, BOOL recompress_audio, LPWAVEFORMATEX audio_recompress_format, DWORD  audio_format_size, BOOL bInterleave, int interleave_factor);	    
BOOL CALLBACK SaveCallback(int iProgress);
BOOL WinYield(void);
CString GetProgPath();

#define NUMSTREAMS   2

CString savedir("");
CString cursordir("");
CString videofilepath("");

//Path to temporary wav file
CString tempaudiopath;
CString tempsilencepath;
int recordaudio=1;

//Audio Recording Variables
UINT AudioDeviceID = WAVE_MAPPER;

HWAVEIN m_hRecord;
WAVEFORMATEX m_Format;
DWORD m_ThreadID;
int m_QueuedBuffers=0;
int	 m_BufferSize = 1000;	// number of samples

#define WM_USER_GENERIC  0x00401

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

/////////////////////////////////////////////
// Audio Recording Module
/////////////////////////////////////////////

#if !defined(WAVE_FORMAT_MPEGLAYER3)
#define  WAVE_FORMAT_MPEGLAYER3 0x0055  
#endif


void InsertAVIFile(PAVIFILE pfile, LPSTR lpszFile,long starttime, int keepcounter, int overwriteaudio,int resetslider);
void InsertAVIFile(PAVIFILE pfile, LPSTR lpszFile,long starttime, int keepcounter, int overwriteaudio);


CSoundFile *m_pFile = NULL;
CSoundFile *m_pSilenceFile = NULL;
BOOL silenceFileValid = FALSE;

BOOL StartAudioRecording(WAVEFORMATEX* format);
void waveInErrorMsg(MMRESULT result, const char *);
int AddInputBufferToQueue();
void SetBufferSize(int NumberOfSamples);
void CALLBACK OnMM_WIM_DATA(UINT parm1, LONG parm2);
void CALLBACK OnMM_WOM_DATA(UINT parm1, LONG parm2);
void DataFromSoundIn(CBuffer* buffer) ;
void StopAudioRecording();
BOOL InitAudioRecording();
void ClearAudioFile();
void GetTempWavePath();
CString GetTempPath();

#define BasicBufSize 32768
BOOL CreateSilenceFile();
void ClearAudioSilenceFile();
void GetSilenceWavePath();
void WriteSilenceFile(CBuffer* buffer); 
 
void RemoveExistingAudioTracks();
void AddAudioWaveFile(char  *FileName);

void BuildRecordingFormat();
void SuggestCompressFormat();
void SuggestRecordingFormat();
void AllocCompressFormat();

void TimeDependentInsert(CString filename, long shiftTime);

int recordstate=0;
int recordpaused=0;

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

int EditStreamReplace(PAVISTREAM pavi, LONG * plPos,  LONG * plLength, PAVISTREAM pstream, LONG lStart, LONG lLength);
int EditStreamPadSilence(PAVISTREAM pavi, LONG * plPos,  LONG * plLength);
long ExAVIStreamTimeToSample( PAVISTREAM pavi, LONG lTime);
int EditStreamSilenceShift(PAVISTREAM pavi, LONG * plPos, LONG * plLength);
long SafeStreamTimeToSample( PAVISTREAM pavi, LONG starttime );
void RecomputeStreamsTime(int resetslider,long timeCurrent);

void  TestConvertFirstAudio();
void CloneAudioStream_ReplaceStreamPool(int i,PAVISTREAM pavi);
void ReInitAudioStream(int i);


void SetAdditionalCompressSettings(BOOL recompress_audio, LPWAVEFORMATEX audio_recompress_format, DWORD  audio_format_size, BOOL bInterleave, int interleave_factor, int interleave_unit);
int TestConvert(PAVISTREAM pavi,PAVISTREAM* paviConverted, PAVISTREAM paviDstFormat);
LPWAVEFORMATEX  allocRetrieveAudioFormat(PAVISTREAM pavi);


void SetDurationLine();
void SetRPSLine();

int allowRecordExtension = 1;

int fileModified = 0;

void Msg(const char fmt[], ...);
void DumpFormat(WAVEFORMATEX*  pwfx, const char* str );

CString GetTempPathEx(CString fileName, CString fxstr, CString exstr);
CString ConvertFileToWorkingPCM(CString filename);



////////////////////////////////////
// Ver 2.0 Flash Converter
////////////////////////////////////
//
// Terminology
// -----------
// Keyframe -- a Full Frame is saved ;  all intermediate layers, and halfkey layers on top are cleared
// Intermediate Frame - a frame at layer 100, on top of Keyframe and halfkeys. This frame is never used as a basis for comparison with the next frame
// HalfKey  -- Frames that are saved as a difference to the previous frame, these frames are used as a comparison for the next frame, the difference are layered on top of the keyframe. Multiple layers are stacked ... up to a depth of Max_HalfkeyDepth
//
// Usage : 
// 1) useHalfKey = 1, usePercent = 1, set HalfKeyThreshold to decide when  a frame is a keyframe and when it is a halfkey
//    in both cases, the frame returned is saved for comparison with the next frame
//    The  HalfKeyThreshold is the percentage of change in the new frame that cause the program to decide the frame should be saved as a keyframe instead of halfkey
//    No intermediate frames are used
//
// 2) useHalfKey = 1, usePercent = 0, all frames are halfkey...until the Max_HalfKeyDepth is reached...then a key frame is stored
//    the frame returned is saved for comparison with the next frame    
//    No intermediate frames are used
//
// 3) useHalfKey = 0, usePercent = 0, set sampleFPS 
//    No halfkey are used.  Key frames occurs at a fixed frequency ... e.g every 20 fps
//    Otherwise, the frames are intermediate, i.e a single layer on top of the key...this layer is removed and a new layer added when the next intermediate frame is shown
//
// 4) useHalfKey = 0, usePercent = 1,  set PercentThreshold, sampleFPS 
//    Key frames usually occurs at a fixed frequency, but if the difference is too big in the current frame  (compared with the keyframe), the keyframe is updated
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

#include <fstream>
#include <sstream>
#include <vector>
#include <afxtempl.h>

#include "./swfsource/FButton.h"
#include "./swfsource/FBase.h"
#include "./swfsource/FShape.h"
#include "./swfsource/FDisplay.h"
#include "./swfsource/FControl.h"
#include "./swfsource/FSprite.h"
#include "./swfsource/FSound.h"
#include "./swfsource/FlashMP3Encoder.h"
#include "./swfsource/FFont.h"
#include "./swfsource/FlashFontObj.h"
#include "./swfsource/FMorph.h"

#include "./swfsource/FTarga.h"
#include "./swfsource/FBitmap.h"
#include "./swfsource/FImport.h"

#include ".\flashsdk\FSoundSDK.h"

#include "Picture.h"

void ConvertToFlash(long currentTime,HDC hdc, HBITMAP hbm, std::ostringstream &f);
void PaintSwfFrame(HDC hdc, HBITMAP hbm, RECT rcFrame, LPBITMAPINFOHEADER lpbi, int iStream, std::ostringstream &f);
LPBYTE makeReverse16(int width, int height,int bitPlanes, LPBITMAPINFOHEADER alpbi);
LPBYTE makeReverse32(int width, int height,int bitPlanes, LPBITMAPINFOHEADER alpbi);
LPBITMAPINFOHEADER GetFrame(LPBYTE* bits,int& BITMAP_X,int& BITMAP_Y, int numbits);

HANDLE  Bitmap2Dib( HBITMAP hbitmap, UINT bits);
LPBITMAPINFOHEADER GetFrame(HBITMAP hbm, LPBYTE* bits,int& BITMAP_X,int& BITMAP_Y, int numbits);
void FreeFrame(LPBITMAPINFOHEADER alpbi);
void WriteSwfFrame(LPBITMAPINFOHEADER alpbi, std::ostringstream &f, LPBYTE bitmap, int BITMAP_X, int BITMAP_Y, int format);
LPBYTE swapPixelBytes16(int width, int height,int bitPlanes,LPBYTE bits16);


LPBITMAPINFOHEADER currentKey_lpbi = NULL;
int ProcessSwfFrame(LPBITMAPINFOHEADER alpbi, std::ostringstream &f, LPBYTE bitmap, int BITMAP_X, int BITMAP_Y, int format);
void finishTemporalCompress();
void initTemporalCompress(int bmWidth, int bmHeight);
void cleanChangeArray();

#define FRAME_KEYFRAME 0
#define FRAME_INTERMEDIATE 1
#define FRAME_HALFKEY 2


int CopyStream(PAVIFILE pavi,PAVISTREAM pstm);
PAVISTREAM ConvertFirstAudioStream(LPWAVEFORMATEX lpFormat);
void cleanTempFile();
#define WAVBUFFER 4096


class CChangeRectSwf  
{
public:
		
	int blockx; //measured in blocks
	int blocky; //measured in blocks
	int blockwidth; //measured in blocks 
	
	int pixelwidth; //measured in pixels
	int pixelheight; //measured in pixels
	int blocksizex; //measured in pixels 
	int blocksizey; //measured in pixels
	CRect blockbounds; //measured in pixels
	int greatestLeft; //measured in pixels
	int greatestTop; //measured in pixels
	int smallestRight; //measured in pixels
	int smallestBottom; //measured in pixels

	CChangeRectSwf();
	virtual ~CChangeRectSwf();
	int initialize(int blockx, int blocky, int blocksizex, int blocksizey,int blockwidth, int pixelwidth, int pixelheight, int x, int y);	

};

CChangeRectSwf :: CChangeRectSwf()
{
	blockx = -1;
	blocky = -1;
}

CChangeRectSwf :: ~CChangeRectSwf()
{


}


// *********************************************************
// Code Derived From Chris Losinger's JpegFile Class
// *********************************************************
BOOL VertFlipBuf(BYTE  * inbuf, 
					   UINT widthBytes, 
					   UINT height);

BOOL ARGBFromBGRA(BYTE *buf, UINT widthPix, UINT height);


// ver 2.21
int sampletimeInc = 1;
int sampleFPS = 40;
int convertBits = 16; 

int noLoop = 0;
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

int blocksize_x =64;
int blocksize_y =64;
int numblocks_x = 0;
int numblocks_y = 0;

int FrameOffsetX = 0;
int FrameOffsetY = 0;
 
//Using thse seems to give lesser errors..
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

//Some predefined depths
//int LoadingDepth =  ObjectDepth + 6;
//int ProgressbarDepth =  ObjectDepth + 5;
//int playButtonDepth = ObjectDepth + 2;
//int pauseButtonDepth = ObjectDepth + 2;
//int stopButtonDepth = ObjectDepth + 2;
//int BitmapplayButtonDepth = ObjectDepth + 2;
//int BitmappauseButtonDepth = ObjectDepth + 2;
//int BitmapstopButtonDepth = ObjectDepth + 2;
//int BackgroundBarDepth = ObjectDepth + 1;
//int writeTextDepth = ObjectDepth;


int expandArea = 0;  //does not seems to expand the size too much
int expandThickness = 2;

double PercentThreshold = 33;
double HalfKeyThreshold = 45;
int Max_HalfKeyDepth = 15; //maximum 90 .... playback will be SSSSSLOW for large values
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
PAVIFILE   PCMConvertedFile = NULL;

CString tempfile1("");
CString tempfile2("");


// ver 2.24
// produce HTML preview
void produceFlashHTML(CString htmlfilename, CString flashfilename, CString flashfilepath, int width, int height,int bk_red, int bk_green, int bk_blue);	

// *****************************
// Audio Compression
// *****************************
int useMP3 = 0;
int useAudio = 1;
int noAudioStream = 0;
int useAudioCompression = 1; // 0 : not compression, 1 : ADPCM ,  2 : MP3 (not yet)
//ADPCM conversion seems to work only on 16 bit PCM wave data .. 8 bit ....don't work

char play_rate;
bool play_16bit;
bool play_stereo;
char compression;
char stream_rate;
bool stream_16bit;
bool stream_stereo;
UWORD samplecountavg;

long slCurrentSwf = 0; 	

int adpcmBPS = 5; //2 to 5
void MakeSoundStreamBlockADPCM(void* buffer, int buffersize, int numsamples, std::ostringstream &f);
void *MakeFullBuffer(void* buffer, long &buffersize, long &numsamples, int avgsamplespersecond,bool streamstereo, bool stream16bit);

int launchPropPrompt = 1;
int launchHTMLPlayer = 1;
int deleteAVIAfterUse = 0;

CString swfname;
CString swfhtmlname;
CString swfbasename;
CString avifilename;

int swfbk_red = 255;
int swfbk_green = 255;
int swfbk_blue = 255;

int swfbar_red = 0;
int swfbar_green = 0;
int swfbar_blue = 255;

int runmode = 0; //0 -- window mode, 1 -- CamStudio recorder internal mode, 2 --  batch mode, silence 
int oldPercent = 0;

void LoadSettings();
void SaveSettings(); 
void LoadCommand(); 

//ver 2.25  Interfaces
int convertMode = 0;
void CreatePropertySheet();

CPropertySheet flashProp;
FlashConversionDlg  page1;
CFlashInterface page2;
CAdvProperty  page3;

int MessageOutINT(HWND hWnd,long strMsg, long strTitle, UINT mbstatus,long val);
int MessageOutINT2(HWND hWnd,long strMsg, long strTitle, UINT mbstatus,long val1,long val2);
int MessageOut(HWND hWnd,long strMsg, long strTitle, UINT mbstatus);

void AdjustOutName(CString avioutpath) ;

int CreateFlashBitmapPlayButton(std::ostringstream &f,int imagewidth, int imageheight, CString subdir,int imageoffset);
int CreateFlashBitmapPauseButton(std::ostringstream &f,int imagewidth, int imageheight, CString subdir,int imageoffset);
int CreateFlashBitmapStopButton(std::ostringstream &f,int imagewidth, int imageheight, CString subdir,int imageoffset);
int DrawRightPiece(std::ostringstream &f,int imagewidth, int imageheight,  CString subdir, int imageoffset,int yoffset);
int DrawLeftPiece(std::ostringstream &f,int imagewidth, int imageheight,  CString subdir, int imageoffset,int yoffset);
int DrawLoading(std::ostringstream &f,int imagewidth, int imageheight,  CString subdir, int imageoffset,int yoffset);

int ButtonSpaceX = 1;
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


//ver 2.27 free character
CArray<int, int> freeCharacterArray;
void gcFlash(std::ostringstream &f);

#include "./swfsource/actioncompiler/actioncompiler.h"

#define MAXFLASHLIMIT 15600

//intenral var
int needbreakapart = 0;
int breakcycle = 0;
int moreSWFsneeded = 0;
int filesAreSplit = 0;
int preloadFrames = 2;

//external var
int allowChaining = 1;
int freecharacter = 0;
double percentLoadedThreshold = 0.7;
int addPreloader = 1;
int applyPreloaderToSplitFiles = 1;
int produceRaw = 0;


//ver 2.28 preloader / actionscripts
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

int CreateProgressBar(std::ostringstream &f, int controlsWidth, int controlsHeight,int FrameOffsetX,int FrameOffsetY,int BITMAP_X, int BITMAP_Y, int additonalOffsetX, int additionalOffsetY);
void FlashActionGetPropertyVar(std::ostringstream &f,CString SpriteTarget,int index, CString varname );
void FlashActionSetPropertyFloat(std::ostringstream &f,CString SpriteTarget,int index, CString valuestr );
void FlashActionSetPropertyFloatVar(std::ostringstream &f,CString SpriteTarget,int index, CString varstr );
void WriteTextOut(std::ostringstream &f, int width, int height, CString Loadstr, CString fontstr, int red,int green, int blue, int pointsize, bool bold, bool italic, bool uLine);
void Preloader(std::ostringstream &f, int widthBar, int bmWidth, int bmHeight, int progressOffset);
void actionLoadBaseMovie(std::ostringstream &f);
void GetBounds(const char *fontname, CString textstr, int pointsize, CSize& retExtent, bool bold = false, bool italic = false, bool uLine = false);
int DrawNodes(std::ostringstream &f,int widthBar,int imagewidth, int imageheight,  CString subdir, int imageoffset,int yoffset, int additionalOffsetX,int additionalOffsetY);

//CString fontname("Times New Roman");
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
	ON_COMMAND(ID_FILE_CONVERTTOSWF, OnFileConverttoswf)
	ON_UPDATE_COMMAND_UI(ID_FILE_CONVERTTOSWF, OnUpdateFileConverttoswf)
	ON_COMMAND(ID_CONVERT, OnConvert)
	ON_COMMAND(ID_HELP_HELP, OnHelpHelp)
	ON_COMMAND(ID_HELP_SWFPRODUCERFAQ, OnHelpSwfproducerfaq)
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
	ON_MESSAGE (WM_USER_GENERIC, OnUserGeneric)	
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

BOOL CPlayplusView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CPlayplusView drawing

void CPlayplusView::OnDraw(CDC* pDC)
{
	CPlayplusDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	// TODO: add draw code for native data here
}

/////////////////////////////////////////////////////////////////////////////
// CPlayplusView printing

BOOL CPlayplusView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CPlayplusView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CPlayplusView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
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

void CPlayplusView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CPlayplusDoc* CPlayplusView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CPlayplusDoc)));
	return (CPlayplusDoc*)m_pDocument;
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
		LoadString( ghInstApp, IDS_OPENTITLE2, gszBuffer, BUFSIZE );	    
	else
		LoadString( ghInstApp, IDS_OPENTITLE, gszBuffer, BUFSIZE );	    
		    
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
	if (GetOpenFileName(&ofn)) {

		OnFileStop(); 

		gwZoom = 4;
		
		InitAvi(gszFileName, MENU_OPEN);
		ResizeToMovie();							   

		AdjustOutName(gszFileName);

	}


	
	
	//...and not recording...
	//CString msx;
	//msx.Format("!gfPlaying %d  giFirstAudio %d  giFirstVideo %d",!gfPlaying,giFirstAudio,giFirstVideo);
	//::MessageBox(NULL,msx,"note", MB_OK|MB_ICONEXCLAMATION|MB_TASKMODAL);


	Invalidate();
}


void ResizeToMovie(BOOL useDefault)
{

	RECT rc;	
	if (useDefault) {

		rc.left = 100;	
		rc.top = 100;
		rc.right=rc.left + 308-1;   
		rc.bottom=rc.top + 222-1;    	
		

	}
	else
		GetImageDimension(rc) ;		
	

	CWnd* mainWindow = AfxGetMainWnd( );
	if (((rc.bottom-rc.top)<20) && ((rc.right-rc.left)<10)) return;

	((CMainFrame*) mainWindow)->ResizeToMovie(rc);


	

									   
}

void CPlayplusView::OnFileClose() 
{

	if ((fileModified == 1) && (pmode == DUBBER)) {

		//int ret = MessageBox("Do you want to save changes ?","Note",MB_YESNOCANCEL | MB_ICONQUESTION);
		int ret = MessageOut(this->m_hWnd,IDS_DOYOUWSC, IDS_NOTE, MB_YESNOCANCEL | MB_ICONQUESTION);

		if (ret == IDYES) {
			SendMessage(WM_COMMAND,ID_FILE_SAVEAS,0);
			return;
		}
		else if (ret == IDCANCEL) 
			return;

	}

    FreeAvi();
	fileModified = 0;
    
	gszFileName[0] = '\0';
	gszFileTitle[0] = '\0';
    FixWindowTitle();

	ResizeToMovie(TRUE);

	//Invalidate();

}

void CPlayplusView::OnFilePlay() 
{
	 
	PlayMovie(SOUND_MODE);

}

void PlayMovie(int mode)
{
		glPlayStartTime = timeGetTime();
	    glPlayStartPos = GetScrollTime();


		//Recording sessions do not reset time
		if ((allowRecordExtension) && (pmode == DUBBER) && (mode == SILENT_MODE)) 
		{
			//do not reset time if this is a recording session (mode == silent) at the end of the longest stream and extension is allowed
		}
		else if ((!allowRecordExtension) && (pmode == DUBBER) && (mode == SILENT_MODE)) 
		{
			//do not reset time also if recording sesson even if  not allowRecordExtension
			
		}
		else if (glPlayStartPos>=timeEnd) {

			//resetting time for playing sessions

			SetScrollTime(timeStart);
			glPlayStartPos = GetScrollTime();
		
		}

		
		if (mode == SOUND_MODE) { 

			//should this be put here since there is adjustment to the scroll time 
			if (gfAudioFound)
			aviaudioPlay(viewWnd,
					 gapavi[giFirstAudio],
					 AVIStreamTimeToSample(gapavi[giFirstAudio], GetScrollTime()),
					 AVIStreamEnd(gapavi[giFirstAudio]),
					 FALSE);

		}
		//else { //silent mode
		//}

		gfPlaying = TRUE;

}

void CPlayplusView::OnFileRewind() 
{
	// TODO: Add your command handler code here
	SetScrollTime( timeStart);
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

	if (gfRecording) {		

		
		endAudioRecording  =TRUE;
		FileStop(SILENT_MODE);
		
		
	}
	else
		FileStop(SOUND_MODE);


}


void PaintVideo(HDC hdc, RECT rcFrame, int iStream, LPBITMAPINFOHEADER lpbi, LONG lCurSamp, LONG lPos)
{
    int         iLen;
    char        szText[BUFSIZE];

	//if lbpi is present, draw it
    if (lpbi)
    {



		//Added code for centering
		CRect rect;	
		::GetClientRect( viewWnd,&rect);

		int offsetx=0;
		int offsety=0;
		int bitmapwidth = rcFrame.right-rcFrame.left+1;
		int bitmapheight = rcFrame.bottom-rcFrame.top+1;
			
		if (rect.right> bitmapwidth)
				offsetx = (rect.right - bitmapwidth)/2;

		if (rect.bottom> bitmapheight)
				offsety = (rect.bottom - bitmapheight)/2;


		DrawDibDraw(ghdd[iStream], hdc,
		    rcFrame.left+offsetx, rcFrame.top+offsety,
		    rcFrame.right - rcFrame.left-1,
		    rcFrame.bottom - rcFrame.top-1,
		    lpbi, NULL,
		    0, 0, -1, -1,
		    (iStream == giFirstVideo) ? 0 :DDF_BACKGROUNDPAL);


	iLen = wsprintf(szText, "%ld %ld.%03lds",
			lCurSamp, lPos / 1000, lPos % 1000);
    }

    //
    // Before or after the movie (or read error) draw GRAY
    //
    else {

		//do nothing


    }

}



int PaintStuff(HDC hdc, HWND hwnd, BOOL fDrawEverything)
{
	int         xStreamLeft;
    int         yStreamTop;
    int         iFrameWidth;
    LONG        lSamp, lCurSamp;
    int         n;
    int         nFrames;
    LPBITMAPINFOHEADER lpbi = NULL;
    LONG        l;
    LONG        lTime;
    LONG        lSize = 0;
    RECT        rcFrame, rcC;
    int         i;
    

    GetClientRect(hwnd, &rcC);

    
    lTime = GetScrollTime();
    yStreamTop = -GetScrollPos(hwnd, SB_VERT);
	xStreamLeft = -GetScrollPos(hwnd, SB_HORZ);

    // for all streams
    for (i=0; i<gcpavi; i++) {
	
		AVISTREAMINFO   avis;		
		LONG             lEndTime;
		
		gStreamTop[i] = yStreamTop + GetScrollPos(hwnd, SB_VERT);	
		AVIStreamInfo(gapavi[i], &avis, sizeof(avis));

	
		if (avis.fccType == streamtypeVIDEO) {
			if (gapgf[i] == NULL)
			continue;

			//
			// Which frame belongs at this time?
			//
			lEndTime = AVIStreamEndTime(gapavi[i]);
			if (lTime <= lEndTime)
			lSamp = AVIStreamTimeToSample(gapavi[i], lTime);
			else {      // we've scrolled past the end of this stream
				
				//lSamp = AVIStreamTimeToSample(gapavi[i], AVIStreamStartTime(gapavi[i]));
				lSamp = AVIStreamTimeToSample(gapavi[i], AVIStreamStartTime(gapavi[i])+timeLength-1);
								 
			}
			
			//ver 1.1
			iFrameWidth = (avis.rcFrame.right - avis.rcFrame.left)  + HSPACE;
			nFrames = 0;
			
			
			for (n = -nFrames; n <= nFrames; n++) {

			
				if (i == giFirstVideo) {
				
					lCurSamp = lSamp + n;		
					l = AVIStreamSampleToTime(gapavi[i], lCurSamp);

				} else {    // NOT the first video stream
    
					l = lTime + MulDiv32(n * (iFrameWidth+HSPACE), gdwMicroSecPerPixel, 1000);		    		    
					lCurSamp = AVIStreamTimeToSample(gapavi[i], l);		    
					l = AVIStreamSampleToTime(gapavi[i], lCurSamp);
				}

			
				if (gapgf[i] && lCurSamp >= AVIStreamStart(gapavi[i])) 		    
					lpbi = (LPBITMAPINFOHEADER) AVIStreamGetFrame(gapgf[i], lCurSamp);
				else
					lpbi = NULL;

			
				// Figure out where to draw this frame		
				rcFrame.left    =xStreamLeft;
				rcFrame.top    = yStreamTop ;
				rcFrame.right  = rcFrame.left +
						 (avis.rcFrame.right - avis.rcFrame.left) * gwZoom / 4;
				rcFrame.bottom = rcFrame.top +
						 (avis.rcFrame.bottom - avis.rcFrame.top) * gwZoom / 4;

				//Patch to prevent blank screen
				if (lpbi == NULL)
				{
					lpbi = (LPBITMAPINFOHEADER) AVIStreamGetFrame(gapgf[i], AVIStreamEnd(gapavi[i])-1);

				}
			
				PaintVideo(hdc, rcFrame, i, lpbi, lCurSamp, l);
			}

			
			if (lpbi)	AVIStreamSampleSize(gapavi[i], lSamp, &lSize);

			//ver 1.1
			yStreamTop +=   (rcFrame.bottom - rcFrame.top); 

		} //If Video Stream

		// Give up once we're painting below the bottom of the window
		if (!fDrawEverything && yStreamTop >= rcC.bottom)
			break;

	} //for all streams

    // The bottom of all the streams;
    gStreamTop[gcpavi] = yStreamTop + GetScrollPos(hwnd, SB_VERT);

    // Return number of lines drawn     
    return yStreamTop + GetScrollPos(hwnd, SB_VERT);

}



//Separation of init streams into 1)avioptions preparation 2)timestart, timeend, time length finding  3)reseting counter components
//Pre-condition : Init streams is called only in InserAVIfile..if it needed to be called in other places, make sure the relevant and only the revlant components are included
void InsertAVIFile(PAVIFILE pfile, LPSTR lpszFile,long starttime, int keepcounter, int overwriteaudio,int resetslider)
{

	//Note: the keepcounter variable is not used	
	//It's function is taken by the resetslider

	//Currently the resetslider is used only if the mode is not OVERWRITEAUDIO
	//if it is FALSE, then we force the counter not to reset
	//by default, for most cases, the reset silder is set to TRUE, especially when insertAviFile  is used to insert new streams into the current stream pool

    int         i;
    PAVISTREAM  pavi;
	int needshift = 0;

	


	long timeCurrent = GetScrollTime();

	//ErrMsg("overover %d",overwriteaudio);
	if (overwriteaudio) {

		//Assume this case is only 1 audio track
		if (giFirstAudio>=0) {

			
			if (AVIFileGetStream(pfile, &pavi, streamtypeAUDIO , 0) != AVIERR_OK) 
			{
				ErrMsg("Unable to load audio file");
				return;
 
			}
			
			
			//if not PCM, if not at same audio rate, then covert to working PCM format
			//TestConvert(pavi);

			//editreplace stream

			long startsample = 0;			
			startsample = SafeStreamTimeToSample( gapavi[giFirstAudio], starttime );
			//startsample = AVIStreamTimeToSample( gapavi[giFirstAudio], starttime );

			//startsample=10000;

			
			PAVISTREAM paviConverted = NULL;
			if (startsample > -1) {
				
				int retval = TestConvert(pavi, &paviConverted, gapavi[giFirstAudio] );
				if (retval > 0) //Conversion is performed
				{				
					AVIStreamRelease(pavi);
					pavi = paviConverted;					
					
						
				}
				else if (retval == 0) {

					//No conversion is needed..proceed with the display

				}	
				else if (retval < 0) {

					//Error in the conversion, abort the Stream Replace
					AVIStreamRelease(pavi);
					AVIFileRelease(pfile);

				}			
				
				
				long lx = AVIStreamLength(pavi);
				//if (EditStreamPaste(gapavi[giFirstAudio], &startsample, &lx, pavi, AVIStreamStart(pavi), AVIStreamLength(pavi)) !=  AVIERR_OK)
				
				//ErrMsg("overwrite  %d",retval);
								
				//ErrMsg("EditStreamReplace startsample %ld, lx %ld, pavi %ld, AVIStreamStart(pavi) %ld, AVIStreamLength(pavi) %ld",startsample, lx, pavi, AVIStreamStart(pavi), AVIStreamLength(pavi));
				if (EditStreamReplace(gapavi[giFirstAudio], &startsample, &lx, pavi, AVIStreamStart(pavi), AVIStreamLength(pavi)) !=  AVIERR_OK)
				{
					ErrMsg("Unable to add audio at insertion point");
				}

								

			}		

			
			AVIStreamRelease(pavi);
			AVIFileRelease(pfile);


			ErrMsg("resetslider %d,timeCurrent %d",resetslider,timeCurrent);
			RecomputeStreamsTime(resetslider,timeCurrent);

			return;


		}
	}
	else { //if no overwrite

		//ErrMsg("giFirstAudio = %d, starttime = %d",giFirstAudio, starttime);
		
		if ((giFirstAudio<0) && (starttime>0)) {

			//Do nothing...consider this as normal insert with no overwrite  and startime == 0
			//However need to shift time after insertion

			//Pass on
			needshift = 1;		

		}
		else {

			//Do nothing...consider this as normal insert with no overwrite  and startime == 0			
			//Pass on
		}

	}

    for (i = gcpavi; i <= MAXNUMSTREAMS; i++) {
		if (AVIFileGetStream(pfile, &pavi, 0L, i - gcpavi) != AVIERR_OK)
			break;
		
		if (i == MAXNUMSTREAMS)
		{
			AVIStreamRelease(pavi);
			LoadString( ghInstApp, IDS_MAXSTREAMS, gszBuffer, BUFSIZE );
			ErrMsg(gszBuffer);
			break;
		}
		if (CreateEditableStream(&gapavi[i], pavi) != AVIERR_OK) {
			AVIStreamRelease(pavi);
			break;
		}
		AVIStreamRelease(pavi);
		galSelStart[i] = galSelLen[i] = -1;
    }

    AVIFileRelease(pfile);

    if (gcpavi == i && i != MAXNUMSTREAMS)
    {

		LoadString( ghInstApp, IDS_NOOPEN, gszBuffer, BUFSIZE );

		ErrMsg(gszBuffer, lpszFile);
		return;
    }

    FreeDrawStuff();
    gcpavi = i;    
	InitStreams(); //This function inherent has a RecomputeStreamsTime component


	TestConvertFirstAudio();
	
	//At this point, the gifirstaudio should be succesfully set 
	if ((needshift) && (giFirstAudio>=0)) {

		int value = 0;
		long startsample = 0;
		

		startsample = SafeStreamTimeToSample( gapavi[giFirstAudio], starttime );
		ErrMsg("giFirstAudio = %d,startsample = %d, starttime = %d",giFirstAudio,startsample, starttime);

		if (startsample>0) 
		{
			long startPos = startsample;
			long silenceLengthPasted = 0;
			EditStreamSilenceShift( gapavi[giFirstAudio],  &startPos,  &silenceLengthPasted);
			
		}				

		
	}
	
	//This function is here mainly for the benefit of  needshift ==1 
	//however, for thecase where needshift == 0, the recomputation of Streams Time does not logically affect the original computation by InitStreams
	//in this case, the recomputStreamsTime is basically for 
	//(a) setting the slider value
	//or (b) for recomputing streamstime after a conversion

	RecomputeStreamsTime(resetslider,timeCurrent);
	FixScrollbars();	
	FixWindowTitle();

	


}

void InsertAVIFile(PAVIFILE pfile, LPSTR lpszFile,long starttime, int keepcounter, int overwriteaudio)
{

	//By default, if the resetslider is not specified, it is assumed to be TRUE
	//I.e, the slider will be reset after each InsertAVIFile
	InsertAVIFile(pfile, lpszFile, starttime, keepcounter, overwriteaudio, TRUE);

}


void FreeAvi()
{
    int i;

    FreeDrawStuff();

    AVISaveOptionsFree(gcpavi, galpAVIOptions);

    for (i = 0; i < gcpavi; i++) {
	AVIStreamRelease(gapavi[i]);
    }

    // Good a place as any to make sure audio data gets freed
    if (lpAudio)
	GlobalFreePtr(lpAudio);
    lpAudio = NULL;

    gcpavi = 0;
}


void InitAvi(LPSTR szFile, int nMenu)
{
	InitAvi(szFile, nMenu, -1, KEEPCOUNTER, NEW_AUDIO_TRACK, RESET_TO_START);
}


void InitAvi(LPSTR szFile, int nMenu, long starttime, int keepcounter, int overwriteaudio, int resetslider)
{
    HRESULT     hr;
    PAVIFILE    pfile;

    hr = AVIFileOpen(&pfile, szFile, 0, 0L);

    if (hr != 0)
    {

		ErrMsg("Unable to open file %s", szFile);
		return;
    }

    //
    // If we're opening something new, close other open files, otherwise
    // just close the draw stuff so we'll merge streams with the new file
    //
    if (nMenu == MENU_OPEN) {
		FreeAvi();	
		fileModified = 0;
	}

	//ErrMsg("overwite in initavi %d",overwriteaudio);
    InsertAVIFile(pfile, szFile, starttime,  keepcounter,  overwriteaudio,resetslider);

}


UINT PlayAVIThread(LPVOID pParam) {

	while (playerAlive)
    {	

		if (!gfRecording && initAudioRecording) {

			initAudioRecording = FALSE;			
			gfRecording = TRUE;

			InitAudioRecording();
			StartAudioRecording(&m_Format);				

		}


		if (gfRecording && endAudioRecording) 
		{

			endAudioRecording = FALSE;
			gfRecording = FALSE;
			//gfPlaying = FALSE;

			StopAudioRecording();
			ClearAudioFile();	
			CreateSilenceFile();
	 
			
			::PostMessage(viewWnd,WM_USER_GENERIC,0,0);
			
		

		}

			
		if (gfPlaying) {			
			

			
			LONG    l;

			
			l = aviaudioTime();         // returns -1 if no audio playing


			//
			// If we can't use the audio clock to tell us how long we've been
			// playing, calculate it ourself
			//
			if (l == -1)
				l = timeGetTime() - glPlayStartTime + glPlayStartPos;
				


			if (l != (LONG)GetScrollTime()) {

				if (l < timeStart)      // make sure number isn't out of bounds
					l = timeStart;
				if (l > timeEnd)       { // looks like we're all done!
					
					if (pmode == PLAYER)
						PostMessage(viewWnd,WM_COMMAND,ID_FILE_STOP,0);
					else if (!allowRecordExtension) {
						//StopPlayingRecording();
						PostMessage(viewWnd,WM_COMMAND,ID_FILE_STOP,0);
					}
					else {

						//Allowing Recording Extension even if even if end of current file is reached
						if (gfRecording) {
							//Stop only the playing but not the recording
							gfPlaying = FALSE;
							//l = timeEnd;

						}
						else {

							//playing mode...stop the playing
							PostMessage(viewWnd,WM_COMMAND,ID_FILE_STOP,0);

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

int ErrMsg (LPSTR sz,...)
{
    static char szOutput[4*BUFSIZE];

    va_list va;

    va_start(va, sz);
    wvsprintf (szOutput,sz,va);      /* Format the string */
    va_end(va);
    
	if ((runmode==0) || (runmode==1))
		MessageBox(NULL,szOutput,NULL, MB_OK|MB_ICONEXCLAMATION|MB_TASKMODAL);
    return FALSE;
}


void FreeDrawStuff()
{
    int i;

    // Make sure we're not playing!
    aviaudioStop();

    for (i = 0; i < gcpavi; i++) {
	if (gapgf[i]) {
	    AVIStreamGetFrameClose(gapgf[i]);
	    gapgf[i] = NULL;
	}
	if (ghdd[i]) {
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
		if	(gfAudioFound)
				aviaudioStop();

	}
	//else {//silent mode
	//}

	gfPlaying = FALSE;


}



void RecomputeStreamsTime(int resetslider,long timeCurrent)
{
    
    timeStart = 0x7FFFFFFF;
    timeEnd   = 0;

	
    // Walk through and init all streams loaded    
    for (int i = 0; i < gcpavi; i++) {
		timeStart = min(timeStart, AVIStreamStartTime(gapavi[i]));
		timeEnd   = max(timeEnd, AVIStreamEndTime(gapavi[i]));
	}

	timeLength = timeEnd - timeStart;

    if (timeLength == 0) timeLength = 1;

    // Make sure HSCROLL scrolls enough to be noticeable.
    timehscroll = max(timehscroll, timeLength / MAXTIMERANGE + 2);
	

	//Set the slider positiom after the timeLenght has been adjusted
	if (resetslider == NO_RESET_SLIDER) {
		//Do nothing
	}
	else if (resetslider == RESET_TO_START) 
		SetScrollTime( timeStart);
	else if (resetslider == RESET_TO_CURRENT) //This setting has not been tested, it is set to the position of the scrolltime where no stream functions has been previously altered
		SetScrollTime( timeCurrent);

	return;


}


void InitStreams()
{
    AVISTREAMINFO     avis;
    LONG        lTemp;
    int         i;
	int needTestConvert = 0;

    //
    // Start with bogus times
    //
    timeStart = 0x7FFFFFFF;
    timeEnd   = 0;

    //
    // Walk through and init all streams loaded
    //
    for (i = 0; i < gcpavi; i++) {

	AVIStreamInfo(gapavi[i], &avis, sizeof(avis));

	//
	// Save and SaveOptions code takes a pointer to our compression opts
	//
	galpAVIOptions[i] = &gaAVIOptions[i];

	//
	// clear options structure to zeroes
	//
	_fmemset(galpAVIOptions[i], 0, sizeof(AVICOMPRESSOPTIONS));

	//
	// Initialize the compression options to some default stuff
	// !!! Pick something better
	//
	galpAVIOptions[i]->fccType = avis.fccType;

	switch(avis.fccType) {

	    case streamtypeVIDEO:
		galpAVIOptions[i]->dwFlags = AVICOMPRESSF_VALID |
				AVICOMPRESSF_KEYFRAMES | AVICOMPRESSF_DATARATE;
		galpAVIOptions[i]->fccHandler = 0;
		galpAVIOptions[i]->dwQuality = (DWORD)ICQUALITY_DEFAULT;
		galpAVIOptions[i]->dwKeyFrameEvery = (DWORD)-1; // Default
		galpAVIOptions[i]->dwBytesPerSecond = 0;
		galpAVIOptions[i]->dwInterleaveEvery = 1;
		break;

	    case streamtypeAUDIO:
		galpAVIOptions[i]->dwFlags |= AVICOMPRESSF_VALID;
		galpAVIOptions[i]->dwInterleaveEvery = 1;
		AVIStreamReadFormat(gapavi[i],
				    AVIStreamStart(gapavi[i]),
				    NULL,
				    &lTemp);
		galpAVIOptions[i]->cbFormat = lTemp;
		if (lTemp)
		    galpAVIOptions[i]->lpFormat = GlobalAllocPtr(GHND, lTemp);
		// Use current format as default format
		if (galpAVIOptions[i]->lpFormat)
		    AVIStreamReadFormat(gapavi[i],
					AVIStreamStart(gapavi[i]),
					galpAVIOptions[i]->lpFormat,
					&lTemp);
		break;

	    default:
		break;
	}

	//
	// We're finding the earliest and latest start and end points for
	// our scrollbar.
	//
	timeStart = min(timeStart, AVIStreamStartTime(gapavi[i]));
	timeEnd   = max(timeEnd, AVIStreamEndTime(gapavi[i]));

	//
	// Initialize video streams for getting decompressed frames to display
	//
	if (avis.fccType == streamtypeVIDEO) {

	    gapgf[i] = AVIStreamGetFrameOpen(gapavi[i], NULL);

	    if (gapgf[i] == NULL)
		continue;

	    ghdd[i] = DrawDibOpen();
	    // !!! DrawDibBegin?

	    if (!gfVideoFound) {
	    DWORD        dw;

		//
		// Remember the first video stream --- treat it specially
		//
		giFirstVideo = i;

		//
		// Set the horizontal scrollbar scale to show every frame
		// of the first video stream exactly once
		//
		dw = (avis.rcFrame.right - avis.rcFrame.left) * gwZoom / 4 + HSPACE;

		gdwMicroSecPerPixel = muldiv32(1000000,
					       avis.dwScale,
					       dw * avis.dwRate);
		// Move one frame on the top video screen for each HSCROLL
		timehscroll = muldiv32(1000, avis.dwScale, avis.dwRate);
	    }

	} else if (avis.fccType == streamtypeAUDIO) {

	    // These aren't used and better be NULL!
		ghdd[i] =   NULL;
		gapgf[i] = NULL;
	    //gapgf[i] =  ghdd[i] =   NULL;
		

	    //
	    // If there are no video streams, we base everything on this
	    // audio stream.
	    //
	    if (!gfAudioFound && !gfVideoFound) {

		// Show one sample per pixel
		gdwMicroSecPerPixel = muldiv32(1000000,
					       avis.dwScale,
					       avis.dwRate);
		// Move one sample per HSCROLL
		// Move at least enough to show movement
		timehscroll = muldiv32(1000, avis.dwScale, avis.dwRate);
	    }

	    //
	    // Remember the first audio stream --- treat it specially
	    //
		if (!gfAudioFound) {
			//ErrMsg("reset");
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




//if ret -1, pavi still valid, paviConverted if not null, is not valid
//if ret 0, pavi, which is unconverted, remains the valid stream, paviConverted if not null, is not valid
//if ret 1,2 or 3 , and paviConverted* is not null, both pavi and paviConverted is valid after returned	
//if ret 1,2 or 3 , and paviConverted* is null, only pavi is valid, paviConverted remains null
//if paviDstFormat is NULL, convert to defaultRecordingFormat
//pavi : stream to convert
//paviConverted  :ptr to converted straam
//paviDstFormat  :if not null and in PCM, convert to this format, esle convert to recoring format
int TestConvert(PAVISTREAM pavi,PAVISTREAM* paviConverted, PAVISTREAM paviDstFormat)
{

	//restore
	return 0;

	
	if (pmode == PLAYER) return 0;
	
	//0 :No Error, No Conversion Needed
	//-1 :Error
	//1 : Conversion Succeed, Compressed to PCM Conversion performed
	//2 : Conversion Succeed, PCM to PCM Rate Conversion performed
	//3 : Conversion Succeed, Compressed to PCM *and* PCM Rate Conversion performed
 	int ret = 0; 	


	
	AVISTREAMINFO avis;
	AVIStreamInfo(pavi, &avis, sizeof(avis));
	
	
	if (avis.fccType == streamtypeAUDIO) {

		WAVEFORMATEX PCMFormat;
		PAVISTREAM intermediatePCMStream = NULL;
		PAVISTREAM finalPCMStream = NULL;

		int nonPCM = 0;
		LPWAVEFORMATEX srcWaveFormat;
		srcWaveFormat =allocRetrieveAudioFormat(pavi);
		if (srcWaveFormat == NULL)
			return -1;
		
		if (srcWaveFormat->wFormatTag!=WAVE_FORMAT_PCM) 
		{
			nonPCM = 1;
			// *****************************
			// need conversion to PCM format
			// *****************************
					
			PCMFormat.wFormatTag = WAVE_FORMAT_PCM;
			MMRESULT mmr = acmFormatSuggest(NULL, srcWaveFormat, &PCMFormat, sizeof(PCMFormat), ACM_FORMATSUGGESTF_WFORMATTAG);	
	
			if (mmr!=0) {
				ErrMsg("Error suggesting PCM format for conversion !");
			}

			AVICOMPRESSOPTIONS compressOptions;
			_fmemset(&compressOptions, 0, sizeof(AVICOMPRESSOPTIONS));
			
			compressOptions.dwFlags |= AVICOMPRESSF_VALID;
			compressOptions.dwInterleaveEvery = 1;			
			compressOptions.cbFormat = sizeof(PCMFormat);		
		    compressOptions.lpFormat = &PCMFormat;			

			//ErrMsg("Here is fine 0!");
            if (AVIMakeCompressedStream(&intermediatePCMStream,pavi,&compressOptions,NULL)!=AVIERR_OK) 
			{
				ErrMsg("Error converting to PCM format!");
               return -1;
			}

			ret+=1;

		}		

		
		
		if (nonPCM) {

		
						
			//ReRead the audio format
			if (srcWaveFormat) 	free(srcWaveFormat);

		
			srcWaveFormat =allocRetrieveAudioFormat(intermediatePCMStream);	

			if (srcWaveFormat == NULL)
			{

				ErrMsg("Here is read 5!");

				//release the intermediate stream only if the original stream for conversion is non PCM
				if ((nonPCM) && (intermediatePCMStream)) AVIStreamRelease(intermediatePCMStream);        
				return -1;
			}

		}
		else 
			intermediatePCMStream = pavi;



		
		
		BuildRecordingFormat();		

		LPWAVEFORMATEX dstFormat = NULL;
		int needPhaseTwoConversion = 0;
		if (paviDstFormat) {		
				dstFormat=allocRetrieveAudioFormat(paviDstFormat);
				needPhaseTwoConversion = 1;
		
		}
		else {  // convert to recording format

		
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
				
				//release the intermediate stream only if the original stream for conversion is non PCM
				if (nonPCM) AVIStreamRelease(intermediatePCMStream);
                return -1;

			}

			if (srcWaveFormat->wFormatTag != WAVE_FORMAT_PCM)
			{
				ErrMsg("Invalid Intermediate Format! Intermediate Format is non PCM !");
				free(srcWaveFormat);
				
				//release the intermediate stream only if the original stream for conversion is non PCM
				if (nonPCM) AVIStreamRelease(intermediatePCMStream);
                return -1;

			}

			if (dstFormat) {

				if (dstFormat->wFormatTag != WAVE_FORMAT_PCM) {

					ErrMsg("Invalid Destination Format! Destination Format is non PCM !");
					free(srcWaveFormat);
					free(dstFormat);
				
					//release the intermediate stream only if the original stream for conversion is non PCM
					if (nonPCM) AVIStreamRelease(intermediatePCMStream);

					return -1;

				}

			}

			
			//Setting Dest Format
			PCMFormat.wFormatTag = WAVE_FORMAT_PCM;
			PCMFormat.nChannels = m_Format.nChannels;
			PCMFormat.nSamplesPerSec = m_Format.nSamplesPerSec;
			PCMFormat.nAvgBytesPerSec = m_Format.nAvgBytesPerSec;
			PCMFormat.nBlockAlign = m_Format.nBlockAlign;
			PCMFormat.wBitsPerSample = m_Format.wBitsPerSample;
			PCMFormat.cbSize = 0;

			//ErrMsg("Here is fine 1!");
			MMRESULT mmr;
			if (dstFormat) {

				//By not sugessting, but using convert directly...we increase the chance of no crashing..
				//mmr = acmFormatSuggest(NULL, srcWaveFormat, dstFormat, sizeof(PCMFormat), ACM_FORMATSUGGESTF_NCHANNELS  | ACM_FORMATSUGGESTF_NSAMPLESPERSEC | ACM_FORMATSUGGESTF_WBITSPERSAMPLE | ACM_FORMATSUGGESTF_WFORMATTAG);	


			}
			else {				
				
				mmr = acmFormatSuggest(NULL, srcWaveFormat, &PCMFormat, sizeof(PCMFormat), ACM_FORMATSUGGESTF_NCHANNELS  | ACM_FORMATSUGGESTF_NSAMPLESPERSEC | ACM_FORMATSUGGESTF_WBITSPERSAMPLE | ACM_FORMATSUGGESTF_WFORMATTAG );	

			}
	
			if (mmr!=0) {
			
				ErrMsg("Error suggesting compatible PCM working format for conversion !");
				
				free(srcWaveFormat);
			
				//release the intermediate stream only if the original stream for conversion is non PCM
				if (nonPCM) AVIStreamRelease(intermediatePCMStream);

				return -1;
			}

			if ((PCMFormat.nAvgBytesPerSec != m_Format.nAvgBytesPerSec) || (PCMFormat.nBlockAlign != m_Format.nBlockAlign)) 
			{

				ErrMsg("Note: The format suggested for conversion is not compatible with the working format !");
				
				//free(srcWaveFormat);			
				//release the intermediate stream only if the intermediate stream is not a pointer to the original stream
				//if (nonPCM) AVIStreamRelease(intermediatePCMStream);
				//return -1;

			}

			AVICOMPRESSOPTIONS compressOptionsFinal;
			_fmemset(&compressOptionsFinal, 0, sizeof(AVICOMPRESSOPTIONS));
			
			compressOptionsFinal.dwFlags |= AVICOMPRESSF_VALID;
			compressOptionsFinal.dwInterleaveEvery = 1;			

			//LPWAVEFORMATEX xformat = allocRetrieveAudioFormat(gapavi[giFirstAudio]);

			if (dstFormat) {
				//ErrMsg("1");
				compressOptionsFinal.cbFormat = sizeof(*dstFormat);		
				compressOptionsFinal.lpFormat = dstFormat;			
			}
			else {			
				compressOptionsFinal.cbFormat = sizeof(PCMFormat);		
				compressOptionsFinal.lpFormat = &PCMFormat;			
			}


            if (AVIMakeCompressedStream(&finalPCMStream,intermediatePCMStream,&compressOptionsFinal,NULL)!=AVIERR_OK) 
			{
				ErrMsg("Error converting PCM attributes !");
				free(srcWaveFormat);
				
				//release the intermediate stream only if the original stream for conversion is non PCM
				if (nonPCM) AVIStreamRelease(intermediatePCMStream);
                return -1;

			}

			ret+=2;
			

		}
		

		 
		if (srcWaveFormat) 	free(srcWaveFormat);


		if (ret==0) {//no conversion necessary

			if (paviConverted) //if paviConverted is not null
				*paviConverted = 0;
			
		}
		else if (ret==1)  { // only Compressed to PCM Conversion performed

			if (paviConverted) //if paviConverted is not null
				*paviConverted = intermediatePCMStream;
			else {

				if (intermediatePCMStream)
					AVIStreamRelease(intermediatePCMStream);


			}		

			

		}
		else if (ret==2)  { // only PCM Rate Conversion performed

			if (paviConverted) //if paviConverted is not null
				*paviConverted = finalPCMStream;
			else {
				if (finalPCMStream)
					AVIStreamRelease(finalPCMStream);

			}

			
		}
		else if (ret==3)  { // both Conversion performed


			if (paviConverted) //if paviConverted is not null
				*paviConverted = finalPCMStream;
			else {
				if (finalPCMStream)
					AVIStreamRelease(finalPCMStream);

			}

			if (intermediatePCMStream)
				AVIStreamRelease(intermediatePCMStream);

			
		}			



	}
	else {

		ErrMsg("Error converting audio, (Streamtype not Audio)");
		return -1;

	}
	

	return ret; 

}

//Assume Stream is an audio stream
//Need to free the waveformat after using this function
LPWAVEFORMATEX  allocRetrieveAudioFormat(PAVISTREAM pavi)
{
		LONG lStart,lLength;
		LPVOID srcFormat;

		lStart=AVIStreamStart(pavi);
		
		if (AVIStreamReadFormat(pavi,lStart,NULL,&lLength)!=0) {

			ErrMsg("Error converting audio (1)");
			return 0;

		}
		if ((srcFormat=malloc(lLength))==NULL) {
			ErrMsg("Error converting audio (2)");
			return 0;
		}
		if (AVIStreamReadFormat(pavi,lStart,srcFormat,&lLength)!=0)
		{
			ErrMsg("Error converting audio (3)");
			free(srcFormat);
			return 0;
		}

		return (LPWAVEFORMATEX) srcFormat;


}


void FixWindowTitle()
{

	char szTitle[512];    
	
	if (pmode == PLAYER) {

		if (gszFileTitle[0])     
			//wsprintf(szTitle, "Player - %s", (LPSTR)gszFileTitle);
			wsprintf(szTitle, "Player :- %s", (LPSTR)gszFileTitle);
		else
			wsprintf(szTitle, "Player");	

	}
	else {

		if (gszFileTitle[0])     
			//wsprintf(szTitle, "Player - %s", (LPSTR)gszFileTitle);
			wsprintf(szTitle, "SWF Producer :- %s", (LPSTR)gszFileTitle);
		else
			wsprintf(szTitle, "SWF Producer");

	}
	

	SetTitleBar(szTitle);
    
	
}

void SetTitleBar(CString title) {
	
	CWinApp* app = NULL;
	app	= AfxGetApp();
	if (app) {

		HWND mainwnd = NULL;
		mainwnd= app->m_pMainWnd->m_hWnd;
		if (mainwnd)
			::SetWindowText(mainwnd, LPCTSTR(title));

	}
}




void FixScrollbars()
{
	
    int                 nHeight = 0;
	int                 nWidth = 0;
    RECT                rc;
    HDC                 hdc;
	HWND				hwnd;


	hwnd = viewWnd;

	
    //
    // Determine how tall our window needs to be to display everything.
    //
    hdc = GetDC(NULL);
    ExcludeClipRect(hdc, 0, 0, 32767, 32767);   // don't actually draw
    nHeight = PaintStuff(hdc, hwnd, TRUE);
    ReleaseDC(NULL, hdc);

    //
    // Set vertical scrollbar for scrolling the visible area
    //
    GetClientRect(hwnd, &rc);
    nVertHeight = nHeight;      // total height in pixels of entire display
	
	
	RECT rcImage;
	GetImageDimension(rcImage);	
	
	if (gwZoom>=4)
		nHorzWidth = ((rcImage.right-rcImage.left+1)*gwZoom)/4;
	else
		nHorzWidth = rcImage.right-rcImage.left+1;

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


//Using this function assume that gcpavi, the number of streams
//and gapavi[], the stream array is properly initialized
//Return the Dimension of the first video stream
void GetImageDimension(RECT& rcFrame) 
{
	AVISTREAMINFO   avis;	

	avis.rcFrame.left=0;
	avis.rcFrame.right=0;
	avis.rcFrame.top=0;
	avis.rcFrame.bottom=0;


	int j;
	for (j=0; j<gcpavi; j++) {			
	
		AVIStreamInfo(gapavi[j], &avis, sizeof(avis));

		if (avis.fccType == streamtypeVIDEO) 
		{
			if (gapgf[j] != NULL)			
				break; //break from for loop	

		}		

	}

	rcFrame=avis.rcFrame;

}

int CPlayplusView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
    WORD wVer = HIWORD(VideoForWindowsVersion());
    if (wVer < 0x010a)
    {	
		if ((runmode==0) || (runmode==1))
			MessageOut(NULL,IDS_TPRVFW11, IDS_NOTE, MB_OK | MB_ICONEXCLAMATION);
		return -1;
    }

	AVIFileInit();

	
	HDC hScreenDC = ::GetDC(NULL);	
	nColors = ::GetDeviceCaps(hScreenDC, BITSPIXEL );		 	
	maxxScreen = GetDeviceCaps(hScreenDC,HORZRES);
	maxyScreen = GetDeviceCaps(hScreenDC,VERTRES);		
	::ReleaseDC(NULL,hScreenDC);		

	
	if (pmode == PLAYER)
		hLogoBM = LoadBitmap( AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BITMAP_LOGO_PLAYER)); 
	else if (pmode == DUBBER)
		hLogoBM = LoadBitmap( AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BITMAP_LOGO_DUBBER)); 
	else
		hLogoBM = LoadBitmap( AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BITMAP2)); 
	

	int tdata=0;
	CWinThread * pThread = AfxBeginThread(PlayAVIThread, &tdata);	

	audioPlayable = waveOutGetNumDevs ();
	
	audioRecordable = waveInGetNumDevs();
	if (audioRecordable) {
		
		SuggestRecordingFormat();
		SuggestCompressFormat();

	}

	if (pmode == PLAYER)
		allowRecordExtension = 0;
	else if (pmode == DUBBER)
		allowRecordExtension = 1;


	//ver 2.25
	CreatePropertySheet();


	return 0;
}

void CPlayplusView::OnDestroy() 
{
		
	CView::OnDestroy();
	

	AVIClearClipboard();
    FreeAvi();      // close all open streams
    AVIFileExit();      // shuts down the AVIFile system

	if (hLogoBM) {
		DeleteObject(hLogoBM);
		hLogoBM = NULL;
	}


	if (pwfx) {
		
		GlobalFreePtr(pwfx);
		pwfx = NULL;

	}



	if (runmode == 0)
		SaveSettings() ;


	//SaveController();

	
}

void CPlayplusView::OnPaint() 
{

	if (!doneOnce) {
	
		if (strlen(playfiledir)!=0) {		

			OpenMovieFileInit(playfiledir);
			doneOnce=1;
			
			if (runmode>0)
			{	
				
				//Assign default output name
				//swfname = playfiledir;
				AdjustOutName(playfiledir);
								
				//silence mode
				if (runmode == 2)
				{
					//these settings will override those in CamStudio.Producer.param file 
					//becuase loadsettings is called first
					launchHTMLPlayer = 0;
					launchPropPrompt = 0;
					deleteAVIAfterUse = 0;
					

				}				
				
				//launchHTMLPlayer = 0;
				//OnFileConverttoswf();
				PostMessage(WM_COMMAND,ID_FILE_CONVERTTOSWF,0);
				
			}

		}
		doneOnce=1;

	}


	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here
	if (gcpavi<=0) {
		

		if (nColors >= 8) {

			HDC hdcBits = CreateCompatibleDC(dc.m_hDC);
			
			CRect rect;	
			GetClientRect( &rect);


			int offsetx=0;
			int offsety=0;
			int bitmapwidth = 300;
			int bitmapheight = 220;
			
			if (rect.right> bitmapwidth)
				offsetx = (rect.right - bitmapwidth)/2;

			if (rect.bottom> bitmapheight)
				offsety = (rect.bottom - bitmapheight)/2;

		
			
			HBITMAP old_bitmap =  (HBITMAP) ::SelectObject(hdcBits,hLogoBM);
		
			//OffScreen Buffer
			::BitBlt(dc.m_hDC, offsetx, offsety, rect.right-rect.left+1, rect.bottom-rect.top+1, hdcBits, 0, 0, SRCCOPY);              
		
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
	if (timeLength > 0) {	
		
		long timenow = GetScrollTime();
		float durationPlayed = (float) ((timenow - timeStart)/1000.0);		
		float durationLength = (float) ((timeEnd - timeStart)/1000.0);
		durationStr.Format(" %6.1f /%6.1f sec",durationPlayed,durationLength);		
		
	}

	if (statusbarCtrl)
			statusbarCtrl->SetPaneText(2,durationStr);

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

	//istr += durationStr;

	if (statusbarCtrl)
		statusbarCtrl->SetPaneText(1,istr);

}

BOOL CPlayplusView::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) 
{
	// TODO: Add your specialized code here and/or call the base class
	BOOL retval = CWnd::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);

	viewWnd = m_hWnd;
	
	swfname = GetProgPath() + "\\untitled.swf";
	swfhtmlname = GetProgPath() + "\\untitled.swf.html";
	swfbasename = "untitled.swf";
	
	if (runmode == 1)
		LoadCommand(); //used internally by CamStudio Recorder
	else if (runmode == 2)
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

	if (gcpavi<=0) {

		if (hwnd) {
			::SetScrollRange(hwnd, SB_VERT, 0, 0, TRUE);
			::SetScrollRange(hwnd, SB_HORZ, 0, 0, TRUE);		
		}
		
		return;
	}

	//AdjustToolBar (hwnd);

    ::GetClientRect(hwnd, &rc);

	::GetWindowRect(hwnd, &rcWnd);
    //
    // There is not enough vertical room to show all streams. Scrollbars
    // are required.
    //	


	//Added ver 1.0
	int ScrollVertWidth = GetSystemMetrics(SM_CXVSCROLL); 
	int ScrollHorzHeight = GetSystemMetrics(SM_CYHSCROLL ); 

	int MinPos; 
	int MaxPos; 	
	int IncYVal = 0;

	int origbottom=0;
	int origright=0;
	origbottom=rc.bottom;
	origright=rc.right;
	
	//Obtaining true client area including the scroll bars
	
	MaxPos = 0;
	::GetScrollRange( hwnd, SB_HORZ, &MinPos , &MaxPos);	
	if (MaxPos>0)
		rc.bottom += ScrollHorzHeight;


	
	MaxPos = 0;
	::GetScrollRange( hwnd, SB_VERT, &MinPos , &MaxPos);
	if (MaxPos>0) {
	
		rc.right += ScrollVertWidth;

		//This is a hack, as the above code GetScrollRange may sometimes detect the presence of a scroll bar even if it is absent
		if (rc.right>rcWnd.right-rcWnd.left)
			rc.right -= ScrollVertWidth;  //deduct back 

	}

	
	//rc.right and rc.bottom now refers to the true client area
	if ((nVertHeight > rc.bottom  ) && (nHorzWidth > rc.right)) {
	
		
		//Both Scroll Bars
	
		//Adjust the client client to reflect both scroll bars
		rc.bottom -= ScrollHorzHeight;
		rc.right -= ScrollVertWidth;

		nVertSBLen = nVertHeight - rc.bottom;
		::SetScrollRange(hwnd, SB_VERT, 0, nVertSBLen, TRUE);

		nHorzSBLen = nHorzWidth - rc.right;
		::SetScrollRange(hwnd, SB_HORZ, 0, nHorzSBLen, TRUE);		

	}
	else if ((nVertHeight <= rc.bottom  ) && (nHorzWidth <= rc.right))
	{
		
		//No Scroll Bars
		nVertSBLen = 0;
		::SetScrollRange(hwnd, SB_VERT, 0, 0, TRUE);
		nHorzSBLen = 0;
		::SetScrollRange(hwnd, SB_HORZ, 0, 0, TRUE);


	}		
	else  if ((nVertHeight >rc.bottom  ) && (nHorzWidth <= rc.right))
	{

		if (nHorzWidth <= rc.right - ScrollVertWidth) { //if after the addition of vertical SCROLLBAR, the horizontal width is still larger than image

			//Vert Scroll Bar Only
			nHorzSBLen = 0;
			::SetScrollRange(hwnd, SB_HORZ, 0, 0, TRUE);

			nVertSBLen = nVertHeight - rc.bottom;
			::SetScrollRange(hwnd, SB_VERT, 0, nVertSBLen, TRUE);

		}
		else  {			
			
			//Activate Both ScrollBars

			//Adjust the client client to reflect both scroll bars
			rc.bottom -= ScrollHorzHeight;
			rc.right -= ScrollVertWidth;

			nVertSBLen = nVertHeight - rc.bottom;
			::SetScrollRange(hwnd, SB_VERT, 0, nVertSBLen, TRUE);
	
			nHorzSBLen = nHorzWidth - rc.right;
			::SetScrollRange(hwnd, SB_HORZ, 0, nHorzSBLen, TRUE);				

		}



	}
	else  if ((nVertHeight <= rc.bottom  ) && (nHorzWidth > rc.right))
	{


		if (nVertHeight <= rc.bottom - ScrollHorzHeight) { //if after the addition of horiz SCROLLBAR, the vertical height is still larger than image

			//Horiz Scroll Bar Only
			nVertSBLen = 0;
			::SetScrollRange(hwnd, SB_VERT, 0, 0, TRUE);

			nHorzSBLen = nHorzWidth - rc.right;
			::SetScrollRange(hwnd, SB_HORZ, 0, nHorzSBLen, TRUE);		

		}
		else  {			
			
			//Activate Both ScrollBars

			//Adjust the client client to reflect both scroll bars
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


BOOL CPlayplusView::OnEraseBkgnd(CDC* pDC) 
{
	
      // Set brush to desired background color
      CBrush backBrush(RGB(0, 0, 0));

      // Save old brush
      CBrush* pOldBrush = pDC->SelectObject(&backBrush);

      CRect rect;
      pDC->GetClipBox(&rect);     // Erase the area needed

      pDC->PatBlt(rect.left, rect.top, rect.Width(), rect.Height(),
          PATCOPY);
      pDC->SelectObject(pOldBrush);
      return TRUE;
    
}


void CPlayplusView::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
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
    //InvalidateRect(hwnd, NULL, TRUE);
	::InvalidateRect(hwnd, NULL, FALSE);
    ::UpdateWindow(hwnd);

	
	CView::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CPlayplusView::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
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
	//InvalidateRect(hwnd, NULL, TRUE);
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

void CPlayplusView::OnUpdateZoom1(CCmdUI* pCmdUI) 
{

	pCmdUI->SetCheck(gwZoom==4);
	
}

void CPlayplusView::OnUpdateZoom2(CCmdUI* pCmdUI) 
{

	pCmdUI->SetCheck(gwZoom==8);
	
}

void CPlayplusView::OnUpdateZoom4(CCmdUI* pCmdUI) 
{

	pCmdUI->SetCheck(gwZoom==16);
	
}

void CPlayplusView::OnUpdateZoomHalf(CCmdUI* pCmdUI) 
{

	pCmdUI->SetCheck(gwZoom==2);
	
}

void CPlayplusView::OnUpdateZoomQuarter(CCmdUI* pCmdUI) 
{

	pCmdUI->SetCheck(gwZoom==1);
	
}

void CPlayplusView::OnUpdateZoomResizetomoviesize(CCmdUI* pCmdUI) 
{

	pCmdUI->Enable(gwZoom==4);
	
}



long GetScrollTime() {

	return playtime;

}


void SetScrollTime(long time)
{
		
	playtime = time;

	
	if (timeLength > 0) {
	
		float durationPlayed = (float) ((time - timeStart)/1000.0);
		CString durationStr;
		durationStr.Format("%8.1f sec",durationPlayed);
		SetTimeIndicator(durationStr);	

	}	

	int x=0;
	if (timeLength>0)
		x =  ((time - timeStart) * MAXTIMERANGE) / timeLength;
	else
		x = 0;
		

	if (x<0)
		x = 0;

	if (x>MAXTIMERANGE)
		x = MAXTIMERANGE;

	if (sliderCtrlPtr) 
		sliderCtrlPtr->SetPos(x);
	
	
}


void CPlayplusView::OnUpdateFileRewind(CCmdUI* pCmdUI) 
{
	BOOL enablebutton = (!gfPlaying) && (!gfRecording);
	pCmdUI->Enable(enablebutton);
	
}


void CPlayplusView::OnUpdateFileLastframe(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	BOOL enablebutton = (!gfPlaying) && (!gfRecording);
	pCmdUI->Enable(enablebutton);
		
}

void CPlayplusView::OnButtonRecord() 
{

	if (!gfRecording) {				
		
		initAudioRecording  =TRUE;
		
	    glRecordStartTimeValue = GetScrollTime();

		PlayMovie(SILENT_MODE);

		fileModified = 1;

	}
	
	
}

void CPlayplusView::OnUpdateRecord(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here	
	//BOOL enablebutton = (!gfPlaying) && (!gfRecording) && (giFirstAudio<0) && (giFirstVideo>=0) && (audioRecordable);
	BOOL enablebutton = (!gfPlaying) && (!gfRecording) && (giFirstVideo>=0) && (audioRecordable);
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
int Merge_Video_And_Sound_File(CString input_video_path, CString input_audio_path, CString output_avi_path, BOOL recompress_audio, LPWAVEFORMATEX audio_recompress_format, DWORD  audio_format_size, BOOL bInterleave, int interleave_factor) {	    

	PAVISTREAM            AviStream[NUMSTREAMS];      // the editable streams
	AVICOMPRESSOPTIONS    gaAVIOptions[NUMSTREAMS];   // compression options
	LPAVICOMPRESSOPTIONS  galpAVIOptions[NUMSTREAMS];	
   
	PAVIFILE pfileVideo = NULL;
	
	
	AVIFileInit();	    
	
	//Open Video and Audio Files	
	HRESULT hr = AVIFileOpen(&pfileVideo, LPCTSTR(input_video_path), OF_READ | OF_SHARE_DENY_NONE, 0L);
    if (hr != 0)
    {	
		if ((runmode==0) || (runmode==1))
			//MessageBox(NULL,"Unable to open video file.","Note",MB_OK | MB_ICONEXCLAMATION);
			MessageOut(NULL,IDS_UTOVF, IDS_NOTE, MB_OK | MB_ICONEXCLAMATION);
		return 1;
    }
    
        
    
    //Get Video Stream from Video File and Audio Stream from Audio File        
	
	// ==========================================================
	// Important Assumption
    // Assume stream 0 is the correct stream in the files
    // ==========================================================
    if (pfileVideo) {

		PAVISTREAM  pavi;		
	   	if (AVIFileGetStream(pfileVideo, &pavi, streamtypeVIDEO , 0) != AVIERR_OK)
	   	{
			AVIFileRelease(pfileVideo);	   	
	 		if ((runmode==0) || (runmode==1))
				 MessageOut(NULL,IDS_UTOVS, IDS_NOTE, MB_OK | MB_ICONEXCLAMATION);
				//MessageBox(NULL,"Unable to open video stream.","Note",MB_OK | MB_ICONEXCLAMATION);			
			return 1;

    	}
		
		//Set editable stream number as 0
		if (CreateEditableStream(&AviStream[0], pavi) != AVIERR_OK) {
	
			AVIStreamRelease(pavi);
			AVIFileRelease(pfileVideo);
			if ((runmode==0) || (runmode==1))
				MessageOut(NULL,IDS_UTCEVS, IDS_NOTE, MB_OK | MB_ICONEXCLAMATION);
				//MessageBox(NULL,"Unable to create editable video stream.","Note",MB_OK | MB_ICONEXCLAMATION);			
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
		
		PAVISTREAM  pavi;		
		if (AVIStreamOpenFromFile(&pavi,input_audio_path,streamtypeAUDIO,0,OF_READ | OF_SHARE_DENY_NONE,NULL)!=AVIERR_OK)
		{
	   		AVIStreamRelease(AviStream[0]);
			if ((runmode==0) || (runmode==1))
					MessageOut(NULL,IDS_UTOAS, IDS_NOTE, MB_OK | MB_ICONEXCLAMATION);
					//MessageBox(NULL,"Unable to open audio stream.","Note",MB_OK | MB_ICONEXCLAMATION);			
			return 2;
   		}
			
		//Set editable stream number as 1
		if (CreateEditableStream(&AviStream[1], pavi) != AVIERR_OK) {
		
			AVIStreamRelease(pavi);
			AVIStreamRelease(AviStream[0]);
			if ((runmode==0) || (runmode==1))
				MessageOut(NULL,IDS_TOCEAS, IDS_NOTE, MB_OK | MB_ICONEXCLAMATION);
				//MessageBox(NULL,"Unable to create editable audio stream.","Note",MB_OK | MB_ICONEXCLAMATION);			
			return 2;
		}		
		AVIStreamRelease(pavi);

	}	
	
	

    //Verifying streams are of the correct type
    AVISTREAMINFO     avis[NUMSTREAMS];
       
	AVIStreamInfo(AviStream[0], &avis[0], sizeof(avis[0]));
	AVIStreamInfo(AviStream[1], &avis[1], sizeof(avis[1]));
	
	//Assert that the streams we are going to work with are correct in our assumption
	//such that stream 0 is video and stream 1 is audio
	
	if (avis[0].fccType != streamtypeVIDEO) {
	
		if ((runmode==0) || (runmode==1))
			MessageOut(NULL,IDS_UTVVS, IDS_NOTE, MB_OK | MB_ICONEXCLAMATION);
			//MessageBox(NULL,"Unable to verify video stream.","Note",MB_OK | MB_ICONEXCLAMATION);			
		AVIStreamRelease(AviStream[0]);
		AVIStreamRelease(AviStream[1]);
		return 3;	
	
	}
		
	
	if (avis[1].fccType != streamtypeAUDIO) {
	

		if ((runmode==0) || (runmode==1))
			MessageOut(NULL,IDS_UTVAS, IDS_NOTE, MB_OK | MB_ICONEXCLAMATION);
			//MessageBox(NULL,"Unable to verify audio stream.","Note",MB_OK | MB_ICONEXCLAMATION);			
		AVIStreamRelease(AviStream[0]);
		AVIStreamRelease(AviStream[1]);
		return 4;	
	
	}
		
	  
	//
	// AVISaveV code takes a pointer to compression opts
	//
	galpAVIOptions[0] = &gaAVIOptions[0];
	galpAVIOptions[1] = &gaAVIOptions[1];

	//
	// clear options structure to zeroes
	//
	_fmemset(galpAVIOptions[0], 0, sizeof(AVICOMPRESSOPTIONS));
	_fmemset(galpAVIOptions[1], 0, sizeof(AVICOMPRESSOPTIONS));

	//=========================================
	//Set Video Stream Compress Options
	//=========================================
	//No Recompression
	galpAVIOptions[0]->fccType = streamtypeVIDEO;
	galpAVIOptions[0]->fccHandler = 0;
	galpAVIOptions[0]->dwFlags = AVICOMPRESSF_VALID | AVICOMPRESSF_KEYFRAMES | AVICOMPRESSF_DATARATE;
	if (bInterleave) 
		galpAVIOptions[0]->dwFlags = galpAVIOptions[0]->dwFlags | AVICOMPRESSF_INTERLEAVE; 
	galpAVIOptions[0]->dwKeyFrameEvery = (DWORD) -1;
	galpAVIOptions[0]->dwQuality = (DWORD)ICQUALITY_DEFAULT;
	galpAVIOptions[0]->dwBytesPerSecond = 0;
	galpAVIOptions[0]->dwInterleaveEvery = interleave_factor;
	//galpAVIOptions[0]->cbParms = 0;
	//galpAVIOptions[0]->cbFormat = 0;
	
	
	//=========================================
	//Set Audio Stream Compress Options
	//=========================================
	//Recompression may be applied
	//
	//Audio Compress Options seems to be specified by the audio format in avicompressoptions
	galpAVIOptions[1]->fccType = streamtypeAUDIO;
	galpAVIOptions[1]->fccHandler = 0;
	galpAVIOptions[1]->dwFlags = AVICOMPRESSF_VALID;
	if (bInterleave) 
		galpAVIOptions[1]->dwFlags = galpAVIOptions[1]->dwFlags | AVICOMPRESSF_INTERLEAVE; 
	galpAVIOptions[1]->dwKeyFrameEvery = 0;
	galpAVIOptions[1]->dwQuality = 0;
	galpAVIOptions[1]->dwBytesPerSecond = 0;
	galpAVIOptions[1]->dwInterleaveEvery = interleave_factor;
	//galpAVIOptions[1]->cbParms = 0;

	if (recompress_audio) {

		galpAVIOptions[1]->cbFormat = audio_format_size;
		galpAVIOptions[1]->lpFormat = GlobalAllocPtr(GHND, audio_format_size); 	
		memcpy( (void *) galpAVIOptions[1]->lpFormat,  (void *) audio_recompress_format, audio_format_size );
		
	}
	else {

	
		LONG lTemp;
		AVIStreamReadFormat(AviStream[1], AVIStreamStart(AviStream[1]), NULL, &lTemp);
		galpAVIOptions[1]->cbFormat = lTemp;
		
		if (lTemp)  galpAVIOptions[1]->lpFormat = GlobalAllocPtr(GHND, lTemp);		
		// Use existing format as compress format
		if (galpAVIOptions[1]->lpFormat)     
			AVIStreamReadFormat(AviStream[1],	AVIStreamStart(AviStream[1]),galpAVIOptions[1]->lpFormat, &lTemp);
	

	}

	// ============================
	// Do the work! Merging
	// ============================
			
	//Save 	fccHandlers 
	DWORD fccHandler[NUMSTREAMS]; 
	fccHandler[0] = galpAVIOptions[0]->fccHandler;
	fccHandler[1] = galpAVIOptions[1]->fccHandler;
	
	hr = AVISaveV(LPCTSTR(output_avi_path),  NULL, (AVISAVECALLBACK) SaveCallback, NUMSTREAMS, AviStream, galpAVIOptions);	
	//hr = AVISaveV(LPCTSTR(output_avi_path),  NULL, (AVISAVECALLBACK) NULL, NUMSTREAMS, AviStream, galpAVIOptions);
	if (hr != AVIERR_OK) {

		//Error merging with audio compress options, retry merging with default audio options (no recompression)
		if (recompress_audio) {

			
			AVISaveOptionsFree(NUMSTREAMS,galpAVIOptions);
			
			galpAVIOptions[0] = &gaAVIOptions[0];
			galpAVIOptions[1] = &gaAVIOptions[1];
			
			//Resetting Compress Options
			_fmemset(galpAVIOptions[0], 0, sizeof(AVICOMPRESSOPTIONS));
			_fmemset(galpAVIOptions[1], 0, sizeof(AVICOMPRESSOPTIONS));

			galpAVIOptions[0]->fccType = streamtypeVIDEO;
			galpAVIOptions[0]->fccHandler = 0;
			galpAVIOptions[0]->dwFlags = AVICOMPRESSF_VALID | AVICOMPRESSF_KEYFRAMES | AVICOMPRESSF_DATARATE;
			if (bInterleave) 
				galpAVIOptions[0]->dwFlags = galpAVIOptions[0]->dwFlags | AVICOMPRESSF_INTERLEAVE; 
			galpAVIOptions[0]->dwKeyFrameEvery = (DWORD) -1;
			galpAVIOptions[0]->dwQuality = (DWORD)ICQUALITY_DEFAULT;
			galpAVIOptions[0]->dwBytesPerSecond = 0;
			galpAVIOptions[0]->dwInterleaveEvery = interleave_factor;
			
			galpAVIOptions[1]->fccType = streamtypeAUDIO;
			galpAVIOptions[1]->fccHandler = 0;
			galpAVIOptions[1]->dwFlags = AVICOMPRESSF_VALID;
			if (bInterleave) 
				galpAVIOptions[1]->dwFlags = galpAVIOptions[1]->dwFlags | AVICOMPRESSF_INTERLEAVE; 
			galpAVIOptions[1]->dwKeyFrameEvery = 0;
			galpAVIOptions[1]->dwQuality = 0;
			galpAVIOptions[1]->dwBytesPerSecond = 0;
			galpAVIOptions[1]->dwInterleaveEvery = interleave_factor;	

			//Use default audio format
			LONG lTemp;
			AVIStreamReadFormat(AviStream[1], AVIStreamStart(AviStream[1]), NULL, &lTemp);
			galpAVIOptions[1]->cbFormat = lTemp;			
			if (lTemp)  galpAVIOptions[1]->lpFormat = GlobalAllocPtr(GHND, lTemp);		
			// Use existing format as compress format
			if (galpAVIOptions[1]->lpFormat)     
					AVIStreamReadFormat(AviStream[1],	AVIStreamStart(AviStream[1]),galpAVIOptions[1]->lpFormat, &lTemp);
			
			//Do the Work .... Merging
			hr = AVISaveV(LPCTSTR(output_avi_path),  NULL, (AVISAVECALLBACK) NULL, NUMSTREAMS, AviStream, galpAVIOptions);

			if (hr != AVIERR_OK) {		
			
				AVISaveOptionsFree(NUMSTREAMS,galpAVIOptions);						
				AVIStreamRelease(AviStream[0]);
				AVIStreamRelease(AviStream[1]);	    		
				
				if ((runmode==0) || (runmode==1))
					MessageOut(NULL,IDS_UTMAVS, IDS_NOTE, MB_OK | MB_ICONEXCLAMATION);
					//MessageBox(NULL,"Unable to merge audio and video streams (1).","Note",MB_OK | MB_ICONEXCLAMATION);	
				return 5;

			}

			//Succesful Merging, but with no audio recompression
			if ((runmode==0) || (runmode==1))
				MessageOut(NULL,IDS_UAACSOYMIAC, IDS_NOTE, MB_OK | MB_ICONEXCLAMATION);
				//MessageBox(NULL,"Unable to apply audio compression with the selected options. Your movie is saved without audio compression.","Note",MB_OK | MB_ICONEXCLAMATION);		


		} // if recompress audio retry
		else {
			
			AVISaveOptionsFree(NUMSTREAMS,galpAVIOptions);						
			AVIStreamRelease(AviStream[0]);
			AVIStreamRelease(AviStream[1]);	    		
			if ((runmode==0) || (runmode==1))
				MessageOut(NULL,IDS_UTMAVS2, IDS_NOTE, MB_OK | MB_ICONEXCLAMATION);
				//MessageBox(NULL,"Unable to audio and video merge streams (2).","Note",MB_OK | MB_ICONEXCLAMATION);	
			return 5;

		}
	    	 
	}

		    	    
	// Restore fccHandlers
	galpAVIOptions[0]->fccHandler = fccHandler[0];
	galpAVIOptions[1]->fccHandler = fccHandler[1];
	
	//Set Title Bar
	HWND mainwnd = NULL;
	mainwnd = AfxGetApp()->m_pMainWnd->m_hWnd;
	if (mainwnd)
		::SetWindowText(mainwnd,"CamStudio");
    
    
    AVISaveOptionsFree(NUMSTREAMS,galpAVIOptions);		
	
	// Free Editable Avi Streams
	for (int i=0;i<	NUMSTREAMS;i++) {
	
		if (AviStream[i]) {
			AVIStreamRelease(AviStream[i]);
			AviStream[i]=NULL;		
		}
		
	}	
	
	AVIFileExit();
	
	return 0;

}


BOOL CALLBACK SaveCallback(int iProgress)
{
    //Set Progress in Title Bar
	
	char    szText[300];

    wsprintf(szText, "Compressing Audio %d%%", iProgress);

	HWND mainwnd = NULL;
	mainwnd = AfxGetApp()->m_pMainWnd->m_hWnd;
	if (mainwnd)
		::SetWindowText(mainwnd, szText);
    
    return WinYield();
	//return FALSE;
}



BOOL WinYield(void)
{

	//Process 3 messages, then return false
	MSG msg;
	for (int i=0;i<3; i++) {

		if (PeekMessage(&msg,NULL,0,0,PM_REMOVE)) {
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
LRESULT CPlayplusView::OnUserGeneric	(UINT wParam, LONG lParam) 
{
	
	//AddAudioWaveFile((char *) LPCTSTR(tempaudiopath));
	//::MessageBox(NULL,tempaudiopath,"User Generic",MB_OK | MB_ICONEXCLAMATION);
	TimeDependentInsert(tempaudiopath,glRecordStartTimeValue);

	SetRPSLine();


	return 0;

}

void TimeDependentInsert(CString filename,long shiftTime)
{
	int overwriteaudio = NEW_AUDIO_TRACK;
	if (giFirstAudio>=0)
		overwriteaudio = OVERWRITE_AUDIO;
	else
		overwriteaudio = NEW_AUDIO_TRACK;

	CString convertedPath;
	convertedPath = ConvertFileToWorkingPCM(filename);

	
	//Only the NEW_AUDIO_TRACK selection will cause the resetslider var (FALSE) to have any effect
	if (shiftTime > timeStart)	
		InitAvi((char *) LPCTSTR(convertedPath), MENU_MERGE, shiftTime, KEEPCOUNTER, overwriteaudio,RESET_TO_CURRENT);
	else
		InitAvi((char *) LPCTSTR(convertedPath), MENU_MERGE, -1, KEEPCOUNTER, overwriteaudio,RESET_TO_START);


}


CString ConvertFileToWorkingPCM(CString filename)
{

	CString convertedPath = filename;

	//ErrMsg("overwriteaudio %d",overwriteaudio);
	//Only the NEW_AUDIO_TRACK selection will cause the resetslider var (FALSE) to have any effect
		
	HCURSOR  hcur;
	hcur = SetCursor(LoadCursor(NULL, IDC_WAIT));
	ShowCursor(TRUE);


	int retval = 0;	
	BuildRecordingFormat();	
	convertedPath =  GetTempPathEx("\\~converted001.wav", "\\~converted", ".wav");
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
	TCHAR    szTemp[300];
	CFile converter;
	CString result;
  
	// get root
	GetModuleFileName( NULL, szTemp, 300 );

	CString path=(CString)szTemp;
	path=path.Left(path.ReverseFind('\\'));
	return path;	

}


CString GetTempPath()
{
	
	char dirx[300];
	GetWindowsDirectory(dirx,300);
	CString tempdir(dirx);
	tempdir = tempdir + "\\temp";			


	//Verify the chosen temp path is valid
	
	WIN32_FIND_DATA wfd;
	memset(&wfd, 0, sizeof (wfd));	
	HANDLE hdir = FindFirstFile(LPCTSTR(tempdir), &wfd);
	if (!hdir) {
		return GetProgPath();
	}	
	FindClose(hdir);	
	
	//If valid directory, return Windows\temp as temp directory
	if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) 
		return tempdir;
	
	//else return program path as temp directory	
	return GetProgPath();	

}


//===============================================
// AUDIO CODE  
//===============================================
// Ver 1.1
//===============================================

void waveInErrorMsg(MMRESULT result, const char * addstr)
{
	// say error message
	char errorbuffer[300];
	waveInGetErrorText(result, errorbuffer,300);
	//ErrorMsg("WAVEIN:%x:%s %s", result, errorbuffer, addstr);
	CString msgstr;
	msgstr.Format("%s %s",errorbuffer, addstr);
	if ((runmode==0) || (runmode==1))
	{
		CString title;
		title.LoadString(IDS_WIERROR);
		MessageBox(NULL,msgstr,title,MB_OK | MB_ICONEXCLAMATION);

	}
}

BOOL InitAudioRecording()
{
	m_ThreadID = ::GetCurrentThreadId();
	m_QueuedBuffers = 0;
	m_hRecord = NULL;

	m_BufferSize		= 1000;  // samples per callback

	
	BuildRecordingFormat();

	ClearAudioFile();			
	
	//Create temporary wav file for audio recording
	GetTempWavePath();	
	
	//look up here
	m_pFile = new CSoundFile(tempaudiopath, &m_Format);	
	
	//even if we recording with firstaudio's format, we still get the unable to insert problem!
	//m_pFile = new CSoundFile(tempaudiopath, (LPWAVEFORMATEX) galpAVIOptions[giFirstAudio]->lpFormat);	
		
	
	if  (!(m_pFile && m_pFile->IsOK()))
	{
		if ((runmode==0) || (runmode==1))
			MessageOut(NULL,IDS_ECSF, IDS_NOTE, MB_OK | MB_ICONEXCLAMATION);


	}
			
	return TRUE;
}


//Initialize the tempaudiopath variable with a valid temporary path
void GetTempWavePath() {

	CString fileName("\\~twav001.wav");
	tempaudiopath = GetTempPath () + fileName;

	//Test the validity of writing to the file
	int fileverified = 0;
	while (!fileverified) 
	{
		OFSTRUCT ofstruct;	
		HFILE fhandle = OpenFile( tempaudiopath, &ofstruct, OF_SHARE_EXCLUSIVE | OF_WRITE  | OF_CREATE );  
		if (fhandle != HFILE_ERROR) {
			fileverified = 1;
			CloseHandle( (HANDLE) fhandle );
			DeleteFile(tempaudiopath);
		}	 
		else {
			srand( (unsigned)time( NULL ) );
			int randnum = rand();
			char numstr[50];
			sprintf(numstr,"%d",randnum);

			CString cnumstr(numstr);
			CString fxstr("\\~temp");
			CString exstr(".wav");
			tempaudiopath = GetTempPath () + fxstr + cnumstr + exstr;


		}
 
	} 	
	
}


//Using this function affects the silenceFileValid variable
//which may be set in a variety of places

//The silenceFileValid, when TRUE, imply the tempsilencepath is pointing to a compatible silence sound file
BOOL CreateSilenceFile()
{
	silenceFileValid = FALSE;

	BuildRecordingFormat();

	ClearAudioSilenceFile();			
	
	//Create temporary wav file for audio recording
	GetSilenceWavePath();	
	m_pSilenceFile = new CSoundFile(tempsilencepath, &m_Format);	
	
	
	if  (!(m_pSilenceFile && m_pSilenceFile->IsOK())) {
		
		if ((runmode==0) || (runmode==1))
			MessageOut(NULL,IDS_ECSSF, IDS_NOTE, MB_OK | MB_ICONEXCLAMATION);
			//MessageBox(NULL,"Error Creating Silence Sound File","Note",MB_OK | MB_ICONEXCLAMATION);		
		silenceFileValid = FALSE;
		return silenceFileValid;

	}
	
	CBuffer buf(BasicBufSize, TRUE);
	buf.Erase();
	WriteSilenceFile(&buf) ;

	ClearAudioSilenceFile();

	silenceFileValid = TRUE;

	return silenceFileValid;
}



void ClearAudioSilenceFile()	
{
	if(m_pSilenceFile) {
	
		//will close output file
		delete m_pSilenceFile;
		m_pSilenceFile = NULL;
	}

}


void GetSilenceWavePath() {

	CString fileName("\\~tsil001.wav");
	tempsilencepath = GetTempPath () + fileName;

	//Test the validity of writing to the file
	int fileverified = 0;
	while (!fileverified) 
	{
		OFSTRUCT ofstruct;	
		HFILE fhandle = OpenFile( tempsilencepath, &ofstruct, OF_SHARE_EXCLUSIVE | OF_WRITE  | OF_CREATE );  
		if (fhandle != HFILE_ERROR) {
			fileverified = 1;
			CloseHandle( (HANDLE) fhandle );
			DeleteFile(tempsilencepath);
		}	 
		else {
			
			srand( (unsigned)time( NULL ) );
			int randnum = rand();
			char numstr[50];
			sprintf(numstr,"%d",randnum);

			CString cnumstr(numstr);
			CString fxstr("\\~tsil");
			CString exstr(".wav");
			tempsilencepath = GetTempPath () + fxstr + cnumstr + exstr;
			
			
		}
 
	} 	
	
}




//Delete the m_pFile variable and close existing audio file
void ClearAudioFile()	
{
	if(m_pFile) {
	
		//will close output file
		delete m_pFile;
		m_pFile = NULL;
	}

}

void SetBufferSize(int NumberOfSamples)
{
	m_BufferSize = NumberOfSamples;
}

BOOL StartAudioRecording(WAVEFORMATEX* format)
{
		MMRESULT mmReturn = 0;	
	
		if (format != NULL)
			m_Format = *format;

		// open wavein device 
		mmReturn = ::waveInOpen( &m_hRecord, AudioDeviceID, &m_Format,(DWORD) viewWnd, NULL, CALLBACK_WINDOW  ); //use on message to map.....
						
		
		if(mmReturn)
		{
			waveInErrorMsg(mmReturn, "Error in StartAudioRecording()");
			return FALSE;
		}
		else
		{
			// make several input buffers and add them to the input queue
			for(int i=0; i<3; i++)
			{
				AddInputBufferToQueue();
			}
			
			// start recording
			mmReturn = ::waveInStart(m_hRecord);
			if(mmReturn )
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
	if(pHdr == NULL) return NULL;
	ZeroMemory(pHdr, sizeof(WAVEHDR));

	// new a buffer
	CBuffer buf(m_Format.nBlockAlign*m_BufferSize, false);
	pHdr->lpData = buf.ptr.c;
	pHdr->dwBufferLength = buf.ByteLen;
	
	// prepare it
	mmReturn = ::waveInPrepareHeader(m_hRecord,pHdr, sizeof(WAVEHDR));
	if(mmReturn)
	{
		waveInErrorMsg(mmReturn, "in AddInputBufferToQueue()");
		return m_QueuedBuffers;
	}

	// add the input buffer to the queue
	mmReturn = ::waveInAddBuffer(m_hRecord, pHdr, sizeof(WAVEHDR));
	if(mmReturn)
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
	if(mmReturn)
	{		
		waveInErrorMsg(mmReturn, "in Stop()");
		return;
	}	
	else
	{				

		//Sleep(100);

		mmReturn = ::waveInStop(m_hRecord);
		if(mmReturn) waveInErrorMsg(mmReturn, "Error in StopAudioRecording() (WaveinStop)");

		//while (m_QueuedBuffers>0) 
		//	Sleep(50);	
	
		mmReturn = ::waveInClose(m_hRecord);
		if(mmReturn) waveInErrorMsg(mmReturn, "Error in StopAudioRecording() (WaveinClose)");
						

	}	
	
	//if(m_QueuedBuffers != 0) ErrorMsg("Still %d buffers in waveIn queue!", m_QueuedBuffers);
	if(m_QueuedBuffers != 0) 
	{
	
		if ((runmode==0) || (runmode==1))
			MessageOut(NULL,IDS_ABSIQ, IDS_NOTE, MB_OK | MB_ICONEXCLAMATION);
			//MessageBox(NULL,"Audio buffers still in queue!","note", MB_OK);

	}
	
	
}


LRESULT CPlayplusView::OnMM_WIM_DATA(WPARAM parm1, LPARAM parm2)
{
		
	MMRESULT mmReturn = 0;
	
	LPWAVEHDR pHdr = (LPWAVEHDR) parm2;

	

	mmReturn = ::waveInUnprepareHeader(m_hRecord, pHdr, sizeof(WAVEHDR));
	if(mmReturn)
	{
		waveInErrorMsg(mmReturn, "in OnWIM_DATA() Unprepare Header");
		return 0;
	}

	TRACE("WIM_DATA %4d\n", pHdr->dwBytesRecorded);
	
	if (gfRecording) 	
	{	
		CBuffer buf(pHdr->lpData, pHdr->dwBufferLength);
	
		if (!recordpaused) { //write only if not paused
		
			//Write Data to file
			DataFromSoundIn(&buf);

		}

		// reuse the buffer:
		// prepare it again
		mmReturn = ::waveInPrepareHeader(m_hRecord,pHdr, sizeof(WAVEHDR));
		if(mmReturn)
		{
			waveInErrorMsg(mmReturn, "in OnWIM_DATA() Prepare Header");
		}
		else // no error
		{
			// add the input buffer to the queue again
			mmReturn = ::waveInAddBuffer(m_hRecord, pHdr, sizeof(WAVEHDR));
			if(mmReturn) waveInErrorMsg(mmReturn, "in OnWIM_DATA() Add Buffer");
			else return 0;  // no error
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


void WriteSilenceFile(CBuffer* buffer) 
{		

	
	if(m_pSilenceFile)
	{

		if(!m_pSilenceFile->Write(buffer))
		{
			
			ClearAudioSilenceFile();
			if ((runmode==0) || (runmode==1))
				MessageOut(NULL,IDS_EWSSF, IDS_NOTE, MB_OK | MB_ICONEXCLAMATION);
				//MessageBox(NULL,"Error Writing Silence Sound File","Note",MB_OK | MB_ICONEXCLAMATION);
			silenceFileValid = FALSE;
		}
		
			
	}
	 

}


void DataFromSoundIn(CBuffer* buffer) 
{		

	
	if(m_pFile)
	{

		if(!m_pFile->Write(buffer))
		{
			//m_SoundIn.Stop();
			StopAudioRecording();
			ClearAudioFile();
			if ((runmode==0) || (runmode==1))
				MessageOut(NULL,IDS_EWSF, IDS_NOTE, MB_OK | MB_ICONEXCLAMATION);
				//MessageBox(NULL,"Error Writing Sound File","Note",MB_OK | MB_ICONEXCLAMATION);
		}
		
			
	}
	 

}



void BuildRecordingFormat() {

	m_Format.wFormatTag	= WAVE_FORMAT_PCM;		
	m_Format.wBitsPerSample = audio_bits_per_sample;
	m_Format.nSamplesPerSec = audio_samples_per_seconds;
	m_Format.nChannels = audio_num_channels;
	m_Format.nBlockAlign = m_Format.nChannels * (m_Format.wBitsPerSample/8);
	m_Format.nAvgBytesPerSec = m_Format.nSamplesPerSec * m_Format.nBlockAlign;
	m_Format.cbSize = 0;

}


//Suggest Save/Compress Format to pwfx
void SuggestRecordingFormat() {

		
	WAVEINCAPS pwic;
	MMRESULT mmr = waveInGetDevCaps( AudioDeviceID ,  &pwic, sizeof(pwic) ); 

	//Ordered in preference of choice
	if ((pwic.dwFormats) & WAVE_FORMAT_2S16) { 

		audio_bits_per_sample = 16;
		audio_num_channels = 2;
		audio_samples_per_seconds = 22050;				
		waveinselected = WAVE_FORMAT_2S16;
		

	}
	else if ((pwic.dwFormats) & WAVE_FORMAT_2M08) { 

		audio_bits_per_sample = 8;
		audio_num_channels = 1;
		audio_samples_per_seconds = 22050;			
		waveinselected = WAVE_FORMAT_2M08;
	

	}	
	else if ((pwic.dwFormats) & WAVE_FORMAT_2S08)  {

		audio_bits_per_sample = 8;
		audio_num_channels = 2;
		audio_samples_per_seconds = 22050;					
		waveinselected = WAVE_FORMAT_2S08;

	}
	else if ((pwic.dwFormats) & WAVE_FORMAT_2M16) { 

		audio_bits_per_sample = 16;
		audio_num_channels = 1;
		audio_samples_per_seconds = 22050;						
		waveinselected = WAVE_FORMAT_2M16;
	
	}
	else if ((pwic.dwFormats) & WAVE_FORMAT_1M08) {

		audio_bits_per_sample = 8;
		audio_num_channels = 1;
		audio_samples_per_seconds = 11025;		
		waveinselected = WAVE_FORMAT_1M08;
		
	}
	else if ((pwic.dwFormats) & WAVE_FORMAT_1M16) {

		audio_bits_per_sample = 16;
		audio_num_channels = 1;
		audio_samples_per_seconds = 11025;	
		waveinselected = WAVE_FORMAT_1M16;

	}
	else if ((pwic.dwFormats) & WAVE_FORMAT_1S08) {

		audio_bits_per_sample = 8;
		audio_num_channels = 2;
		audio_samples_per_seconds = 11025;				
		waveinselected = WAVE_FORMAT_1S08;

	}
	else if ((pwic.dwFormats) & WAVE_FORMAT_1S16) {

		audio_bits_per_sample = 16;
		audio_num_channels = 2;
		audio_samples_per_seconds = 11025;			
		waveinselected = WAVE_FORMAT_1S16;

	}
	else if ((pwic.dwFormats) & WAVE_FORMAT_4M08) { 		

		audio_bits_per_sample = 8;
		audio_num_channels = 1;
		audio_samples_per_seconds = 44100;						
		waveinselected = WAVE_FORMAT_4M08;

	}
	else if ((pwic.dwFormats) & WAVE_FORMAT_4M16) {

		audio_bits_per_sample = 16;
		audio_num_channels = 1;
		audio_samples_per_seconds = 44100;				
		waveinselected = WAVE_FORMAT_4M16;
		
	}
	else if ((pwic.dwFormats) & WAVE_FORMAT_4S08) { 

		audio_bits_per_sample = 8;
		audio_num_channels = 2;
		audio_samples_per_seconds = 44100;		
		waveinselected = WAVE_FORMAT_4S08;
		
	}
	else if ((pwic.dwFormats) & WAVE_FORMAT_4S16) {

		audio_bits_per_sample = 16;
		audio_num_channels = 2;
		audio_samples_per_seconds = 44100;			
		waveinselected = WAVE_FORMAT_4S16;
	}
	else {

		//Arbitrarily choose one 
		audio_bits_per_sample = 8;
		audio_num_channels = 1;
		audio_samples_per_seconds = 22050;			
		waveinselected = WAVE_FORMAT_2M08;

	}

	//Build m_Format
	BuildRecordingFormat();
	
}


void SuggestCompressFormat() {

	bAudioCompression = TRUE;

	AllocCompressFormat();		
	MMRESULT mmr;

	  
	//1st try MPEGLAYER3	
	BuildRecordingFormat();		
	if ((m_Format.nSamplesPerSec == 22050) && (m_Format.nChannels==2) && (m_Format.wBitsPerSample <= 16)) {

		pwfx->wFormatTag = WAVE_FORMAT_MPEGLAYER3;
		mmr = acmFormatSuggest(NULL, &m_Format,  pwfx, cbwfx, ACM_FORMATSUGGESTF_WFORMATTAG);	

	}	
	
	
	if (mmr!=0) {
	

			//Use PCM in order to handle most cases
			BuildRecordingFormat();
			pwfx->wFormatTag = WAVE_FORMAT_PCM;
			MMRESULT mmr = acmFormatSuggest(NULL, &m_Format,  pwfx, cbwfx, ACM_FORMATSUGGESTF_WFORMATTAG);


			if (mmr!=0) {

				bAudioCompression = FALSE;

			}


	}

}


void AllocCompressFormat() {

	int initial_audiosetup=1;

	if (pwfx) {

		initial_audiosetup=0;
		//Do nothing....already allocated

	}
	else {

		MMRESULT mmresult = acmMetrics(NULL, ACM_METRIC_MAX_SIZE_FORMAT, &cbwfx);
		if (MMSYSERR_NOERROR != mmresult)
		{

			CString msgstr;
			msgstr.Format("Metrics failed mmresult=%u!", mmresult);
			if ((runmode==0) || (runmode==1))
			{
				CString title;
				title.LoadString(IDS_NOTE);
				::MessageBox(NULL,msgstr,title, MB_OK | MB_ICONEXCLAMATION);			

			}
			return ;
		}


		pwfx = (LPWAVEFORMATEX)GlobalAllocPtr(GHND, cbwfx);
		if (NULL == pwfx)
		{

			CString msgstr;
			msgstr.Format("GlobalAllocPtr(%lu) failed!", cbwfx);

			if ((runmode==0) || (runmode==1))
			{
				CString title;
				title.LoadString(IDS_NOTE);
				::MessageBox(NULL,msgstr,title, MB_OK | MB_ICONEXCLAMATION);			

			}
			//::MessageBox(NULL,msgstr,"Note", MB_OK | MB_ICONEXCLAMATION);			
			return ;
		}

		initial_audiosetup=1;

	}

}



void SetTimeIndicator(CString timestr)
{
	
	//restore
	//if (statusbarCtrl)
	//	statusbarCtrl->SetPaneText(1,timestr);
		
	//CWnd* mainWindow = AfxGetMainWnd( );
	//((CMainFrame*) mainWindow)->SetTimeIndicator(timestr);

}

void NukeAVIStream(int i)
{
    int j;

    //
    // Make sure it's a real stream number
    //
    if (i < 0 || i >=gcpavi)
	return;

    //
    // Free all the resources associated with this stream
    //
    AVIStreamRelease(gapavi[i]);
    if (galpAVIOptions[i]->lpFormat) {
	GlobalFreePtr(galpAVIOptions[i]->lpFormat);
    }
    if (gapgf[i]) {
	AVIStreamGetFrameClose(gapgf[i]);
	gapgf[i] = NULL;
    }
    if (ghdd[i]) {
	DrawDibClose(ghdd[i]);
	ghdd[i] = 0;
    }

    //
    // Compact the arrays of junk
    //
    for (j = i; j < gcpavi - 1; j++) {
	gapavi[j] = gapavi[j+1];
	galpAVIOptions[j] = galpAVIOptions[j+1];
	gapgf[j] = gapgf[j+1];
	ghdd[j] = ghdd[j+1];
    }

    gcpavi--;
}


LRESULT CPlayplusView::OnMM_WOM_DONE(WPARAM parm1, LPARAM parm2)
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
    
	LoadString( ghInstApp, IDS_OPENWAVE, Buffer, BUFSIZE );	    
		    
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
	if (GetOpenFileName(&ofn)) {

		//AddAudioWaveFile(FileName);
		//TimeDependentInsert(FileName);

		if (giFirstAudio>=0)  {
				
			//this case has no need to padd silence (except at the end)
			//so the following code should be used with confidence
			
			//The silence file is still unable to work...unless it is converted...??
			CreateSilenceFile();

			long shiftTime = GetScrollTime();
			if (shiftTime<0) shiftTime = 0;
			if (shiftTime>timeEnd) shiftTime = timeEnd;		
			TimeDependentInsert(FileName,shiftTime);

		}
		else {

			//Unable to measure startsample		
			TimeDependentInsert(FileName,-1);

		}

		//Unable to insert silence for wave files becuase no slience file has been created
		//TimeDependentInsert(FileName,-1);

		fileModified = 1;
		
	}
	
}


void RemoveExistingAudioTracks() {

	AVISTREAMINFO     avis;
	 int i;
	for (i = gcpavi - 1; i >= 0; i--) {
		
		AVIStreamInfo(gapavi[i], &avis, sizeof(avis));
		if (avis.fccType==streamtypeAUDIO) {
			
		    NukeAVIStream(i);
		}
	}	
	giFirstAudio = -1;

	RecomputeStreamsTime(RESET_TO_START,0);

}


void AddAudioWaveFile(char  *FileName)
{
	//if merging (ie audio exists).. then we do not use initavi..but overwrite existing...

	InitAvi(FileName, MENU_MERGE);

}

void CPlayplusView::OnUpdateAudioRemoveexistingaudiotracks(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	BOOL enablebutton = (!gfPlaying) && (giFirstAudio>=0) && (giFirstVideo>=0) && (!gfRecording);
	pCmdUI->Enable(enablebutton);
	
}

void CPlayplusView::OnUpdateAudioAddaudiofromwavefile(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	BOOL enablebutton;
	enablebutton = (!gfPlaying) && (giFirstVideo>=0) && (!gfRecording);

	pCmdUI->Enable(enablebutton);
	
}

void CPlayplusView::OnFileSaveas() 
{
	
	OPENFILENAME openfilename;
	gszSaveFileName[0] = 0;


	LoadString( ghInstApp, IDS_SAVETITLE, gszBuffer, BUFSIZE );	
	openfilename.lStructSize = sizeof(OPENFILENAME);	
	openfilename.lpstrTitle = gszBuffer;
	openfilename.hwndOwner = viewWnd;
	openfilename.hInstance = NULL;

		
	//AVIBuildFilter(gszFilter, sizeof(gszFilter), TRUE);
	//openfilename.lpstrFilter = gszFilter;
	openfilename.lpstrFilter = "AVI Movie Files (*.avi)\0*.avi\0\0";	
	openfilename.lpstrCustomFilter = NULL;
	openfilename.nMaxCustFilter = 0;
	openfilename.nFilterIndex = 0;
	openfilename.lpstrFile = gszSaveFileName;
	openfilename.nMaxFile = sizeof(gszSaveFileName);
	openfilename.lpstrFileTitle = NULL;
	openfilename.nMaxFileTitle = 0;
	openfilename.lpstrInitialDir = NULL;		
	openfilename.Flags = OFN_PATHMUSTEXIST | OFN_HIDEREADONLY |
		OFN_OVERWRITEPROMPT ;
	openfilename.nFileOffset = 0;
	openfilename.nFileExtension = 0;
	
	LoadString( ghInstApp, IDS_DEFEXT, gszExtBuffer, BUFSIZE );	
	openfilename.lpstrDefExt = gszExtBuffer;
	openfilename.lCustData = 0;
	openfilename.lpfnHook = NULL;
	openfilename.lpTemplateName = NULL;


	// If we get a filename, save it
	if (GetSaveFileName(&openfilename))
	{
		DWORD       fccHandler[MAXNUMSTREAMS];
		int         i;
		HRESULT     hr;
	
		//StartWait();
	
		for (i = 0; i < gcpavi; i++)
		fccHandler[i] = galpAVIOptions[i]->fccHandler;

		SetAdditionalCompressSettings(bAudioCompression, pwfx, cbwfx,interleaveFrames,interleaveFactor, interleaveUnit);
	
		hr = AVISaveV(gszSaveFileName,
			  NULL,
			  (AVISAVECALLBACK) SaveCallback,
			  gcpavi,
			  gapavi,
			  galpAVIOptions);


		if (hr != AVIERR_OK) {
			switch (hr) {
			case AVIERR_FILEOPEN:
				LoadString( ghInstApp, IDS_ERROVERWRITE, gszBuffer, BUFSIZE );
				ErrMsg(gszBuffer);
			break;
				default:
				LoadString( ghInstApp, IDS_SAVEERROR, gszBuffer, BUFSIZE );
				ErrMsg(gszBuffer);
			}
		}
		else
			fileModified = 0;

		
		// Now put the video compressors back that we stole
		for (i = 0; i < gcpavi; i++)
		galpAVIOptions[i]->fccHandler = fccHandler[i];
	
		//EndWait();
		FixWindowTitle();

		
	}

	
}

void CPlayplusView::OnAudioAudiooptions() 
{
		

	AudioFormat aod;
	aod.DoModal();

	
}



//proven: after edit paste, the start playing at non zero can easiy crash!
//this is due to the AVISTREAMREAD !!
#define AUDIO_BUFFER_SIZE       16384
void AuditAudio(PAVISTREAM pavi,long startsample) {


    LONG            lRead;
    LONG            lSamplesToPlay;
	long slSampleSize;
	long slEnd, slBegin, slCurrent;


	slBegin = AVIStreamStart(pavi);
	slEnd = AVIStreamEnd(pavi);
	

	if (startsample>=0)
		slBegin = startsample;

	slCurrent  = slBegin;


	void* buffer = malloc(AUDIO_BUFFER_SIZE); 
	long retlen = 0;


	AVISTREAMINFO       strhdr;
    AVIStreamInfo(pavi, &strhdr, sizeof(strhdr));
    slSampleSize = (LONG) strhdr.dwSampleSize;
    
	if (slSampleSize <= 0 || slSampleSize > AUDIO_BUFFER_SIZE) {

		ErrMsg("Not Pass");
		return;

	}
	
	lSamplesToPlay = slEnd - slCurrent;
	if (lSamplesToPlay > AUDIO_BUFFER_SIZE / slSampleSize )
	    lSamplesToPlay = AUDIO_BUFFER_SIZE / slSampleSize ;

	ErrMsg("slCurrent %ld, slEnd %ld, lSamplesToPlay %ld",slCurrent, slEnd,lSamplesToPlay);


	while (slCurrent<slEnd) {

		//ErrMsg("slCurrent %ld, lSamplesToPlay %ld, toplay %ld",slCurrent, lSamplesToPlay, slEnd - slCurrent);
		long retval = AVIStreamRead(pavi, slCurrent, lSamplesToPlay,
				  buffer,
				  AUDIO_BUFFER_SIZE,
				  &retlen,
				  &lRead);

		slCurrent += lRead;


	}

	if (buffer) free(buffer);

		
	ErrMsg("Ok");


}

void CPlayplusView::OnZoomTestaudio() 
{

	long sPos = GetScrollTime();
	long samplepos = SafeStreamTimeToSample( gapavi[giFirstAudio], sPos  );
	AuditAudio(gapavi[giFirstAudio], samplepos);

}


void Msg(const char fmt[], ...) {
  
  static int debug = GetPrivateProfileInt("debug", "log", 0, "huffyuv.ini");
  if (!debug) return;

  DWORD written;
  char buf[2000];
  va_list val;
  
  va_start(val, fmt);
  wvsprintf(buf, fmt, val);

  const COORD _80x50 = {80,50};
  static BOOL startup = (AllocConsole(), SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), _80x50));
  WriteConsole(GetStdHandle(STD_OUTPUT_HANDLE), buf, lstrlen(buf), &written, 0);
}

void MsgC(const char fmt[], ...) {

	
	
  DWORD written;
  char buf[2000];
  va_list val;
  
  va_start(val, fmt);
  wvsprintf(buf, fmt, val);

  const COORD _80x50 = {80,50};
  static BOOL startup = (AllocConsole(), SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), _80x50));
  WriteConsole(GetStdHandle(STD_OUTPUT_HANDLE), buf, lstrlen(buf), &written, 0);
  


}

void DumpFormat(WAVEFORMATEX*  pwfx,const char* str)
{

	Msg("\n Str %s: wFormatTag %d channels %d nSamplesPerSec %d abytesPerSec %d blockalign %d wBitsPerSample %d cbSize  %d",str,pwfx->wFormatTag,pwfx->nChannels,pwfx->nSamplesPerSec,pwfx->nAvgBytesPerSec,pwfx->nBlockAlign,pwfx->wBitsPerSample,pwfx->cbSize );
	

		

}
	
/////////////////////////////////////////////////////////////////////////////
// Some of the follwing functions are not used because they are unstable
/////////////////////////////////////////////////////////////////////////////

//////////////////////////////
	
//1)function fails (in streamcut) if stream length is smaller then stream start
//2)editpaste : if pasting at point much further than the end of the stream, the pasting point is set to the end instead rathr than the specified point that is beyond
	//this may cause the cutting to fails...

//The plLength returns the number of bytes *removed* from the original streams
//This value may be *different* from the number of bytes inserted into the stream

//This function allows the lStart to be larger than the existing stream length
//In this case, the stream will be padded with silence
int EditStreamReplace(PAVISTREAM pavi, LONG * plPos,  LONG * plLength, PAVISTREAM pstream, LONG lStart, LONG lLength)
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
	
	DumpFormat((WAVEFORMATEX *) galpAVIOptions[giFirstAudio]->lpFormat,"1stAud (Existing)");
	DumpFormat((WAVEFORMATEX *) &m_Format,"Working    Format");
	DumpFormat((WAVEFORMATEX *) lpFormat,"To  replace  with");
	DumpFormat((WAVEFORMATEX *) orgFormat,"pavi(same as 1st)");
	
    
	int value = 0;
	
	
	//Add Silence if start point is larger than existing stream length
	long startPos = *plPos;
	long silenceLengthPasted = *plLength;
	if (value = EditStreamPadSilence( pavi,  &startPos,  &silenceLengthPasted))
		return value;
	
	
	//ErrMsg("pavi %ld, *plPos %ld, *plLength %ld, pstream %ld, lStart %ld,lLength %ld",pavi, *plPos, *plLength, pstream, lStart,lLength);
	if (value = EditStreamPaste(pavi, plPos, plLength, pstream, lStart,lLength) !=  AVIERR_OK)
	{
			ErrMsg("Unable to add audio at insertion point (Stream Replace)");
	}
	//ErrMsg("pavi %ld, *plPos %ld, *plLength %ld, pstream %ld, lStart %ld,lLength %ld",pavi, *plPos, *plLength, pstream, lStart,lLength);

	
	PAVISTREAM tempStream;
	long cutStartPoint;
	cutStartPoint = *plPos + *plLength;

	
	long lx = AVIStreamLength(pavi);
	if (lx >= (cutStartPoint + *plLength)) {

		//if pavi is long enough to be cut
		//do nothing...

	}
	else {
	
		//if not, try to cut from startpoint to 
		 *plLength  = lx - cutStartPoint - 1;

	}

	//ErrMsg("toCut_length plength %ld cutStartPoint %ld AVIStreamLength %ld", (*plLength) ,cutStartPoint,lx);

	
	//restore
	
	if  (*plLength>0) {

		if (value = EditStreamCut(pavi, &cutStartPoint, plLength,  &tempStream ) !=  AVIERR_OK)
		{
					ErrMsg("Unable to remove audio at replace point (Stream Replace)");
		}
		AVIStreamRelease(tempStream);



	}
	

	return value;

}



//Will pad silence from end of stream to plPos if plPos>length(pavi)
//  before pad> |---org stream-----|                         plPos
//  after  pad> |---org stream-----|_____silence_padded_____|plPos
int EditStreamPadSilence(PAVISTREAM pavi, LONG * plPos,  LONG * plLength)
{

	HRESULT   hr;

	long lz = AVIStreamLength(pavi);
	long startPasteLocation = *plPos;

	//CString msgstr;
	//msgstr.Format("lz %ld startPasteLocation %ld silenceFileValid %d",lz, startPasteLocation, silenceFileValid);
	//MessageBox(NULL,msgstr,"Note",MB_OK | MB_ICONEXCLAMATION);

	
	if (startPasteLocation>=lz) {

		if  (silenceFileValid) {

			long SilencePasteLength = startPasteLocation - (lz-1);

			PAVISTREAM paviSilence;
			PAVIFILE    pfileSilence;
			
			hr = AVIFileOpen(&pfileSilence, tempsilencepath, 0, 0L);

			if (hr != 0)
			{
				ErrMsg("Unable to open silence file");
				return hr;
			}

			if (hr = AVIFileGetStream(pfileSilence, &paviSilence, streamtypeAUDIO , 0) != AVIERR_OK) 
			{
					ErrMsg("Unable to load silence stream");
					return hr; 
			}

			long SilenceStreamLength = AVIStreamLength(paviSilence);
			//long initialStreamEnd = AVIStreamEnd(pavi)+1;
			long initialStreamEnd = AVIStreamEnd(pavi);
			long totalPastedLength = 0;
			while (SilencePasteLength>0) {

				LONG PastedLength;
				LONG lengthToPaste;
				
				if (SilencePasteLength>=SilenceStreamLength)
					lengthToPaste=SilenceStreamLength;   //restricting to ceiling of SilenceStreamLength
				else
					lengthToPaste=SilencePasteLength;

				//CString msgstr;
				//msgstr.Format("initialStreamEnd %ld lengthToPaste %ld SilenceStreamLength %ld totalPastedLength %ld,SilencePasteLength %ld",initialStreamEnd, lengthToPaste, SilenceStreamLength,totalPastedLength,SilencePasteLength);
				//MessageBox(NULL,msgstr,"Note",MB_OK | MB_ICONEXCLAMATION);

				if (hr = EditStreamPaste(pavi, &initialStreamEnd, &PastedLength, paviSilence, 0,lengthToPaste) !=  AVIERR_OK)
				{
					ErrMsg("Unable to pad silence to existing stream at position %ld (Stream Replace)",initialStreamEnd);
					return hr;
				}

				totalPastedLength += PastedLength;
				lz = AVIStreamLength(pavi);
				SilencePasteLength = startPasteLocation - (lz-1);

			}


			AVIStreamRelease(paviSilence);
			AVIFileRelease(pfileSilence);		

			*plLength = totalPastedLength;
			*plPos = initialStreamEnd;

		}
		else {
			
			//no silence added
			ErrMsg("Invalid Silence File! No audio [silence] added");
			
			//return -1;

		}

	}

	return 0;

}


//Will insert silence at begining of stream to shift the original stream to start sounding at a later time
int EditStreamSilenceShift(PAVISTREAM pavi, LONG * plPos, LONG * plLength)
{

	HRESULT   hr;

	long lz = AVIStreamLength(pavi);
	long ShiftToLocation = *plPos;
	long ShiftFromLocation = AVIStreamStart(pavi);
		
	if (ShiftToLocation>ShiftFromLocation) {

		if  (silenceFileValid) {

			long SilencePasteLength = ShiftToLocation - ShiftFromLocation;

			PAVISTREAM paviSilence;
			PAVIFILE    pfileSilence;
			
			hr = AVIFileOpen(&pfileSilence, tempsilencepath, 0, 0L);

			if (hr != 0)
			{
				ErrMsg("Unable to open silence file (2) ");
				return hr;
			}

			if (hr = AVIFileGetStream(pfileSilence, &paviSilence, streamtypeAUDIO , 0) != AVIERR_OK) 
			{
					ErrMsg("Unable to load silence stream (2)");
					return hr; 
			}

			long SilenceStreamLength = AVIStreamLength(paviSilence);
			long StreamPastePoint = ShiftFromLocation;
			long totalPastedLength = 0;
			while (SilencePasteLength>0) {

				LONG PastedLength;
				LONG lengthToPaste;
				
				if (SilencePasteLength>=SilenceStreamLength)
					lengthToPaste=SilenceStreamLength;   //restricting to ceiling of SilenceStreamLength
				else
					lengthToPaste=SilencePasteLength;

				//CString msgstr;
				//msgstr.Format("initialStreamEnd %ld lengthToPaste %ld SilenceStreamLength %ld totalPastedLength %ld,SilencePasteLength %ld",initialStreamEnd, lengthToPaste, SilenceStreamLength,totalPastedLength,SilencePasteLength);
				//MessageBox(NULL,msgstr,"Note",MB_OK | MB_ICONEXCLAMATION);

				PastedLength = 0;
				if (hr = EditStreamPaste(pavi, &StreamPastePoint, &PastedLength, paviSilence, 0,lengthToPaste) !=  AVIERR_OK)
				{
					ErrMsg("Unable to pad silence to existing stream at position %ld (2)",StreamPastePoint);
					return hr;
				}

				if (PastedLength<=0) {
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
		else {
			
			//no silence added
			ErrMsg("Invalid Silence File! No audio [silence] added (2)");
			
			//return -1;

		}

	}

	return 0;

}

//Returns a sample that correspond to the lTime of the stream
//The lTime can be *larger* than  the length of the original pavi
long ExAVIStreamTimeToSample( PAVISTREAM pavi, LONG lTime  )
{

	AVISTREAMINFO avis;
	AVIStreamInfo(pavi, &avis, sizeof(avis));

	long sampleStreamStart;
	long sampleStreamEnd;
	long timeStreamStart;
	long timeStreamEnd;
	
	sampleStreamStart = AVIStreamStart( pavi );
	sampleStreamEnd = AVIStreamEnd( pavi );
	timeStreamStart  = AVIStreamStartTime( pavi );
	timeStreamEnd = AVIStreamEndTime( pavi );

	//double timefrac = ((double ) lTime-timeStreamStart)/((double) (timeStreamEnd -  timeStreamStart));
	//timefrac =timefrac * ((double) (sampleStreamEnd - sampleStreamStart));
	//long sampleAtTime =  ((long) timefrac) + sampleStreamStart;
	//ErrMsg("lTime %ld sampletime %d sampleStreamStart %ld sampleStreamEnd %ld timeStreamStart %ld timeStreamEnd %ld",lTime,sampletime, sampleStreamStart,sampleStreamEnd,timeStreamStart,timeStreamEnd);
	//return sampleAtTime;	

	
	long sampleByAVIS = (long) (((double) (avis.dwRate/avis.dwScale)) * ((double) (lTime-timeStreamStart)/1000.0)) + sampleStreamStart;
	//ErrMsg("lTime %ld sampleAttime %ld sampleByAVIS %ld",lTime, sampleAtTime, sampleByAVIS);
	return sampleByAVIS;	

}


long SafeStreamTimeToSample( PAVISTREAM pavi, LONG starttime  )
{



	long startsample = 0;
	if (starttime > AVIStreamLengthTime(pavi))
		startsample = ExAVIStreamTimeToSample( pavi,  starttime);
	else
		startsample = AVIStreamTimeToSample( pavi,  starttime);
	

	
	if (startsample <= -1) 
		startsample = 0;

	return startsample;
			
}

// The interleave factor is set * correctly * (i.e adjusted with respect to the interleaveUnit of MILLISECONDS or FRAMES for saving 
// on other occasations, it is assumed to be 1 (and is not used?)
void SetAdditionalCompressSettings(BOOL recompress_audio, LPWAVEFORMATEX audio_recompress_format, DWORD  audio_format_size, BOOL bInterleave, int interleave_factor, int interleave_unit)
{ 	   
	int i=0;
	int frames_per_second = -1;

	for (i = 0; i < gcpavi; i++) {

		//use the firstvideo to calculate the frames per seconds for use in interleave
		if (bInterleave) {

			if (i==giFirstVideo) {

				AVISTREAMINFO avis;
				AVIStreamInfo(gapavi[giFirstVideo], &avis, sizeof(avis));		

				if (avis.dwScale>0) 
					frames_per_second = avis.dwRate/avis.dwScale; 
		
			}
			else {
				//Do nothing	
			}
		}

	}


	
	for (i = 0; i < gcpavi; i++) {

		if (bInterleave) {
			galpAVIOptions[i]->dwFlags = galpAVIOptions[i]->dwFlags | AVICOMPRESSF_INTERLEAVE; 

			if (interleave_unit==FRAMES)
				galpAVIOptions[i]->dwInterleaveEvery = interleave_factor;
			else {


				//Interleave by milliseconds
				if (frames_per_second>0) {

					double interfloat = (((double) interleaveFactor) * ((double) frames_per_second))/1000.0;
					int interint = (int) interfloat;
					if (interint<=0)
						interint = 1;
					galpAVIOptions[i]->dwInterleaveEvery = interint;

				}
				else
					galpAVIOptions[i]->dwInterleaveEvery = interleave_factor;

			}

			
		}
		else {
			

			galpAVIOptions[i]->dwFlags = galpAVIOptions[i]->dwFlags & ~AVICOMPRESSF_INTERLEAVE; 
			galpAVIOptions[i]->dwInterleaveEvery = 1;
		}




		if (recompress_audio) {

			if (i==giFirstAudio) {
			
					if (galpAVIOptions[i]->lpFormat) {
						GlobalFreePtr(galpAVIOptions[i]->lpFormat);
    				}
					galpAVIOptions[i]->cbFormat = audio_format_size;
					galpAVIOptions[i]->lpFormat = GlobalAllocPtr(GHND, audio_format_size); 	
					memcpy( (void *) galpAVIOptions[i]->lpFormat,  (void *) audio_recompress_format, audio_format_size );

				}			
			}
			else {
				//Do nothing	
			}

	}


}



void CPlayplusView::OnUpdateFileSaveas(CCmdUI* pCmdUI) 
{
	
	pCmdUI->Enable(giFirstVideo>=0);
	
}





// * Only use this function to perform the conversion *
//Put inside insertAVIFILE for NEW_AUDIO_TRACK case
//The rest of the conversion helper functions (except TestConvert) should be protected from use in other functions/code

//Important, need to call RecomputeStreamsTime after this function
//RecomputeStreamsTime not called inside ReiInitStreams becuase thiss allow flexibility in setting the slider
void  TestConvertFirstAudio()
{
	
	//restore
	return;

	//if player mode..returns
	if (pmode == PLAYER) return;

	if (giFirstAudio>=0) {
	
		PAVISTREAM paviConverted = NULL;
		int retval = TestConvert(gapavi[giFirstAudio], &paviConverted, NULL);
		if (retval > 0) //Conversion is performed
		{
			
			CloneAudioStream_ReplaceStreamPool(giFirstAudio,paviConverted);
			
			//no need to free the streams as ReplaceStreamPool already does that
				
		}
		else if (retval == 0){
		
			//no conversion necessary 
			ErrMsg("No Conversion Performed");
			
			
		
		}
		else {

			//Error
			ErrMsg("Error in Conversion");
			//no need to free resources as gapavi[giFirstAudio] remains valid and paviConverted has not been used  		

		}
	
	}

}


//Note : Using test convert will affect the behavior of player
//use an option that can turn test convert off


//Creates an Editable copy of pavi and use it to replace the orignal stream i
void CloneAudioStream_ReplaceStreamPool(int i,PAVISTREAM pavi)
{
    
    if (i < 0 || i >=gcpavi)
	return;	 
   
    //Creates an editable clone of pavi and set it to 
    PAVISTREAM paviConverted;
   	if (CreateEditableStream(&paviConverted, pavi) != AVIERR_OK) {
   	
   		//error
   		ErrMsg("Unable to Create Editable Stream for Converted Audio");
   		
   		//will not affect original stream
   		
   		return;
	
	}
	else {	
				
	    // Free all the resources associated with old stream
	    AVIStreamRelease(gapavi[i]);
	    if (galpAVIOptions[i]->lpFormat) {
			GlobalFreePtr(galpAVIOptions[i]->lpFormat);
	    }
	    if (gapgf[i]) {
			AVIStreamGetFrameClose(gapgf[i]);
			gapgf[i] = NULL;
	    }
	    if (ghdd[i]) {
			DrawDibClose(ghdd[i]);
			ghdd[i] = 0;
	    }
	    
	    
	    gapavi[i]=paviConverted;		
	    galSelStart[i] = galSelLen[i] = -1;
		
	
		ReInitAudioStream(i); 	
	}  
    
}


//This function should be called inside initstreams before the line  
//timeLength = timeEnd - timeStart;
void ReInitAudioStream(int i)
{
	
	//if (giFirstAudio>=0)
	//	i=giFirstAudio;

	AVISTREAMINFO avis;
	AVIStreamInfo(gapavi[i], &avis, sizeof(avis));
	galpAVIOptions[i] = &gaAVIOptions[i];
	_fmemset(galpAVIOptions[i], 0, sizeof(AVICOMPRESSOPTIONS));	
	galpAVIOptions[i]->fccType = avis.fccType;

    //case streamtypeAUDIO:
	LONG        lTemp;
	galpAVIOptions[i]->dwFlags |= AVICOMPRESSF_VALID;
	galpAVIOptions[i]->dwInterleaveEvery = 1;
	AVIStreamReadFormat(gapavi[i], AVIStreamStart(gapavi[i]), NULL, &lTemp);
	galpAVIOptions[i]->cbFormat = lTemp;
	if (lTemp)  galpAVIOptions[i]->lpFormat = GlobalAllocPtr(GHND, lTemp);
	
	// Use current format as default format
	if (galpAVIOptions[i]->lpFormat)
	    AVIStreamReadFormat(gapavi[i], AVIStreamStart(gapavi[i]),galpAVIOptions[i]->lpFormat,&lTemp);


	// We're finding the earliest and latest start and end points for
	// our scrollbar.
	//timeStart = min(timeStart, AVIStreamStartTime(gapavi[i]));
	//timeEnd   = max(timeEnd, AVIStreamEndTime(gapavi[i]));
	
	ghdd[i] =   NULL;
	gapgf[i] = NULL;

	
	//RecomputeStreamsTime(RESET_TO_START,0);


}

void CPlayplusView::OnUpdateFilePlay(CCmdUI* pCmdUI) 
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

void CPlayplusView::OnUpdateAudioExtension(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(!allowRecordExtension);
	
}



/////////////////////////////////////
// more stable methods / functions //
/////////////////////////////////////

CString GetTempPathEx(CString fileName, CString fxstr, CString exstr) {

	
	CString tempPathEx = GetTempPath () + fileName;

	//Test the validity of writing to the file
	int fileverified = 0;
	while (!fileverified) 
	{
		OFSTRUCT ofstruct;	
		HFILE fhandle = OpenFile( tempPathEx, &ofstruct, OF_SHARE_EXCLUSIVE | OF_WRITE  | OF_CREATE );  
		if (fhandle != HFILE_ERROR) {
			fileverified = 1;
			CloseHandle( (HANDLE) fhandle );
			
			DeleteFile(tempPathEx);
			
		}	 
		else {
			srand( (unsigned)time( NULL ) );
			int randnum = rand();
			char numstr[50];
			sprintf(numstr,"%d",randnum);

			CString cnumstr(numstr);
			
			tempPathEx = GetTempPath () + fxstr + cnumstr + exstr;

			
		}	
		
 
	} 	

	return tempPathEx;
	
}


//Code for open link
BOOL CPlayplusView::Openlink (CString link)
{

  BOOL bSuccess = FALSE;

  //As a last resort try ShellExecuting the URL, may
  //even work on Navigator!
  if (!bSuccess)
    bSuccess = OpenUsingShellExecute (link);

  if (!bSuccess)
    bSuccess = OpenUsingRegisteredClass (link);
  return bSuccess;
}


BOOL CPlayplusView::OpenUsingShellExecute (CString link)
{
  LPCTSTR mode;
  mode = _T ("open");

  //HINSTANCE hRun = ShellExecute (GetParent ()->GetSafeHwnd (), mode, m_sActualLink, NULL, NULL, SW_SHOW);
  HINSTANCE hRun = ShellExecute (GetSafeHwnd (), mode, link, NULL, NULL, SW_SHOW);
  if ((int) hRun <= HINSTANCE_ERROR)
    {
      TRACE ("Failed to invoke URL using ShellExecute\n");
      return FALSE;
    }
  return TRUE;
}


BOOL CPlayplusView::
OpenUsingRegisteredClass (CString link)
{
  TCHAR key[MAX_PATH + MAX_PATH];
  HINSTANCE result;

  if (GetRegKey (HKEY_CLASSES_ROOT, _T (".htm"), key) == ERROR_SUCCESS)
    {
      LPCTSTR mode;
      mode = _T ("\\shell\\open\\command");
      _tcscat (key, mode);
      if (GetRegKey (HKEY_CLASSES_ROOT, key, key) == ERROR_SUCCESS)
        {
          LPTSTR pos;
          pos = _tcsstr (key, _T ("\"%1\""));
          if (pos == NULL)
            {              // No quotes found

              pos = strstr (key, _T ("%1"));   // Check for %1, without quotes

              if (pos == NULL)     // No parameter at all...

                pos = key + _tcslen (key) - 1;
              else
                *pos = _T ('\0');  // Remove the parameter

            }
          else
            *pos = _T ('\0');  // Remove the parameter

          _tcscat (pos, _T (" "));
          _tcscat (pos, link);
          result = (HINSTANCE) WinExec (key, SW_SHOW);
          if ((int) result <= HINSTANCE_ERROR)
            {
              CString str;
              switch ((int) result)
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
                  str = _T ("The DDE transaction could not\nbe completed because other DDE transactions\nwere being processed.");
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
                  str.Format (_T ("Unknown Error (%d) occurred."), (int) result);
                }
              str = _T ("Unable to open hyperlink:\n\n") + str;
			
			  AfxMessageBox (str, MB_ICONEXCLAMATION | MB_OK);
            }
          else
            return TRUE;
        }
    }
  return FALSE;
}

  

LONG CPlayplusView::GetRegKey (HKEY key, LPCTSTR subkey, LPTSTR retdata)
{
  HKEY hkey;
  LONG retval = RegOpenKeyEx (key, subkey, 0, KEY_QUERY_VALUE, &hkey);

  if (retval == ERROR_SUCCESS)
    {
      long datasize = MAX_PATH;
      TCHAR data[MAX_PATH];
      RegQueryValue (hkey, NULL, data, &datasize);
      _tcscpy (retdata, data);
      RegCloseKey (hkey);
    }

  return retval;
}


////////////////////////// 
//ver 2.2
//Flash Output

int CChangeRectSwf :: initialize(int _blockx, int _blocky , int _blocksizex, int _blocksizey,int _blockwidth, int _pixelwidth, int _pixelheight, int x, int y)
{

	blockx = _blockx;
	blocky = _blocky;
	blocksizex = _blocksizex;
	blocksizey = _blocksizey;
	blockwidth = _blockwidth;
	
	pixelwidth = _pixelwidth;
	pixelheight = _pixelheight;

	blockbounds.left  = blockx * blocksizex; 
	blockbounds.right = blockbounds.left + blocksizex-1;
	blockbounds.top  = blocky * blocksizey; 
	blockbounds.bottom  = blockbounds.top + blocksizey-1;		

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

CArray<CChangeRectSwf*, CChangeRectSwf*> changeArray;

int IsDifferent(LPBITMAPINFOHEADER alpbi, int BITMAP_X,int BITMAP_Y, int x, int y, int format);
void AddExpandBlock(int BITMAP_X,int BITMAP_Y,int x,int y,int format);
void AddNewBlock(int BITMAP_X,int BITMAP_Y,int x,int y,int format);
void ExpandBlock(CChangeRectSwf *itemRect, int BITMAP_X,int BITMAP_Y,int x,int y,int format);
LPBYTE MakeFullRect(LPBITMAPINFOHEADER alpbi,LPBYTE bitmap, int BITMAP_X, int BITMAP_Y, int format, int max);
double ComputePercentCovered(int BITMAP_X,int BITMAP_Y);

void CreatePlayButton(std::ostringstream &f, int controlsWidth, int controlsHeight,int FrameOffsetX,int FrameOffsetY,int BITMAP_X, int BITMAP_Y);
void CreatePauseButton(std::ostringstream &f, int controlsWidth, int controlsHeight,int FrameOffsetX,int FrameOffsetY,int BITMAP_X, int BITMAP_Y);
void CreateStopButton(std::ostringstream &f, int controlsWidth, int controlsHeight,int FrameOffsetX,int FrameOffsetY,int BITMAP_X, int BITMAP_Y);
void CreateBackgroundBar(std::ostringstream &f, int controlsWidth, int controlsHeight,int FrameOffsetX,int FrameOffsetY,int BITMAP_X, int BITMAP_Y);


//LPBYTE MakeBitmapRect(LPBITMAPINFOHEADER alpbi, int BITMAP_X, int BITMAP_Y, int format, CChangeRectSwf * itemrect, int lenght, int height, int wLineLen, int dwSize);


void CPlayplusView::PerformFlash(int &ww, int &hh, LONG& currentTime) 
{
		
		//Indirect lpbi
		AVISTREAMINFO   avis;						
		AVIStreamInfo(gapavi[giFirstVideo], &avis, sizeof(avis));		
		int bmWidth = avis.rcFrame.right - avis.rcFrame.left  + 1;
		int bmHeight = avis.rcFrame.bottom - avis.rcFrame.top + 1;
		
		HDC hdc, screenDC;
		screenDC = ::GetDC(NULL);
		hdc = CreateCompatibleDC(screenDC);
		
		HBITMAP hbm = (HBITMAP) ::CreateCompatibleBitmap(screenDC,bmWidth, bmHeight);    	
		HBITMAP old_bitmap = (HBITMAP) ::SelectObject(hdc,hbm);		
		
		initTemporalCompress(bmWidth,bmHeight);	
		
				
		//CreateSwfHeader()
		CString modname, modbasename, modbasenextname;
		modname = swfname;
		modbasename = swfbasename;

		if ((needbreakapart) &&  (breakcycle>0))
		{

			//swfname 
			CString numstr;
			numstr.Format("_%d",breakcycle);
			CString xpath=swfname;
			xpath=xpath.Left(xpath.ReverseFind('.'));
			modname = xpath + numstr + ".swf";					
			
			xpath=swfbasename;			
			xpath=xpath.Left(xpath.ReverseFind('.'));
			modbasename = xpath + numstr + ".swf";	

		}

			
		//need to next name even if breakcycle == 0
		if (needbreakapart)
		{
			CString xpath=swfbasename;	
			xpath=xpath.Left(xpath.ReverseFind('.'));
			
			CString numnextstr;
			numnextstr.Format("_%d",breakcycle+1);
			
			modbasenextname = xpath + numnextstr + ".swf";	

		}

		std::ofstream fileout(LPCTSTR(modname),std::ios::binary);
		std::ostringstream f(std::ios::binary);	

		

		f << FlashTagBackgroundColor(swfbk_red,swfbk_green,swfbk_blue); //white

		
	
		if ((giFirstAudio>=0) && (useAudio))
		{
			AVISTREAMINFO       strhdr;		
			LONG cbFormat;
			LPVOID lpFormat;
			

			AVIStreamInfo(gapavi[giFirstAudio], &strhdr, sizeof(strhdr));
			AVIStreamFormatSize(gapavi[giFirstAudio], AVIStreamStart(gapavi[giFirstAudio]), &cbFormat);


			lpFormat = GlobalAllocPtr(GHND, cbFormat);
			if (lpFormat)
			{

				
			
				AVIStreamReadFormat(gapavi[giFirstAudio], AVIStreamStart(gapavi[giFirstAudio]), lpFormat, &cbFormat);
				
				int SamplesPerSec = ((LPWAVEFORMATEX) lpFormat)->nSamplesPerSec;
				int BytesPerSec = ((LPWAVEFORMATEX) lpFormat)->nAvgBytesPerSec;
				int BitsPerSample = ((LPWAVEFORMATEX) lpFormat)->wBitsPerSample;
				int nChannels = ((LPWAVEFORMATEX) lpFormat)->nChannels;
				DWORD wFormatTag = ((LPWAVEFORMATEX) lpFormat)->wFormatTag;

				GlobalFreePtr(lpFormat);				
				
				if (SamplesPerSec < 6000)
					play_rate = stream_rate = 0;		
				else if (SamplesPerSec==11025)
					play_rate = stream_rate = 1;
				else if (SamplesPerSec==22050)
					play_rate = stream_rate = 2;
				else if (SamplesPerSec==44100)
					play_rate = stream_rate = 3;

				if (BitsPerSample == 16)
					play_16bit = stream_16bit = true;
				else
					play_16bit = stream_16bit = false;


				if (nChannels==2)
					play_stereo = stream_stereo = true;
				else
					play_stereo = stream_stereo = false;			

				if (wFormatTag == WAVE_FORMAT_PCM) 				
				{
					if (useAudioCompression == 0)
						compression = 0; 
					else if (useAudioCompression == 1)
					{
						compression = 1; 

					}

				}
				else {												

					// Convert Audio to PCM ,  16 bit sample					
					WAVEFORMATEX WaveFormat;
					WaveFormat.wFormatTag	= WAVE_FORMAT_PCM;		
					WaveFormat.wBitsPerSample = swf_bits_per_sample;
					WaveFormat.nSamplesPerSec = swf_samples_per_seconds;
					WaveFormat.nChannels = swf_num_channels;
					WaveFormat.nBlockAlign = WaveFormat.nChannels * (WaveFormat.wBitsPerSample/8);
					WaveFormat.nAvgBytesPerSec = WaveFormat.nSamplesPerSec * WaveFormat.nBlockAlign;
					WaveFormat.cbSize = 0;

					
					//if single file ... will convert
					//if multi-part file...will convert only on the first part
					if ((!needbreakapart) || (breakcycle ==0)) 
					{
						BeginWaitCursor();
						PCMConvertedStream = ConvertFirstAudioStream(&WaveFormat);
						EndWaitCursor();

					}
					
					
					if (!PCMConvertedStream)
					{
						noAudioStream = 1;
						compression = 0;
					}
					else
					{
						usePCMConvertedStream = 1;

						//Reformat swf sound stream parameters to the converted stream
						if (swf_samples_per_seconds < 6000)
							play_rate = stream_rate = 0;		
						else if (swf_samples_per_seconds==11025)
							play_rate = stream_rate = 1;
						else if (swf_samples_per_seconds==22050)
							play_rate = stream_rate = 2;
						else if (swf_samples_per_seconds==44100)
							play_rate = stream_rate = 3;

						if (swf_bits_per_sample == 16)
							play_16bit = stream_16bit = true;
						else
							play_16bit = stream_16bit = false;

						if (swf_num_channels==2)
							play_stereo = stream_stereo = true;
						else
							play_stereo = stream_stereo = false;			

						if (useAudioCompression == 0)
							compression = 0; 
						else if (useAudioCompression == 1)
							compression = 1; 

						SamplesPerSec = swf_samples_per_seconds;
						BitsPerSample = swf_bits_per_sample;
						nChannels = swf_num_channels;

					}

				
				}
				

				samplecountavg = SamplesPerSec / sampleFPS;

				//need this ?
				//added to ensure each sound stream block is less than 4096 samples
				if ((!noAudioStream) && (useAudioCompression<2))
				{
					if (samplecountavg > 4096)
					{
						//adjust sampleFPS
						sampleFPS = 12; 
						samplecountavg = SamplesPerSec / sampleFPS;
						sampletimeInc = 1000/ sampleFPS;
						

					}
				}	

				long startsampleSwf = 0;	
				if ((needbreakapart) && (breakcycle>0))
				{

					//can we just do nothing ...lets use the old value of startsampleSwf

					/*
					if (usePCMConvertedStream)  
						startsampleSwf = AVIStreamTimeToSample( PCMConvertedStream,  currentTime);	
					else
						startsampleSwf = AVIStreamTimeToSample( gapavi[giFirstAudio],  currentTime);	

					slCurrentSwf = startsampleSwf;
					*/


				}
				else
				{
					if (usePCMConvertedStream)  
						startsampleSwf = AVIStreamTimeToSample( PCMConvertedStream,  AVIStreamStartTime(PCMConvertedStream));	
					else
						startsampleSwf = AVIStreamTimeToSample( gapavi[giFirstAudio],  AVIStreamStartTime(gapavi[giFirstAudio]));	
					
					slCurrentSwf = startsampleSwf;
				}
				
				
				


				if (!noAudioStream)
				{

					if ((useAudioCompression == 1) && (!stream_16bit))
					{
						//need to force 16 bit because internally, when compressing thru' ADPCM,
						//there is a conversion to 16 bit !						

						FlashTagSoundStreamHead2 ssh( play_rate,  play_16bit, play_stereo,  compression,	 stream_rate, true, stream_stereo, samplecountavg);	
						f << ssh;

					}
					else
					{
						FlashTagSoundStreamHead2 ssh( play_rate,  play_16bit, play_stereo,  compression,	 stream_rate, stream_16bit, stream_stereo, samplecountavg);
						f << ssh;
					}

				}

			}
			else  //if !lpFormat
				noAudioStream = 1;

		}
		else //if (giFirstAudio<0)
			noAudioStream = 1;


	
		
		if (addControls)
		{
			controlsWidth = bmWidth;			

			if (controlsType == 1)
			{			
				bitmapBarHeight = 19;
				CreateBackgroundBar(f, controlsWidth, bitmapBarHeight,FrameOffsetX,FrameOffsetY,bmWidth,bmHeight);					
				int offset = CreateFlashBitmapPlayButton(f,bmWidth,bmHeight,"\\playbar",FrameOffsetX);
				offset = CreateFlashBitmapPauseButton(f,bmWidth,bmHeight,"\\playbar",offset);
				offset = CreateFlashBitmapStopButton(f,bmWidth,bmHeight,"\\playbar",offset);

			}
			else if (controlsType == 2)
			{			
				
				bitmapBarHeight = 25;
				int offset;				
				
				if (ControllerAlignment == 0)
					offset = 0;
				else if (ControllerAlignment == 1)
					offset = (bmWidth - ControllerWidth)/2 + FrameOffsetX; //centering .. 296 is the hard code lenght of the bar...
				if (yes_drawLeftPiece)
					offset = DrawLeftPiece(f,bmWidth, bmHeight, "\\controller", offset, PieceOffsetY);				
				offset = CreateFlashBitmapPlayButton(f,bmWidth,bmHeight,"\\controller",offset);
				offset = CreateFlashBitmapPauseButton(f,bmWidth,bmHeight,"\\controller",offset);				
				if (yes_drawStopButton)
					offset = CreateFlashBitmapStopButton(f,bmWidth,bmHeight,"\\controller",offset);
				if (yes_drawRightPiece)
					offset = DrawRightPiece(f,bmWidth, bmHeight, "\\controller", offset, PieceOffsetY);
					

			}
			else
			{	
				bitmapBarHeight = 25;
				CreateBackgroundBar(f, controlsWidth, controlsHeight,FrameOffsetX,FrameOffsetY,bmWidth,bmHeight);	
				CreatePlayButton(f, controlsWidth, controlsHeight,FrameOffsetX,FrameOffsetY,bmWidth,bmHeight);
				CreatePauseButton(f, controlsWidth, controlsHeight,FrameOffsetX,FrameOffsetY,bmWidth,bmHeight);
				CreateStopButton(f, controlsWidth, controlsHeight,FrameOffsetX,FrameOffsetY,bmWidth,bmHeight);								

			}

		}		

		controlsHeight = bitmapBarHeight;


		//add after add controls
		if ((breakcycle==0) && (addPreloader))
		{
			controlsWidth = bmWidth;			
			
			int additonalOffsetY = 0;
			if (!addControls)
				additonalOffsetY = 3;

			int widthBar = CreateProgressBar(f, controlsWidth, bitmapBarHeight,FrameOffsetX,FrameOffsetY,bmWidth,bmHeight,(int) (0.1*bmWidth)+FrameOffsetX,additonalOffsetY);
			DrawLoading(f,  bmWidth, bmHeight,  loadingPath, 0,0);	
			DrawNodes(f,widthBar,bmWidth, bmHeight,  "\\controller", 0,0, int(0.1*bmWidth)+FrameOffsetX,additonalOffsetY);
			Preloader(f, widthBar, bmWidth, bmHeight, int(0.1*bmWidth)+FrameOffsetX);
			//Preloader(f, widthBar, bmWidth, bmHeight, 12);

		}
		else if ((breakcycle>0) && (addPreloader) && (applyPreloaderToSplitFiles))
		{
			controlsWidth = bmWidth;			
			
			int additonalOffsetY = 0;
			if (!addControls)
				additonalOffsetY = 3;

			int widthBar = CreateProgressBar(f, controlsWidth, bitmapBarHeight,FrameOffsetX,FrameOffsetY,bmWidth,bmHeight,(int) (0.1*bmWidth)+FrameOffsetX,additonalOffsetY);
			DrawLoading(f,  bmWidth, bmHeight,  loadingPath, 0,0);	
			DrawNodes(f,widthBar,bmWidth, bmHeight,  "\\controller", 0,0, int(0.1*bmWidth)+FrameOffsetX,additonalOffsetY);
			Preloader(f, widthBar, bmWidth, bmHeight, int(0.1*bmWidth)+FrameOffsetX);

		}


		
		//Progress Dialog
		CProgressDlg* progressdlogptr= new CProgressDlg;
		progressdlogptr->Create(this);
		progressdlogptr->SetStep(1);
		
		CString msgx,msgout;
		msgx.LoadString(IDS_GENERATING);
		//msgout.Format(msgx,swfbasename);		
		msgout.Format(msgx,modbasename);		
		((CStatic *) progressdlogptr->GetDlgItem(IDC_CONVERSIONTEXT))->SetWindowText(msgout);			
				
		//Main Conversion Block
		int gfConverting = TRUE;		
			
		while (gfConverting)
		{

				CString msgStr;

				//Progress Dialog
				int percentcompeleted;
				if  ((timeEnd>1) && (currentTime <= timeEnd))
					percentcompeleted	= (int) ((float) currentTime * 100.0 / (float) timeEnd);
				else
					percentcompeleted	= 0;

				if ((percentcompeleted > oldPercent) || (percentcompeleted == 0))
				{
					
					progressdlogptr->SetPos(percentcompeleted);
					oldPercent = percentcompeleted;

				}				

				if (currentTime < timeStart)
					currentTime = timeStart;
				
				if (currentTime > timeEnd) 	{			
						moreSWFsneeded = 0;
						gfConverting = FALSE;							
				}											
				
				ConvertToFlash(currentTime, hdc, hbm, f);				

				//ConvertToFlash(currentTime, NULL);
				
				currentTime += sampletimeInc;	

				//the click sound that sometime occurs at the end of swf playback is due to the button-click sound record with "Record from Speakers" option
				//if (framecount > 200)
				//	break;

				if (framecount > MAXFLASHLIMIT)
				{			

						if (!needbreakapart) //this case is to cater to the case when user do not want to split even if the limit is reached
							moreSWFsneeded = 0;
						else  //normal case....split
							moreSWFsneeded = 1;
						gfConverting = FALSE;	
						
						
				}
				
												  
		}
		
		//Progress Dialog		
		if (progressdlogptr) delete progressdlogptr;
		oldPercent = 0;


		if ((moreSWFsneeded) && (allowChaining))
		{			
				char actionScript[1000];
				CString actionscriptFormat("loadMovie (\"%s\", \"\");");
				CString actionscriptStr;
				actionscriptStr.Format(actionscriptFormat,modbasenextname);				
				strcpy(actionScript,LPCTSTR(actionscriptStr));
				actionScript[actionscriptStr.GetLength()]=0; 

				//MessageBox(actionscriptStr,"Note",MB_OK);

				ActionCompiler acom(5);
				acom.Compile(actionScript,f);

				f << FlashTagShowFrame();
				framecount++;

				filesAreSplit = 1;
				


		}
		else if ((moreSWFsneeded) && (!allowChaining))
		{

			//we dont do anything...just let the use edit them



		}
		else { //this is either final part of a split file or the only part of a single file 

			
			//if no loop
			if (noLoop)
			{
				

				//if split file
				if ((filesAreSplit) && (allowChaining)) //no more swfsneeded ... but already split
				{	
					
					//We jsut want to stop playing 
					

					FlashActionStop s;
					FlashTagDoAction ftd;
					ftd.AddAction(&s);	 
					f << ftd;


					/*
					char actionScript[1000];
					CString actionscriptFormat("loadMovie (\"%s\", \"\");");
					CString actionscriptStr;
					actionscriptStr.Format(actionscriptFormat,swfbasename);				
					strcpy(actionScript,LPCTSTR(actionscriptStr));
					actionScript[actionscriptStr.GetLength()]=0; 

					ActionCompiler acom(5);
					acom.Compile(actionScript,f);
					*/


					f << FlashTagShowFrame();
					framecount++;

					
				}
				else
				{

					if (noAutoPlay == 1)
					{

					}
					else
					{
				
						FlashActionStop s;
						FlashTagDoAction ftd;
						ftd.AddAction(&s);	
 
						f << ftd;
						f << FlashTagShowFrame();
						framecount++;
					
					}

				}

			}
			else  //if looping
			{

				//if split file
				if ((filesAreSplit) && (allowChaining)) //no more swfsneeded ... but already split
				{

					char actionScript[1000];
					CString actionscriptFormat("loadMovie (\"%s\", \"\");");
					CString actionscriptStr;
					actionscriptStr.Format(actionscriptFormat,swfbasename);				
					strcpy(actionScript,LPCTSTR(actionscriptStr));
					actionScript[actionscriptStr.GetLength()]=0; 

					//MessageBox(actionscriptStr,"Note",MB_OK);

					ActionCompiler acom(5);
					acom.Compile(actionScript,f);

					f << FlashTagShowFrame();
					framecount++;

					
				}				
				else if ((filesAreSplit) && (!allowChaining))
				{
					//If we want to loop and yet does not allow chianing of a split file
					//we just do nothing

					//..let user edit the file themselves

					
				}
				else
				{


					
					int baseframe = 0;
					if (addPreloader)
						baseframe = preloadFrames;


					FlashActionGotoFrame gotoAction(baseframe);
					FlashActionPlay px;
					FlashTagDoAction ftd;
					ftd.AddAction(&gotoAction);	
					ftd.AddAction(&px);	

					f << ftd;
					f << FlashTagShowFrame();
					framecount++;

				}
				

			}

		}

		//CreateSwfEnd()
		f << FlashTagEnd();	
		//fileout << FlashHeader(5,f.str().size(),(bmWidth+2)*20,(bmHeight+2)*20,15.8,1);
		
		//Currently, the framecount must be counted/incremented manually at each FlashTagShowFrame
		//The value is used when the swf is imported
		
		if (addControls)  //may include Preloader
		{
			//int ButtonSpaceY = 8;
			fileout << FlashHeader(5,f.str().size(),(bmWidth+(FrameOffsetX*2))*20,(bmHeight+(FrameOffsetY*2)+controlsHeight+ButtonSpaceY+1)*20,(float) sampleFPS,framecount);		
			ww = (bmWidth+(FrameOffsetX*2));
			hh = (bmHeight+(FrameOffsetY*2)+controlsHeight+ButtonSpaceY+1);
		}
		else if (addPreloader)
		{
			//int ButtonSpaceY = 8;
			fileout << FlashHeader(5,f.str().size(),(bmWidth+(FrameOffsetX*2))*20,(bmHeight+(FrameOffsetY*2)+controlsHeight+ButtonSpaceY+1)*20,(float) sampleFPS,framecount);		
			ww = (bmWidth+(FrameOffsetX*2));
			hh = (bmHeight+(FrameOffsetY*2)+ButtonSpaceY+1);
		}
		else
		{
			fileout << FlashHeader(5,f.str().size(),(bmWidth+(FrameOffsetX*2))*20,(bmHeight+(FrameOffsetY*2))*20,(float) sampleFPS,framecount);
			ww = (bmWidth+(FrameOffsetX*2));
			hh = (bmHeight+(FrameOffsetY*2));

		}
		fileout << f.str();

		//MsgC("yes");

		
		//Indirect lpbi
		::SelectObject(hdc, old_bitmap);        
		DeleteObject(hbm);
		DeleteDC(hdc);
		::ReleaseDC(NULL, hdc);

		
		finishTemporalCompress();
}


//CFlash
void CPlayplusView::OnFileConverttoswf() 
{

		LoadController();

		if (launchPropPrompt)
		{
			if (flashProp.DoModal()==IDOK)
			{

			}
			else
				return;
		}

		int ww =0 ;
		int hh =0 ;
		needbreakapart = 0;
		breakcycle = 0;	
		filesAreSplit = 0;


		firstvideoFrame = 1;
		usePCMConvertedStream = 0;
		PCMConvertedStream = NULL;
		PCMConvertedFile = NULL;

		if (sampleFPS>0)
			sampletimeInc = 1000/ sampleFPS;
		else return;		


		int numframesTotal;
		if (sampletimeInc>=1)
		{
			numframesTotal = timeEnd/sampletimeInc;
			if (numframesTotal>=MAXFLASHLIMIT)
			{
				
				int ret = IDYES;
				//MessageBox("Your Movie Exceeds the 16000 Frame Limit. Only 16000 Frames will be encoded into the SWF file.","Note",MB_OK | MB_ICONEXCLAMATION);				
				if ((runmode==0) || (runmode==1))
					ret = MessageOut(NULL,IDS_MAXFRAME,IDS_NOTE,MB_YESNO | MB_ICONEXCLAMATION);
				if (ret == IDYES)
					needbreakapart = 1;
				else
					needbreakapart = 0;


			}
			//MsgC("numframesTotal %d",numframesTotal);

		}		

		LONG currentTime  = 0;
		moreSWFsneeded = 1;

		if (!needbreakapart)
			PerformFlash(ww,hh,currentTime);
		else
		{
			while (moreSWFsneeded)
			{
				PerformFlash(ww,hh,currentTime);
				breakcycle++;

			}


		}
		///
	
		produceFlashHTML(LPCTSTR(swfhtmlname),LPCTSTR(swfbasename),LPCTSTR(swfname), ww , hh ,swfbk_red, swfbk_green, swfbk_blue);		


		BeginWaitCursor();
		if (usePCMConvertedStream)
		{
					
			cleanTempFile();

		}

		if (launchHTMLPlayer)
		{
			Openlink(swfhtmlname);
		}
		EndWaitCursor();

		if (runmode == 1) //allow this only in CamStudio internal mode
		{
			if (deleteAVIAfterUse)
			{				
				if (strlen(playfiledir) > 0)
				{
				
					OnFileClose();
					DeleteFile(playfiledir);

				}

				//MessageBox(playfiledir,"Delete",MB_OK);

			}


		}


		if (runmode>0)
			::PostMessage(AfxGetMainWnd()->m_hWnd,WM_COMMAND,ID_APP_EXIT,0);
	
}



void ConvertToFlash(long currentTime,HDC hdc, HBITMAP hbm,std::ostringstream &f)
{

	if (gcpavi<=0) 
		return;
	
	//PaintStuff(dc.m_hDC, viewWnd, FALSE);
	{
		long lTime = currentTime;
		long lSamp = 0;
		RECT rcFrame;
		LPBITMAPINFOHEADER lpbi = NULL;
    
		// for all streams
		for (int i=0; i<gcpavi; i++) {
		
				AVISTREAMINFO   avis;		
				LONG             lEndTime;	
				AVIStreamInfo(gapavi[i], &avis, sizeof(avis));
		
				if (avis.fccType == streamtypeVIDEO) {
				
					if (gapgf[i] == NULL)
					continue;								
															
					if (i == giFirstVideo) {						

						//
						// Which frame belongs at this time?
						//
						lEndTime = AVIStreamEndTime(gapavi[i]);
						if (lTime <= lEndTime)
							lSamp = AVIStreamTimeToSample(gapavi[i], lTime);
						else {      // we've scrolled past the end of this stream						
							lSamp = AVIStreamTimeToSample(gapavi[i], AVIStreamStartTime(gapavi[i])+timeLength-1);	
						}										 
				
						if (gapgf[i] && lSamp >= AVIStreamStart(gapavi[i]))
							lpbi = (LPBITMAPINFOHEADER) AVIStreamGetFrame(gapgf[i], lSamp);
						else
							lpbi = NULL;

					
						// Figure out where to draw this frame		
						rcFrame.left    =0;
						rcFrame.top    = 0 ;
						rcFrame.right  = rcFrame.left +
								 (avis.rcFrame.right - avis.rcFrame.left);
						rcFrame.bottom = rcFrame.top +
								 (avis.rcFrame.bottom - avis.rcFrame.top);

						//Patch to prevent blank screen
						if (lpbi == NULL)
						{
							lpbi = (LPBITMAPINFOHEADER) AVIStreamGetFrame(gapgf[i], AVIStreamEnd(gapavi[i])-1);

						}
					
						//HDC hdc = GetDC(NULL);
						//PaintVideo(hdc, rcFrame, i, lpbi, lSamp, 0);	
						//ReleaseDC(NULL,hdc);
						PaintSwfFrame(hdc, hbm, rcFrame,  lpbi,  i,f);

					}				

		
				} //If Video Stream

				
			} //for all streams		


	}

	return;

}



void CPlayplusView::OnUpdateFileConverttoswf(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	BOOL enablebutton = (!gfPlaying) && (!gfRecording) && (giFirstVideo>=0);
	pCmdUI->Enable(enablebutton);
	
}



void CreateBackgroundBar(std::ostringstream &f, int controlsWidth, int controlsHeight,int FrameOffsetX,int FrameOffsetY,int BITMAP_X, int BITMAP_Y)
{
		int buttonRadius = controlsHeight;
		int barDepth = ObjectDepth + 1;
		int ButtonSpaceY = 5;
		int lineThickness = 3;		
		
		//FlashRGB colorBar1(0xc0,0xc0,0xff);
		//FlashRGB colorBar1(0x00,0x00,0xff);
		FlashRGB colorBar1(swfbar_red,swfbar_green,swfbar_blue);		
		FlashRGB colorBar2(0xcff,0xcff,0xff);
		
		FlashMatrix m;		
		FlashGradientRecord gr;
		gr.AddGradient(0, colorBar1);
		gr.AddGradient(128, colorBar2);
		gr.AddGradient(255, colorBar1);
		
		//ffa.AddFillStyle(&ffc2);

		//FlashLineStyleArray fla;
		//FlashLineStyle ls(lineThickness*20,colorPlay);
		//FlashLineStyle ls2(1*20,colorPlay);
		//fla.AddLineStyle(&ls);			
		//fla.AddLineStyle(&ls2);			
		
		FlashShapeWithStyle s;	
		int left = FrameOffsetX;
		int top = (BITMAP_Y+FrameOffsetY+ButtonSpaceY/2);
		int widthBar = controlsWidth;
		int heightBar = (controlsHeight + (ButtonSpaceY*2));
		
		//m.SetTranslate(left,top);
		//m.SetScale((double) widthBar/32768.0 ,(double) heightBar/32768.0);		
		FlashFillStyleGradient ffg(m, gr);
		FlashFillStyleArray ffa;	
		ffa.AddFillStyle(&ffg);			

				
		FlashShapeRecordChange changerec,changerec2 ;
		changerec.ChangeFillStyle1(1);					
		//changerec.ChangeLineStyle(1);	
		s.AddRecord(changerec);
		s.AddRecord(FlashShapeRecordChange(left*20,top*20));
		s.AddRecord(FlashShapeRecordStraight(widthBar*20,0*20));
		s.AddRecord(FlashShapeRecordStraight(0*20,heightBar*20));
		s.AddRecord(FlashShapeRecordStraight(-widthBar*20,0*20));
		s.AddRecord(FlashShapeRecordStraight(0*20,-heightBar*20));	

		s.SetFillStyleArray(ffa);	
		//s.SetLineStyleArray(fla);	

		FlashTagDefineShape3 fsws(s);							
		f << fsws;


		FlashTagPlaceObject2 po(barDepth, fsws.GetID()); 
		f << po;	

}


void CreatePlayButton(std::ostringstream &f, int controlsWidth, int controlsHeight,int FrameOffsetX,int FrameOffsetY,int BITMAP_X, int BITMAP_Y)
{
		int buttonRadius = controlsHeight;
		int playButtonDepth = ObjectDepth + 2;
		int ButtonSpaceY = 5;
		int lineThickness = 3;		
		FlashRGB white(0xff,0xff,0xff);
		FlashRGB colorPlay(0x0,0x0,0xff);
		FlashRGB colorPlayLight(0xe0,0xe0,0xff);

		int shapeUP_ID, shapeDOWN_ID;

		{
			//Shape Up, Over
			FlashFillStyleSolid ffc(white);
			FlashFillStyleSolid ffc2(colorPlay);

			FlashFillStyleArray ffa;	
			ffa.AddFillStyle(&ffc);			
			ffa.AddFillStyle(&ffc2);

			FlashLineStyleArray fla;
			FlashLineStyle ls(lineThickness*20,colorPlay);
			FlashLineStyle ls2(1*20,colorPlay);
			fla.AddLineStyle(&ls);			
			fla.AddLineStyle(&ls2);			
			
			FlashShapeWithStyle s;	
			int left = (FrameOffsetX+(lineThickness+1)/2);
			int top = (BITMAP_Y+FrameOffsetY+ButtonSpaceY);
					
			FlashShapeRecordChange changerec,changerec2 ;
			changerec.ChangeFillStyle1(1);					
			changerec.ChangeLineStyle(1);	
			s.AddRecord(changerec);
			s.AddRecord(FlashShapeRecordChange(left*20,top*20));
			s.AddRecord(FlashShapeRecordStraight(buttonRadius*20,0*20));
			s.AddRecord(FlashShapeRecordStraight(0*20,buttonRadius*20));
			s.AddRecord(FlashShapeRecordStraight(-buttonRadius*20,0*20));
			s.AddRecord(FlashShapeRecordStraight(0*20,-buttonRadius*20));					
			
			changerec2.ChangeFillStyle1(2);					
			changerec2.ChangeLineStyle(2);	
			s.AddRecord(changerec2);
			s.AddRecord(FlashShapeRecordChange(left*20+buttonRadius*20/4+20,top*20+buttonRadius*20/4+20));
			s.AddRecord(FlashShapeRecordStraight(0*20,buttonRadius*20*2/4));
			s.AddRecord(FlashShapeRecordStraight(buttonRadius*20*2/4,-buttonRadius*20*1/4));
			s.AddRecord(FlashShapeRecordStraight(-buttonRadius*20*2/4,-buttonRadius*20*1/4));

			s.SetFillStyleArray(ffa);	
			s.SetLineStyleArray(fla);	

			FlashTagDefineShape3 fsws(s);							
			f << fsws;

			shapeUP_ID = fsws.GetID();

		}


		{
			
			//Shape Down
			FlashFillStyleSolid ffc(colorPlayLight);
			FlashFillStyleSolid ffc2(colorPlay);

			FlashFillStyleArray ffa;	
			ffa.AddFillStyle(&ffc);			
			ffa.AddFillStyle(&ffc2);

			FlashLineStyleArray fla;
			FlashLineStyle ls(lineThickness*20,colorPlay);
			FlashLineStyle ls2(1*20,colorPlay);
			fla.AddLineStyle(&ls);			
			fla.AddLineStyle(&ls2);			


			FlashShapeWithStyle s;	
			int left = (FrameOffsetX+(lineThickness+1)/2);
			int top = (BITMAP_Y+FrameOffsetY+ButtonSpaceY);
					
			FlashShapeRecordChange changerec,changerec2 ;
			changerec.ChangeFillStyle1(1);					
			changerec.ChangeLineStyle(1);	
			s.AddRecord(changerec);
			s.AddRecord(FlashShapeRecordChange(left*20,top*20));
			s.AddRecord(FlashShapeRecordStraight(buttonRadius*20,0*20));
			s.AddRecord(FlashShapeRecordStraight(0*20,buttonRadius*20));
			s.AddRecord(FlashShapeRecordStraight(-buttonRadius*20,0*20));
			s.AddRecord(FlashShapeRecordStraight(0*20,-buttonRadius*20));					
			
			changerec2.ChangeFillStyle1(2);					
			changerec2.ChangeLineStyle(2);	
			s.AddRecord(changerec2);
			s.AddRecord(FlashShapeRecordChange(left*20+buttonRadius*20/4+20,top*20+buttonRadius*20/4+20));
			s.AddRecord(FlashShapeRecordStraight(0*20,buttonRadius*20*2/4));
			s.AddRecord(FlashShapeRecordStraight(buttonRadius*20*2/4,-buttonRadius*20*1/4));
			s.AddRecord(FlashShapeRecordStraight(-buttonRadius*20*2/4,-buttonRadius*20*1/4));

			s.SetFillStyleArray(ffa);	
			s.SetLineStyleArray(fla);	

			FlashTagDefineShape3 fsws(s);							
			f << fsws;

			shapeDOWN_ID = fsws.GetID();

		}

		//FlashButtonRecord(UWORD _charID, UWORD _depth, char _validstates, FlashMatrix &_matrix,	FlashColorTransform &_cfx);
		//FButtonRecord2 *bRec = new FButtonRecord2(false, false, false, true, buttonUpID, 1, mx, cxf);

		FlashMatrix m;
		//m.SetScale(20,20);
		
		FlashColorTransform cfxUp , cfxOver , cfxDown ;	
		cfxUp.SetMultRGB(FlashRGB(256,256,256,128));	
		cfxOver.SetMultRGB(FlashRGB(256,256,256,256));		
		cfxDown.SetMultRGB(FlashRGB(256,256,256,256));		
		FlashButtonRecord brecUp(shapeUP_ID , playButtonDepth, FBR_UP, m, cfxUp);
		FlashButtonRecord brecOver(shapeUP_ID, playButtonDepth, FBR_OVER  | FBR_HIT_TEST, m, cfxOver);	
		FlashButtonRecord brecDown(shapeDOWN_ID, playButtonDepth, FBR_DOWN , m, cfxDown);
				
		FlashTagDefineButton2 buttonPlay;
		FlashActionPlay playAction;
		buttonPlay.AddButtonRecord(&brecUp);
		buttonPlay.AddButtonRecord(&brecOver);
		buttonPlay.AddButtonRecord(&brecDown);
		
		N_STD::vector<FlashActionRecord *> acrs;
		acrs.push_back(&playAction);		
		  
		buttonPlay.AddActionRecords(acrs,SWFSOURCE_BST_OverUpToOverDown);	
		
		//FlashTagDefineButton buttonPlay;		
		//buttonPlay.AddButtonRecord(&brecUp);
		//buttonPlay.AddButtonRecord(&brecOver);		

		f << buttonPlay;			

		FlashTagPlaceObject2 po(playButtonDepth, buttonPlay.GetID()); 
		f << po;	

}

void CreatePauseButton(std::ostringstream &f, int controlsWidth, int controlsHeight,int FrameOffsetX,int FrameOffsetY,int BITMAP_X, int BITMAP_Y)
{
		int buttonRadius = controlsHeight;
		int pauseButtonDepth = ObjectDepth + 2;
		int ButtonSpaceY = 5;
		int lineThickness = 3;		
		FlashRGB white(0xff,0xff,0xff);
		FlashRGB colorPause(0x80,0x80,0x0);
		FlashRGB colorPauseLight(0xff,0xff,0x80);

		int shapeUP_ID, shapeDOWN_ID;

		{
			FlashFillStyleSolid ffc(white);
			FlashFillStyleSolid ffc2(colorPause);

			FlashFillStyleArray ffa;	
			ffa.AddFillStyle(&ffc);			
			ffa.AddFillStyle(&ffc2);

			FlashLineStyleArray fla;
			FlashLineStyle ls(lineThickness*20,colorPause);
			FlashLineStyle ls2(1*20,colorPause);
			fla.AddLineStyle(&ls);			
			fla.AddLineStyle(&ls2);			

			FlashShapeWithStyle s;	
			int left = (FrameOffsetX+(lineThickness+1)/2) + buttonRadius + ButtonSpaceY;
			int top = (BITMAP_Y+FrameOffsetY+ButtonSpaceY);
					
			FlashShapeRecordChange changerec,changerec2 ;
			changerec.ChangeFillStyle1(1);					
			changerec.ChangeLineStyle(1);	
			s.AddRecord(changerec);
			s.AddRecord(FlashShapeRecordChange(left*20,top*20));
			s.AddRecord(FlashShapeRecordStraight(buttonRadius*20,0*20));
			s.AddRecord(FlashShapeRecordStraight(0*20,buttonRadius*20));
			s.AddRecord(FlashShapeRecordStraight(-buttonRadius*20,0*20));
			s.AddRecord(FlashShapeRecordStraight(0*20,-buttonRadius*20));					
			
			changerec2.ChangeFillStyle1(2);					
			changerec2.ChangeLineStyle(2);	
			s.AddRecord(changerec2);
			s.AddRecord(FlashShapeRecordChange(left*20+buttonRadius*20/5+buttonRadius/4,top*20+buttonRadius*20/4+20));
			s.AddRecord(FlashShapeRecordStraight(0*20,buttonRadius*20*2/4));
			s.AddRecord(FlashShapeRecordStraight(buttonRadius*20*1/5,0*20));
			s.AddRecord(FlashShapeRecordStraight(0*20,-buttonRadius*20*2/4));
			s.AddRecord(FlashShapeRecordStraight(-buttonRadius*20*1/5,0*20));
			
			s.AddRecord(FlashShapeRecordChange(left*20+buttonRadius*20*3/5-buttonRadius/4,top*20+buttonRadius*20/4+20));
			s.AddRecord(FlashShapeRecordStraight(0*20,buttonRadius*20*2/4));
			s.AddRecord(FlashShapeRecordStraight(buttonRadius*20*1/5,0*20));
			s.AddRecord(FlashShapeRecordStraight(0*20,-buttonRadius*20*2/4));
			s.AddRecord(FlashShapeRecordStraight(-buttonRadius*20*1/5,0*20));				

			s.SetFillStyleArray(ffa);	
			s.SetLineStyleArray(fla);	

			FlashTagDefineShape3 fsws(s);							
			f << fsws;

			shapeUP_ID = fsws.GetID();

		}



		{
			FlashFillStyleSolid ffc(colorPauseLight);
			FlashFillStyleSolid ffc2(colorPause);

			FlashFillStyleArray ffa;	
			ffa.AddFillStyle(&ffc);			
			ffa.AddFillStyle(&ffc2);

			FlashLineStyleArray fla;
			FlashLineStyle ls(lineThickness*20,colorPause);
			FlashLineStyle ls2(1*20,colorPause);
			fla.AddLineStyle(&ls);			
			fla.AddLineStyle(&ls2);			

			FlashShapeWithStyle s;	
			int left = (FrameOffsetX+(lineThickness+1)/2) + buttonRadius + ButtonSpaceY;
			int top = (BITMAP_Y+FrameOffsetY+ButtonSpaceY);
					
			FlashShapeRecordChange changerec,changerec2 ;
			changerec.ChangeFillStyle1(1);					
			changerec.ChangeLineStyle(1);	
			s.AddRecord(changerec);
			s.AddRecord(FlashShapeRecordChange(left*20,top*20));
			s.AddRecord(FlashShapeRecordStraight(buttonRadius*20,0*20));
			s.AddRecord(FlashShapeRecordStraight(0*20,buttonRadius*20));
			s.AddRecord(FlashShapeRecordStraight(-buttonRadius*20,0*20));
			s.AddRecord(FlashShapeRecordStraight(0*20,-buttonRadius*20));					
			
			changerec2.ChangeFillStyle1(2);					
			changerec2.ChangeLineStyle(2);	
			s.AddRecord(changerec2);
			s.AddRecord(FlashShapeRecordChange(left*20+buttonRadius*20/5+buttonRadius/4,top*20+buttonRadius*20/4+20));
			s.AddRecord(FlashShapeRecordStraight(0*20,buttonRadius*20*2/4));
			s.AddRecord(FlashShapeRecordStraight(buttonRadius*20*1/5,0*20));
			s.AddRecord(FlashShapeRecordStraight(0*20,-buttonRadius*20*2/4));
			s.AddRecord(FlashShapeRecordStraight(-buttonRadius*20*1/5,0*20));
			
			s.AddRecord(FlashShapeRecordChange(left*20+buttonRadius*20*3/5-buttonRadius/4,top*20+buttonRadius*20/4+20));
			s.AddRecord(FlashShapeRecordStraight(0*20,buttonRadius*20*2/4));
			s.AddRecord(FlashShapeRecordStraight(buttonRadius*20*1/5,0*20));
			s.AddRecord(FlashShapeRecordStraight(0*20,-buttonRadius*20*2/4));
			s.AddRecord(FlashShapeRecordStraight(-buttonRadius*20*1/5,0*20));				

			s.SetFillStyleArray(ffa);	
			s.SetLineStyleArray(fla);	

			FlashTagDefineShape3 fsws(s);							
			f << fsws;

			shapeDOWN_ID = fsws.GetID();

		}

		FlashMatrix m;		
		
		FlashColorTransform cfxUp , cfxOver , cfxDown ;	
		cfxUp.SetMultRGB(FlashRGB(256,256,256,128));	
		cfxOver.SetMultRGB(FlashRGB(256,256,256,256));		
		cfxDown.SetMultRGB(FlashRGB(256,256,256,256));		
		FlashButtonRecord brecUp(shapeUP_ID , pauseButtonDepth, FBR_UP, m, cfxUp);
		FlashButtonRecord brecOver(shapeUP_ID, pauseButtonDepth, FBR_OVER  | FBR_HIT_TEST, m, cfxOver);	
		FlashButtonRecord brecDown(shapeDOWN_ID, pauseButtonDepth, FBR_DOWN , m, cfxDown);
				
		FlashTagDefineButton2 buttonPause;
		FlashActionStop stopAction;
		buttonPause.AddButtonRecord(&brecUp);
		buttonPause.AddButtonRecord(&brecOver);
		buttonPause.AddButtonRecord(&brecDown);
		
		N_STD::vector<FlashActionRecord *> acrs;
		acrs.push_back(&stopAction);		
		  
		buttonPause.AddActionRecords(acrs,SWFSOURCE_BST_OverUpToOverDown);					
		f << buttonPause;			

		FlashTagPlaceObject2 po(pauseButtonDepth, buttonPause.GetID()); 
		f << po;	


}

void CreateStopButton(std::ostringstream &f, int controlsWidth, int controlsHeight,int FrameOffsetX,int FrameOffsetY, int BITMAP_X, int BITMAP_Y)
{
		int buttonRadius = controlsHeight;
		int stopButtonDepth = ObjectDepth + 2;
		int ButtonSpaceY = 5;
		int lineThickness = 3;		
		FlashRGB white(0xff,0xff,0xff);
		FlashRGB colorStop(0xff,0x0,0x0);
		FlashRGB colorStopLight(0xff,0xe0,0xe0);

		int shapeUP_ID, shapeDOWN_ID;

		{

			FlashFillStyleSolid ffc(white);
			FlashFillStyleSolid ffc2(colorStop);

			FlashFillStyleArray ffa;	
			ffa.AddFillStyle(&ffc);			
			ffa.AddFillStyle(&ffc2);

			FlashLineStyleArray fla;
			FlashLineStyle ls(lineThickness*20,colorStop);
			FlashLineStyle ls2(1*20,colorStop);
			fla.AddLineStyle(&ls);			
			fla.AddLineStyle(&ls2);			

			FlashShapeWithStyle s;	
			int left = (FrameOffsetX+(lineThickness+1)/2) + (buttonRadius + ButtonSpaceY) * 2;
			int top = (BITMAP_Y+FrameOffsetY+ButtonSpaceY);
					
			FlashShapeRecordChange changerec,changerec2 ;
			changerec.ChangeFillStyle1(1);					
			changerec.ChangeLineStyle(1);	
			s.AddRecord(changerec);
			s.AddRecord(FlashShapeRecordChange(left*20,top*20));
			s.AddRecord(FlashShapeRecordStraight(buttonRadius*20,0*20));
			s.AddRecord(FlashShapeRecordStraight(0*20,buttonRadius*20));
			s.AddRecord(FlashShapeRecordStraight(-buttonRadius*20,0*20));
			s.AddRecord(FlashShapeRecordStraight(0*20,-buttonRadius*20));					
			
			changerec2.ChangeFillStyle1(2);					
			changerec2.ChangeLineStyle(2);	
			s.AddRecord(changerec2);
			s.AddRecord(FlashShapeRecordChange(left*20+buttonRadius*20/4+20,top*20+buttonRadius*20/4+20));
			s.AddRecord(FlashShapeRecordStraight(0*20,buttonRadius*20*2/4));
			s.AddRecord(FlashShapeRecordStraight(buttonRadius*20*2/4,0*20));
			s.AddRecord(FlashShapeRecordStraight(0*20,-buttonRadius*20*2/4));
			s.AddRecord(FlashShapeRecordStraight(-buttonRadius*20*2/4,0*20));				

			s.SetFillStyleArray(ffa);	
			s.SetLineStyleArray(fla);	

			FlashTagDefineShape3 fsws(s);							
			f << fsws;

			shapeUP_ID = fsws.GetID();


		}


		{

			FlashFillStyleSolid ffc(colorStopLight);
			FlashFillStyleSolid ffc2(colorStop);

			FlashFillStyleArray ffa;	
			ffa.AddFillStyle(&ffc);			
			ffa.AddFillStyle(&ffc2);

			FlashLineStyleArray fla;
			FlashLineStyle ls(lineThickness*20,colorStop);
			FlashLineStyle ls2(1*20,colorStop);
			fla.AddLineStyle(&ls);			
			fla.AddLineStyle(&ls2);			

			FlashShapeWithStyle s;	
			int left = (FrameOffsetX+(lineThickness+1)/2) + (buttonRadius + ButtonSpaceY) * 2;
			int top = (BITMAP_Y+FrameOffsetY+ButtonSpaceY);
					
			FlashShapeRecordChange changerec,changerec2 ;
			changerec.ChangeFillStyle1(1);					
			changerec.ChangeLineStyle(1);	
			s.AddRecord(changerec);
			s.AddRecord(FlashShapeRecordChange(left*20,top*20));
			s.AddRecord(FlashShapeRecordStraight(buttonRadius*20,0*20));
			s.AddRecord(FlashShapeRecordStraight(0*20,buttonRadius*20));
			s.AddRecord(FlashShapeRecordStraight(-buttonRadius*20,0*20));
			s.AddRecord(FlashShapeRecordStraight(0*20,-buttonRadius*20));					
			
			changerec2.ChangeFillStyle1(2);					
			changerec2.ChangeLineStyle(2);	
			s.AddRecord(changerec2);
			s.AddRecord(FlashShapeRecordChange(left*20+buttonRadius*20/4+20,top*20+buttonRadius*20/4+20));
			s.AddRecord(FlashShapeRecordStraight(0*20,buttonRadius*20*2/4));
			s.AddRecord(FlashShapeRecordStraight(buttonRadius*20*2/4,0*20));
			s.AddRecord(FlashShapeRecordStraight(0*20,-buttonRadius*20*2/4));
			s.AddRecord(FlashShapeRecordStraight(-buttonRadius*20*2/4,0*20));				

			s.SetFillStyleArray(ffa);	
			s.SetLineStyleArray(fla);	

			FlashTagDefineShape3 fsws(s);							
			f << fsws;

			shapeDOWN_ID = fsws.GetID();

		}


		FlashMatrix m;		
		
		FlashColorTransform cfxUp , cfxOver , cfxDown ;	
		cfxUp.SetMultRGB(FlashRGB(256,256,256,128));	
		cfxOver.SetMultRGB(FlashRGB(256,256,256,256));		
		cfxDown.SetMultRGB(FlashRGB(256,256,256,256));		
		FlashButtonRecord brecUp(shapeUP_ID , stopButtonDepth, FBR_UP, m, cfxUp);
		FlashButtonRecord brecOver(shapeUP_ID, stopButtonDepth, FBR_OVER  | FBR_HIT_TEST, m, cfxOver);	
		FlashButtonRecord brecDown(shapeDOWN_ID, stopButtonDepth, FBR_DOWN , m, cfxDown);
				
		FlashTagDefineButton2 buttonStop;
		FlashActionStop stopAction;
		

		//handle baseframe
		int baseframe = 0;
		if (addPreloader) 
			baseframe = preloadFrames;
		
		FlashActionGotoFrame gotoAction(baseframe);
		buttonStop.AddButtonRecord(&brecUp);
		buttonStop.AddButtonRecord(&brecOver);
		buttonStop.AddButtonRecord(&brecDown);
		
		N_STD::vector<FlashActionRecord *> acrs;
		acrs.push_back(&stopAction);		
		acrs.push_back(&gotoAction);		
		  
		buttonStop.AddActionRecords(acrs,SWFSOURCE_BST_OverUpToOverDown);					
		f << buttonStop;			

		FlashTagPlaceObject2 po(stopButtonDepth, buttonStop.GetID()); 
		f << po;	


}




void PaintSwfFrame(HDC hdc, HBITMAP hbm, RECT rcFrame, LPBITMAPINFOHEADER lpbi, int iStream, std::ostringstream &f)
{
   
	//if lbpi is present, draw it
    if (lpbi)
    {

		int offsetx=0;
		int offsety=0;
		int bitmapwidth = rcFrame.right-rcFrame.left+1;
		int bitmapheight = rcFrame.bottom-rcFrame.top+1;	
		

		//Draw the Frame		
		DrawDibDraw(ghdd[iStream], hdc,
		    rcFrame.left+offsetx, rcFrame.top+offsety,
		    rcFrame.right - rcFrame.left-1,
		    rcFrame.bottom - rcFrame.top-1,
		    lpbi, NULL,
		    0, 0, -1, -1,
		    (iStream == giFirstVideo) ? 0 :DDF_BACKGROUNDPAL);

		//Get it Back in RGB
		int format = 5;
		int BITMAP_X = 0;
		int BITMAP_Y = 0;
		LPBYTE bitmap;
		LPBITMAPINFOHEADER alpbi = GetFrame(hbm, &bitmap, BITMAP_X, BITMAP_Y, convertBits);


		if (convertBits == 16)
			format = 4;
		else if (convertBits == 32)
			format = 5;
		
		if (alpbi)
		{			
			WriteSwfFrame(alpbi, f,  bitmap,  BITMAP_X, BITMAP_Y,  format);
			FreeFrame(alpbi);

		}
		
		
				
	
    }

	return;


}




LPBYTE makeReverse32(int width, int height,int bitPlanes, LPBITMAPINFOHEADER alpbi)
{

	LPBYTE bitsOrg = (LPBYTE) alpbi + // pointer to data
						alpbi->biSize +
						alpbi->biClrUsed * sizeof(RGBQUAD);

	UINT widthBytes = (width*32+31)/32 * 4;
	VertFlipBuf(bitsOrg, widthBytes, height);	
	ARGBFromBGRA(bitsOrg, width, height);
	
	return bitsOrg;


}


LPBYTE makeReverse16(int width, int height,int bitPlanes, LPBITMAPINFOHEADER alpbi)
{

	LPBYTE bitsOrg = (LPBYTE) alpbi + // pointer to data
						alpbi->biSize +
						alpbi->biClrUsed * sizeof(RGBQUAD);

	UINT widthBytes = (width*16+31)/32 * 4;
	VertFlipBuf(bitsOrg, widthBytes, height);	
	swapPixelBytes16( width,  height, bitPlanes,bitsOrg);
	
	return bitsOrg;


}


LPBYTE swapPixelBytes16(int width, int height,int bitPlanes,LPBYTE bits16)
{

	BYTE *tmp;
	tmp=bits16;

	long counter16 = 0;
	long rowcounter16 = 0;
	long widthBytes = (width*16+31)/32 * 4;
	for (int h = 0; h< height; h++)
	{
		counter16 = rowcounter16;
		for (int w = 0; w< width; w++) 
		{
			WORD  oldval;
			oldval = *((WORD *) (tmp+counter16));			
			*((WORD *) (tmp+counter16)) = (oldval << 8) | (oldval >> 8);
			counter16+=2;			

		}

		rowcounter16 += widthBytes;
	}

	return tmp;
}



BOOL VertFlipBuf(BYTE  * inbuf, 
					   UINT widthBytes, 
					   UINT height)
{   
	BYTE  *tb1;
	BYTE  *tb2;

	if (inbuf==NULL)
		return FALSE;

	UINT bufsize;

	bufsize=widthBytes;

	tb1= (BYTE *)new BYTE[bufsize];
	if (tb1==NULL) {
		return FALSE;
	}

	tb2= (BYTE *)new BYTE [bufsize];
	if (tb2==NULL) {
		delete [] tb1;
		return FALSE;
	}
	
	UINT row_cnt;     
	ULONG off1=0;
	ULONG off2=0;

	for (row_cnt=0;row_cnt<(height+1)/2;row_cnt++) {
		off1=row_cnt*bufsize;
		off2=((height-1)-row_cnt)*bufsize;   
		
		memcpy(tb1,inbuf+off1,bufsize);
		memcpy(tb2,inbuf+off2,bufsize);	
		memcpy(inbuf+off1,tb2,bufsize);
		memcpy(inbuf+off2,tb1,bufsize);
	}	

	delete [] tb1;
	delete [] tb2;

	return TRUE;
}        



BOOL ARGBFromBGRA(BYTE *buf, UINT widthPix, UINT height)
{
	if (buf==NULL)
		return FALSE;

	UINT col, row;
	for (row=0;row<height;row++) {
		for (col=0;col<widthPix;col++) {
			
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
			*pRed = 255;  //alpha works for higer level ...code
		}
	}
	return TRUE;
}




LPBITMAPINFOHEADER GetFrame(HBITMAP hbm, LPBYTE* bits,int& BITMAP_X,int& BITMAP_Y, int numbits)
{
	if ((numbits == 16) || (numbits == 32))
	{
		LPBITMAPINFOHEADER alpbi = (LPBITMAPINFOHEADER)GlobalLock(Bitmap2Dib(hbm, numbits));	
	
		if (alpbi)
		{
			BITMAP_X = alpbi->biWidth;
			BITMAP_Y = alpbi->biHeight;

			if (numbits == 16)
				*bits = makeReverse16(alpbi->biWidth, alpbi->biHeight,2, alpbi);
			else
				*bits = makeReverse32(alpbi->biWidth, alpbi->biHeight,4, alpbi);

		}	

		return alpbi;

	}

	return NULL;
		
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


void FreeFrame(LPBITMAPINFOHEADER alpbi)
{
 
	if (!alpbi)
		return ;
	
	GlobalFreePtr(alpbi);
	//GlobalFree(alpbi);
	alpbi = NULL;
}	

// Create a new bitmap of the same width and height of alpbi
// Fill it with zeros,
// Then fill up the changeArray areas with alpbi data
LPBYTE MakeFullRect(LPBITMAPINFOHEADER alpbi,LPBYTE bitmap, int BITMAP_X, int BITMAP_Y, int format, int max)
{		
	int imageLineLen, pixelbytes;
	CChangeRectSwf * itemRect = NULL;

	if (format==4)
	{
			imageLineLen = ((BITMAP_X)*16+31)/32 * 4;	
			pixelbytes = 2;
	}
	else if (format==5)
	{
			imageLineLen = ((BITMAP_X)*32+31)/32 * 4;	
			pixelbytes = 4;

	}
	else
		return NULL;

	int dwSize = imageLineLen * BITMAP_Y;
	LPBYTE bitNew = NULL;
	bitNew = (LPBYTE) malloc (dwSize);

	if (bitNew==NULL)
		return NULL;

	memset(bitNew,0,dwSize);

	for (int i=0;i<max; i++)
	{
		itemRect = changeArray[i];
		if (itemRect)
		{			

			//MsgC(" Count %d",i);

			
		
			
			int top = itemRect->greatestTop;
			int bottom = itemRect->smallestBottom;
			int left = itemRect->greatestLeft;
			int right = itemRect->smallestRight;

			if (expandArea)  //try adding more information to allow zooming
			{
				
				left = left - expandThickness;
				if (left < 0) left = 0;

				right = right + expandThickness;
				if (right > BITMAP_X-1) 
					right = BITMAP_X-1;

				top = top - expandThickness;
				if (top < 0) top = 0;

				bottom = bottom + expandThickness;
				if (bottom > BITMAP_Y-1) 
					bottom = BITMAP_Y-1;

			}


			int length = right - left + 1;
			int height = bottom - top + 1;
			int wLineLen;
			if (format==4)
				wLineLen = ((length)*16+31)/32 * 4;	
			else if (format==5)
				wLineLen = ((length)*32+31)/32 * 4;	

						
			int imageStart = (top * imageLineLen) + left * pixelbytes;
			int imageRow = imageStart;

			int rectStart = 0;
			for (int y=top; y<=bottom; y++)
			{
				imageStart = imageRow;

				for (int x=left; x<=right; x++)
				{

					if (format==4)
					{
						*(bitNew+imageStart) =  *(bitmap+imageStart);
						*(bitNew+imageStart+1) =  *(bitmap+imageStart+1);

					}
					else if (format==5)
					{
						*(bitNew+imageStart) =  *(bitmap+imageStart);
						*(bitNew+imageStart+1) =  *(bitmap+imageStart+1);
						*(bitNew+imageStart+2) =  *(bitmap+imageStart+2);
						*(bitNew+imageStart+3) =  *(bitmap+imageStart+3);

					}

					imageStart += pixelbytes;

				} // for x

				imageRow += imageLineLen;

			} //for y
			
		}  //if rect

	} //for each rect

	return bitNew;

		
}


//Can be used only after processswfframe / or inside processswfframe
double ComputePercentCovered(int BITMAP_X,int BITMAP_Y)
{

	double percent = 0;
	double covered = 0;
	double total =  BITMAP_X * BITMAP_Y;

	int max= changeArray.GetSize();	
	if (max > 0)
	{	

		CChangeRectSwf * itemRect = NULL;
		for (int i=0;i<max; i++)
		{
			itemRect = changeArray[i];
			if (itemRect)
			{					
				int length = itemRect->smallestRight - itemRect->greatestLeft + 1;
				int height = itemRect->smallestBottom - itemRect->greatestTop + 1;					
				covered = covered + (length * height);		
			
			}
		}

	}

	
	if (total>0)
		percent = (covered*100)/total ;

	return percent;



}

void WriteSwfFrame(LPBITMAPINFOHEADER alpbi, std::ostringstream &f, LPBYTE bitmap, int BITMAP_X, int BITMAP_Y, int format)
{	

		

		int frametype = ProcessSwfFrame( alpbi, f,  bitmap, BITMAP_X,  BITMAP_Y,  format);		

		int max= changeArray.GetSize();	
		
		if (max > 0) 
		//if ((max > 0) && (determinant))		
		{	
							 
			LPBYTE bitRect = MakeFullRect( alpbi, bitmap, BITMAP_X, BITMAP_Y,  format,  max);


			if (!bitRect)
				return;

			//Set BitsLossless
			int wLineLen;
			if (format==4)
				wLineLen = ((BITMAP_X)*16+31)/32 * 4;	
			else if (format==5)
				wLineLen = ((BITMAP_X)*32+31)/32 * 4;	
			
			int dwSize = wLineLen * (BITMAP_Y);			
			
			FlashZLibBitmapData zdata((unsigned char *) bitRect,dwSize); 
			FlashTagDefineBitsLossless2 bll(format,BITMAP_X, BITMAP_Y, zdata);
			f << bll;	
			
			
			if (bitRect)
			{
				free(bitRect);

			}

			FlashMatrix m;
			m.SetScale(20,20);				
			m.SetTranslate((FrameOffsetX-MatrixOffsetX)*20,(FrameOffsetY-MatrixOffsetY)*20);

			FlashFillStyleBitmap ffbm(bll.GetID(),m);
			//to handle
			if (freecharacter)
				freeCharacterArray.Add(bll.GetID());

			FlashFillStyleArray ffa;	
			ffa.AddFillStyle(&ffbm);			


			FlashShapeWithStyle s;			
			CChangeRectSwf * itemRect = NULL;
			for (int i=0;i<max; i++)
			{
				itemRect = changeArray[i];
				if (itemRect)
				{	

					int length = itemRect->smallestRight - itemRect->greatestLeft + 1 ;
					int height = itemRect->smallestBottom - itemRect->greatestTop + 1 ;					
					
					FlashShapeRecordChange changerec;
					changerec.ChangeFillStyle1(1);					
					s.AddRecord(changerec);
					s.AddRecord(FlashShapeRecordChange((itemRect->greatestLeft+FrameOffsetX+MoveOffsetX)*20,(itemRect->greatestTop+FrameOffsetY+MoveOffsetY)*20));
					s.AddRecord(FlashShapeRecordStraight(length*20,0*20));
					s.AddRecord(FlashShapeRecordStraight(0*20,height*20));
					s.AddRecord(FlashShapeRecordStraight(-length*20,0*20));
					s.AddRecord(FlashShapeRecordStraight(0*20,-height*20));									
					

				}				

			}	

			s.SetFillStyleArray(ffa);	

			FlashTagDefineShape3 fsws(s);							
			f << fsws;

			if (hasIntermediateFrame)
			{

				FlashTagRemoveObject2 ro(IFrameDepth);
				f << ro;
				hasIntermediateFrame = 0;

			}

			
			if (frametype==FRAME_HALFKEY) 
			{	
				 //Unlike Intermediate Frames, we do not remove the Halfkey object up to a certain depth				 
				 FlashTagPlaceObject2 po(HalfKeyDepthBase+HalfKeyDepthInc, fsws.GetID()); 
				 f << po;				 
				 HalfKeyDepthInc++;


			}
			else { //if  frametype==FRAME_INTERMEDIATE
			
				FlashTagPlaceObject2 po(IFrameDepth, fsws.GetID()); 
				f << po;
				hasIntermediateFrame = 1;

			}
			

			f << FlashTagShowFrame();			
			framecount++;

			
	
		}  //if (max > 0)
		else
		{		

			if (frametype==FRAME_KEYFRAME) //one reason for max == 0
			{

				if (freecharacter)
					gcFlash(f);

				int wLineLen;
				if (format==4)
					wLineLen = ((alpbi->biWidth)*16+31)/32 * 4;	
				else if (format==5)
					wLineLen = ((alpbi->biWidth)*32+31)/32 * 4;	
				
				int dwSize = wLineLen * (alpbi->biHeight);
			
			
				FlashZLibBitmapData zdata((unsigned char *) bitmap,dwSize); //need to takeinto account .... alignment?
				FlashTagDefineBitsLossless2 bll(format,BITMAP_X, BITMAP_Y, zdata);
				f << bll;	


				FlashMatrix m;
				m.SetScale(20,20);			
				m.SetTranslate((FrameOffsetX-MatrixOffsetX)*20,(FrameOffsetY-MatrixOffsetY)*20);
									
				FlashFillStyleBitmap ffb(bll.GetID(),m); 
				
				//to handle
				if (freecharacter)
					freeCharacterArray.Add(bll.GetID());
					
				
				FlashFillStyleArray ffa;					
				ffa.AddFillStyle(&ffb);
								
				
				FlashShapeWithStyle s;
				FlashShapeRecordChange changerec;
				changerec.ChangeFillStyle1(1); //must set to FillStyle1 to be imported
				
				s.AddRecord(changerec);
				s.AddRecord(FlashShapeRecordChange(FrameOffsetX*20,FrameOffsetY*20));				
				s.AddRecord(FlashShapeRecordStraight((BITMAP_X-MoveOffsetX)*20,0*20));
				s.AddRecord(FlashShapeRecordStraight(0*20,(BITMAP_Y-MoveOffsetY)*20));
				s.AddRecord(FlashShapeRecordStraight(-(BITMAP_X-MoveOffsetX)*20,0*20));
				s.AddRecord(FlashShapeRecordStraight(0*20,-(BITMAP_Y-MoveOffsetY)*20));
				
				s.SetFillStyleArray(ffa);
				FlashTagDefineShape3 fsws(s);		
				
				f << fsws;

				//adding multiple depths greatly slow down,flash drawings..
				//so we need the remove
				//if (framei > 0) {	

				if (hasIntermediateFrame)
				{

					FlashTagRemoveObject2 ro(IFrameDepth);
					f << ro;
					hasIntermediateFrame = 0;

				}

				if (HalfKeyDepthInc)
				{

					for (int j=HalfKeyDepthInc;j>=0;j--)
					{
						FlashTagRemoveObject2 ro(HalfKeyDepthBase+j);
						f << ro;			
					}
					HalfKeyDepthInc = 0;
				
				}

				if (hasKeyFrame)
				{
					FlashTagRemoveObject2 ro(KeyFrameDepth);
					f << ro;
					hasKeyFrame = 0;

				}

				
					FlashTagPlaceObject2 po(KeyFrameDepth, fsws.GetID()); 					
					
					f << po;
					hasKeyFrame = 1;

			}
			else 
			{
				//This block here usually capture the case of frametype = halfkey, 
				//and yet changeblocks == 0
				
				//else if not keyframe...do nothing..just show frame			

				//We have to remove this if it is present because
				//the comparision is made without the intermediate frame
				if (hasIntermediateFrame)
				{

					FlashTagRemoveObject2 ro(IFrameDepth);
					f << ro;
					hasIntermediateFrame = 0;
				

				}


			}

			if (noAutoPlay)
			{
				if (firstvideoFrame) //this variable is set to 1 only at the beginning of the full conversion
									 //split parts will always have	firstvideoFrame = 0
				{
					FlashActionStop s;
					FlashTagDoAction ftd;
					ftd.AddAction(&s);	

					f << ftd;
					

				}
			}
			
			
			f << FlashTagShowFrame();
			framecount++;

		}


		// mp3 background music
		// no synchronization
		// allow only for single file
		if ((firstvideoFrame) && (!needbreakapart))
		{
			if (useMP3)
			{
					
					if (mp3FileLoaded)
					{				
						FlashMP3Encoder MP3(LPCTSTR(mp3File),sampleFPS);
						
						UWORD MP3_id = MP3.WriteDefineTag(f);
									
						std::vector<FlashSoundEnvelope> v;
						v.push_back(FlashSoundEnvelope(0, mp3volume, mp3volume));
						v.push_back(FlashSoundEnvelope(1013526784, mp3volume, mp3volume));
						FlashSoundInfo fsi(FSI_SYNC_NO_MULTIPLE | FSI_HAS_ENVELOPE, 0, 0, 1, v);			
						f << FlashTagStartSound(MP3_id, fsi);
						
						f << FlashTagShowFrame(); // this will be frame 0 if mp3 background music is used
						framecount++;

						
					}
			}
		}
		firstvideoFrame = 0;		
		
		
		if (!noAudioStream)
		{	
				if (giFirstAudio>=0)
				{
					
					int absize;
					absize = samplecountavg * 2 * 2;
					void* buffer = malloc(absize); 
					long retlenSwf = 0;		
					long lReadSwf = 0;		
					long retval;				
					

					if (usePCMConvertedStream)
					{
						retval = AVIStreamRead(PCMConvertedStream, slCurrentSwf, samplecountavg,
								  buffer,
								  absize,
								  &retlenSwf,
								  &lReadSwf);
					}
					else
					{	

						retval = AVIStreamRead(gapavi[giFirstAudio], slCurrentSwf, samplecountavg,
								  buffer,
								  absize,
								  &retlenSwf,
								  &lReadSwf);

					}

					slCurrentSwf += lReadSwf;					
					
					buffer = MakeFullBuffer(buffer, retlenSwf, lReadSwf, samplecountavg, stream_stereo, stream_16bit);
							 
					if (useAudioCompression == 0) 
					{
						if (retlenSwf>0)
						f << FlashTagSoundStreamBlock((char *) buffer, retlenSwf);
						
					}
					else
					{
						if ((retlenSwf>0) && (lReadSwf>0))
							MakeSoundStreamBlockADPCM( buffer, retlenSwf, lReadSwf, f);

					}				

					if (buffer) free(buffer);

				}

		}

}



//Determine if the frame is intermediate or key frame
int ProcessSwfFrame(LPBITMAPINFOHEADER alpbi, std::ostringstream &f, LPBYTE bitmap, int BITMAP_X, int BITMAP_Y, int format)
{
	cleanChangeArray(); //need to clean it for each frame

	int ret = 0;
	int wColSize = 0;
	int dwSize = alpbi->biSizeImage + sizeof(BITMAPINFOHEADER) +  wColSize;

	if (useHalfKey) 
	{				
		//using halfkey will no longer rely on keyframerate variable
		if (HalfKeyDepthInc > Max_HalfKeyDepth)
		{
			framei = 0;			
			//HalfKeyDepthInc = 0;

		}
	}
	
	if (framei ==0)
	{

		//KeyFrame;
		if (currentKey_lpbi) 
		{
			if ((currentKey_lpbi->biSizeImage != alpbi->biSizeImage))
			{
				free(currentKey_lpbi);
				currentKey_lpbi = (LPBITMAPINFOHEADER) malloc(dwSize);
			}
		}
		else
			currentKey_lpbi = (LPBITMAPINFOHEADER) malloc(dwSize);

		memcpy( (void *) currentKey_lpbi, (void *) alpbi, dwSize );

		ret = FRAME_KEYFRAME;
		

	}	
	else 
	{
		

		//Intermediate Frame
		for (int y=0;y<BITMAP_Y;y++)
		{
			for (int x=0;x<BITMAP_X;x++)
			{					
				if (IsDifferent(alpbi,BITMAP_X,BITMAP_Y,  x,y, format))
				{
					//MsgC(" ID");
					AddExpandBlock(BITMAP_X,BITMAP_Y,x,y, format);
				}

			}
		}


		ret = FRAME_INTERMEDIATE;

		if (useHalfKey)
		{ 
			//if using useHalfKey, then will definitely usePercent 
			
			//Force every frame as Halfkey, ==> every frame is compared to previous frame
			//whether the number of change blocks is zero or greater, we just save this frame as halfkey
			//and indicate it when returning
			
			
			//Whether this frame is going to be a Key Frame or Halfkey Frame
			//We need to copy the current frame as a basis for comparison for the next frame
			{
				//Halfkey frame
				//we must not clean up the array and must leave it be
				//we will need it later on to update the keyframe 
				//cleanChangeArray(); 
				
				if (currentKey_lpbi) 
				{
					if ((currentKey_lpbi->biSizeImage != alpbi->biSizeImage))
					{
						free(currentKey_lpbi);
						currentKey_lpbi = (LPBITMAPINFOHEADER) malloc(dwSize);
					}
				}
				else
					currentKey_lpbi = (LPBITMAPINFOHEADER) malloc(dwSize);

				memcpy( (void *) currentKey_lpbi, (void *) alpbi, dwSize );

				ret = FRAME_HALFKEY;				

				if (usePercent)
				{
					double percent= ComputePercentCovered( BITMAP_X, BITMAP_Y);
					int determinant = (percent > HalfKeyThreshold);
					if (determinant)
					{
						//since we have decided this to be a keyframe
						//we do not need the chnageblocks anymore
						cleanChangeArray(); 
						ret = FRAME_KEYFRAME;						

					}

				}
			
			}


		}
		else if (usePercent)
		{
				//The introduction of new keyframes seems to result in patches...
				//The patches are a result of MS Video 1 -- not full 100% quality!
				
				//If there are too much change in intermediate frame
				double percent= ComputePercentCovered( BITMAP_X, BITMAP_Y);
				int determinant = (percent > PercentThreshold);
				
				if (determinant)  //Force keyframe
				{
					cleanChangeArray(); //this will clean up changeArray and force ChangeArray.getSize()==0  
					
					if (currentKey_lpbi) 
					{
						if ((currentKey_lpbi->biSizeImage != alpbi->biSizeImage))
						{
							free(currentKey_lpbi);
							currentKey_lpbi = (LPBITMAPINFOHEADER) malloc(dwSize);
						}
					}
					else
						currentKey_lpbi = (LPBITMAPINFOHEADER) malloc(dwSize);

					memcpy( (void *) currentKey_lpbi, (void *) alpbi, dwSize );				
					
					
					ret = FRAME_KEYFRAME;
					

				}

		}


	}

	
	framei++;
	if (!useHalfKey)
	{
		if (framei>=keyframerate)
			framei = 0;
	}
	
	
	

	return ret;

}

int IsDifferent(LPBITMAPINFOHEADER alpbi, int BITMAP_X,int BITMAP_Y, int x, int y, int format)
{

	int widthBytes, pixbytes;
	if (format==4)
	{
		pixbytes = 2;
		widthBytes = (BITMAP_X*16+31)/32 * 4;

	}
	else if (format==5)
	{	
		pixbytes = 4;
		widthBytes = (BITMAP_X*32+31)/32 * 4;
	}
	else 
		return FALSE;
	 
	int access = y * widthBytes + x * pixbytes;

	LPBYTE bitsFrame = (LPBYTE) alpbi + // pointer to data
						alpbi->biSize +
						alpbi->biClrUsed * sizeof(RGBQUAD);

	LPBYTE bitsKey = (LPBYTE) currentKey_lpbi + // pointer to data
						currentKey_lpbi->biSize +
						currentKey_lpbi->biClrUsed * sizeof(RGBQUAD);

	int diff = FALSE;

	if (format==4)
	{	
		int val1 = *(bitsFrame+access);
		int val2 = *(bitsFrame+access+1);

		int valKey1 = *(bitsKey+access);
		int valKey2 = *(bitsKey+access+1);

		if ((val1-valKey1) || (val2-valKey2))
			diff = TRUE;

	}
	else if (format==5)
	{
		int Blue = *(bitsFrame+access);
		int Green = *(bitsFrame+access+1);
		int Red = *(bitsFrame+access+2);
		int Alpha = *(bitsFrame+access+3);

		int BlueKey = *(bitsKey+access);
		int GreenKey = *(bitsKey+access+1);
		int RedKey = *(bitsKey+access+2);
		int AlphaKey = *(bitsKey+access+3);

		if ((Blue-BlueKey) || (Green-GreenKey) || (Red-RedKey) || (Alpha-AlphaKey))
			diff = TRUE;
		
	}		

	return diff;	

}


void AddExpandBlock(int BITMAP_X,int BITMAP_Y,int x,int y,int format)
{

	int blockx = x/blocksize_x;
	int blocky = y/blocksize_y;

	int max= changeArray.GetSize();
	if (max>0)
	{		
			// SearchBlock
			int found = 0;			
			CChangeRectSwf * itemRect = NULL;
			for (int i=0;i<max; i++)
			{
				itemRect = changeArray[i];
				if (itemRect)
				{
					if ((itemRect->blockx == blockx) && (itemRect->blocky == blocky))
					{
						found=1;
						break;
					}
				}
				
			}
			
			if (found)			
			{
				ExpandBlock(itemRect,BITMAP_X,BITMAP_Y, x, y, format);
			}
			else
				AddNewBlock(BITMAP_X,BITMAP_Y, x, y, format);

	}
	else
		AddNewBlock(BITMAP_X,BITMAP_Y, x, y, format);


}

//Assuming the itemRect is correctly chosen
void ExpandBlock(CChangeRectSwf *itemRect, int BITMAP_X,int BITMAP_Y,int x,int y,int format)
{
	if (itemRect)
	{
		//Expand left bounds
		if (x < itemRect->greatestLeft) 
		{
			itemRect->greatestLeft = x;
		}

		//Expand top bounds
		if (y < itemRect->greatestTop) 
		{
			itemRect->greatestTop = y;
		}
		
		//Expand right bounds
		if (x > itemRect->smallestRight)
		{
			itemRect->smallestRight = x;

		}

		//Expand bottom bounds
		if (y > itemRect->smallestBottom)
		{
			itemRect->smallestBottom = y;

		}	

	}

}


void AddNewBlock(int BITMAP_X,int BITMAP_Y,int x,int y,int format)
{	
	CChangeRectSwf *itemRect = new CChangeRectSwf;	
	
	int blockx = x/ blocksize_x;
	int blocky = y/ blocksize_y;
	itemRect->initialize(blockx, blocky , blocksize_x, blocksize_y, numblocks_x, BITMAP_X, BITMAP_Y, x ,y);
	changeArray.Add(itemRect);
	
}


void finishTemporalCompress()
{
	initTemporalCompress(0,0);
}


void initTemporalCompress(int bmWidth, int bmHeight)
{
	if (currentKey_lpbi) {
		free(currentKey_lpbi);
		currentKey_lpbi = NULL;
	}
	cleanChangeArray();
	framei = 0;	
	framecount = 0;

	hasKeyFrame = 0;
	hasIntermediateFrame = 0;
	HalfKeyDepthInc = 0;
		
	numblocks_x = bmWidth/blocksize_x;
	numblocks_y = bmHeight/blocksize_y;

	if ((bmWidth % blocksize_x)>0)
		numblocks_x++;

	if ((bmHeight % blocksize_y)>0)
		numblocks_y++;

	freeCharacterArray.RemoveAll();

}

void cleanChangeArray()
{
	int max = changeArray.GetSize();
	CChangeRectSwf * itemRect = NULL;
	for (int i=0;i<max; i++)
	{
		itemRect = changeArray[i];
		if (itemRect)		
			delete itemRect;						
		
	}

	changeArray.RemoveAll();

	
}


void *MakeFullBuffer(void* buffer, long &buffersize, long &numsamples, int avgsamplespersecond,bool streamstereo, bool stream16bit)
{

	if (buffersize==0)
		return buffer;

	if (numsamples >= (avgsamplespersecond))
		return buffer;

	if (!stream16bit)
		return buffer;	

	int newsize, bytespersample; 
	int smul = 1;
	if (streamstereo)
		smul = 2;

	bytespersample = 2; //only handle 16 bits per sample

	newsize = avgsamplespersecond * smul * bytespersample;
	
	if (newsize<=buffersize)
		return buffer;
	

	void * newbuffer = realloc(buffer,newsize);	
	buffersize = newsize;
	numsamples = avgsamplespersecond;
	return newbuffer;

}

void MakeSoundStreamBlockADPCM(void* buffer, int buffersize, int numsamples, std::ostringstream &f)
{

	if (buffersize<=0)
		return;

	
	FSound sound;
	int streamsize = 0;
	int streamchannels = 0;
	if (stream_16bit) //external var
		streamsize = 1;
	if (stream_stereo) //external var
		streamchannels = 1;	
	sound.format =  4 * ( stream_rate ) + ( streamsize ) * 2 + ( streamchannels );
	sound.nSamples = numsamples;
	sound.samples  = (U8*)( buffer );		
	sound.dataLen  = buffersize;
	sound.delay    = 0;

	std::vector<U8> adpcmData;
	adpcmData.clear();
	FSoundComp compress( &sound, adpcmBPS );
	compress.Compress( (U8*)( buffer ), numsamples, &adpcmData ); 
	compress.Flush( &adpcmData );

	FlashTagSoundStreamBlock stb((char*)&adpcmData[0],adpcmData.size());
	f << stb;  

	adpcmData.clear();
	

}


// ver 2/24
// **************************************
// Conversion to PCM
// **************************************
int SaveFirstAudioToFile(CString filename)
{

	PAVIFILE pavi;
	if (AVIFileOpen(&pavi,LPCTSTR(filename),OF_CREATE | OF_WRITE | OF_SHARE_DENY_NONE,NULL)!=0)
		return 0;
	if (CopyStream(pavi,gapavi[giFirstAudio])!=0)
		return 0;
	AVIFileRelease(pavi);
	return 1;

}


// Code Derived from article "Concerning Video for Windows API" 
// See credit links in help file for more info
int CopyStream(PAVIFILE pavi,PAVISTREAM pstm)
{
	AVISTREAMINFO si;
	LONG st,ed,leng;
	BYTE p[20000];
	PAVISTREAM ptmp;

	st=AVIStreamStart(pstm);
	ed=st+AVIStreamLength(pstm)-1;
	if (AVIStreamInfo(pstm,&si,sizeof(AVISTREAMINFO))!=0)
		return -1;
	if (AVIFileCreateStream(pavi,&ptmp,&si)!=0)
		return -1;
	if (AVIStreamReadFormat(pstm,st,NULL,&leng)!=0)
		return -1;
	if (AVIStreamReadFormat(pstm,st,p,&leng)!=0)
		return -1;
	if (AVIStreamSetFormat(ptmp,st,p,leng)!=0)
		return -1;


	long sampleread = 4096;
	long byteread = 0; 
	long bufferlength = 20000;
	long slCurrentWav = 0;

	slCurrentWav = st;
	while (slCurrentWav <= ed)
	{
		AVIStreamRead(pstm,slCurrentWav,WAVBUFFER,
			p,
			bufferlength,
			&byteread,
			&sampleread);

		if (sampleread > 0)
		{
		
			AVIStreamWrite(ptmp,slCurrentWav,sampleread,p,
				bufferlength,
				AVIIF_KEYFRAME,
				NULL,
				NULL);

			slCurrentWav+=sampleread;

		}
		else
			break;
		
	}


	AVIStreamRelease(ptmp);
	free(p);
	return 0;
}


//return <=0 if fails / no conversion
int ConvertFileToPCM(CString infilename, CString outfilename, LPWAVEFORMATEX lpFormat)
{
		
	HCURSOR  hcur;
	hcur = SetCursor(LoadCursor(NULL, IDC_WAIT));
	ShowCursor(TRUE);


	int retval = 0;		
	retval = MultiStepConvertToPCM(infilename, outfilename, lpFormat, sizeof(WAVEFORMATEX));

		
    ShowCursor(FALSE);
    SetCursor(hcur);

	return retval;

}

//Using this function assumes we have checked giFirstAudio stream is non PCM
PAVISTREAM ConvertFirstAudioStream(LPWAVEFORMATEX lpFormat)
{
	if (giFirstAudio<0)
		return NULL;

	tempfile1 = "";
	tempfile2 = "";

	CString nonPCMfileName("\\~tnonPCM.wav");
	CString yesPCMfileName("\\~tyesPCM.wav");
	CString tpath = GetProgPath();
	CString infilename = tpath + nonPCMfileName;
	CString outfilename = tpath + yesPCMfileName;

	SaveFirstAudioToFile(infilename);
	
	int retval = ConvertFileToPCM(infilename, outfilename,  lpFormat);	
	if (retval<=0) return NULL;


	tempfile1 = infilename;
	tempfile2 = outfilename;

	PAVISTREAM pstm;
	
	if (AVIFileOpen(&PCMConvertedFile,LPCTSTR(outfilename),OF_READ | OF_SHARE_DENY_NONE,NULL)!=0)
		return NULL;

	if (AVIFileGetStream(PCMConvertedFile,&pstm,0,0)!=0)
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

void produceFlashHTML(CString htmlfilename, CString flashfilename, CString flashfilepath, int width, int height,int bk_red, int bk_green, int bk_blue)	
{

	COLORREF bkcolor = RGB(bk_blue,bk_green,bk_red);
	
	FILE * htmlfile = NULL;
		
	htmlfile = fopen(LPCTSTR(htmlfilename),"wt");
	if (!htmlfile) return;


	fprintf(htmlfile,"<HTML> \n");
	fprintf(htmlfile,"<HEAD> \n");
	fprintf(htmlfile,"<TITLE> %s </TITLE> \n",LPCTSTR(flashfilename));
	fprintf(htmlfile,"</HEAD> \n");
	fprintf(htmlfile,"<BODY> \n");


	fprintf(htmlfile,"<!-- Flash movie tag--> \n");
    fprintf(htmlfile,"<OBJECT classid=\"clsid:D27CDB6E-AE6D-11cf-96B8-444553540000\" \n");
	fprintf(htmlfile,"codebase=\"http://download.macromedia.com/pub/shockwave/cabs/flash/swflash.cab#version=6,0,0,0\" \n");
	fprintf(htmlfile,"WIDTH=\"%d\" HEIGHT=\"%d\" id=\"",width,height);
	fprintf(htmlfile,"%s",LPCTSTR(flashfilename));
	fprintf(htmlfile,"\" ALIGN=\"\"> \n");
	fprintf(htmlfile," <PARAM NAME=movie VALUE=\"");
	fprintf(htmlfile,"%s",LPCTSTR(flashfilename));
	fprintf(htmlfile,"\"> \n <PARAM NAME=quality VALUE=high> ");
	fprintf(htmlfile,"\n <PARAM NAME=bgcolor VALUE=#%x> \n <EMBED src=\"",bkcolor);
	fprintf(htmlfile,"%s",LPCTSTR(flashfilename));	
	fprintf(htmlfile,"\" quality=high bgcolor=#%x  WIDTH=\"%d\" HEIGHT=\"%d\" ",width, height,bkcolor);	
	fprintf(htmlfile,"NAME=\"");
	fprintf(htmlfile,"%s",LPCTSTR(flashfilename));
	fprintf(htmlfile,"\" ALIGN=\"\" TYPE=\"application/x-shockwave-flash\" ");
	fprintf(htmlfile,"PLUGINSPAGE=\"http://www.macromedia.com/go/getflashplayer\">\n</EMBED> \n");
	fprintf(htmlfile,"</OBJECT>\n");	

	fprintf(htmlfile,"</BODY> \n");
	fprintf(htmlfile,"</HTML> \n");

	fclose(htmlfile);

}



void LoadSettings() 
{
	//Do not load saved settings for now
	if (runmode==0)
		return;

	//if runmode==2 (batch mode...attempt to load settings)	
	
	FILE * sFile;
	CString setDir,setPath;
	CString fileName;
	
	
	//********************************************
	//Loading CamProducer.ini for storing text data
	//********************************************
	if (runmode==0)
		fileName="\\CamStudio.Producer.ini";	
	else if (runmode==2)
		fileName="\\CamStudio.Producer.param";	//command line mode 

	setDir=GetProgPath();
	setPath=setDir+fileName;

	sFile = fopen(LPCTSTR(setPath),"rt");
	if (sFile == NULL) {	
		return;
	}


	// ****************************
	// Read Variables	
	// ****************************
	
	//char sdata[1000]; 	
	float ver=1.0;
		
	fscanf(sFile, "[ CamStudio Flash Producer Settings ver%f -- Please do not edit ] \n\n",&ver);

	int swfnameLen = 0;
	int swfhtmlnameLen = 0;
	int swfbasenameLen = 0;
	//int avifilenameLen = 0;
		

	//Ver 1.0
	if (ver>=0.99999)	{ 

		//Important Variables

		//fscanf(sFile, "avifilenameLen = %d \n",&avifilenameLen);

		fscanf(sFile, "usePercent = %d \n",&usePercent);
		
		fscanf(sFile, "useHalfKey = %d \n",&useHalfKey);
		fscanf(sFile, "keyframerate = %d \n",&keyframerate);
		fscanf(sFile, "Max_HalfKeyDepth = %d \n",&Max_HalfKeyDepth); 

		fscanf(sFile, "FrameOffsetX = %d \n",&FrameOffsetX);
		fscanf(sFile, "FrameOffsetY = %d \n",&FrameOffsetY);

		fscanf(sFile, "useAudio = %d \n",&useAudio);
		fscanf(sFile, "useAudioCompression = %d \n",&useAudioCompression); 
		fscanf(sFile, "useMP3 = %d \n",&useMP3);
		fscanf(sFile, "mp3volume = %d \n",&mp3volume); 




		fscanf(sFile, "sampleFPS = %d \n",&sampleFPS);
		fscanf(sFile, "convertBits = %d \n",&convertBits); 

		fscanf(sFile, "noLoop = %d \n",&noLoop);
		fscanf(sFile, "noAutoPlay = %d \n",&noAutoPlay);
		fscanf(sFile, "addControls = %d \n",&addControls);
		fscanf(sFile, "controlsWidth = %d \n",&controlsWidth);
		fscanf(sFile, "controlsHeight = %d \n",&controlsHeight);

		

		fscanf(sFile, "adpcmBPS = %d \n",&adpcmBPS);
		fscanf(sFile, "launchPropPrompt = %d \n",&launchPropPrompt);
		fscanf(sFile, "launchHTMLPlayer = %d \n",&launchHTMLPlayer);
		
		fscanf(sFile, "swfnameLen = %d \n",&swfnameLen);
		fscanf(sFile, "swfhtmlnameLen = %d \n",&swfhtmlnameLen);
		fscanf(sFile, "swfbasenameLen = %d \n",&swfbasenameLen);


			
		
		//Lesser Variables
		
		fscanf(sFile, "PercentThreshold = %f \n",&PercentThreshold);
		fscanf(sFile, "HalfKeyThreshold = %f \n",&HalfKeyThreshold);
		
		fscanf(sFile, "blocksize_x = %d \n",&blocksize_x);
		fscanf(sFile, "blocksize_y = %d \n",&blocksize_y);
		fscanf(sFile, "numblocks_x = %d \n",&numblocks_x);
		fscanf(sFile, "numblocks_y = %d \n",&numblocks_y);
		fscanf(sFile, "expandArea = %d \n",&expandArea);  
		fscanf(sFile, "expandThickness = %d \n",&expandThickness);

		fscanf(sFile, "MatrixOffsetX = %d \n",&MatrixOffsetX);
		fscanf(sFile, "MatrixOffsetY = %d \n",&MatrixOffsetY);
		fscanf(sFile, "MoveOffsetX = %d \n",&MoveOffsetX);
		fscanf(sFile, "MoveOffsetY = %d \n",&MoveOffsetY);

		fscanf(sFile, "KeyFrameDepth = %d \n",&KeyFrameDepth);
		fscanf(sFile, "HalfKeyDepthBase = %d \n",&HalfKeyDepthBase);
		fscanf(sFile, "IFrameDepth = %d \n",&IFrameDepth);
		fscanf(sFile, "ObjectDepth = %d \n",&ObjectDepth);

		fscanf(sFile, "swfbar_red = %d \n",&swfbar_red);
		fscanf(sFile, "swfbar_green = %d \n",&swfbar_green);
		fscanf(sFile, "swfbar_blue = %d \n",&swfbar_blue);

		fscanf(sFile, "swfbk_red = %d \n",&swfbk_red);
		fscanf(sFile, "swfbk_green = %d \n",&swfbk_green);
		fscanf(sFile, "swfbk_blue = %d \n",&swfbk_blue);
		
		fscanf(sFile, "swf_bits_per_sample = %d \n",&swf_bits_per_sample);
		fscanf(sFile, "swf_samples_per_seconds = %d \n",&swf_samples_per_seconds);
		fscanf(sFile, "swf_num_channels = %d \n",&swf_num_channels);							

		fscanf(sFile, "allowChaining = %d \n",&allowChaining);
		fscanf(sFile, "freecharacter = %d \n",&freecharacter);
		fscanf(sFile, "percentLoadedThreshold = %f \n",&percentLoadedThreshold);
		
		fscanf(sFile, "addPreloader = %d \n",&addPreloader);
		fscanf(sFile, "applyPreloaderToSplitFiles = %d \n",&applyPreloaderToSplitFiles);
		fscanf(sFile, "produceRaw = %d \n",&produceRaw);		
	

	}		

	fclose(sFile);



	//********************************************
	//Loading Camdata.ini  binary data
	//********************************************	
	FILE * tFile;
	if (runmode==0)
		fileName="\\CamStudio.Producer.Data.ini";
	else
		fileName="\\CamStudio.Producer.Data.command"; //command line mode
	setDir=GetProgPath();
	setPath=setDir+fileName;

	tFile = fopen(LPCTSTR(setPath),"rb");
	if (tFile == NULL) {
		return;		
	}

	

	if (ver> 0.999999)	{ //ver 1.0

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
		
		
		//ver 1.2
		if (ver>1.199999) {

				
		}

	} 

	fclose(tFile);	


}

void SaveController() 
{
	
	FILE * sFile;
	CString setDir,setPath;
	CString fileName;
		
	fileName="\\controller\\controller.ini";	
	setDir=GetProgPath();
	setPath=setDir+fileName;

	sFile = fopen(LPCTSTR(setPath),"wt");
	if (sFile == NULL) {	
		return;
	}


	// ****************************
	// Write Variables	
	// ****************************	

	float ver=1.0;
		
	fprintf(sFile, "[ CamStudio Controller Settings ver%.2f ] \n\n",ver);


	//Ver 1.0
	{ 
				
		fprintf(sFile, "Spacing_Between_Buttons_X = %d \n",ButtonSpaceX);
		fprintf(sFile, "Spacing_Between_Controller_And_Movie_Y = %d \n",ButtonSpaceY);
		fprintf(sFile, "Shift_LR_Pieces_Down_By_Y = %d \n",PieceOffsetY);
		fprintf(sFile, "Spacing_Between_ProgressBar_And_Movie_Y = %d \n",ProgressOffsetY); 

		fprintf(sFile, "Draw_Left_Piece = %d \n",yes_drawLeftPiece);
		fprintf(sFile, "Draw_Right_Piece = %d \n",yes_drawRightPiece);
		fprintf(sFile, "Draw_Stop_Button = %d \n",yes_drawStopButton);

		fprintf(sFile, "Full_Controller_Width = %d \n",ControllerWidth);		
		fprintf(sFile, "Controller_Alignment = %d \n",ControllerAlignment);
		
	}		

	fclose(sFile);

}


void LoadController() 
{
	
	FILE * sFile;
	CString setDir,setPath;
	CString fileName;
		
	fileName="\\controller\\controller.ini";	
	setDir=GetProgPath();
	setPath=setDir+fileName;

	sFile = fopen(LPCTSTR(setPath),"rt");
	if (sFile == NULL) {	
		return;
	}

	// ****************************
	// Write Variables	
	// ****************************	

	float ver=1.0;
		
	fscanf(sFile, "[ CamStudio Controller Settings ver%f ] \n\n",&ver);

	//Ver 1.0
	if (ver > 0.99)
	{ 				
		fscanf(sFile, "Spacing_Between_Buttons_X = %d \n",&ButtonSpaceX);
		fscanf(sFile, "Spacing_Between_Controller_And_Movie_Y = %d \n",&ButtonSpaceY);
		fscanf(sFile, "Shift_LR_Pieces_Down_By_Y = %d \n",&PieceOffsetY);
		fscanf(sFile, "Spacing_Between_ProgressBar_And_Movie_Y = %d \n",&ProgressOffsetY); 

		fscanf(sFile, "Draw_Left_Piece = %d \n",&yes_drawLeftPiece);
		fscanf(sFile, "Draw_Right_Piece = %d \n",&yes_drawRightPiece);
		fscanf(sFile, "Draw_Stop_Button = %d \n",&yes_drawStopButton);

		fscanf(sFile, "Full_Controller_Width = %d \n",&ControllerWidth);		
		fscanf(sFile, "Controller_Alignment = %d \n",&ControllerAlignment);
		
	}		

	fclose(sFile);

}


void SaveSettings() 
{

	
	FILE * sFile;
	CString setDir,setPath;
	CString fileName;
	
	
	//********************************************
	//Saving CamProducer.ini for storing text data
	//********************************************

	fileName="\\CamStudio.Producer.ini";	
	setDir=GetProgPath();
	setPath=setDir+fileName;

	sFile = fopen(LPCTSTR(setPath),"wt");
	if (sFile == NULL) {	
		return;
	}


	// ****************************
	// Write Variables	
	// ****************************	

	float ver=1.0;
		
	fprintf(sFile, "[ CamStudio Flash Producer Settings ver%.2f -- Please do not edit ] \n\n",ver);

	int swfnameLen = 0;
	int swfhtmlnameLen = 0;
	int swfbasenameLen = 0;	

	//Ver 1.0
	//if (ver>=0.99999)	
	{ 
		//Important Variables
		//fprintf(sFile, "avifilenameLen = %d \n",avifilename.GetLength());

		fprintf(sFile, "usePercent = %d \n",usePercent);
		fprintf(sFile, "useHalfKey = %d \n",useHalfKey);
		fprintf(sFile, "keyframerate = %d \n",keyframerate);
		fprintf(sFile, "Max_HalfKeyDepth = %d \n",Max_HalfKeyDepth); 

		fprintf(sFile, "FrameOffsetX = %d \n",FrameOffsetX);
		fprintf(sFile, "FrameOffsetY = %d \n",FrameOffsetY);

		fprintf(sFile, "useAudio = %d \n",useAudio);
		fprintf(sFile, "useAudioCompression = %d \n",useAudioCompression); 
		fprintf(sFile, "useMP3 = %d \n",useMP3);
		fprintf(sFile, "mp3volume = %d \n",mp3volume); 

		fprintf(sFile, "sampleFPS = %d \n",sampleFPS);
		fprintf(sFile, "convertBits = %d \n",convertBits); 

		fprintf(sFile, "noLoop = %d \n",noLoop);
		fprintf(sFile, "noAutoPlay = %d \n",noAutoPlay);
		fprintf(sFile, "addControls = %d \n",addControls);
		fprintf(sFile, "controlsWidth = %d \n",controlsWidth);
		fprintf(sFile, "controlsHeight = %d \n",controlsHeight);

		fprintf(sFile, "adpcmBPS = %d \n",adpcmBPS);
		fprintf(sFile, "launchPropPrompt = %d \n",launchPropPrompt);
		fprintf(sFile, "launchHTMLPlayer = %d \n",launchHTMLPlayer);
		
		fprintf(sFile, "swfnameLen = %d \n",swfname.GetLength());
		fprintf(sFile, "swfhtmlnameLen = %d \n",swfhtmlname.GetLength());
		fprintf(sFile, "swfbasenameLen = %d \n",swfbasename.GetLength());
			
	
		//Lesser Variables
		
		fprintf(sFile, "PercentThreshold = %f \n",PercentThreshold);
		fprintf(sFile, "HalfKeyThreshold = %f \n",HalfKeyThreshold);
		
		fprintf(sFile, "blocksize_x = %d \n",blocksize_x);
		fprintf(sFile, "blocksize_y = %d \n",blocksize_y);
		fprintf(sFile, "numblocks_x = %d \n",numblocks_x);
		fprintf(sFile, "numblocks_y = %d \n",numblocks_y);
		fprintf(sFile, "expandArea = %d \n",expandArea);  
		fprintf(sFile, "expandThickness = %d \n",expandThickness);

		fprintf(sFile, "MatrixOffsetX = %d \n",MatrixOffsetX);
		fprintf(sFile, "MatrixOffsetY = %d \n",MatrixOffsetY);
		fprintf(sFile, "MoveOffsetX = %d \n",MoveOffsetX);
		fprintf(sFile, "MoveOffsetY = %d \n",MoveOffsetY);

		fprintf(sFile, "KeyFrameDepth = %d \n",KeyFrameDepth);
		fprintf(sFile, "HalfKeyDepthBase = %d \n",HalfKeyDepthBase);
		fprintf(sFile, "IFrameDepth = %d \n",IFrameDepth);
		fprintf(sFile, "ObjectDepth = %d \n",ObjectDepth);

		fprintf(sFile, "swfbar_red = %d \n",swfbar_red);
		fprintf(sFile, "swfbar_green = %d \n",swfbar_green);
		fprintf(sFile, "swfbar_blue = %d \n",swfbar_blue);

		fprintf(sFile, "swfbk_red = %d \n",swfbk_red);
		fprintf(sFile, "swfbk_green = %d \n",swfbk_green);
		fprintf(sFile, "swfbk_blue = %d \n",swfbk_blue);
		
		fprintf(sFile, "swf_bits_per_sample = %d \n",swf_bits_per_sample);
		fprintf(sFile, "swf_samples_per_seconds = %d \n",swf_samples_per_seconds);
		fprintf(sFile, "swf_num_channels = %d \n",swf_num_channels);								

		fprintf(sFile, "allowChaining = %d \n",allowChaining);
		fprintf(sFile, "freecharacter = %d \n",freecharacter);
		fprintf(sFile, "percentLoadedThreshold = %.2f \n",percentLoadedThreshold);
		
		fprintf(sFile, "addPreloader = %d \n",addPreloader);
		fprintf(sFile, "applyPreloaderToSplitFiles = %d \n",applyPreloaderToSplitFiles);
		fprintf(sFile, "produceRaw = %d \n",produceRaw);		



	}		

	fclose(sFile);



	//********************************************
	//Saving Camdata.ini  binary data
	//********************************************	
	FILE * tFile;	
	fileName="\\CamStudio.Producer.Data.ini";
	
	setDir=GetProgPath();
	setPath=setDir+fileName;

	tFile = fopen(LPCTSTR(setPath),"wb");
	if (tFile == NULL) {
		return;		
	}

	

	//if (ver> 0.999999)	
	{ //ver 1.0

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
		
		
		//ver 1.2
		if (ver>1.199999) {

				
		}

	} 

	fclose(tFile);	


}


//ver 2.25
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

		
	if (giFirstVideo >= 0)
		OnFileConverttoswf();
	
	convertMode = 0;
}



void CreatePropertySheet()
{
	flashProp.Construct("Conversion to Flash");	
	flashProp.AddPage( &page1);
	flashProp.AddPage( &page2);
	flashProp.AddPage( &page3);
	
}



int MessageOut(HWND hWnd,long strMsg, long strTitle, UINT mbstatus)
{
	CString tstr("");
	CString mstr("");
	tstr.LoadString( strTitle );
	mstr.LoadString( strMsg );

	return ::MessageBox(hWnd,mstr,tstr,mbstatus);

}

int MessageOutINT(HWND hWnd,long strMsg, long strTitle, UINT mbstatus,long val)
{
	CString tstr("");
	CString mstr("");
	CString fstr("");
	tstr.LoadString( strTitle );
	mstr.LoadString( strMsg );
	fstr.Format(mstr,val);

	return ::MessageBox(hWnd,fstr,tstr,mbstatus);

}

int MessageOutINT2(HWND hWnd,long strMsg, long strTitle, UINT mbstatus,long val1, long val2)
{
	CString tstr("");
	CString mstr("");
	CString fstr("");
	tstr.LoadString( strTitle );
	mstr.LoadString( strMsg );
	fstr.Format(mstr,val1,val2);

	return ::MessageBox(hWnd,fstr,tstr,mbstatus);

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
	if ((lenx>=5) &&
		((swfname[lenx-1]=='i') || (swfname[lenx-1]=='I')) &&
		((swfname[lenx-2]=='v') || (swfname[lenx-2]=='V')) &&
		((swfname[lenx-3]=='a') || (swfname[lenx-3]=='A')) &&
		(swfname[lenx-4]=='.'))
	{

		swfname.SetAt(lenx-1,'f');
		swfname.SetAt(lenx-2,'w');
		swfname.SetAt(lenx-3,'s');
		swfname.SetAt(lenx-4,'.');
							
		swfbasename=swfname.Right(lenx - swfname.ReverseFind('\\') - 1);
		swfhtmlname = swfname + ".html";

	}
	else {

		swfname += ".swf";
		swfhtmlname = swfname + ".html";
		swfbasename = swfname.Right(lenx - swfname.ReverseFind('\\') - 1);

	}	

}



void LoadCommand() 
{
	FILE * sFile;
	CString setDir,setPath;
	CString fileName;
	fileName="\\CamStudio.Producer.command";	//command line mode 

	setDir=GetProgPath();
	setPath=setDir+fileName;

	sFile = fopen(LPCTSTR(setPath),"rt");
	if (sFile == NULL) {	
		return;
	}	
	

	// ****************************
	// Read Variables	
	// ****************************	

	float ver=1.0;
	
	//Debugging info
	//The use of scanf("%.2f") instead of scanf("%f") results in hard-to-detect bugs 
	fscanf(sFile,"[ CamStudio Flash Producer Commands ver%f ] \n\n",&ver);
	fscanf(sFile,"launchPropPrompt=%d \n",&launchPropPrompt);
	fscanf(sFile,"launchHTMLPlayer=%d \n",&launchHTMLPlayer);
	fscanf(sFile,"deleteAVIAfterUse=%d \n",&deleteAVIAfterUse);
	
		
	fclose(sFile);


}






LPBITMAPINFOHEADER LoadBitmapFile(CString bitmapFile)
{
	LPBITMAPINFOHEADER alpbi = NULL;
	CPicture picture;
	
	if (picture.Load(bitmapFile)) {			
				
		HBITMAP hbitmap = NULL;
		if (picture.m_IPicture->get_Handle( (unsigned int *) &hbitmap ) == S_OK ) 	
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


int CreateFlashBitmapPlayButton(std::ostringstream &f,int imagewidth, int imageheight, CString subdir,int imageoffset)
{

	LPBITMAPINFOHEADER alpbi = NULL;
	LPBITMAPINFOHEADER alpbi2 = NULL;

	CString bitmapFile,bitmapFile2;
	bitmapFile = GetProgPath() + subdir + "\\playbutton.bmp";
	bitmapFile2 = GetProgPath() + subdir + "\\playbutton2.bmp";
	
	alpbi = LoadBitmapFile(bitmapFile);
	alpbi2 = LoadBitmapFile(bitmapFile2);

	int right;	

	if ((alpbi) && (alpbi2))
	{		
		
		int playButtonDepth = ObjectDepth + 2;
		
		
		int shapeUP_ID, shapeOVER_ID;		

		int BITMAP_X, BITMAP_Y;

		{
			LPBYTE bitmap = makeReverse32(alpbi->biWidth, alpbi->biHeight,4, alpbi);
			
			BITMAP_X = alpbi->biWidth;
			BITMAP_Y = alpbi->biHeight;

			//Shape Up, Over
			int wLineLen = ((BITMAP_X)*32+31)/32 * 4;				
			int dwSize = wLineLen * (BITMAP_Y);			
			int format = 5; //32 bit

			FlashZLibBitmapData zdata((unsigned char *) bitmap,dwSize); //need to takeinto account .... alignment?
			FlashTagDefineBitsLossless2 bll(format,BITMAP_X, BITMAP_Y, zdata);
			f << bll;	

			
			int left = imageoffset + ButtonSpaceX;			
			int top = imageheight+FrameOffsetY+ButtonSpaceY;
			right = left + BITMAP_X;
			

			FlashMatrix m;
			m.SetScale(20,20);			
			m.SetTranslate((left-MatrixOffsetX)*20,(top-MatrixOffsetY)*20);								
			//m.SetTranslate((-left-MatrixOffsetX),(-top-MatrixOffsetY));								
			FlashFillStyleBitmap ffb(bll.GetID(),m); 
			
			
			FlashFillStyleArray ffa;	
			ffa.AddFillStyle(&ffb);			
			
			FlashShapeWithStyle s;						
			FlashShapeRecordChange changerec,changerec2 ;
			changerec.ChangeFillStyle1(1);					
			s.AddRecord(changerec);
			s.AddRecord(FlashShapeRecordChange(left*20,top*20));
			s.AddRecord(FlashShapeRecordStraight(BITMAP_X*20,0*20));
			s.AddRecord(FlashShapeRecordStraight(0*20,BITMAP_Y*20));
			s.AddRecord(FlashShapeRecordStraight(-BITMAP_X*20,0*20));
			s.AddRecord(FlashShapeRecordStraight(0*20,-BITMAP_Y*20));					
			
			s.SetFillStyleArray(ffa);		

			FlashTagDefineShape3 fsws(s);							
			f << fsws;

			shapeUP_ID = fsws.GetID();

		}



		{

			LPBYTE bitmap = makeReverse32(alpbi2->biWidth, alpbi2->biHeight,4, alpbi2);

			BITMAP_X = alpbi2->biWidth;
			BITMAP_Y = alpbi2->biHeight;

			//Shape Up, Over
			int wLineLen = ((BITMAP_X)*32+31)/32 * 4;				
			int dwSize = wLineLen * (BITMAP_Y);			
			int format = 5; //32 bit

			FlashZLibBitmapData zdata((unsigned char *) bitmap,dwSize); //need to takeinto account .... alignment?
			FlashTagDefineBitsLossless2 bll(format,BITMAP_X, BITMAP_Y, zdata);
			f << bll;	
			
			int left = imageoffset + ButtonSpaceX;			
			int top = imageheight+FrameOffsetY+ButtonSpaceY;
			right = left + BITMAP_X;
			
			//int left = 15;
			//int top = 15;

			FlashMatrix m;
			m.SetScale(20,20);			
			m.SetTranslate((left-MatrixOffsetX)*20,(top-MatrixOffsetY)*20);								
			//m.SetTranslate((-left-MatrixOffsetX),(-top-MatrixOffsetY));								
			FlashFillStyleBitmap ffb(bll.GetID(),m); 
			
			FlashFillStyleArray ffa;	
			ffa.AddFillStyle(&ffb);			
			
			FlashShapeWithStyle s;						
			FlashShapeRecordChange changerec,changerec2 ;
			changerec.ChangeFillStyle1(1);					
			s.AddRecord(changerec);
			s.AddRecord(FlashShapeRecordChange(left*20,top*20));
			s.AddRecord(FlashShapeRecordStraight(BITMAP_X*20,0*20));
			s.AddRecord(FlashShapeRecordStraight(0*20,BITMAP_Y*20));
			s.AddRecord(FlashShapeRecordStraight(-BITMAP_X*20,0*20));
			s.AddRecord(FlashShapeRecordStraight(0*20,-BITMAP_Y*20));					
			
			s.SetFillStyleArray(ffa);		

			FlashTagDefineShape3 fsws(s);							
			f << fsws;

			shapeOVER_ID = fsws.GetID();

		}
		
		FlashMatrix m;
		//m.SetScale(20,20);
		
		FlashColorTransform cfxUp , cfxOver , cfxDown ;	
		cfxUp.SetMultRGB(FlashRGB(256,256,256,200));	
		cfxOver.SetMultRGB(FlashRGB(256,256,256,256));		
		cfxDown.SetMultRGB(FlashRGB(256,256,256,256));		
		FlashButtonRecord brecUp(shapeUP_ID , playButtonDepth, FBR_UP, m, cfxUp);
		FlashButtonRecord brecOver(shapeUP_ID, playButtonDepth, FBR_OVER  | FBR_HIT_TEST, m, cfxOver);	
		FlashButtonRecord brecDown(shapeOVER_ID, playButtonDepth, FBR_DOWN , m, cfxDown);
				
		FlashTagDefineButton2 buttonPlay;
		FlashActionPlay playAction;
		buttonPlay.AddButtonRecord(&brecUp);
		buttonPlay.AddButtonRecord(&brecOver);
		buttonPlay.AddButtonRecord(&brecDown);
		
		N_STD::vector<FlashActionRecord *> acrs;
		acrs.push_back(&playAction);		
		  
		buttonPlay.AddActionRecords(acrs,SWFSOURCE_BST_OverUpToOverDown);	
		
		//FlashTagDefineButton buttonPlay;		
		//buttonPlay.AddButtonRecord(&brecUp);
		//buttonPlay.AddButtonRecord(&brecOver);		

		f << buttonPlay;			

		FlashTagPlaceObject2 po(playButtonDepth, buttonPlay.GetID()); 
		f << po;	


	}

	if (alpbi)
		GlobalFreePtr(alpbi);

	if (alpbi2)
		GlobalFreePtr(alpbi2);

	return right;

}


int CreateFlashBitmapPauseButton(std::ostringstream &f,int imagewidth, int imageheight, CString subdir,int imageoffset)
{	
	
	LPBITMAPINFOHEADER alpbi = NULL;
	LPBITMAPINFOHEADER alpbi2 = NULL;

	CString bitmapFile,bitmapFile2;
	bitmapFile = GetProgPath() + subdir + "\\pausebutton.bmp";
	bitmapFile2 = GetProgPath() + subdir + "\\pausebutton2.bmp";
	
	alpbi = LoadBitmapFile(bitmapFile);
	alpbi2 = LoadBitmapFile(bitmapFile2);

	int right;

	if ((alpbi) && (alpbi2))
	{		
		
		int ButtonDepth = ObjectDepth + 2;
		
		
		int buttonWidth = alpbi->biWidth; 
				
		int shapeUP_ID, shapeOVER_ID;		

		int BITMAP_X, BITMAP_Y;

		{
			LPBYTE bitmap = makeReverse32(alpbi->biWidth, alpbi->biHeight,4, alpbi);
			
			BITMAP_X = alpbi->biWidth;
			BITMAP_Y = alpbi->biHeight;

			//Shape Up, Over
			int wLineLen = ((BITMAP_X)*32+31)/32 * 4;				
			int dwSize = wLineLen * (BITMAP_Y);			
			int format = 5; //32 bit

			FlashZLibBitmapData zdata((unsigned char *) bitmap,dwSize); //need to takeinto account .... alignment?
			FlashTagDefineBitsLossless2 bll(format,BITMAP_X, BITMAP_Y, zdata);
			f << bll;	


			int left = imageoffset + ButtonSpaceX;			
			int top = imageheight+FrameOffsetY+ButtonSpaceY;
			right = left + BITMAP_X;
			
			//int left = 15;
			//int top = 15;

			FlashMatrix m;
			m.SetScale(20,20);			
			m.SetTranslate((left-MatrixOffsetX)*20,(top-MatrixOffsetY)*20);								
			//m.SetTranslate((-left-MatrixOffsetX),(-top-MatrixOffsetY));								
			FlashFillStyleBitmap ffb(bll.GetID(),m); 
			
			FlashFillStyleArray ffa;	
			ffa.AddFillStyle(&ffb);			
			
			FlashShapeWithStyle s;						
			FlashShapeRecordChange changerec,changerec2 ;
			changerec.ChangeFillStyle1(1);					
			s.AddRecord(changerec);
			s.AddRecord(FlashShapeRecordChange(left*20,top*20));
			s.AddRecord(FlashShapeRecordStraight(BITMAP_X*20,0*20));
			s.AddRecord(FlashShapeRecordStraight(0*20,BITMAP_Y*20));
			s.AddRecord(FlashShapeRecordStraight(-BITMAP_X*20,0*20));
			s.AddRecord(FlashShapeRecordStraight(0*20,-BITMAP_Y*20));					
			
			s.SetFillStyleArray(ffa);		

			FlashTagDefineShape3 fsws(s);							
			f << fsws;

			shapeUP_ID = fsws.GetID();

		}



		{

			LPBYTE bitmap = makeReverse32(alpbi2->biWidth, alpbi2->biHeight,4, alpbi2);

			BITMAP_X = alpbi2->biWidth;
			BITMAP_Y = alpbi2->biHeight;

			//Shape Up, Over
			int wLineLen = ((BITMAP_X)*32+31)/32 * 4;				
			int dwSize = wLineLen * (BITMAP_Y);			
			int format = 5; //32 bit

			FlashZLibBitmapData zdata((unsigned char *) bitmap,dwSize); //need to takeinto account .... alignment?
			FlashTagDefineBitsLossless2 bll(format,BITMAP_X, BITMAP_Y, zdata);
			f << bll;	
			
			int left = imageoffset + ButtonSpaceX;			
			int top = imageheight+FrameOffsetY+ButtonSpaceY;
			right = left + BITMAP_X;
			

			FlashMatrix m;
			m.SetScale(20,20);			
			m.SetTranslate((left-MatrixOffsetX)*20,(top-MatrixOffsetY)*20);								
			FlashFillStyleBitmap ffb(bll.GetID(),m); 
			
			FlashFillStyleArray ffa;	
			ffa.AddFillStyle(&ffb);			
			
			FlashShapeWithStyle s;						
			FlashShapeRecordChange changerec,changerec2 ;
			changerec.ChangeFillStyle1(1);					
			s.AddRecord(changerec);
			s.AddRecord(FlashShapeRecordChange(left*20,top*20));
			s.AddRecord(FlashShapeRecordStraight(BITMAP_X*20,0*20));
			s.AddRecord(FlashShapeRecordStraight(0*20,BITMAP_Y*20));
			s.AddRecord(FlashShapeRecordStraight(-BITMAP_X*20,0*20));
			s.AddRecord(FlashShapeRecordStraight(0*20,-BITMAP_Y*20));					
			
			s.SetFillStyleArray(ffa);		

			FlashTagDefineShape3 fsws(s);							
			f << fsws;

			shapeOVER_ID = fsws.GetID();

		}
		
		FlashMatrix m;
				
		FlashColorTransform cfxUp , cfxOver , cfxDown ;	
		cfxUp.SetMultRGB(FlashRGB(256,256,256,200));	
		cfxOver.SetMultRGB(FlashRGB(256,256,256,256));		
		cfxDown.SetMultRGB(FlashRGB(256,256,256,256));		
		FlashButtonRecord brecUp(shapeUP_ID , ButtonDepth, FBR_UP, m, cfxUp);
		FlashButtonRecord brecOver(shapeUP_ID, ButtonDepth, FBR_OVER  | FBR_HIT_TEST, m, cfxOver);	
		FlashButtonRecord brecDown(shapeOVER_ID, ButtonDepth, FBR_DOWN , m, cfxDown);

		FlashTagDefineButton2 buttonPause;
		FlashActionStop stopAction;
		buttonPause.AddButtonRecord(&brecUp);
		buttonPause.AddButtonRecord(&brecOver);
		buttonPause.AddButtonRecord(&brecDown);
		
		N_STD::vector<FlashActionRecord *> acrs;
		acrs.push_back(&stopAction);		
				  
		buttonPause.AddActionRecords(acrs,SWFSOURCE_BST_OverUpToOverDown);	
		
		//FlashTagDefineButton buttonPlay;		
		//buttonPlay.AddButtonRecord(&brecUp);
		//buttonPlay.AddButtonRecord(&brecOver);		

		f << buttonPause;			

		FlashTagPlaceObject2 po(ButtonDepth, buttonPause.GetID()); 
		f << po;	


	}

	if (alpbi)
		GlobalFreePtr(alpbi);

	if (alpbi2)
		GlobalFreePtr(alpbi2);

	return right;

}



int CreateFlashBitmapStopButton(std::ostringstream &f,int imagewidth, int imageheight,  CString subdir,int imageoffset)
{	
	
	LPBITMAPINFOHEADER alpbi = NULL;
	LPBITMAPINFOHEADER alpbi2 = NULL;

	CString bitmapFile,bitmapFile2;
	bitmapFile = GetProgPath() + subdir + "\\stopbutton.bmp";
	bitmapFile2 = GetProgPath() + subdir + "\\stopbutton2.bmp";
	
	alpbi = LoadBitmapFile(bitmapFile);
	alpbi2 = LoadBitmapFile(bitmapFile2);

	int right;

	if ((alpbi) && (alpbi2))
	{		
		
		int ButtonDepth = ObjectDepth + 2;
		
		
		int buttonWidth = alpbi->biWidth; 
				
		int shapeUP_ID, shapeOVER_ID;		

		int BITMAP_X, BITMAP_Y;

		{
			LPBYTE bitmap = makeReverse32(alpbi->biWidth, alpbi->biHeight,4, alpbi);
			
			BITMAP_X = alpbi->biWidth;
			BITMAP_Y = alpbi->biHeight;

			//Shape Up, Over
			int wLineLen = ((BITMAP_X)*32+31)/32 * 4;				
			int dwSize = wLineLen * (BITMAP_Y);			
			int format = 5; //32 bit

			FlashZLibBitmapData zdata((unsigned char *) bitmap,dwSize); //need to takeinto account .... alignment?
			FlashTagDefineBitsLossless2 bll(format,BITMAP_X, BITMAP_Y, zdata);
			f << bll;	

			
			int left = imageoffset + ButtonSpaceX;			
			int top = imageheight+FrameOffsetY+ButtonSpaceY;
			right = left + BITMAP_X;
			

			FlashMatrix m;
			m.SetScale(20,20);			
			m.SetTranslate((left-MatrixOffsetX)*20,(top-MatrixOffsetY)*20);								
			//m.SetTranslate((-left-MatrixOffsetX),(-top-MatrixOffsetY));								
			FlashFillStyleBitmap ffb(bll.GetID(),m); 
			
			FlashFillStyleArray ffa;	
			ffa.AddFillStyle(&ffb);			
			
			FlashShapeWithStyle s;						
			FlashShapeRecordChange changerec,changerec2 ;
			changerec.ChangeFillStyle1(1);					
			s.AddRecord(changerec);
			s.AddRecord(FlashShapeRecordChange(left*20,top*20));
			s.AddRecord(FlashShapeRecordStraight(BITMAP_X*20,0*20));
			s.AddRecord(FlashShapeRecordStraight(0*20,BITMAP_Y*20));
			s.AddRecord(FlashShapeRecordStraight(-BITMAP_X*20,0*20));
			s.AddRecord(FlashShapeRecordStraight(0*20,-BITMAP_Y*20));					
			
			s.SetFillStyleArray(ffa);		

			FlashTagDefineShape3 fsws(s);							
			f << fsws;

			shapeUP_ID = fsws.GetID();

		}



		{

			LPBYTE bitmap = makeReverse32(alpbi2->biWidth, alpbi2->biHeight,4, alpbi2);

			BITMAP_X = alpbi2->biWidth;
			BITMAP_Y = alpbi2->biHeight;

			//Shape Up, Over
			int wLineLen = ((BITMAP_X)*32+31)/32 * 4;				
			int dwSize = wLineLen * (BITMAP_Y);			
			int format = 5; //32 bit

			FlashZLibBitmapData zdata((unsigned char *) bitmap,dwSize); //need to takeinto account .... alignment?
			FlashTagDefineBitsLossless2 bll(format,BITMAP_X, BITMAP_Y, zdata);
			f << bll;	
			
			int left = imageoffset + ButtonSpaceX;			
			int top = imageheight+FrameOffsetY+ButtonSpaceY;
			right = left + BITMAP_X;

			FlashMatrix m;
			m.SetScale(20,20);			
			m.SetTranslate((left-MatrixOffsetX)*20,(top-MatrixOffsetY)*20);											
			FlashFillStyleBitmap ffb(bll.GetID(),m); 
			
			FlashFillStyleArray ffa;	
			ffa.AddFillStyle(&ffb);			
			
			FlashShapeWithStyle s;						
			FlashShapeRecordChange changerec,changerec2 ;
			changerec.ChangeFillStyle1(1);					
			s.AddRecord(changerec);
			s.AddRecord(FlashShapeRecordChange(left*20,top*20));
			s.AddRecord(FlashShapeRecordStraight(BITMAP_X*20,0*20));
			s.AddRecord(FlashShapeRecordStraight(0*20,BITMAP_Y*20));
			s.AddRecord(FlashShapeRecordStraight(-BITMAP_X*20,0*20));
			s.AddRecord(FlashShapeRecordStraight(0*20,-BITMAP_Y*20));					
			
			s.SetFillStyleArray(ffa);		

			FlashTagDefineShape3 fsws(s);							
			f << fsws;

			shapeOVER_ID = fsws.GetID();

		}
		
		FlashMatrix m;
				
		FlashColorTransform cfxUp , cfxOver , cfxDown ;	
		cfxUp.SetMultRGB(FlashRGB(256,256,256,200));	
		cfxOver.SetMultRGB(FlashRGB(256,256,256,256));		
		cfxDown.SetMultRGB(FlashRGB(256,256,256,256));		
		FlashButtonRecord brecUp(shapeUP_ID , ButtonDepth, FBR_UP, m, cfxUp);
		FlashButtonRecord brecOver(shapeUP_ID, ButtonDepth, FBR_OVER  | FBR_HIT_TEST, m, cfxOver);	
		FlashButtonRecord brecDown(shapeOVER_ID, ButtonDepth, FBR_DOWN , m, cfxDown);

		FlashTagDefineButton2 buttonStop;
		FlashActionStop stopAction;
		

		//handle baseframe
		int baseframe = 0;
		if (addPreloader)
			baseframe = preloadFrames;		
		//so that we will not get a blank screen when we click stop
		
		FlashActionGotoFrame gotoAction(baseframe);		
		buttonStop.AddButtonRecord(&brecUp);
		buttonStop.AddButtonRecord(&brecOver);
		buttonStop.AddButtonRecord(&brecDown);		
		
		N_STD::vector<FlashActionRecord *> acrs;
		acrs.push_back(&stopAction);		
		acrs.push_back(&gotoAction);		
				  
		buttonStop.AddActionRecords(acrs,SWFSOURCE_BST_OverUpToOverDown);	
		
		//FlashTagDefineButton buttonPlay;		
		//buttonPlay.AddButtonRecord(&brecUp);
		//buttonPlay.AddButtonRecord(&brecOver);		

		f << buttonStop;			

		FlashTagPlaceObject2 po(ButtonDepth, buttonStop.GetID()); 
		f << po;	


	}

	if (alpbi)
		GlobalFreePtr(alpbi);

	if (alpbi2)
		GlobalFreePtr(alpbi2);

	return right;

}

int DrawRightPiece(std::ostringstream &f,int imagewidth, int imageheight,  CString subdir, int imageoffset,int yoffset)
{	
	
	LPBITMAPINFOHEADER alpbi = NULL;


	CString bitmapFile;
	bitmapFile = GetProgPath() + subdir + "\\rightpiece.bmp";
		
	alpbi = LoadBitmapFile(bitmapFile);

	int right;

	if (alpbi)
	{		
		
		int ButtonDepth = ObjectDepth + 2;
		
		
		int buttonWidth = alpbi->biWidth; 
		

		int BITMAP_X, BITMAP_Y;

		{
			LPBYTE bitmap = makeReverse32(alpbi->biWidth, alpbi->biHeight,4, alpbi);
			
			BITMAP_X = alpbi->biWidth;
			BITMAP_Y = alpbi->biHeight;

			//Shape Up, Over
			int wLineLen = ((BITMAP_X)*32+31)/32 * 4;				
			int dwSize = wLineLen * (BITMAP_Y);			
			int format = 5; //32 bit

			FlashZLibBitmapData zdata((unsigned char *) bitmap,dwSize); //need to takeinto account .... alignment?
			FlashTagDefineBitsLossless2 bll(format,BITMAP_X, BITMAP_Y, zdata);
			f << bll;	

			
			int left = imageoffset;			
			int top = imageheight+FrameOffsetY+ButtonSpaceY-yoffset;
			right = left + alpbi->biWidth;
			
			FlashMatrix m;
			m.SetScale(20,20);			
			m.SetTranslate((left-MatrixOffsetX)*20,(top-MatrixOffsetY)*20);		
			FlashFillStyleBitmap ffb(bll.GetID(),m); 
			
			FlashFillStyleArray ffa;	
			ffa.AddFillStyle(&ffb);			
			
			FlashShapeWithStyle rightPiece;						
			FlashShapeRecordChange changerec ;
			changerec.ChangeFillStyle1(1);					
			rightPiece.AddRecord(changerec);
			rightPiece.AddRecord(FlashShapeRecordChange(left*20,top*20));
			rightPiece.AddRecord(FlashShapeRecordStraight(BITMAP_X*20,0*20));
			rightPiece.AddRecord(FlashShapeRecordStraight(0*20,BITMAP_Y*20));
			rightPiece.AddRecord(FlashShapeRecordStraight(-BITMAP_X*20,0*20));
			rightPiece.AddRecord(FlashShapeRecordStraight(0*20,-BITMAP_Y*20));					
			
			rightPiece.SetFillStyleArray(ffa);		

			FlashTagDefineShape3 fsws(rightPiece);							
			f << fsws;
			
			FlashTagPlaceObject2 po(ButtonDepth, fsws.GetID()); 
			f << po;	

		}


	}

	if (alpbi)
		GlobalFreePtr(alpbi);
	
	return right;

}

int DrawLeftPiece(std::ostringstream &f,int imagewidth, int imageheight,  CString subdir, int imageoffset,int yoffset)
{	
	
	LPBITMAPINFOHEADER alpbi = NULL;


	CString bitmapFile;
	bitmapFile = GetProgPath() + subdir + "\\leftpiece.bmp";
		
	alpbi = LoadBitmapFile(bitmapFile);

	int right;

	if (alpbi)
	{		
		
		int ButtonDepth = ObjectDepth + 2;
		
		
		int buttonWidth = alpbi->biWidth; 
		

		int BITMAP_X, BITMAP_Y;

		{
			LPBYTE bitmap = makeReverse32(alpbi->biWidth, alpbi->biHeight,4, alpbi);
			
			BITMAP_X = alpbi->biWidth;
			BITMAP_Y = alpbi->biHeight;

			//Shape Up, Over
			int wLineLen = ((BITMAP_X)*32+31)/32 * 4;				
			int dwSize = wLineLen * (BITMAP_Y);			
			int format = 5; //32 bit

			FlashZLibBitmapData zdata((unsigned char *) bitmap,dwSize); //need to takeinto account .... alignment?
			FlashTagDefineBitsLossless2 bll(format,BITMAP_X, BITMAP_Y, zdata);
			f << bll;	

			
			int left = imageoffset;			
			int top = imageheight+FrameOffsetY+ButtonSpaceY-yoffset;
			right = left + alpbi->biWidth;
			
			FlashMatrix m;
			m.SetScale(20,20);			
			m.SetTranslate((left-MatrixOffsetX)*20,(top-MatrixOffsetY)*20);								
			//m.SetTranslate((-left-MatrixOffsetX),(-top-MatrixOffsetY));								
			FlashFillStyleBitmap ffb(bll.GetID(),m); 
			
			FlashFillStyleArray ffa;	
			ffa.AddFillStyle(&ffb);			
			
			FlashShapeWithStyle leftPiece;						
			FlashShapeRecordChange changerec ;
			changerec.ChangeFillStyle1(1);					
			leftPiece.AddRecord(changerec);
			leftPiece.AddRecord(FlashShapeRecordChange(left*20,top*20));
			leftPiece.AddRecord(FlashShapeRecordStraight(BITMAP_X*20,0*20));
			leftPiece.AddRecord(FlashShapeRecordStraight(0*20,BITMAP_Y*20));
			leftPiece.AddRecord(FlashShapeRecordStraight(-BITMAP_X*20,0*20));
			leftPiece.AddRecord(FlashShapeRecordStraight(0*20,-BITMAP_Y*20));					
			
			leftPiece.SetFillStyleArray(ffa);		

			FlashTagDefineShape3 fsws(leftPiece);							
			f << fsws;
			
			FlashTagPlaceObject2 po(ButtonDepth, fsws.GetID()); 
			f << po;	

		}


	}

	if (alpbi)
		GlobalFreePtr(alpbi);
	
	return right;

}



void gcFlash(std::ostringstream &f)
{

	int max= freeCharacterArray.GetSize();	
	if (max > 0)
	{	

		int valueObjectID;
		for (int i=0;i<max; i++)
		{
			valueObjectID = freeCharacterArray[i];

			//MsgC("valueObjectID = %d",valueObjectID);
			
			FlashTagFreeCharacter  ftfc(valueObjectID);			
			f << ftfc;

		}

		freeCharacterArray.RemoveAll();

	}


}


void CPlayplusView::OnHelpSwfproducerfaq() 
{

	Openlink("http://www.rendersoftware.com/products/camstudio/producerFAQ.htm");
}




//v 2.28
//int sprID = 0;
int CreateProgressBar(std::ostringstream &f, int controlsWidth, int controlsHeight,int FrameOffsetX,int FrameOffsetY,int BITMAP_X, int BITMAP_Y, int additonalOffsetX, int additionalOffsetY)
{
		controlsHeight = 2;

		int buttonRadius = controlsHeight;		
		int barDepth = ObjectDepth + 5;
		int ButtonSpaceY = 5;
		int lineThickness = 3;		
		int downOffset = ProgressOffsetY;

		//Todo : 
		//make the fixed colors of progressBar into variables
		FlashRGB colorBar1(38,133,172);
		FlashRGB colorBar2(82,175,212);		
		
		
		FlashMatrix m;		
		FlashGradientRecord gr;
		gr.AddGradient(0, colorBar1);
		gr.AddGradient(128, colorBar2);
		gr.AddGradient(255, colorBar1);
		
		FlashShapeWithStyle s;			
		int left = 0;
		int top = (BITMAP_Y+FrameOffsetY + downOffset+additionalOffsetY) ;
		
		int widthBar = controlsWidth-additonalOffsetX-additonalOffsetX; //center
		int heightBar = controlsHeight;

		
		FlashFillStyleGradient ffg(m, gr);
		FlashFillStyleArray ffa;	
		ffa.AddFillStyle(&ffg);			
				
		FlashShapeRecordChange changerec,changerec2 ;
		changerec.ChangeFillStyle1(1);							
		s.AddRecord(changerec);
		s.AddRecord(FlashShapeRecordChange(left*20,top*20));
		s.AddRecord(FlashShapeRecordStraight(widthBar*20,0*20));
		s.AddRecord(FlashShapeRecordStraight(0*20,heightBar*20));
		s.AddRecord(FlashShapeRecordStraight(-widthBar*20,0*20));
		s.AddRecord(FlashShapeRecordStraight(0*20,-heightBar*20));	

		s.SetFillStyleArray(ffa);			

		FlashTagDefineShape3 fsws(s);		
		f << fsws;

		FlashTagPlaceObject2 poISP(10, fsws.GetID()); 
		FlashTagSprite fts;
		fts.Add(&poISP);
		f << fts;
		
		
		N_STD::string nameX("Progress");		
		FlashMatrix m2;
		m2.SetTranslate((FrameOffsetX+additonalOffsetX)*20,200*20); //place somewhere invisible ...y supposed to be 0, but set to 200*20
		FlashTagPlaceObject2 po(barDepth, fts.GetID(),m2); 
		po.SetName(nameX);
		f << po;			
				
		return widthBar;

}


void FlashActionGetPropertyVar(std::ostringstream &f,CString SpriteTarget,int index, CString varname )
{

	CString indexstr;
	indexstr.Format("%d",index);
	FlashActionPush param1(0, (char *) LPCTSTR(varname), varname.GetLength()+1);		
	FlashActionPush param2(0, (char *) LPCTSTR(SpriteTarget), SpriteTarget.GetLength()+1 );			
	FlashActionPush param3(0, (char *) LPCTSTR(indexstr), indexstr.GetLength()+1);				
	FlashActionGetProperty fagp;
	FlashActionSetVariable fasv;
	
	FlashTagDoAction ftd;
	ftd.AddAction(&param1);	 
	ftd.AddAction(&param2);	
	ftd.AddAction(&param3);	
	ftd.AddAction(&fagp);
	ftd.AddAction(&fasv);

	f << ftd;

}


void FlashActionSetPropertyFloat(std::ostringstream &f,CString SpriteTarget,int index, CString valuestr )
{
	float fvalue = (float) index;  //property index
	char fstr[5];
	fstr[0] = *(((char *) (&fvalue)));
	fstr[1] = *(((char *) (&fvalue))+1);
	fstr[2] = *(((char *) (&fvalue))+2);
	fstr[3] = *(((char *) (&fvalue))+3);

	FlashActionPush param11(0, (char *) LPCTSTR(SpriteTarget), SpriteTarget.GetLength()+1);	//target
	FlashActionPush param12(1, fstr, 4); //index
	FlashActionPush param13(0, (char *) LPCTSTR(valuestr), valuestr.GetLength()+1);		
	FlashActionSetProperty fasp;
		
	FlashTagDoAction ftd2;
	ftd2.AddAction(&param11);	
	ftd2.AddAction(&param12);	 
	ftd2.AddAction(&param13);	 
	ftd2.AddAction(&fasp);
	
	f << ftd2;


}




void FlashActionSetPropertyFloatVar(std::ostringstream &f,CString SpriteTarget,int index, CString varstr )
{

	float fvalue = (float) index;  //property index
	char fstr[5];
	fstr[0] = *(((char *) (&fvalue)));
	fstr[1] = *(((char *) (&fvalue))+1);
	fstr[2] = *(((char *) (&fvalue))+2);
	fstr[3] = *(((char *) (&fvalue))+3);

	FlashActionPush param11(0, (char *) LPCTSTR(SpriteTarget), SpriteTarget.GetLength()+1);	//target
	FlashActionPush param12(1, fstr, 4); //index
	FlashActionPush param13(0, (char *) LPCTSTR(varstr), varstr.GetLength()+1);		
	FlashActionGetVariable fagv;
	FlashActionSetProperty fasp;
		
	FlashTagDoAction ftd2;
	ftd2.AddAction(&param11);	
	ftd2.AddAction(&param12);	 
	ftd2.AddAction(&param13);	 
	ftd2.AddAction(&fagv);	
	ftd2.AddAction(&fasp);
	
	f << ftd2;


}



void WriteTextOut(std::ostringstream &f, int width, int height, CString Loadstr, CString fontstr, int red,int green, int blue, int pointsize, bool bold, bool italic, bool uLine)
{

	FlashRect bounds;
	UWORD depth = ObjectDepth;	
	FlashFontFactory fff;
	
	//centering
	CSize Extent;
	GetBounds(LPCTSTR(fontstr), LPCTSTR(Loadstr),pointsize, Extent, bold,  italic,  uLine);
	
	//int xpos = (width * 20 - Extent.cx * 20)/2 + FrameOffsetX;	
	//inaccurate Extent ==> so use 1.5 compensate factor
	int xpos = (width * 20 - int(Extent.cx * 1.5 * 20))/2 + FrameOffsetX;	
	int ypos = (height * 20 - (bounds.GetY2() - bounds.GetY1()))/2 + + FrameOffsetY;
	
	fff.WriteText(f,LPCTSTR(fontstr),LPCTSTR(Loadstr), xpos, ypos, FlashRGB(red,green,blue),pointsize,depth,bounds, 0,bold, italic, uLine);			


}



//This preloading works only if addControls is turned on
//because the flash dimension is not large enough!!!!
//percent = 1 becuase 1 is max! (100%)

void Preloader(std::ostringstream &f, int widthBar, int bmWidth, int bmHeight, int progressOffset)
{

	
	char actionScript[1000];	
	char actionScript2[1000];	

	FlashActionGetPropertyVar(f,"",_totalframes, "tframes" );  
	FlashActionGetPropertyVar(f,"",_framesloaded, "floaded" );  

	//is this actionscript stable ?
	sprintf(actionScript,"FullWidth = %d;percent = floaded / tframes; adjustedPercent = percent / %.2f ; currentWidth = adjustedPercent * FullWidth; if (currentWidth > FullWidth) currentWidth = FullWidth;", widthBar, percentLoadedThreshold); 	
	
	ActionCompiler acom(5);	
	acom.Compile(actionScript,f);	

	 	
		
	//is this stable ?
	sprintf(actionScript2,"alphaVal = 150 - counter; if (dir==0) counter=counter+2;  if (dir==1) counter=counter-2; if (counter > 148) dir = 1; if (counter < 2) dir = 0;");
	ActionCompiler acom3(5);	
	acom3.Compile(actionScript2,f);		

	
	FlashActionSetPropertyFloatVar(f,"Progress",_width, "currentWidth" );  	
	FlashActionSetPropertyFloatVar(f,"Loading",_alpha, "alphaVal" );  
	FlashActionSetPropertyFloat(f,"Progress",_Y, "0" );  	
	

	//Preloader Frame
	f << FlashTagShowFrame();
	framecount++;	

	//WriteTextOut(f, bmWidth, bmHeight, "Loading ...", fontname, font_red + 10, font_green + 10, font_blue + 10, font_pointsize, font_bold,  font_italic, font_uLine);
	

	//added open
	sprintf(actionScript,"condLoad = 0;if (percent >= %.2f) condLoad = 1;",percentLoadedThreshold);
	
	ActionCompiler acom2(5);
	acom2.Compile(actionScript,f);

	FlashTagDoAction ftd;

	FlashActionPush paramVar(0,"condLoad" , strlen("condLoad")+1);		
	FlashActionGetVariable fagv;
	FlashActionGotoFrame gotoAction(0); //5 bytes
	FlashActionPlay px;	 //1 byte?	
	FlashActionIf faif(6);  //5+1 = 6 bytes

	ftd.AddAction(&paramVar);	
	ftd.AddAction(&fagv);	
	
	ftd.AddAction(&faif);	//if true, jump 6 bytes
	ftd.AddAction(&gotoAction);	
	ftd.AddAction(&px);	
	
	
	f << ftd;	
	//added close


	int LoadingDepth = ObjectDepth + 6;
	FlashTagRemoveObject2 ro(LoadingDepth);
	f << ro;	
	
	f << FlashTagShowFrame();
	framecount++;	


}



void actionLoadBaseMovie(std::ostringstream &f)
{

	char actionScript[1000];
	CString actionscriptFormat("loadMovie (\"%s\", \"\");");
	CString actionscriptStr;
	actionscriptStr.Format(actionscriptFormat,swfbasename);				
	strcpy(actionScript,LPCTSTR(actionscriptStr));
	actionScript[actionscriptStr.GetLength()]=0; 					

	ActionCompiler acom(5);
	acom.Compile(actionScript,f);

}



void GetBounds(const char *font, CString textstr, int pointsize, CSize& retExtent, bool bold, bool italic, bool uLine)
{

	unsigned int fWeight=400;
    if(bold==true)fWeight=700;
	
	CSize Extent;
	HDC hdcBits = GetDC(NULL);
	HFONT myfont = CreateFont(pointsize,0,0,0,fWeight,italic ? TRUE : FALSE,uLine ? TRUE : FALSE,FALSE,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,PROOF_QUALITY,DEFAULT_PITCH,font);	
	HFONT oldFont = (HFONT) SelectObject(hdcBits,myfont);
	
	GetTextExtentPoint32( hdcBits, LPCTSTR(textstr),textstr.GetLength(), &Extent); 	
	
	if (oldFont)
	SelectObject(hdcBits,oldFont);	
	::ReleaseDC(NULL,hdcBits);
	
	DeleteObject(myfont);

	retExtent = Extent;

}


int DrawLoading(std::ostringstream &f,int imagewidth, int imageheight,  CString subdir, int imageoffset,int yoffset)
{	
	
	LPBITMAPINFOHEADER alpbi = NULL;
	int LoadingDepth = ObjectDepth + 6;

	CString bitmapFile;
	bitmapFile = subdir;
		
	alpbi = LoadBitmapFile(bitmapFile);

	int right;

	if (alpbi)
	{		
		
		int ButtonDepth = ObjectDepth + 2;
		
		
		int buttonWidth = alpbi->biWidth; 
		

		int BITMAP_X, BITMAP_Y;

		{
			LPBYTE bitmap = makeReverse32(alpbi->biWidth, alpbi->biHeight,4, alpbi);
			
			BITMAP_X = alpbi->biWidth;
			BITMAP_Y = alpbi->biHeight;

			//Shape Up, Over
			int wLineLen = ((BITMAP_X)*32+31)/32 * 4;				
			int dwSize = wLineLen * (BITMAP_Y);			
			int format = 5; //32 bit

			FlashZLibBitmapData zdata((unsigned char *) bitmap,dwSize); //need to takeinto account .... alignment?
			FlashTagDefineBitsLossless2 bll(format,BITMAP_X, BITMAP_Y, zdata);
			f << bll;	
			
			int left = (imagewidth - BITMAP_X)/2 + FrameOffsetX;	
			int top =  (imageheight - BITMAP_Y)/2 + FrameOffsetY;
			
			right = left + alpbi->biWidth;
			
			FlashMatrix m;
			m.SetScale(20,20);			
			m.SetTranslate((left-MatrixOffsetX)*20,(top-MatrixOffsetY)*20);								
			//m.SetTranslate((-left-MatrixOffsetX),(-top-MatrixOffsetY));								
			FlashFillStyleBitmap ffb(bll.GetID(),m); 
			
			FlashFillStyleArray ffa;	
			ffa.AddFillStyle(&ffb);			
			
			FlashShapeWithStyle leftPiece;						
			FlashShapeRecordChange changerec ;
			changerec.ChangeFillStyle1(1);					
			leftPiece.AddRecord(changerec);
			leftPiece.AddRecord(FlashShapeRecordChange(left*20,top*20));
			leftPiece.AddRecord(FlashShapeRecordStraight(BITMAP_X*20,0*20));
			leftPiece.AddRecord(FlashShapeRecordStraight(0*20,BITMAP_Y*20));
			leftPiece.AddRecord(FlashShapeRecordStraight(-BITMAP_X*20,0*20));
			leftPiece.AddRecord(FlashShapeRecordStraight(0*20,-BITMAP_Y*20));					
			
			leftPiece.SetFillStyleArray(ffa);		


			FlashTagDefineShape3 fsws(leftPiece);
			f << fsws;
			
			FlashTagPlaceObject2 poISP(10, fsws.GetID()); 
			FlashTagSprite fts;
			fts.Add(&poISP);			
			f << fts;
			
			N_STD::string nameX("Loading");		
			//FlashTagPlaceObject2 po(10, fts.GetID()); 
			FlashTagPlaceObject2 po(LoadingDepth, fts.GetID()); 
			po.SetName(nameX);			
			f << po;	

		}


	}

	if (alpbi)
		GlobalFreePtr(alpbi);
	
	return right;

}


//ver 2.29
//this version some slight instability..somethimes the converted html/swf is not launched
int DrawNodes(std::ostringstream &f,int widthBar,int imagewidth, int imageheight,  CString subdir, int imageoffset,int yoffset, int additionalOffsetX,int additionalOffsetY)
{	

	//int downOffset = 2;
	int downOffset = ProgressOffsetY;

	LPBITMAPINFOHEADER alpbi = NULL;


	CString bitmapFile;
	bitmapFile = GetProgPath() + subdir + "\\loadnode.bmp";
		
	alpbi = LoadBitmapFile(bitmapFile);

	int right;

	if (alpbi)
	{		
		
		int ButtonDepth = ObjectDepth + 3;		
		int buttonWidth = alpbi->biWidth; 
		

		int BITMAP_X, BITMAP_Y;

		{
			LPBYTE bitmap = makeReverse32(alpbi->biWidth, alpbi->biHeight,4, alpbi);
			
			BITMAP_X = alpbi->biWidth;
			BITMAP_Y = alpbi->biHeight;

			//Shape Up, Over
			int wLineLen = ((BITMAP_X)*32+31)/32 * 4;				
			int dwSize = wLineLen * (BITMAP_Y);			
			int format = 5; //32 bit

			FlashZLibBitmapData zdata((unsigned char *) bitmap,dwSize); //need to takeinto account .... alignment?
			FlashTagDefineBitsLossless2 bll(format,BITMAP_X, BITMAP_Y, zdata);
			f << bll;	
			

			int left = FrameOffsetX+additionalOffsetX - BITMAP_X - 1;
			int top = (imageheight+FrameOffsetY + downOffset) - BITMAP_Y/2 +additionalOffsetY ;
			
			right = left + alpbi->biWidth;
			
			FlashMatrix m;
			m.SetScale(20,20);			
			m.SetTranslate((left-MatrixOffsetX)*20,(top-MatrixOffsetY)*20);		
			FlashFillStyleBitmap ffb(bll.GetID(),m); 
			
			FlashFillStyleArray ffa;	
			ffa.AddFillStyle(&ffb);			
			
			FlashShapeWithStyle nodePiece;						
			FlashShapeRecordChange changerec ;
			changerec.ChangeFillStyle1(1);					
			nodePiece.AddRecord(changerec);
			nodePiece.AddRecord(FlashShapeRecordChange(left*20,top*20));
			nodePiece.AddRecord(FlashShapeRecordStraight((BITMAP_X+1)*20,0*20));
			nodePiece.AddRecord(FlashShapeRecordStraight(0*20,(BITMAP_Y+1)*20));
			nodePiece.AddRecord(FlashShapeRecordStraight(-(BITMAP_X+1)*20,0*20));
			nodePiece.AddRecord(FlashShapeRecordStraight(0*20,-(BITMAP_Y+1)*20));					
			
			nodePiece.SetFillStyleArray(ffa);		


			FlashTagDefineShape3 fsws(nodePiece);
			f << fsws;
						
			FlashTagPlaceObject2 po(ButtonDepth, fsws.GetID()); 			
			f << po;	

			FlashMatrix m2;
			m2.SetTranslate((widthBar+1)*20,0);
			FlashTagPlaceObject2 po2(ButtonDepth, fsws.GetID(),m2); 			
			f << po2;	
			

		}


	}

	if (alpbi)
		GlobalFreePtr(alpbi);
	
	return right;

}


