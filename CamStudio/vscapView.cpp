// RenderSoft CamStudio
//
// Copyright 2001 - 2003 RenderSoft Software & Web Publishing
// 
//
// vscapView.cpp : implementation of the CVscapView class
//


#include "stdafx.h"
#include "vscap.h"

#include "vscapDoc.h"
#include "vscapView.h"
#include "hook/hook.h"
#include "hookkey/hookkey.h"

#include <windowsx.h>
#include <memory.h>
#include <mmsystem.h>
#include <vfw.h>
#include <setjmp.h>

#include "VideoOptions.h"
#include "CursorOptionsDlg.h"
#include "AutopanSpeed.h"
#include "FixedRegion.h"
#include "FlashingWnd.h"
#include "AudioFormat.h"
#include "Keyshortcuts.h"
#include "TroubleShoot.h"

#include "MainFrm.h"

#include <stdio.h>
#include "fister/soundfile.h"

#include "FolderDialog.h"


//#include "htmlhelp.h"
#include "AutoSearchDialog.h"
//#include "AudioSpeakers.h"

//ver 1.8
#include "ListManager.h"
#include "ScreenAnnotations.h"
#include "VideoWnd.h"
#include "SyncDialog.h"
#include "muldiv32.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



//////////////////////////////////////
//Function prototypes
//////////////////////////////////////

//Region Display  Functions
#define SWAP(x,y)   ((x)^=(y)^=(x)^=(y))
void DrawSelect(HDC hdc, BOOL fDraw, LPRECT lprClip);
void DrawFlashingRect(BOOL bDraw , int mode);
BOOL isRectEqual(RECT a, RECT b);
void WINAPI NormalizeRect(LPRECT prc);
void FixRectSizePos(LPRECT prc,int maxxScreen, int maxyScreen);
RECT FrameWindow(HWND hWnd);
void WINAPI NormalizeRect(LPRECT prc);
void DrawClientArea(HWND hwnd, HDC hdc);
long rounddouble(double dbl);
void SetTitleBar(CString title); // Ver 1.1

//Region Select  Functions
long WINAPI MouseCaptureWndProc(HWND hWnd, UINT wMessage, WPARAM wParam, LPARAM lParam);
int InitDrawShiftWindow();
int InitSelectRegionWindow();
int CreateShiftWindow();
int DestroyShiftWindow();


//Mouse Capture functions 
HCURSOR FetchCursorHandle();
HCURSOR hSavedCursor = NULL;


//AVI functions  and #defines
#define AVIIF_KEYFRAME	0x00000010L // this frame is a key frame.
#define BUFSIZE 260
#define LPLPBI	LPBITMAPINFOHEADER *
#define N_FRAMES	50
#define TEXT_HEIGHT	20

HANDLE Bitmap2Dib(HBITMAP, UINT);
int RecordVideo(int top,int left,int width,int height,int numframes,const char *szFileName);
UINT RecordAVIThread(LPVOID pParam);

//Use these 2 functions to create frames and free frames
LPBITMAPINFOHEADER captureScreenFrame(int left,int top,int width, int height,int tempDisableRect);
void FreeFrame(LPBITMAPINFOHEADER) ;


//Misc Functions
CString GetTempPath();
CString GetProgPath();
void InsertHighLight(HDC hdc,int xoffset, int yoffset);


//////////////////
//State variables
///////////////////

//Vars used for selecting fixed /variableregion
RECT   rcOffset;
RECT   rcClip; 
RECT   rcUse; 
RECT   old_rcClip;
BOOL   bCapturing=FALSE;
POINT  ptOrigin;

int maxxScreen;
int maxyScreen;
RECT rc;
HWND hMouseCaptureWnd;
HWND hWnd_FixedRegion;


HBITMAP savedBitmap = NULL;
HBITMAP hLogoBM = NULL;
CFlashingWnd* pFrame = NULL;


//Misc Vars
int recordstate=0;
int recordpaused=0;
UINT interruptkey = 0;
int tdata=0;
DWORD initialtime=0;
int initcapture = 0;
int irsmallcount=0;

// Messaging
HWND hWndGlobal = NULL;
static UINT WM_USER_RECORDINTERRUPTED = ::RegisterWindowMessage(WM_USER_RECORDINTERRUPTED_MSG);
static UINT WM_USER_SAVECURSOR = ::RegisterWindowMessage(WM_USER_SAVECURSOR_MSG);
static UINT WM_USER_GENERIC = ::RegisterWindowMessage(WM_USER_GENERIC_MSG);
static UINT WM_USER_RECORDSTART = ::RegisterWindowMessage(WM_USER_RECORDSTART_MSG);
static UINT WM_USER_KEYSTART = ::RegisterWindowMessage(WM_USER_KEYSTART_MSG);

/////////////////////////////////////////////////////////
//Variables/Options requiring interface
/////////////////////////////////////////////////////////
int bits = 24; 
int flashingRect=1;
int launchPlayer=3;
int minimizeOnStart=0;
int MouseCaptureMode = 0;
int DefineMode = 0; //set only in FixedRegion.cpp
int capturewidth=320;
int captureheight=240;


//Video Options and Compressions
//int timelapse=40;
//int frames_per_second = 25;
//int keyFramesEvery = 25;

int timelapse=5;
int frames_per_second = 200;
int keyFramesEvery = 200;

int compquality = 7000;
DWORD compfccHandler = 0;
ICINFO * compressor_info = NULL;
int num_compressor =0;
int selected_compressor = -1;

//Ver 1.2
//Video Compress Parameters
LPVOID pVideoCompressParams = NULL; 
DWORD CompressorStateIsFor = 0;
DWORD CompressorStateSize = 0;

void FreeVideoCompressParams();
BOOL AllocVideoCompressParams(DWORD paramsSize);
void GetVideoCompressState (HIC hic, DWORD fccHandler);
void SetVideoCompressState (HIC hic , DWORD fccHandler);

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
HCURSOR g_loadcursor = NULL;
HCURSOR g_customcursor = NULL;
int g_customsel = 0;
int g_recordcursor=1; 
int g_cursortype=0; 
int g_highlightcursor=0;
int g_highlightsize=64;
int g_highlightshape=0;
COLORREF g_highlightcolor = RGB(255,255,125);


//Path to temporary avi file
CString tempfilepath;


//Files Directory
CString savedir("");
CString cursordir("");

//Autopan
int autopan=0;
int maxpan = 20;
RECT panrect_current;
RECT panrect_dest;


// Ver 1.1
// ===========================================================================
// Audio Functions and Variables
// ===========================================================================
// The program records video and sound separately, into 2 files
// ~temp.avi and ~temp.wav, before merging these 2 file into a single avi file
// using the Merge_Video_And_Sound_File function
// ===========================================================================

//Path to temporary wav file
CString tempaudiopath;
int recordaudio=0;

//Audio Recording Variables
UINT AudioDeviceID = WAVE_MAPPER;

HWAVEIN m_hRecord;
WAVEFORMATEX m_Format;
DWORD m_ThreadID;
int m_QueuedBuffers=0;
int	 m_BufferSize = 1000;	// number of samples
CSoundFile *m_pFile = NULL;

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

BOOL StartAudioRecording(WAVEFORMATEX* format);
void waveInErrorMsg(MMRESULT result, const char *);
int AddInputBufferToQueue();
void SetBufferSize(int NumberOfSamples);
void CALLBACK OnMM_WIM_DATA(UINT parm1, LONG parm2);
void DataFromSoundIn(CBuffer* buffer) ;
void StopAudioRecording();
BOOL InitAudioRecording();
void ClearAudioFile();
void GetTempWavePath();

void BuildRecordingFormat();
void SuggestCompressFormat();
void SuggestRecordingFormat();
void AllocCompressFormat();


// Video/Audio Merging Functions and Definitions
#define NUMSTREAMS   2
int Merge_Video_And_Sound_File(CString input_video_path, CString input_audio_path, CString output_avi_path, BOOL recompress_audio, LPWAVEFORMATEX audio_recompress_format, DWORD  audio_format_size, BOOL bInterleave, int interleave_factor,int interleave_unit); 	   
BOOL WinYield(void);
BOOL CALLBACK SaveCallback(int iProgress);


// ===========================================================================
//ver 1.2 
// ===========================================================================
//Key short-cuts variables
// ===========================================================================
UINT keyRecordStart = VK_F8;
UINT keyRecordEnd = VK_F9;
UINT keyRecordCancel = VK_F10;

//state vars
BOOL AllowNewRecordStartKey=TRUE;
int doneOnce=0;
int savesettings=1;

extern int viewtype;

//Enhanced video options
int g_autoadjust=1;
//int g_valueadjust=32;
int g_valueadjust=1;

//Functions that select audio options based on settings read
void AttemptRecordingFormat();
void AttemptCompressFormat();

//Link to customized icon info
extern DWORD icon_info[];

//Cursor Path, used for copying cursor file
CString g_cursorFilePath;

//Tray Icon
#define WM_TRAY_ICON_NOTIFY_MESSAGE (WM_USER + 50)
NOTIFYICONDATA	IconData;
BOOL bTrayIconVisible				= FALSE;
BOOL DefaultMenuItem				= 0;
BOOL bMinimizeToTray				= TRUE;
UINT	nDefaultMenuItem;
CMenu TrayMenu;

void initTrayIconData();
void finishTrayIconData();
void TraySetIcon(HICON hIcon);
void TraySetIcon(UINT nResourceID);
void TraySetIcon(LPCTSTR lpszResourceName);
void TraySetToolTip(LPCTSTR lpszToolTip);

void TraySetMinimizeToTray(BOOL bMinimizeToTray = TRUE);
BOOL TraySetMenu(UINT nResourceID,UINT nDefaultPos=0);	
BOOL TraySetMenu(HMENU hMenu,UINT nDefaultPos=0);	
BOOL TraySetMenu(LPCTSTR lpszMenuName,UINT nDefaultPos=0);	
BOOL TrayUpdate();
BOOL TrayShow();
BOOL TrayHide();	
void OnTrayLButtonDown(CPoint pt);
void OnTrayLButtonDblClk(CPoint pt);	
void OnTrayRButtonDown(CPoint pt);
void OnTrayRButtonDblClk(CPoint pt);
void OnTrayMouseMove(CPoint pt);
	

//ver 1.3
int threadPriority = THREAD_PRIORITY_NORMAL;
CString GetCodecDescription(long fccHandler);
void AutoSetRate(int val,int& framerate,int& delayms);

#if !defined(WAVE_FORMAT_MPEGLAYER3)
#define  WAVE_FORMAT_MPEGLAYER3 0x0055  
#endif


//ver 1.5
int captureleft=100;
int capturetop=100;
int fixedcapture=0;



//version 1.6
#define USE_WINDOWS_TEMP_DIR 0
#define USE_INSTALLED_DIR 1
#define USE_USER_SPECIFIED_DIR 2

int tempPath_Access  = USE_WINDOWS_TEMP_DIR;
CString specifieddir;

int captureTrans=1;
int versionOp = 0;


//version 1.6


extern int NumberOfMixerDevices;
extern int SelectedMixer;

extern int feedback_line;
extern int feedback_lineInfo;

extern void FreeWaveoutResouces();

extern BOOL useWavein(BOOL ,int );
extern BOOL useWaveout(BOOL ,int );
extern BOOL WaveoutUninitialize();

extern BOOL initialSaveMMMode();
extern BOOL finalRestoreMMMode();
extern BOOL onLoadSettings(int recordaudio); 


MCI_OPEN_PARMS mop;
MCI_SAVE_PARMS msp;
PSTR strFile;

WAVEFORMATEX m_FormatSpeaker;
DWORD waveinselected_Speaker = WAVE_FORMAT_4S16;
int audio_bits_per_sample_Speaker = 16;
int audio_num_channels_Speaker = 2;
int audio_samples_per_seconds_Speaker = 44100 ;

void BuildSpeakerRecordingFormat();
void SuggestSpeakerRecordingFormat(int i);
void mciRecordOpen();
void mciRecordStart();
void mciRecordStop(CString strFile);
void mciRecordClose();
void mciSetWaveFormat();  //add before mcirecord 

CAutoSearchDialog asd;
int asdCreated = FALSE;



int TroubleShootVal=0;
int performAutoSearch=1;

int MessageOutINT(HWND hWnd,long strMsg, long strTitle, UINT mbstatus,long val);
int MessageOutINT2(HWND hWnd,long strMsg, long strTitle, UINT mbstatus,long val1,long val2);
int MessageOut(HWND hWnd,long strMsg, long strTitle, UINT mbstatus);


//ver 1.8
//Region Movement
extern CRect newRect;
extern int newRegionUsed;
extern int readingRegion;
extern int writingRegion;

extern int settingRegion;
extern int capturingRegion;

int supportMouseDrag = 1;

int GetOperatingSystem(); 

CScreenAnnotations sadlg;
int sadlgCreated = 0; 
CTransparentWnd* transWnd;
CMenu contextmenu;

extern CListManager gList;

void mciRecordPause(CString strFile);
void mciRecordResume(CString strFile);
int isMciRecordOpen= 0;
int alreadyMCIPause=0;


//ver 1.8 key shortcuts
UINT keyRecordStartCtrl = 0;
UINT keyRecordEndCtrl = 0;
UINT keyRecordCancelCtrl = 0;

UINT keyRecordStartAlt = 0;
UINT keyRecordEndAlt = 0;
UINT keyRecordCancelAlt = 0;

UINT keyRecordStartShift = 0;
UINT keyRecordEndShift = 0;
UINT keyRecordCancelShift = 0;

UINT keyNext = VK_F11; 
UINT keyPrev = VK_F12;
UINT keyShowLayout = 100000; //none

UINT keyNextCtrl = 1;
UINT keyPrevCtrl = 1;
UINT keyShowLayoutCtrl = 0;

UINT keyNextAlt = 0;
UINT keyPrevAlt = 0;
UINT keyShowLayoutAlt = 0;

UINT keyNextShift = 0;
UINT keyPrevShift = 0;
UINT keyShowLayoutShift = 0;

int UnSetHotKeys();
int SetHotKeys(int succ[]);
int SetAdjustHotKeys();


//ver 1.8
extern int useMCI;
extern int g_refreshRate;

extern int shapeNameInt;
extern CString shapeName;
extern int layoutNameInt;
extern CString g_layoutName;

int vanWndCreated = 0;
CVideoWnd vanWnd;

int keySCOpened = 0;

int audioTimeInitiated = 0;
LONG audioTime(void);
int sdwSamplesPerSec = 22050;
int sdwBytesPerSec = 44100;

int timeshift = 100;
int frameshift = 0;
int shiftType = 0; // 0 : no shift, 1 : delayAudio, 2: delayVideo

int currentLayout=0;



//ver 2.26 Vscap Interface
#define ModeAVI 0
#define ModeFlash 1
int RecordingMode = 0;
void ErrMsg(char frmt[], ...);
void SaveProducerCommand(); 

int launchPropPrompt = 0;
int launchHTMLPlayer = 1;
int deleteAVIAfterUse = 1;

int autonaming = 0;
int restrictVideoCodecs = 0;
//int base_nid = 0; //for keyboard shotcuts


//LPBITMAPINFO GetTextBitmap(CDC *thisDC, CRect* caprect,int factor,CRect* drawtextRect, LOGFONT* drawfont, CString textstr, CPen* pPen, CBrush * pBrush, COLORREF textcolor, int horzalign); 
//HBITMAP DrawResampleRGB(CDC *thisDC, CRect* caprect,int factor, LPBITMAPINFOHEADER expanded_bmi, int xmove, int ymove);
//extern HANDLE  AllocMakeDib( int reduced_width, int reduced_height, UINT bits );


//ver 2.4
#include "PresetTime.h"


int presettime = 60;
int recordpreset = 0;

#ifndef CAPTUREBLT
	#define CAPTUREBLT (DWORD)0x40000000 
#endif

/////////////////////////////////////////////////////////////////////////////
// CVscapView

IMPLEMENT_DYNCREATE(CVscapView, CView)

BEGIN_MESSAGE_MAP(CVscapView, CView)
	//{{AFX_MSG_MAP(CVscapView)
	ON_COMMAND(ID_REGION_RUBBER, OnRegionRubber)
	ON_COMMAND(ID_REGION_PANREGION, OnRegionPanregion)
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_COMMAND(ID_RECORD, OnRecord)
	ON_COMMAND(ID_STOP, OnStop)
	ON_UPDATE_COMMAND_UI(ID_REGION_PANREGION, OnUpdateRegionPanregion)
	ON_UPDATE_COMMAND_UI(ID_REGION_RUBBER, OnUpdateRegionRubber)
	ON_COMMAND(ID_FILE_VIDEOOPTIONS, OnFileVideooptions)
	ON_COMMAND(ID_OPTIONS_CURSOROPTIONS, OnOptionsCursoroptions)
	ON_COMMAND(ID_OPTIONS_AUTOPAN, OnOptionsAutopan)
	ON_COMMAND(ID_OPTIONS_ATUOPANSPEED, OnOptionsAtuopanspeed)
	ON_UPDATE_COMMAND_UI(ID_OPTIONS_AUTOPAN, OnUpdateOptionsAutopan)
	ON_UPDATE_COMMAND_UI(ID_RECORD, OnUpdateRecord)
	ON_COMMAND(ID_REGION_FULLSCREEN, OnRegionFullscreen)
	ON_UPDATE_COMMAND_UI(ID_REGION_FULLSCREEN, OnUpdateRegionFullscreen)
	ON_COMMAND(ID_OPTIONS_MINIMIZEONSTART, OnOptionsMinimizeonstart)
	ON_UPDATE_COMMAND_UI(ID_OPTIONS_MINIMIZEONSTART, OnUpdateOptionsMinimizeonstart)
	ON_COMMAND(ID_OPTIONS_HIDEFLASHING, OnOptionsHideflashing)
	ON_UPDATE_COMMAND_UI(ID_OPTIONS_HIDEFLASHING, OnUpdateOptionsHideflashing)
	ON_COMMAND(ID_OPTIONS_PROGRAMOPTIONS_PLAYAVI, OnOptionsProgramoptionsPlayavi)
	ON_UPDATE_COMMAND_UI(ID_OPTIONS_PROGRAMOPTIONS_PLAYAVI, OnUpdateOptionsProgramoptionsPlayavi)
	ON_COMMAND(ID_HELP_WEBSITE, OnHelpWebsite)
	ON_COMMAND(ID_HELP_HELP, OnHelpHelp)
	ON_COMMAND(ID_PAUSE, OnPause)
	ON_UPDATE_COMMAND_UI(ID_PAUSE, OnUpdatePause)
	ON_UPDATE_COMMAND_UI(ID_STOP, OnUpdateStop)
	ON_COMMAND(ID_OPTIONS_RECORDAUDIO, OnOptionsRecordaudio)
	ON_UPDATE_COMMAND_UI(ID_OPTIONS_RECORDAUDIO, OnUpdateOptionsRecordaudio)
	ON_COMMAND(ID_OPTIONS_AUDIOFORMAT, OnOptionsAudioformat)
	ON_COMMAND(ID_HELP_FAQ, OnHelpFaq)
	ON_COMMAND(ID_OPTIONS_KEYBOARDSHORTCUTS, OnOptionsKeyboardshortcuts)
	ON_COMMAND(ID_OPTIONS_PROGRAMOPTIONS_SAVESETTINGSONEXIT, OnOptionsProgramoptionsSavesettingsonexit)
	ON_UPDATE_COMMAND_UI(ID_OPTIONS_PROGRAMOPTIONS_SAVESETTINGSONEXIT, OnUpdateOptionsProgramoptionsSavesettingsonexit)
	ON_COMMAND(ID_OPTIONS_RECORDINGTHREADPRIORITY_NORMAL, OnOptionsRecordingthreadpriorityNormal)
	ON_COMMAND(ID_OPTIONS_RECORDINGTHREADPRIORITY_HIGHEST, OnOptionsRecordingthreadpriorityHighest)
	ON_COMMAND(ID_OPTIONS_RECORDINGTHREADPRIORITY_ABOVENORMAL, OnOptionsRecordingthreadpriorityAbovenormal)
	ON_COMMAND(ID_OPTIONS_RECORDINGTHREADPRIORITY_TIMECRITICAL, OnOptionsRecordingthreadpriorityTimecritical)
	ON_UPDATE_COMMAND_UI(ID_OPTIONS_RECORDINGTHREADPRIORITY_TIMECRITICAL, OnUpdateOptionsRecordingthreadpriorityTimecritical)
	ON_UPDATE_COMMAND_UI(ID_OPTIONS_RECORDINGTHREADPRIORITY_NORMAL, OnUpdateOptionsRecordingthreadpriorityNormal)
	ON_UPDATE_COMMAND_UI(ID_OPTIONS_RECORDINGTHREADPRIORITY_HIGHEST, OnUpdateOptionsRecordingthreadpriorityHighest)
	ON_UPDATE_COMMAND_UI(ID_OPTIONS_RECORDINGTHREADPRIORITY_ABOVENORMAL, OnUpdateOptionsRecordingthreadpriorityAbovenormal)
	ON_COMMAND(ID_OPTIONS_CAPTURETRANS, OnOptionsCapturetrans)
	ON_UPDATE_COMMAND_UI(ID_OPTIONS_CAPTURETRANS, OnUpdateOptionsCapturetrans)
	ON_COMMAND(ID_OPTIONS_TEMPDIR_WINDOWS, OnOptionsTempdirWindows)
	ON_UPDATE_COMMAND_UI(ID_OPTIONS_TEMPDIR_WINDOWS, OnUpdateOptionsTempdirWindows)
	ON_COMMAND(ID_OPTIONS_TEMPDIR_INSTALLED, OnOptionsTempdirInstalled)
	ON_UPDATE_COMMAND_UI(ID_OPTIONS_TEMPDIR_INSTALLED, OnUpdateOptionsTempdirInstalled)
	ON_COMMAND(ID_OPTIONS_TEMPDIR_USER, OnOptionsTempdirUser)
	ON_UPDATE_COMMAND_UI(ID_OPTIONS_TEMPDIR_USER, OnUpdateOptionsTempdirUser)
	ON_COMMAND(ID_OPTIONS_RECORDAUDIO_DONOTRECORDAUDIO, OnOptionsRecordaudioDonotrecordaudio)
	ON_UPDATE_COMMAND_UI(ID_OPTIONS_RECORDAUDIO_DONOTRECORDAUDIO, OnUpdateOptionsRecordaudioDonotrecordaudio)
	ON_COMMAND(ID_OPTIONS_RECORDAUDIO_RECORDFROMSPEAKERS, OnOptionsRecordaudioRecordfromspeakers)
	ON_UPDATE_COMMAND_UI(ID_OPTIONS_RECORDAUDIO_RECORDFROMSPEAKERS, OnUpdateOptionsRecordaudioRecordfromspeakers)
	ON_COMMAND(ID_OPTIONS_RECORDAUDIOMICROPHONE, OnOptionsRecordaudiomicrophone)
	ON_UPDATE_COMMAND_UI(ID_OPTIONS_RECORDAUDIOMICROPHONE, OnUpdateOptionsRecordaudiomicrophone)	
	ON_COMMAND(ID_OPTIONS_PROGRAMOPTIONS_TROUBLESHOOT, OnOptionsProgramoptionsTroubleshoot)
	ON_COMMAND(ID_OPTIONS_PROGRAMOPTIONS_CAMSTUDIOPLAY, OnOptionsProgramoptionsCamstudioplay)
	ON_UPDATE_COMMAND_UI(ID_OPTIONS_PROGRAMOPTIONS_CAMSTUDIOPLAY, OnUpdateOptionsProgramoptionsCamstudioplay)
	ON_COMMAND(ID_OPTIONS_PROGRAMOPTIONS_DEFAULTPLAY, OnOptionsProgramoptionsDefaultplay)
	ON_UPDATE_COMMAND_UI(ID_OPTIONS_PROGRAMOPTIONS_DEFAULTPLAY, OnUpdateOptionsProgramoptionsDefaultplay)
	ON_COMMAND(ID_OPTIONS_PROGRAMOPTIONS_NOPLAY, OnOptionsProgramoptionsNoplay)
	ON_UPDATE_COMMAND_UI(ID_OPTIONS_PROGRAMOPTIONS_NOPLAY, OnUpdateOptionsProgramoptionsNoplay)
	ON_COMMAND(ID_HELP_DONATIONS, OnHelpDonations)
	ON_COMMAND(ID_OPTIONS_PROGRAMOPTIONS_PLAYAVIFILEWHENRECORDINGSTOPS_USECAMSTUDIOPLAYER20, OnOptionsUsePlayer20)
	ON_UPDATE_COMMAND_UI(ID_OPTIONS_PROGRAMOPTIONS_PLAYAVIFILEWHENRECORDINGSTOPS_USECAMSTUDIOPLAYER20, OnUpdateUsePlayer20)
	ON_COMMAND(ID_VIEW_SCREENANNOTATIONS, OnViewScreenannotations)
	ON_UPDATE_COMMAND_UI(ID_VIEW_SCREENANNOTATIONS, OnUpdateViewScreenannotations)
	ON_COMMAND(ID_VIEW_VIDEOANNOTATIONS, OnViewVideoannotations)
	ON_COMMAND(ID_OPTIONS_AUDIOOPTIONS_AUDIOVIDEOSYNCHRONIZATION, OnOptionsSynchronization)
	ON_WM_SETFOCUS()
	ON_COMMAND(ID_TOOLS_SWFPRODUCER, OnToolsSwfproducer)
	ON_COMMAND(ID_OPTIONS_SWF_LAUNCHHTML, OnOptionsSwfLaunchhtml)
	ON_COMMAND(ID_OPTIONS_SWF_DELETEAVIFILE, OnOptionsSwfDeleteavifile)
	ON_COMMAND(ID_OPTIONS_SWF_DISPLAYPARAMETERS, OnOptionsSwfDisplayparameters)
	ON_UPDATE_COMMAND_UI(ID_OPTIONS_SWF_LAUNCHHTML, OnUpdateOptionsSwfLaunchhtml)
	ON_UPDATE_COMMAND_UI(ID_OPTIONS_SWF_DISPLAYPARAMETERS, OnUpdateOptionsSwfDisplayparameters)
	ON_UPDATE_COMMAND_UI(ID_OPTIONS_SWF_DELETEAVIFILE, OnUpdateOptionsSwfDeleteavifile)
	ON_COMMAND(ID_AVISWF, OnAviswf)
	ON_WM_ERASEBKGND()
	ON_COMMAND(ID_OPTIONS_NAMING_AUTODATE, OnOptionsNamingAutodate)
	ON_UPDATE_COMMAND_UI(ID_OPTIONS_NAMING_AUTODATE, OnUpdateOptionsNamingAutodate)
	ON_COMMAND(ID_OPTIONS_NAMING_ASK, OnOptionsNamingAsk)
	ON_UPDATE_COMMAND_UI(ID_OPTIONS_NAMING_ASK, OnUpdateOptionsNamingAsk)
	ON_COMMAND(ID_OPTIONS_PROGRAMOPTIONS_PRESETTIME, OnOptionsProgramoptionsPresettime)
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
	ON_REGISTERED_MESSAGE (WM_USER_RECORDSTART, OnRecordStart)
	ON_REGISTERED_MESSAGE (WM_USER_RECORDINTERRUPTED, OnRecordInterrupted)
	ON_REGISTERED_MESSAGE (WM_USER_SAVECURSOR, OnSaveCursor)	
	ON_REGISTERED_MESSAGE (WM_USER_GENERIC, OnUserGeneric)	
	ON_REGISTERED_MESSAGE (WM_USER_KEYSTART, OnKeyStart)
	ON_MESSAGE(MM_WIM_DATA, OnMM_WIM_DATA)
	ON_MESSAGE(WM_TRAY_ICON_NOTIFY_MESSAGE,OnTrayNotify)	
	ON_MESSAGE(WM_HOTKEY, OnHotKey)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CVscapView construction/destruction

CVscapView::CVscapView()
{
	// TODO: add construction code here

}

CVscapView::~CVscapView()
{
}

BOOL CVscapView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CVscapView drawing

void CVscapView::OnDraw(CDC* pDC)
{
	CVscapDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	// TODO: add draw code for native data here
}

/////////////////////////////////////////////////////////////////////////////
// CVscapView printing

BOOL CVscapView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CVscapView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CVscapView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// CVscapView diagnostics

#ifdef _DEBUG
void CVscapView::AssertValid() const
{
	CView::AssertValid();
}

void CVscapView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CVscapDoc* CVscapView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CVscapDoc)));
	return (CVscapDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CVscapView message handlers


void CVscapView::OnRegionRubber() 
{	
	MouseCaptureMode=1;	
}

void CVscapView::OnRegionPanregion() 
{

	DefineMode = 0;

	CFixedRegion  cfrdlg;
	cfrdlg.DoModal();
	
	MouseCaptureMode=0;
	DefineMode = 0;
	
}

void CVscapView::OnRegionFullscreen() 
{
	// TODO: Add your command handler code here
	MouseCaptureMode=2;
	
}


void CVscapView::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	if (!doneOnce) {
	
		((CMainFrame*) AfxGetMainWnd())->UpdateViewtype(); 
		doneOnce=1;

	}
	
	DrawClientArea(m_hWnd, dc.m_hDC);	



	/*
	//ver 2.26
	//Draw Message msgRecMode
	if (recordstate==0) {		
		
		CString msgRecMode;		
		CString msgRecAVI;
		CString msgRecFlash;
		
		msgRecAVI.LoadString(IDS_RECAVI); 		
		msgRecFlash.LoadString(IDS_RECSWF);

		if (RecordingMode == ModeAVI)
		{
			msgRecMode = msgRecAVI;			
		}
		else
		{
			msgRecMode = msgRecFlash;

		}		
		
		
		int xoffset =12 ;
		int yoffset =12 ;
		RECT tmodeRect;
		SIZE Extent;

		RECT rect;
		::GetClientRect(this->m_hWnd,&rect );

		//GetTextExtentPoint32( dc.m_hDC, LPCTSTR(msgRecMode), msgRecMode.GetLength(), &Extent); 			
		
		Extent.cx = 140;
		Extent.cy = 32;
		
		CRect ExtentRect;
		ExtentRect.left=0;
		ExtentRect.right=Extent.cx;
		ExtentRect.top=0;
		ExtentRect.bottom=Extent.cy;
		
		int xmove = (rect.right - rect.left + 1) - (Extent.cx) - xoffset;
		//int ymove = (rect.bottom - rect.top + 1) - (Extent.cy) - yoffset;
		int ymove = yoffset;
		//ErrMsg("xmove %d",xmove);
		tmodeRect.left= xmove;
		tmodeRect.right = tmodeRect.left + Extent.cx ;
		tmodeRect.top = ymove  ;
		tmodeRect.bottom = tmodeRect.top + Extent.cy  ;
		//dc.Rectangle( tmodeRect.left,tmodeRect.top,tmodeRect.right,tmodeRect.bottom);
		
		//::Rectangle(hdc, tmodeRect.left - 3,tmodeRect.top - 3,tmodeRect.right + 3,tmodeRect.bottom + 3);
		//::RoundRect(hdc, tmodeRect.left - 3,tmodeRect.top - 3,tmodeRect.right + 3,tmodeRect.bottom + 3,5,5);
		//TextOut(hdc,tmodeRect.left,tmodeRect.top, LPCTSTR(msgRecMode), msgRecMode.GetLength());
		
		LOGFONT textfont;
		ZeroMemory(&textfont, sizeof(LOGFONT));
		textfont.lfHeight = 11;
		textfont.lfWidth = 7;	
		strcpy(textfont.lfFaceName,"Arial");
		
		LPBITMAPINFO pbmiText = GetTextBitmap(&dc, &ExtentRect,2,&ExtentRect, &textfont, msgRecMode, NULL, NULL, RGB(255,255,255), DT_CENTER); 
		HBITMAP newbm = DrawResampleRGB(&dc, &CRect(tmodeRect),2, (LPBITMAPINFOHEADER) pbmiText,  xmove, ymove); 

		if (pbmiText) {
			GlobalFreePtr(pbmiText);
			pbmiText = NULL;

		}
		

	}
	*/
	
	// Do not call CView::OnPaint() for painting messages
}

int CVscapView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
	versionOp = GetOperatingSystem();		
	
	CreateShiftWindow();
	
	HDC hScreenDC = ::GetDC(NULL);	
	bits = ::GetDeviceCaps(hScreenDC, BITSPIXEL );	
	nColors = bits;
	::ReleaseDC(NULL,hScreenDC);		
	
	compfccHandler = mmioFOURCC('M', 'S', 'V', 'C');	

	hLogoBM = LoadBitmap( AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BITMAP3)); 

	pFrame = new CFlashingWnd;	
	CRect rect(0, 0, maxxScreen-1, maxyScreen-1);
	pFrame->CreateFlashing("Flashing", rect);		

		
	//Ver 1.2 
	//cursordir default
	char dirx[300];
	GetWindowsDirectory(dirx,300);
	CString windir(dirx);
	cursordir = windir + "\\cursors";			

	//savedir default
	savedir=GetProgPath();

	
	//Ver 1.1
	//SuggestRecordingFormat();
	//SuggestCompressFormat();

	//ver 1.6
	initialSaveMMMode();

	srand( (unsigned)time( NULL ) );

	return 0;
}

void CVscapView::OnDestroy() 
{
	CView::OnDestroy();
	
	// TODO: Add your message handler code here

	//ver 1.2
	DecideSaveSettings();	
	
	//ver 1.2
	//UninstallMyKeyHook(hWndGlobal);	
	UnSetHotKeys();

	//ver 1.2
	finishTrayIconData();

	DestroyShiftWindow();

	if  (compressor_info != NULL) {		
	
		free(compressor_info);
		num_compressor = 0;

	}

	if (savedBitmap) {
		DeleteObject(savedBitmap);
		savedBitmap = NULL;
	}

	
	if (hLogoBM) {
		DeleteObject(hLogoBM);
		hLogoBM = NULL;
	}

	
	if (pFrame) {
	
		pFrame->DestroyWindow();
		delete pFrame;

	}	

	if (pwfx) {
		
		GlobalFreePtr(pwfx);
		pwfx = NULL;

	}

	FreeVideoCompressParams();
	FreeParamsUse();

	//ver 1.6
	if (TroubleShootVal) {

		//Safety code	
		if ((waveInGetNumDevs() == 0) || (waveOutGetNumDevs() == 0) || (mixerGetNumDevs() == 0)) {
			//Do nothing
		}
		else
			useWavein(TRUE,FALSE);

	}
	else
		finalRestoreMMMode();

	FreeWaveoutResouces();
	WaveoutUninitialize();


	//ver 1.8
	gList.FreeDisplayArray();
	gList.FreeShapeArray();
	gList.FreeLayoutArray();

	
}




LRESULT CVscapView::OnRecordStart (UINT wParam, LONG lParam) {

	
	CStatusBar* pStatus = (CStatusBar*) AfxGetApp()->m_pMainWnd->GetDescendantWindow(AFX_IDW_STATUS_BAR);
	pStatus->SetPaneText(0,"Press the Stop Button to stop recording");


	//if (minimizeOnStart == 1)
	//	AfxGetMainWnd()->ShowWindow(SW_MINIMIZE);

	//ver 1.2
	if (minimizeOnStart == 1)
		::PostMessage(AfxGetMainWnd()->m_hWnd,WM_SYSCOMMAND,SC_MINIMIZE,0);
	
	
	
	//Check validity of rc and fix it		
	HDC hScreenDC = ::GetDC(NULL);	
	maxxScreen = GetDeviceCaps(hScreenDC,HORZRES);
	maxyScreen = GetDeviceCaps(hScreenDC,VERTRES);		
	FixRectSizePos(&rc,maxxScreen, maxyScreen);		
	
	
	::ReleaseDC(NULL,hScreenDC);	

	
	InstallMyHook(hWndGlobal,WM_USER_SAVECURSOR);	
	
		
	recordstate=1;	
	interruptkey = 0;
	
	CWinThread * pThread = AfxBeginThread(RecordAVIThread, &tdata);	

	//Ver 1.3
	if (pThread)
		pThread->SetThreadPriority(threadPriority);


	//Ver 1.2
	AllowNewRecordStartKey = TRUE; //allow this only after recordstate is set to 1
	
 
	return 0;	

}


int matchSpecial(int keyCtrl,int keyShift,int keyAlt,int ctrlDown,int shiftDown,int altDown)
{

	int ret = 1;
	
		
	if ((keyCtrl) && (!ctrlDown))
		ret  = 0;

	if ((keyShift) && (!shiftDown))
		ret  = 0;

	if ((keyAlt) && (!altDown))
		ret  = 0;
		

	if ((!keyCtrl) && (ctrlDown))
		ret  = 0;

	if ((!keyShift) && (shiftDown))
		ret  = 0;

	if ((!keyAlt) && (altDown))
		ret  = 0;
	

	/*
	
	if ((keyCtrl) && (ctrlDown))
	{

		if ((keyShift) && (shiftDown))
		{

			if ((keyAlt) && (altDown))
			{

			}
			else if ((!keyAlt) && (!altDown))
			{

			}
			else if (keyAlt)
				ret = 0;

		}
		else if ((!keyShift) && (!shiftDown))
		{

			if ((keyAlt) && (altDown))
			{

			}
			else if ((!keyAlt) && (!altDown))
			{

			}
			else if (keyAlt)
				ret = 0;


		}
		else if (keyShift)
			ret = 0;


	}
	else if ((!keyCtrl) && (!ctrlDown))
	{

		if ((keyShift) && (shiftDown))
		{
			if ((keyAlt) && (altDown))
			{

			}
			else if ((!keyAlt) && (!altDown))
			{

			}
			else  if (keyAlt)
				ret = 0;

		}
		else if ((!keyShift) && (!shiftDown))
		{
			if ((keyAlt) && (altDown))
			{

			}
			else if ((!keyAlt) && (!altDown))
			{

			}
			else  if (keyAlt)
				ret = 0;

		}
		else if (keyShift)
			ret = 0;

	}
	else if (keyCtrl)
		ret = 0;

  */
	
	

	return ret;


}

//ver 1.8
int UnSetHotKeys()
{

	
	UnregisterHotKey(hWndGlobal,0);
	UnregisterHotKey(hWndGlobal,1);
	UnregisterHotKey(hWndGlobal,2);
	UnregisterHotKey(hWndGlobal,3);
	UnregisterHotKey(hWndGlobal,4);
	UnregisterHotKey(hWndGlobal,5);
	

	
	/*
	UnregisterHotKey(hWndGlobal,base_nid+0);
	UnregisterHotKey(hWndGlobal,base_nid+1);
	UnregisterHotKey(hWndGlobal,base_nid+2);
	UnregisterHotKey(hWndGlobal,base_nid+3);
	UnregisterHotKey(hWndGlobal,base_nid+4);
	UnregisterHotKey(hWndGlobal,base_nid+5);
	*/
	

	return 0;

}

int SetAdjustHotKeys()
{
	int succ[6];
	int ret = SetHotKeys(succ);

	CString keystr;
	CString msgstr;
	CString outstr;	
	if (succ[0])
	{
		keyRecordStart = 100000;
		keyRecordStartCtrl = 0;
		keyRecordStartShift = 0;
		keyRecordStartAlt = 0;

		keystr.LoadString(IDS_STRINGSPRKEY);
		
		msgstr.LoadString(IDS_STRING_UNSSC);
		outstr.Format(msgstr,keystr);		
		//keystr = "Unable to set shortcuts for %d" + keystr;
		MessageBox(NULL,outstr,"Note",MB_OK | MB_ICONEXCLAMATION);
	
	}


	
	if (succ[1])
	{
		keyRecordEnd = 100000;
		keyRecordEndCtrl = 0;
		keyRecordEndShift = 0;
		keyRecordEndAlt = 0;

		keystr.LoadString(IDS_STRINGSRKEY);
		
		msgstr.LoadString(IDS_STRING_UNSSC);
		outstr.Format(msgstr,keystr);		
		//keystr = "Unable to set shortcuts for %d" + keystr;
		MessageBox(NULL,outstr,"Note",MB_OK | MB_ICONEXCLAMATION);
	
	}

	if (succ[2])
	{
		keyRecordCancel = 100000;
		keyRecordCancelCtrl = 0;
		keyRecordCancelShift = 0;
		keyRecordCancelAlt = 0;

		keystr.LoadString(IDS_STRINGCRKEY);
		
		msgstr.LoadString(IDS_STRING_UNSSC);
		outstr.Format(msgstr,keystr);		
		//keystr = "Unable to set shortcuts for %d" + keystr;
		MessageBox(NULL,outstr,"Note",MB_OK | MB_ICONEXCLAMATION);
	
	}

	if (succ[3])
	{
		keyNext = 100000;
		keyNextCtrl = 0;
		keyNextShift = 0;
		keyNextAlt = 0;
		
		keystr.LoadString(IDS_STRINGNLKEY);	
		
		msgstr.LoadString(IDS_STRING_UNSSC);
		outstr.Format(msgstr,keystr);		
		//keystr = "Unable to set shortcuts for %d" + keystr;
		MessageBox(NULL,outstr,"Note",MB_OK | MB_ICONEXCLAMATION);
	
	}

	if (succ[4])
	{
		keyPrev = 100000;
		keyPrevCtrl = 0;
		keyPrevShift = 0;
		keyPrevAlt = 0;

		keystr.LoadString(IDS_STRINGPLKEY);	
		
		msgstr.LoadString(IDS_STRING_UNSSC);
		outstr.Format(msgstr,keystr);		
		//keystr = "Unable to set shortcuts for %d" + keystr;
		MessageBox(NULL,outstr,"Note",MB_OK | MB_ICONEXCLAMATION);
	
	}

	if (succ[5])
	{
		keyShowLayout = 100000;
		keyShowLayoutCtrl = 0;
		keyShowLayoutShift = 0;
		keyShowLayoutAlt = 0;

		keystr.LoadString(IDS_STRINGSHLKEY);
		msgstr.LoadString(IDS_STRING_UNSSC);
		outstr.Format(msgstr,keystr);		
		//keystr = "Unable to set shortcuts for %d" + keystr;
		MessageBox(NULL,outstr,"Note",MB_OK | MB_ICONEXCLAMATION);
	
	}

	return ret;

}

int SetHotKeys(int succ[])
{

	UnSetHotKeys();
	
	
	for (int i =0 ;i<6 ;i++)
		succ[i]=0;

	int tstatus = 0;

	BOOL ret;
	//int nid = base_nid;
	int nid = 0;
	UINT modf = 0;


	if (keyRecordStartCtrl)
		modf |= MOD_CONTROL;  
		
	if (keyRecordStartShift)
		modf |= MOD_SHIFT;  
		
	if (keyRecordStartAlt)
		modf |= MOD_ALT;
	
	if (keyRecordStart!=100000) {
		ret = RegisterHotKey(hWndGlobal,nid,modf,keyRecordStart);
		if (!ret) 
			succ[0] = 1;
	}


	nid++;
	modf = 0;
	if (keyRecordEndCtrl)
		modf |= MOD_CONTROL;  
		
	if (keyRecordEndShift)
		modf |= MOD_SHIFT;  
		
	if (keyRecordEndAlt)
		modf |= MOD_ALT;
	
	if (keyRecordEnd!=100000) {
		ret = RegisterHotKey(hWndGlobal,nid,modf,keyRecordEnd);
		if (!ret) 
			succ[1] = 1;
	}



	nid++;
	modf = 0;
	if (keyRecordCancelCtrl)
		modf |= MOD_CONTROL;  
		
	if (keyRecordCancelShift)
		modf |= MOD_SHIFT;  
		
	if (keyRecordCancelAlt)
		modf |= MOD_ALT;
	
	if (keyRecordCancel!=100000) {
		ret = RegisterHotKey(hWndGlobal,nid,modf,keyRecordCancel);
		if (!ret) 
			succ[2] = 1;
	}


	nid++;
	modf = 0;
	if (keyNextCtrl)
		modf |= MOD_CONTROL;  
		
	if (keyNextShift)
		modf |= MOD_SHIFT;  
		
	if (keyNextAlt)
		modf |= MOD_ALT;
	
	if (keyNext!=100000) {	
		ret = RegisterHotKey(hWndGlobal,nid,modf,keyNext);
		if (!ret) 
			succ[3] = 1;
	}


	nid++;
	modf = 0;
	if (keyPrevCtrl)
		modf |= MOD_CONTROL;  
		
	if (keyPrevShift)
		modf |= MOD_SHIFT;  
		
	if (keyPrevAlt)
		modf |= MOD_ALT;
	
	if (keyPrev!=100000) {	
		ret = RegisterHotKey(hWndGlobal,nid,modf,keyPrev);
		if (!ret) 
			succ[4] = 1;

	}


	nid++;
	modf = 0;
	if (keyShowLayoutCtrl)
		modf |= MOD_CONTROL;  
		
	if (keyShowLayoutShift)
		modf |= MOD_SHIFT;  
		
	if (keyShowLayoutAlt)
		modf |= MOD_ALT;
	
	if (keyShowLayout!=100000) {
		ret = RegisterHotKey(hWndGlobal,nid,modf,keyShowLayout);
		if (!ret) 			
			succ[5] = 1;
	}


	return tstatus;

}


//ver 1.8
LRESULT CVscapView::OnKeyStart (UINT wParam, LONG lParam) 
{
			
	return 0;

}



LRESULT CVscapView::OnRecordInterrupted (UINT wParam, LONG lParam) {

	//if (recordstate==0) return;
		
	UninstallMyHook(hWndGlobal);	
	
	//Ver 1.1	
	if (recordpaused) {
		recordpaused = 0;
		SetTitleBar("CamStudio");
	}
	
	recordstate=0;
	
	//Store the interrupt key in case this function is triggered by a keypress
	interruptkey = wParam;	
	
	CStatusBar* pStatus = (CStatusBar*) AfxGetApp()->m_pMainWnd->GetDescendantWindow(AFX_IDW_STATUS_BAR);
	pStatus->SetPaneText(0,"Press the Record Button to start recording");

	Invalidate();

	//ver 1.2
	::SetForegroundWindow( AfxGetMainWnd()->m_hWnd ); 			
	AfxGetMainWnd()->ShowWindow(SW_RESTORE);	

	return 0;

}

LRESULT CVscapView::OnSaveCursor (UINT wParam, LONG lParam) {	

	hSavedCursor = (HCURSOR) wParam;		

	return 0;

}



//This function is called when the avi saving is completed
LRESULT CVscapView::OnUserGeneric	(UINT wParam, LONG lParam) {
	
	
	//ver 1.2
	::SetForegroundWindow( AfxGetMainWnd()->m_hWnd ); 
	AfxGetMainWnd()->ShowWindow(SW_RESTORE);	
	
	//ver 1.2
	if (interruptkey==keyRecordCancel) {
	//if (interruptkey==VK_ESCAPE) {

		//Perform processing for cancel operation
		DeleteFile(tempfilepath);
		if (recordaudio) DeleteFile(tempaudiopath);
		return 0;

	}
	
	//Normal thread exit	
	//Prompt the user for the filename

	char szFilter[100];
	char szTitle[100];
	char extFilter[30];

	//ver 2.26
	if (RecordingMode == ModeAVI)	
	{
		strcpy(szFilter,"AVI Movie Files (*.avi)|*.avi||");
		strcpy(szTitle,"Save AVI File");
		strcpy(extFilter,"*.avi");

	}
	else
	{
		strcpy(szFilter,"FLASH Movie Files (*.swf)|*.swf||");
		strcpy(szTitle,"Save SWF File");
		strcpy(extFilter,"*.swf");

	}
	
	
	CFileDialog fdlg(FALSE,extFilter,extFilter,OFN_LONGNAMES,szFilter,this);	
	
	//static char BASED_CODE szFilter[] =	"AVI Movie Files (*.avi)|*.avi||";
	//static char szTitle[]="Save AVI File";		
	//CFileDialog fdlg(FALSE,"*.avi","*.avi",OFN_LONGNAMES,szFilter,this);	


	
	
	
	fdlg.m_ofn.lpstrTitle=szTitle;	
	
	if (savedir=="") 
		savedir=GetProgPath();
	
	fdlg.m_ofn.lpstrInitialDir = savedir;
	
	CString m_newfile;
	CString m_newfileTitle;


	if ((RecordingMode == ModeAVI)	&&  (autonaming > 0))
	{	

		savedir=GetProgPath();

		//ErrMsg("hereh");

		time_t osBinaryTime;  // C run-time time (defined in <time.h>)
		time( &osBinaryTime ); 

		CTime ctime(osBinaryTime);	

		int day = ctime.GetDay();
		int month = ctime.GetMonth();
		int year = ctime.GetYear();
		int hour = ctime.GetHour() ;
		int minutes = ctime.GetMinute() ;
		int second = ctime.GetSecond() ;

		CString filestr;
		//filestr.Format("%d%d%d_%d%d",day,month,year,hour,minutes);
		filestr.Format("%d%d%d_%d%d_%d",day,month,year,hour,minutes,second);

		fdlg.m_ofn.lpstrInitialDir = savedir;
		
		m_newfile = savedir + "\\" + filestr + ".avi";
		m_newfileTitle = savedir + "\\" + filestr + ".avi";
		m_newfileTitle=m_newfileTitle.Left(m_newfileTitle.ReverseFind('\\'));


    }
	else 
    {
	
		if(fdlg.DoModal() == IDOK)
		{
			m_newfile = fdlg.GetPathName();
			
			m_newfileTitle = fdlg.GetPathName();		
			m_newfileTitle=m_newfileTitle.Left(m_newfileTitle.ReverseFind('\\'));		
			savedir = m_newfileTitle;
		}
		else {
			
			DeleteFile(tempfilepath);
			if (recordaudio) DeleteFile(tempaudiopath);
			return 0;

		}

	}

	
	//ver 2.26
	if (RecordingMode == ModeFlash)
	{	
		int lenx = m_newfile.GetLength();
		if (((m_newfile.GetAt(lenx-1) == 'f') || (m_newfile.GetAt(lenx-1) == 'F')) &&
			((m_newfile.GetAt(lenx-2) == 'w') || (m_newfile.GetAt(lenx-2) == 'W')) &&
			((m_newfile.GetAt(lenx-3) == 's') || (m_newfile.GetAt(lenx-3) == 'S')) &&
			(m_newfile.GetAt(lenx-4) == '.'))
		{

			m_newfile.SetAt(lenx-1,'i');
			m_newfile.SetAt(lenx-2,'v');
			m_newfile.SetAt(lenx-3,'a');
			m_newfile.SetAt(lenx-4,'.');

		}
		else
		{
			m_newfile += ".avi";

		}

	}
	
	//Ver 1.1
	if (recordaudio) {	

		//Check if file exists  and if so, does it allow overwite		
		HANDLE hfile = CreateFile(
			  m_newfile,          // pointer to name of the file
			  GENERIC_WRITE,       // access (read-write) mode
			  0,           // share mode
			  NULL,        // pointer to security attributes
			  CREATE_ALWAYS,  // how to create
			  FILE_ATTRIBUTE_NORMAL,  // file attributes
			  NULL        // handle to file with attributes to                            // copy
			);

		if (hfile == INVALID_HANDLE_VALUE) {

			//::MessageBox(NULL,"Unable to create new file. The file may be opened by another application. Please use another filename.","Note",MB_OK | MB_ICONEXCLAMATION);		

			MessageOut(this->m_hWnd,IDS_STRING_NOCREATEWFILE ,IDS_STRING_NOTE,MB_OK | MB_ICONEXCLAMATION);
			
			
			::PostMessage(hWndGlobal,WM_USER_GENERIC,0,0);	
			return 0;

		}
		else {			
			
			CloseHandle(hfile);
			DeleteFile(m_newfile);

		}

		//ver 1.8
		if (vanWndCreated)
		{
			if (vanWnd.IsWindowVisible())
			{
				//Otherwise, will slow down the merging significantly 
				vanWnd.ShowWindow(SW_HIDE);
			}
		}
	
		int result;
		//Mergefile video with audio		
		//if (recordaudio==2) {
		//if ((recordaudio==2) || (useMCI)) {	
		
		//ver 2.26 ...overwrite audio settings for Flash Moe recording ... no compression used...
		if ((recordaudio==2) || (useMCI) || (RecordingMode == ModeFlash)) {	
			result = Merge_Video_And_Sound_File(tempfilepath, tempaudiopath, m_newfile, FALSE, pwfx, cbwfx,interleaveFrames,interleaveFactor, interleaveUnit);
		}
		else if (recordaudio==1) {
			result = Merge_Video_And_Sound_File(tempfilepath, tempaudiopath, m_newfile, bAudioCompression, pwfx, cbwfx,interleaveFrames,interleaveFactor, interleaveUnit);
		}

		//Check Results : Attempt Recovery on error		
		if (result==0) {

			//Successful
			DeleteFile(tempfilepath);
			DeleteFile(tempaudiopath);

		}
		else if (result==1) { //video file broken

			//Unable to recover
			DeleteFile(tempfilepath);
			DeleteFile(tempaudiopath);


		}
		else if (result==3) { //this case is rare

			//Unable to recover
			DeleteFile(tempfilepath);
			DeleteFile(tempaudiopath);


		}
		else if ((result==2) || (result==4)) { //recover video file

			//video file is ok, but not audio file
			//so copy the video file as avi	and ignore the audio
			if (!CopyFile( tempfilepath,m_newfile,FALSE)) {
			
				//Although there is error copying, the temp file still remains in the temp directory and is not deleted, in case user wants a manual recover
				//MessageBox("File Creation Error. Unable to rename/copy file.","Note",MB_OK | MB_ICONEXCLAMATION);
				MessageOut(this->m_hWnd,IDS_STRING_FILECREATIONERROR ,IDS_STRING_NOTE,MB_OK | MB_ICONEXCLAMATION);
				return 0;	
			
			}
			else {
		
				
				DeleteFile(tempfilepath);
				DeleteFile(tempaudiopath);
		
			} 	

			//::MessageBox(NULL,"Your AVI movie will not contain a soundtrack. CamStudio is unable to merge the video with audio.","Note",MB_OK | MB_ICONEXCLAMATION);		
			MessageOut(this->m_hWnd,IDS_STRING_NOSOUNDTRACK ,IDS_STRING_NOTE,MB_OK | MB_ICONEXCLAMATION);
			

		}
		else if (result == 5) { //recover both files, but as separate files

			CString m_audiofile;
			CString m_audioext(".wav");
			m_audiofile = m_newfile + m_audioext;

			if (!CopyFile( tempfilepath,m_newfile,FALSE)) {

					//MessageBox("File Creation Error. Unable to rename/copy video file.","Note",MB_OK | MB_ICONEXCLAMATION);
					MessageOut(this->m_hWnd,IDS_STRINGFILECREATION2 ,IDS_STRING_NOTE,MB_OK | MB_ICONEXCLAMATION);
					return 0;					
					
			}		
			else {
		
				DeleteFile(tempfilepath);				
		
			} 	
			
			if (!CopyFile(tempaudiopath,m_audiofile,FALSE)) {

					//MessageBox("File Creation Error. Unable to rename/copy audio file.","Note",MB_OK | MB_ICONEXCLAMATION);
					MessageOut(this->m_hWnd,IDS_STRING_FILECREATION3 ,IDS_STRING_NOTE,MB_OK | MB_ICONEXCLAMATION);
					return 0;					
					
			}		
			else {
		
				DeleteFile(tempaudiopath);
		
			} 				

			CString tstr,msgstr;
			tstr.LoadString(IDS_STRING_NOTE);
			msgstr.LoadString(IDS_STRING_NOMERGE);	
			//CString msgstr("CamStudio is unable to merge the video with audio files. Your video and audio files are saved separately as \n\n");	
			
			msgstr = msgstr  + m_newfile + "\n";
			msgstr = msgstr + m_audiofile;			
			::MessageBox(NULL,msgstr,tstr,MB_OK | MB_ICONEXCLAMATION);		

			

		} //if result


	
	}	   //if record audio

	else { //no audio, just do a plain copy of temp avi to final avi

		if (!CopyFile( tempfilepath,m_newfile,FALSE)) {

			//Ver 1.1
			//DeleteFile(m_newfile);
			
			//MessageBox("File Creation Error. Unable to rename/copy file. The file may be opened by another application. Please use another filename.","Note",MB_OK | MB_ICONEXCLAMATION);
			MessageOut(this->m_hWnd,IDS_STRING_FILECREATION4 ,IDS_STRING_NOTE,MB_OK | MB_ICONEXCLAMATION);

	
			//Repeat this function until success
			::PostMessage(hWndGlobal,WM_USER_GENERIC,0,0);			
			return 0;
	
		}
		else {
	
			DeleteFile(tempfilepath);
			if (recordaudio) DeleteFile(tempaudiopath);
	
		} 	
		
	} 


	//ver 2.26
	if (RecordingMode == ModeAVI)
	{	
	
		//Launch the player
		if (launchPlayer == 1) {

			CString AppDir=GetProgPath();
			CString launchPath;
			CString exefileName("\\player.exe ");
			launchPath=AppDir+exefileName+m_newfile;
			
			
			if (WinExec(launchPath,SW_SHOW)!=0) {

					
			}
			else {

				//MessageBox("Error launching avi player!","Note",MB_OK | MB_ICONEXCLAMATION);
				MessageOut(this->m_hWnd,IDS_STRING_ERRPLAYER ,IDS_STRING_NOTE,MB_OK | MB_ICONEXCLAMATION);

			}
			

		}
		else if (launchPlayer == 2)
		{
			if (Openlink(m_newfile)) {

			}
			else {

				//MessageBox("Error launching avi player!","Note",MB_OK | MB_ICONEXCLAMATION);
				MessageOut(this->m_hWnd,IDS_STRING_ERRDEFAULTPLAYER ,IDS_STRING_NOTE,MB_OK | MB_ICONEXCLAMATION);

			}

		}
		else if (launchPlayer == 3) {

			CString AppDir=GetProgPath();
			CString launchPath;
			CString exefileName("\\Playplus.exe ");
			launchPath=AppDir+exefileName+m_newfile;
			
			
			if (WinExec(launchPath,SW_SHOW)!=0) {

					
			}
			else {

				//MessageBox("Error launching avi player!","Note",MB_OK | MB_ICONEXCLAMATION);
				MessageOut(this->m_hWnd,IDS_STRING_ERRPLAYER ,IDS_STRING_NOTE,MB_OK | MB_ICONEXCLAMATION);

			}
			

		}


	}
	else
	{

		/*
		CString swfname;
		swfname = m_newfile;
		int lenx = m_newfile.GetLength();
		if (((m_newfile.GetAt(lenx-1) == 'i') || (m_newfile.GetAt(lenx-1) == 'I')) &&
			((m_newfile.GetAt(lenx-2) == 'v') || (m_newfile.GetAt(lenx-2) == 'V')) &&
			((m_newfile.GetAt(lenx-3) == 'a') || (m_newfile.GetAt(lenx-3) == 'A')) &&
			(m_newfile.GetAt(lenx-4) == '.'))
		{

			m_newfile.SetAt(lenx-1,'f');
			m_newfile.SetAt(lenx-2,'w');
			m_newfile.SetAt(lenx-3,'s');
			m_newfile.SetAt(lenx-4,'.');

		}
		*/

			

			//ver 2.26
			SaveProducerCommand();

			//Sleep(2000);

			CString AppDir=GetProgPath();
			CString launchPath;
			CString exefileName("\\producer.exe -x ");
			launchPath=AppDir+exefileName+m_newfile; 


						
			if (WinExec(launchPath,SW_SHOW)!=0) {

					
			}
			else {

				MessageBox("Error launching SWF Producer!","Note",MB_OK | MB_ICONEXCLAMATION);
				//MessageOut(this->m_hWnd,IDS_STRING_ERRPRODUCER ,IDS_STRING_NOTE,MB_OK | MB_ICONEXCLAMATION);

			}


	}

	return 0;

}



//**********************************************************************
//
// Utility()
//
//**********************************************************************

void WINAPI NormalizeRect(LPRECT prc)
{
    if (prc->right  < prc->left) SWAP(prc->right,  prc->left);
    if (prc->bottom < prc->top)  SWAP(prc->bottom, prc->top);
}


void FixRectSizePos(LPRECT prc,int maxxScreen, int maxyScreen)
{
	NormalizeRect(prc);
    
	int width=((prc->right)-(prc->left))+1;
	int height=((prc->bottom)-(prc->top))+1;
	
	if (width>maxxScreen) {
		
		prc->left=0;
		prc->right=maxxScreen-1;

	}

	if (height>maxyScreen) {
		
		prc->top=0;
		prc->bottom=maxyScreen-1;

	}

	if (prc->left <0) {

		prc->left= 0;
		prc->right=width-1;

	}

	if (prc->top <0) {

		prc->top= 0;
		prc->bottom=height-1;

	}

	if (prc->right > maxxScreen-1 ) {

		prc->right = maxxScreen-1;
		prc->left= maxxScreen-width;

	}

	if (prc->bottom > maxyScreen-1 ) {

		prc->bottom = maxyScreen-1;
		prc->top= maxyScreen-height;

	}


	
}

BOOL isRectEqual(RECT a, RECT b) {

	if ((a.left==b.left) && (a.right==b.right) && (a.top==b.top) && (a.bottom==b.bottom)) 
		return TRUE;
	else return FALSE;


}


//****************************************************************************
//
// Function: FrameWindow()
//
// Purpose:  Highlight the window frame
//
//
//***************************************************************************

RECT FrameWindow(HWND hWnd)
{
    HDC     hdc;	
	RECT    rectWin;
    RECT    rectFrame;

	rectWin.left=0;
	rectWin.top=0;
	rectWin.right=maxxScreen-1;
	rectWin.bottom=maxyScreen-1;

#define DINV    3
    
    if (!IsWindow(hWnd))
        return rectWin;

    hdc = GetWindowDC(hWnd);
    GetWindowRect(hWnd, &rectWin);
    
	rectFrame=rectWin;
	OffsetRect(&rectFrame, -rectFrame.left, -rectFrame.top);



    if (!IsRectEmpty(&rectFrame))
    {

		HBRUSH newbrush = (HBRUSH) CreateHatchBrush(HS_BDIAGONAL, RGB(0,0,100));
		HBRUSH oldbrush = (HBRUSH) SelectObject(hdc,newbrush);

        PatBlt(hdc, rcClip.left, rcClip.top, rcClip.right-rcClip.left, DINV, PATINVERT);
        PatBlt(hdc, rcClip.left, rcClip.bottom-DINV, DINV, -(rcClip.bottom-rcClip.top-2*DINV),  PATINVERT);
        PatBlt(hdc, rcClip.right-DINV, rcClip.top+DINV, DINV, rcClip.bottom-rcClip.top-2*DINV,   PATINVERT);
        PatBlt(hdc, rcClip.right, rcClip.bottom-DINV, -(rcClip.right-rcClip.left), DINV,  PATINVERT);

		SelectObject(hdc,oldbrush);
		DeleteObject(newbrush);

    }

    ReleaseDC(hWnd, hdc);

	return rectWin;
}



//***************************************************************************
//
// DrawSelect
//
// Draws the selected clip rectangle with its dimensions on the DC
//
//***************************************************************************

void SaveBitmapCopy(HDC hdc,HDC hdcbits, int x, int y, int sx, int sy) {

	if (savedBitmap) DeleteObject(savedBitmap);
	savedBitmap = NULL;

	savedBitmap = (HBITMAP) CreateCompatibleBitmap(hdc,sx, sy);
	HBITMAP oldbitmap = (HBITMAP) SelectObject(hdcbits,savedBitmap);	
	BitBlt(hdcbits, 0, 0, sx, sy, hdc, x, y, SRCCOPY);
	
	SelectObject(hdcbits,oldbitmap);


}

void RestoreBitmapCopy(HDC hdc,HDC hdcbits, int x, int y, int sx, int sy) {

	if (savedBitmap) {

		HBITMAP oldbitmap = (HBITMAP) SelectObject(hdcbits,savedBitmap);
		BitBlt(hdc, x, y, sx, sy, hdcbits, 0, 0, SRCCOPY);
		SelectObject(hdcbits,oldbitmap);

		if (savedBitmap) DeleteObject(savedBitmap);
		savedBitmap = NULL;

	}

}

void DrawSelect(HDC hdc, BOOL fDraw, LPRECT lprClip)
{
    char sz[80];
    DWORD dw;
    int x, y, len, dx, dy;
    HDC hdcBits;    
    RECT rectDraw;
    SIZE sExtent;

    rectDraw = *lprClip;
    if (!IsRectEmpty(&rectDraw))
    {

        // If a rectangular clip region has been selected, draw it
		HBRUSH newbrush = (HBRUSH) CreateHatchBrush(HS_BDIAGONAL, RGB(0,0,100));
		HBRUSH oldbrush = (HBRUSH) SelectObject(hdc,newbrush);

		//PatBlt SRCINVERT regardless fDraw is TRUE or FALSE
        PatBlt(hdc, rectDraw.left, rectDraw.top, rectDraw.right-rectDraw.left, DINV, PATINVERT);
        PatBlt(hdc, rectDraw.left, rectDraw.bottom-DINV, DINV, -(rectDraw.bottom-rectDraw.top-2*DINV),  PATINVERT);
        PatBlt(hdc, rectDraw.right-DINV, rectDraw.top+DINV, DINV, rectDraw.bottom-rectDraw.top-2*DINV,   PATINVERT);
        PatBlt(hdc, rectDraw.right, rectDraw.bottom-DINV, -(rectDraw.right-rectDraw.left), DINV,  PATINVERT);

		SelectObject(hdc,oldbrush);
		DeleteObject(newbrush);
		
		
		hdcBits = CreateCompatibleDC(hdc);
		HFONT newfont = (HFONT) GetStockObject(ANSI_VAR_FONT);
		HFONT oldfont = (HFONT) SelectObject(hdc, newfont);            
		//HFONT oldfont = (HFONT) SelectObject(hdcBits, newfont);            
		
		wsprintf(sz, "Left : %d  Top : %d  Width : %d  Height : %d", rectDraw.left, rectDraw.top, rectDraw.right - rectDraw.left+1, rectDraw.bottom -  rectDraw.top+1);
        len = lstrlen(sz);        
		dw = GetTextExtentPoint(hdc, sz, len, &sExtent);
		//dw = GetTextExtentPoint(hdcBits, sz, len, &sExtent);
        
		dx = sExtent.cx;
        dy = sExtent.cy;
		x=  rectDraw.left +10;
		
		if (rectDraw.top < (dy + DINV + 2)) 
			y=  rectDraw.bottom + DINV + 2;
		else
			y=  rectDraw.top - dy - DINV - 2;
        

		
		if (fDraw)	{		
			
			//Save Original Picture
			SaveBitmapCopy(hdc,hdcBits,  x-4, y-4, dx+8, dy+8); 
			
			
			//Text
			COLORREF oldtextcolor = SetTextColor(hdc,RGB(0,0,0));
			COLORREF oldbkcolor = SetBkColor(hdc,RGB(255,255,255));
			SetBkMode(hdc,TRANSPARENT);
			
			//Rectangle(hdc,x-1,y-1,x+dx, y+dy);
			RoundRect(hdc,x-4,y-4,x+dx+4, y+dy+4,10,10);

			SetBkMode(hdc,OPAQUE);

			ExtTextOut(hdc, x, y, 0, NULL, sz, len, NULL);
			SetBkColor(hdc,oldbkcolor);
			SetTextColor(hdc,oldtextcolor);
			SelectObject(hdc, oldfont);	
		}
		else 
			RestoreBitmapCopy(hdc,hdcBits,  x-4, y-4, dx+8, dy+8);


		

		//Icon
		if ((rectDraw.right-rectDraw.left-10 >  35) &&  (rectDraw.bottom-rectDraw.top-10 > dy + 40)) {
	
			HBITMAP hbv = LoadBitmap( AfxGetInstanceHandle(),  MAKEINTRESOURCE(IDB_BITMAP1)); 
			HBITMAP old_bitmap = (HBITMAP) SelectObject(hdcBits, hbv);
			BitBlt(hdc, rectDraw.left+10, rectDraw.bottom-42, 30, 32,hdcBits, 0,0, SRCINVERT);
			SelectObject(hdcBits,old_bitmap);
			DeleteObject(hbv);

		}

        DeleteDC(hdcBits);
    }
	
}


void DrawFlashingRect(BOOL bDraw , int mode) {

		
	if (mode == 1) { 
			
			pFrame->PaintInvertedBorder(RGB(0,255,80));
	}
	else {	
	
		if (bDraw)
			pFrame->PaintBorder(RGB(255,255,180));
		else
			pFrame->PaintBorder(RGB(0,255,80));

	}
	
	
}


//**********************************************************************
//
// MouseCaptureWndProc()
//
//
//*********************************************************************

long WINAPI MouseCaptureWndProc(HWND hWnd, UINT wMessage, WPARAM wParam, LPARAM lParam)
{

    
    switch (wMessage)
    {

		case WM_MOUSEMOVE:
        {


			if (MouseCaptureMode==0) { //Fixed Region

				POINT pt;
				GetCursorPos(&pt);				

				rcClip.left = pt.x+rcOffset.left;   // Update rect with new mouse info
				rcClip.top = pt.y+rcOffset.top;               				
				rcClip.right = pt.x+rcOffset.right;
				rcClip.bottom = pt.y+rcOffset.bottom;

				if (rcClip.left<0) {

					rcClip.left=0;
					rcClip.right=((rc.right)-(rc.left));						

				}
				if (rcClip.top<0) {

					rcClip.top=0;
					rcClip.bottom=((rc.bottom)-(rc.top));									
					
				}
				if (rcClip.right>maxxScreen-1) {

					rcClip.right=maxxScreen-1;
					rcClip.left=maxxScreen-1-((rc.right)-(rc.left));
					

				}
				if (rcClip.bottom>maxyScreen-1) {

					rcClip.bottom=maxyScreen-1;
					rcClip.top=maxyScreen-1-((rc.bottom)-(rc.top));					
								
				}                

			
				if (!isRectEqual(old_rcClip,rcClip)) {

					HDC hScreenDC = GetDC(hWnd);		
					DrawSelect(hScreenDC, FALSE, &old_rcClip);  // erase old rubber-band																	
					DrawSelect(hScreenDC, TRUE, &rcClip); // new rubber-band
					ReleaseDC(hWnd,hScreenDC);
					
				}// if old

				old_rcClip=rcClip;			          
			

			}
			else if (MouseCaptureMode==1) { //Variable Region

				    if (bCapturing)
					{						
						POINT pt;
						GetCursorPos(&pt);

						HDC hScreenDC = GetDC(hWnd);	
						
						DrawSelect(hScreenDC, FALSE, &rcClip);  // erase old rubber-band
						
						rcClip.left = ptOrigin.x;
						rcClip.top = ptOrigin.y;					
						rcClip.right = pt.x;
						rcClip.bottom = pt.y;
						
						NormalizeRect(&rcClip);
						DrawSelect(hScreenDC, TRUE, &rcClip); // new rubber-band
						//TextOut(hScreenDC,pt.x,pt.y,"Lolo",4);

						ReleaseDC(hWnd,hScreenDC);
					}

			}	

			return DefWindowProc(hWnd, wMessage, wParam, lParam);

        }
		case WM_LBUTTONUP:
		{
			
			if (MouseCaptureMode==0) {
			
				//erase final
				HDC hScreenDC = GetDC(hWnd);		
				DrawSelect(hScreenDC, FALSE, &old_rcClip);    
				old_rcClip=rcClip;
				ReleaseDC(hWnd,hScreenDC);

			}
			else if (MouseCaptureMode==1) {

				NormalizeRect(&rcClip);
				old_rcClip=rcClip;
				bCapturing = FALSE;				

			}				
			
			ShowWindow(hWnd,SW_HIDE);

			if (!IsRectEmpty(&old_rcClip)) {
			
				NormalizeRect(&old_rcClip);
				CopyRect(&rcUse, &old_rcClip);

				if (DefineMode==0)
					PostMessage (hWndGlobal,WM_USER_RECORDSTART,0,(LPARAM) 0); 
				else
					PostMessage (hWnd_FixedRegion,WM_USER_REGIONUPDATE,0,(LPARAM) 0); 

			}		        

            return DefWindowProc(hWnd, wMessage, wParam, lParam);    
		
		}		
		case WM_LBUTTONDOWN:
        {
             // User pressed left button, initialize selection
             // Set origin to current mouse position (in window coords)			

			if (MouseCaptureMode==1) {

                POINT pt;
				GetCursorPos(&pt);
				
				ptOrigin=pt;

                rcClip.left = rcClip.right = pt.x;
                rcClip.top = rcClip.bottom = pt.y;                
				
				NormalizeRect(&rcClip);     // Make sure it is a normal rect
				HDC hScreenDC = GetDC(hWnd);
                DrawSelect(hScreenDC, TRUE, &rcClip); // Draw the rubber-band box
				ReleaseDC(hWnd,hScreenDC);

                bCapturing = TRUE;

			}

			return DefWindowProc(hWnd, wMessage, wParam, lParam);    
        }
		
		case WM_RBUTTONDOWN:
        {


			if (MouseCaptureMode==0) {	//Cancel the operation				 
				
					//erase final
					HDC hScreenDC = GetDC(hWnd);		
					DrawSelect(hScreenDC, FALSE, &old_rcClip);    
					ReleaseDC(hWnd,hScreenDC);

					//Cancel the operation
					ShowWindow(hWnd,SW_HIDE);				

					//ver 1.2
					AllowNewRecordStartKey = TRUE; 

				}

			return DefWindowProc(hWnd, wMessage, wParam, lParam);    

		}		
		case WM_KEYDOWN:
        {
			int nVirtKey = (int) wParam;    // virtual-key code 
			int lKeyData = lParam;          // key data 			

			//ver 1.2
 			if (nVirtKey==(int) keyRecordCancel) { //Cancel the operation
			//if (nVirtKey==VK_ESCAPE) { 

				if (MouseCaptureMode==0) {
				
					//erase final
					HDC hScreenDC = GetDC(hWnd);		
					DrawSelect(hScreenDC, FALSE, &old_rcClip);    
					ReleaseDC(hWnd,hScreenDC);

				}
				else if (MouseCaptureMode==1) {

					NormalizeRect(&rcClip);
					old_rcClip=rcClip;
					if (bCapturing) bCapturing = FALSE;				

				}		
				
				ShowWindow(hWnd,SW_HIDE);				

				//ver 1.2
				AllowNewRecordStartKey = TRUE; 

			} //VK_ESCAPE (keyRecordCancel)

			return DefWindowProc(hWnd, wMessage, wParam, lParam);    

		}

        
		default:
			return DefWindowProc(hWnd, wMessage, wParam, lParam);    
            
    }
	
	
	return 0;
	
}




int CreateShiftWindow() {

		// Code For Creating a Window for Specifying Region
		// A borderless, invisible window used only for capturing mouse input for the whole screen
	    HINSTANCE hInstance = AfxGetInstanceHandle( );

		WNDCLASS wndclass;

		wndclass.style = 0;
        wndclass.lpfnWndProc = (WNDPROC)MouseCaptureWndProc;
        wndclass.cbClsExtra = 0;
        wndclass.cbWndExtra = 0;
        wndclass.hInstance = hInstance;
        wndclass.hIcon = LoadIcon(hInstance, "WINCAP");
        
		HICON hcur= LoadIcon(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDI_ICONCROSSHAIR));
		//wndclass.hCursor = LoadCursor(NULL, IDC_CROSS);
		wndclass.hCursor = hcur;
        
        wndclass.hbrBackground = NULL;

        //wndclass.lpszMenuName = (LPSTR)"MAINMENU";
		wndclass.lpszMenuName = NULL;
        wndclass.lpszClassName = (LPSTR) "ShiftRegionWindow";

        if (!RegisterClass(&wndclass))
            return 0;
        
		HDC hScreenDC=::GetDC(NULL);
		maxxScreen = GetDeviceCaps(hScreenDC,HORZRES);
		maxyScreen = GetDeviceCaps(hScreenDC,VERTRES);
		::ReleaseDC(NULL,hScreenDC);		
		
		hMouseCaptureWnd = CreateWindowEx(WS_EX_TOPMOST,"ShiftRegionWindow", "Title",WS_POPUP,0,0,maxxScreen,maxyScreen,NULL,NULL, hInstance,  NULL);

		return 0;

}

int InitSelectRegionWindow() {


	return 0;


}

int InitDrawShiftWindow() {

			
		HDC hScreenDC = ::GetDC(hMouseCaptureWnd);

		FixRectSizePos(&rc, maxxScreen,  maxyScreen);			

		rcClip.left=rc.left;
		rcClip.top=rc.top;
		rcClip.right=rc.right;
		rcClip.bottom=rc.bottom;
		DrawSelect(hScreenDC, TRUE, &rcClip);

		old_rcClip = rcClip;

		//Set Curosr at the centre of the clip rectangle
		POINT   ptOrigin;
		ptOrigin.x=(rcClip.right+rcClip.left)/2;
		ptOrigin.y=(rcClip.top+rcClip.bottom)/2;
		
		rcOffset.left=rcClip.left-ptOrigin.x;
		rcOffset.top=rcClip.top-ptOrigin.y;
		rcOffset.right=rcClip.right-ptOrigin.x;
		rcOffset.bottom=rcClip.bottom-ptOrigin.y;  

		::ReleaseDC(hMouseCaptureWnd,hScreenDC);

		return 0;

}


int DestroyShiftWindow() {

	if (hMouseCaptureWnd) ::DestroyWindow(hMouseCaptureWnd);
	return 0;
}





///////////////////////// //////////////////
/////////////// Functions //////////////////
///////////////////////// //////////////////

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


UINT RecordAVIThread(LPVOID pParam) {

	int top=rcUse.top;
	int left=rcUse.left;
	int width=rcUse.right-rcUse.left+1;
	int height=rcUse.bottom - rcUse.top + 1;
	int fps=frames_per_second;
	
	CString fileName("\\~temp.avi");
	tempfilepath = GetTempPath () + fileName;

	//Test the validity of writing to the file
	//Make sure the file to be created is currently not used by another application
	int fileverified = 0;
	while (!fileverified) 
	{
		OFSTRUCT ofstruct;	
		HFILE fhandle = OpenFile( tempfilepath, &ofstruct, OF_SHARE_EXCLUSIVE | OF_WRITE  | OF_CREATE );  
		if (fhandle != HFILE_ERROR) {
			fileverified = 1;
			CloseHandle( (HANDLE) fhandle );
			DeleteFile(tempfilepath);
		}	 
		else {
			srand( (unsigned)time( NULL ) );
			int randnum = rand();
			char numstr[50];
			sprintf(numstr,"%d",randnum);

			CString cnumstr(numstr);
			CString fxstr("\\~temp");
			CString exstr(".avi");
			tempfilepath = GetTempPath () + fxstr + cnumstr + exstr;

		}
 
	} 	
	
	RecordVideo(top,left,width,height,fps,tempfilepath);

	return 0;
}

//int cc=0;
//int unshifted = 1;
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// RecordVideo
//
// The main function used in the recording of video
// Includes opening/closing avi file, initializing avi settings, capturing frames, applying cursor effects etc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int RecordVideo(int top,int left,int width,int height,int fps,const char *szFileName) {

	LPBITMAPINFOHEADER alpbi;
	AVISTREAMINFO strhdr;
	PAVIFILE pfile = NULL;
	PAVISTREAM ps = NULL, psCompressed = NULL;
	AVICOMPRESSOPTIONS opts;
	AVICOMPRESSOPTIONS FAR * aopts[1] = {&opts};
	HRESULT hr;
	WORD wVer;
	char szTitle[BUFSIZE];

	actualwidth=width;
	actualheight=height;	
	
	wVer = HIWORD(VideoForWindowsVersion());
	if (wVer < 0x010a){			 
  		
		//MessageBox(NULL, "Error! Video for Windows version too old!", "Error" , MB_OK|MB_ICONSTOP);
		MessageOut(NULL,IDS_STRING_VERSIONOLD ,IDS_STRING_NOTE,MB_OK | MB_ICONSTOP);

		return FALSE;
	}

	alpbi = NULL;

	////////////////////////////////////////////////
	// CAPTURE FIRST FRAME
	////////////////////////////////////////////////	
	alpbi=captureScreenFrame(left,top,width, height,1);


	////////////////////////////////////////////////
	// TEST VALIDITY OF COMPRESSOR
	//////////////////////////////////////////////////	
	
	if (selected_compressor > 0) {
	
		HIC hic = NULL;		
		hic = ICOpen(compressor_info[selected_compressor].fccType, compressor_info[selected_compressor].fccHandler, ICMODE_QUERY);
		if (hic) {

			
			int newleft,newtop,newwidth,newheight;
			int align = 1;
			while   (ICERR_OK!=ICCompressQuery(hic, alpbi, NULL))
			{		
				//Try adjusting width/height a little bit 
				align = align * 2 ;		
				if (align>8) break;				


				newleft=left;
				newtop=top;
				int wm = (width % align);
				if (wm > 0) {
					newwidth = width + (align - wm);
					if (newwidth>maxxScreen) 
						newwidth = width - wm;
				}


				int hm = (height % align);
				if (hm > 0) {
					newheight = height + (align - hm);
					if (newheight>maxyScreen) 
						newwidth = height - hm;
				}
				
				
				if (alpbi) FreeFrame(alpbi);
				alpbi=captureScreenFrame(newleft,newtop,newwidth, newheight,1);	
					

			}

			//if succeed with new width/height, use the new width and height
			//else if still fails ==> default to MS Video 1 (MSVC)							
			if (align == 1) {
				
				//Compressor has no problem with the current dimensions...so proceed
				//do nothing here
			}
			else if  (align <= 8) {			
					
					//Compressor can work if the dimensions is adjusted slightly
					left=newleft;
					top=newtop;
					width=newwidth;
					height=newheight;	

					actualwidth=newwidth;
					actualheight=newheight;
				
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

	
	/*
	//Special Cases
	//DIVX
	if (compfccHandler==mmioFOURCC('D', 'I', 'V', 'X'))	
	{ //Still Can't Handle DIVX
	  
		compfccHandler = mmioFOURCC('M', 'S', 'V', 'C');	
		strCodec = CString("MS Video 1");
			
	}
	*/

	
	//IV50
	if (compfccHandler==mmioFOURCC('I', 'V', '5', '0'))	
	{ //Still Can't Handle Indeo 5.04

		compfccHandler = mmioFOURCC('M', 'S', 'V', 'C');	
		strCodec = CString("MS Video 1");
		
	}


	
	////////////////////////////////////////////////
	// Set Up Flashing Rect
	////////////////////////////////////////////////
	if (flashingRect) {
		if (autopan) 	
			pFrame->SetUpRegion(left,top,width,height,1);	
		else
			pFrame->SetUpRegion(left,top,width,height,0);
		pFrame->ShowWindow(SW_SHOW);
	}
	

		
	////////////////////////////////////////////////
	// INIT AVI USING FIRST FRAME
	////////////////////////////////////////////////
	AVIFileInit();

	//
	// Open the movie file for writing....
	//
	strcpy(szTitle,"AVI Movie");
	 
	hr = AVIFileOpen(&pfile, szFileName, OF_WRITE | OF_CREATE, NULL);	
	if (hr != AVIERR_OK) goto error;


	// Fill in the header for the video stream....
	// The video stream will run in 15ths of a second....
	_fmemset(&strhdr, 0, sizeof(strhdr));
	strhdr.fccType                = streamtypeVIDEO;// stream type
	
	//strhdr.fccHandler             = compfccHandler;
	strhdr.fccHandler             = 0;
	
	strhdr.dwScale                = 1;
	strhdr.dwRate                 = fps;		    
	strhdr.dwSuggestedBufferSize  = alpbi->biSizeImage;
	SetRect(&strhdr.rcFrame, 0, 0,		    // rectangle for stream
	    (int) alpbi->biWidth,
	    (int) alpbi->biHeight);

	// And create the stream;
	hr = AVIFileCreateStream(pfile,	&ps, &strhdr);
	if (hr != AVIERR_OK) 	goto error; 
	
	
	memset(&opts, 0, sizeof(opts)); 
	aopts[0]->fccType			 = streamtypeVIDEO;
	//aopts[0]->fccHandler		 = mmioFOURCC('M', 'S', 'V', 'C');
	aopts[0]->fccHandler		 = compfccHandler;
	aopts[0]->dwKeyFrameEvery	   = keyFramesEvery;		// keyframe rate 
	aopts[0]->dwQuality		 = compquality;        // compress quality 0-10,000 
	aopts[0]->dwBytesPerSecond	         = 0;		// bytes per second 
	aopts[0]->dwFlags			 = AVICOMPRESSF_VALID | AVICOMPRESSF_KEYFRAMES;    // flags 		
	aopts[0]->lpFormat			 = 0x0;                         // save format 
	aopts[0]->cbFormat			 = 0;
	aopts[0]->dwInterleaveEvery = 0;			// for non-video streams only 
	

	//ver 2.26
	if (RecordingMode == ModeFlash)
	{
		//Internally adjust codec to MSVC 100 Quality 
		aopts[0]->fccHandler = mmioFOURCC('M', 'S', 'V', 'C');	 //msvc
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

				aopts[0]->lpParms			 = pParamsUse;    
				aopts[0]->cbParms			 = CompressorStateSize;

			}

		}

	}
	
	
	

	//The 1 here indicates only 1 stream
	//if (!AVISaveOptions(NULL, 0, 1, &ps, (LPAVICOMPRESSOPTIONS *) &aopts)) 
	//        goto error;
	

	hr = AVIMakeCompressedStream(&psCompressed, ps, &opts, NULL);
	if (hr != AVIERR_OK)  	goto error; 

	hr = AVIStreamSetFormat(psCompressed, 0, 
			       alpbi,	    // stream format
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


	//////////////////////////////////////////////
	// Recording Audio
	//////////////////////////////////////////////	
	if ((recordaudio==2) || (useMCI)) 
	{
		
		mciRecordOpen();
		mciSetWaveFormat();
		mciRecordStart();
	
		/*
		
		if (shiftType == 1)
		{
			mciRecordPause(tempaudiopath);
			unshifted = 1;
		
		}
		*/
		

	}
	else if (recordaudio) {
		InitAudioRecording();
		StartAudioRecording(&m_Format);

	}


	if (shiftType == 2)
	{		
		Sleep(timeshift);
	}
	


	DWORD timeexpended, frametime, oldframetime;		
	
	initialtime = timeGetTime();		
	initcapture = 1;	
	oldframetime = 0;
	nCurrFrame = 0;
	nActualFrame = 0;

	//timeshift = 100;

	

	//////////////////////////////////////////////
	// WRITING FRAMES
	//////////////////////////////////////////////	

	long divx, oldsec;
	divx=0;
	oldsec=0;
	while (recordstate) {  //repeatedly loop

		if (initcapture==0) {			 
			
			timeexpended = timeGetTime() - initialtime;			

		}
		else {
			
			frametime = 0;
			timeexpended = 0;			
			
		}		

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

					alpbi=captureScreenFrame(panrect_current.left,panrect_current.top,width, height,0);					
			
		}
		else {

			//ver 1.8
			//moving region
			readingRegion = 0;
			while (writingRegion)
			{

			}
			readingRegion = 1;

			if (newRegionUsed) {

				left = newRect.left;
				top = newRect.top;
				newRegionUsed = 0;
				//width and height unchanged

			}

			readingRegion = 0;			
			
			
			alpbi=captureScreenFrame(left,top,width, height,0);		
			

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
			if (int(fTimeLength) >= presettime)
				//recordstate = 0; 
				PostMessage(hWndGlobal,WM_USER_RECORDINTERRUPTED,0,0);


			//CString msgStr;
			//msgStr.Format("%.2f %d",fTimeLength,presettime);
			//MessageBox(NULL,msgStr,"N",MB_OK);
							

				//or should we post messages
		}

		
		/*
		if ((shiftType == 1) && (unshifted))
		{
			cc++;
			unsigned long thistime = timeGetTime();
			int diffInTime = thistime - initialtime;
			if (diffInTime >= timeshift)
			{
				
				ErrMsg("cc %d diffInTime %d",cc-1,diffInTime);
				
				if ((recordaudio==2) || (useMCI)) 
				{
					mciRecordResume(tempaudiopath);					
					unshifted = 0;
				}
			

			}

		}
		*/
		
		
		if ((frametime==0) || (frametime>oldframetime)) { 

			
			/*
			//ver 1.8
			if (shiftType == 1)
			{
				if (frametime==0)
				{
					//Discard .. do nothing

				}
				else {

					//writr old frame time instead
					hr = AVIStreamWrite(psCompressed,	// stream pointer
					oldframetime,				// time of this frame
					1,				// number to write
					(LPBYTE) alpbi +		// pointer to data
						alpbi->biSize +
						alpbi->biClrUsed * sizeof(RGBQUAD),
						alpbi->biSizeImage,	// size of this frame
					//AVIIF_KEYFRAME,			 // flags....
					0,    //Dependent n previous frame, not key frame
					NULL,
					NULL);

				}

			}
			else {
			*/


			
				//if frametime repeats (frametime == oldframetime) ...the avistreamwrite will cause an error
				hr = AVIStreamWrite(psCompressed,	// stream pointer
					frametime,				// time of this frame
					1,				// number to write
					(LPBYTE) alpbi +		// pointer to data
						alpbi->biSize +
						alpbi->biClrUsed * sizeof(RGBQUAD),
						alpbi->biSizeImage,	// size of this frame
					//AVIIF_KEYFRAME,			 // flags....
					0,    //Dependent n previous frame, not key frame
					NULL,
					NULL);

			//}

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
				InvalidateRect(hWndGlobal,NULL, FALSE);				
			}			
			
			//free memory
			FreeFrame(alpbi);
			alpbi=NULL;

			oldframetime = frametime;


		} // if frametime is different


		
		//Version 1.1
		int haveslept = 0;
		int pausecounter = 0;
		int pauseremainder = 0;
		int pauseindicator = 1;
		DWORD timestartpause;
		DWORD timeendpause;
		DWORD timedurationpause;		
		while (recordpaused) {			

						
			if (!haveslept) timestartpause = timeGetTime();
			
			
		
			//Flash Pause Indicator in Title Bar
			pausecounter++;
			if ((pausecounter % 8)==0) { 
				
				//if after every 400 milliseconds (8 * 50)				
				if (pauseindicator) {
					SetTitleBar("");
					pauseindicator = 0;
				}
				else {
					SetTitleBar("Paused");
					pauseindicator = 1;
				}							

			}

			if ((recordaudio==2) || (useMCI)) {		

				if (alreadyMCIPause == 0)
				{
					mciRecordPause(tempaudiopath);
					alreadyMCIPause = 1;
				}

			}

			//do nothing.. wait
			::Sleep(50); //Sleep for 50 			
			
			haveslept=1;

		}

		//Version 1.1
		if (haveslept) {

			if ((recordaudio==2) || (useMCI)) {			
				
				if (alreadyMCIPause == 1)
				{
					mciRecordResume(tempaudiopath);
					alreadyMCIPause = 0;
				}
			}

			timeendpause = timeGetTime();			
			timedurationpause =  timeendpause - timestartpause;

			//CString msgstr;
			//msgstr.Format("timestartpause %ld\ntimeendpause %ld\ntimedurationpause %ld",timeendpause,timeendpause,timedurationpause);
			//MessageBox(NULL,msgstr,"Note",MB_OK);

			initialtime = initialtime + timedurationpause;
	

		}
		else  {
		
			//introduce time lapse
			//maximum lapse when recordstate changes will be less than 100 milliseconds
			int no_iteration = timelapse/50;
			int remainlapse = timelapse - no_iteration*50;		 
			for (int j=0;j<no_iteration;j++)		
			{
				::Sleep(50); //Sleep for 50 milliseconds many times
				if (recordstate==0) break;
			}		
			if (recordstate==1) Sleep(remainlapse);

		}
		

	} //for loop
	
	
error:	
	
	//
	// Now close the file
	//		

	if (flashingRect) {
		pFrame->ShowWindow(SW_HIDE);
	}


	//Ver 1.2
	//
	if ((compfccHandler == CompressorStateIsFor) && (compfccHandler != 0)) {

			//Detach pParamsUse from AVICOMPRESSOPTIONS so AVISaveOptionsFree will not free it
			//(we will free it ourselves)

			//Setting this is no harm even if pParamsUse is not attached to lpParams
			aopts[0]->lpParms			 = 0;    
			aopts[0]->cbParms			 = 0;		

	}


	AVISaveOptionsFree(1,(LPAVICOMPRESSOPTIONS FAR *) &aopts);	


	//////////////////////////////////////////////
	// Recording Audio
	//////////////////////////////////////////////	
	if ((recordaudio==2) || (useMCI)) {	

		GetTempWavePath();	
		mciRecordStop(tempaudiopath);
		mciRecordClose();

		//restoreWave();

	}
	else if (recordaudio) {

		StopAudioRecording();
		ClearAudioFile();

	}
	

	if (pfile)
		AVIFileClose(pfile);
	
	
	if (ps)
		AVIStreamClose(ps);
	
	
	if (psCompressed)
		AVIStreamClose(psCompressed);

	AVIFileExit();	

	
	if (hr != NOERROR) 	{
		
		PostMessage(hWndGlobal,WM_USER_RECORDINTERRUPTED,0,0);

		/*
		char *ErrorBuffer; // This really is a pointer - not reserved space!
		FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | 	FORMAT_MESSAGE_FROM_SYSTEM, NULL, GetLastError(), MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US), (LPTSTR)&ErrorBuffer, 0, NULL);

		CString reasonstr(ErrorBuffer);
		CString errorstr("File Creation Error. Unable to rename file.\n\n");
		CString reportstr;

		reportstr = errorstr + reasonstr;
		//MessageBox(NULL,reportstr,"Note",MB_OK | MB_ICONEXCLAMATION);
		*/
		
		
		if (compfccHandler != mmioFOURCC('M', 'S', 'V', 'C'))	{

			//if (IDYES == MessageBox(NULL, "Error recording AVI file using current compressor. Use default compressor ? ", "Note", MB_YESNO | MB_ICONEXCLAMATION)) {
			if (IDYES == MessageOut(NULL, IDS_STRING_ERRAVIDEFAULT, IDS_STRING_NOTE, MB_YESNO | MB_ICONQUESTION	)) {

				compfccHandler = mmioFOURCC('M', 'S', 'V', 'C');
				strCodec = "MS Video 1";
				PostMessage(hWndGlobal,WM_USER_RECORDSTART,0,0);
			}			

		}
		else
			//MessageBox(NULL, "Error Creating AVI File", "Error", MB_OK | MB_ICONEXCLAMATION);			
			MessageOut(NULL,IDS_STRING_ERRCREATEAVI ,IDS_STRING_NOTE,MB_OK | MB_ICONEXCLAMATION);



		
		return 0;

	}


	//Save the file on success
	PostMessage(hWndGlobal,WM_USER_GENERIC,0,0);	

	return 0;

}



//Round positive numbers
long rounddouble(double dbl) {

	long num = (long) dbl;
	double diff = dbl - (double) num;
	if (diff>=0.5) num++;

	return num;

}



LPBITMAPINFOHEADER captureScreenFrame(int left,int top,int width, int height,int tempDisableRect)
{

	
	HDC hScreenDC = ::GetDC(NULL);

	
	//if flashing rect
	if (flashingRect && !tempDisableRect) {

		if (autopan) {				
						
			pFrame->SetUpRegion(left,top,width,height,1);				
			DrawFlashingRect( TRUE , 1);			

		}
		else		
			DrawFlashingRect( TRUE , 0);

	}	
	
	
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
	
	
	
	//Get Cursor Pos
	POINT xPoint; 
	GetCursorPos( &xPoint ); 
	HCURSOR hcur= FetchCursorHandle();
	xPoint.x-=left;
	xPoint.y-=top;

	
	//Draw the HighLight	
	if (g_highlightcursor==1) {	

		POINT highlightPoint; 		

		highlightPoint.x = xPoint.x -64 ;
		highlightPoint.y = xPoint.y -64 ;		
		
		InsertHighLight( hMemDC, highlightPoint.x, highlightPoint.y);

	}
	
	//Draw the Cursor	
	if (g_recordcursor==1) {
	
	
		
		ICONINFO iconinfo ;	
		BOOL ret;
		ret	= GetIconInfo( hcur,  &iconinfo ); 
		if (ret) {

			xPoint.x -= iconinfo.xHotspot;
			xPoint.y -= iconinfo.yHotspot;

			//need to delete the hbmMask and hbmColor bitmaps
			//otherwise the program will crash after a while after running out of resource
			if (iconinfo.hbmMask) DeleteObject(iconinfo.hbmMask);
			if (iconinfo.hbmColor) DeleteObject(iconinfo.hbmColor);

		}		
		
		
		::DrawIcon( hMemDC,  xPoint.x,  xPoint.y, hcur); 							

	}
	
	
	SelectObject(hMemDC,oldbm);    			
	LPBITMAPINFOHEADER pBM_HEADER = (LPBITMAPINFOHEADER)GlobalLock(Bitmap2Dib(hbm, bits));	
	//LPBITMAPINFOHEADER pBM_HEADER = (LPBITMAPINFOHEADER)GlobalLock(Bitmap2Dib(hbm, 24));	
	if (pBM_HEADER == NULL) { 
			
		//MessageBox(NULL,"Error reading a frame!","Error",MB_OK | MB_ICONEXCLAMATION);					
		MessageOut(NULL,IDS_STRING_ERRFRAME ,IDS_STRING_NOTE,MB_OK | MB_ICONEXCLAMATION);

		exit(1);
	}    

	DeleteObject(hbm);			
	DeleteDC(hMemDC);	
	
	
	//if flashing rect
	if (flashingRect && !tempDisableRect) {
		
		if (autopan)	{
			DrawFlashingRect(FALSE , 1);
		}
		else
			DrawFlashingRect(FALSE , 0);

	}	
	
	
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

	if (g_cursortype == 0) {

		if (hSavedCursor == NULL) 
			hSavedCursor = GetCursor();

		return hSavedCursor;

	}
	else if (g_cursortype == 1) {

		return g_customcursor ;

	}
	else {

		return g_loadcursor;
	}


}





void DrawClientArea(HWND hwnd, HDC hdc) {

	RECT rect;
	::GetClientRect(hwnd, &rect );



	//OffScreen Buffer
	HBITMAP hbm = NULL;
	HBITMAP old_bitmap;
	HDC hdcBits = CreateCompatibleDC(hdc);
    hbm = (HBITMAP) CreateCompatibleBitmap(hdc,rect.right-rect.left+1,rect.bottom-rect.top+1);    	
	old_bitmap = (HBITMAP) SelectObject(hdcBits,hbm);
		
		
	//Drawing to OffScreen Buffer
	//TRACE("\nRect coords: %d %d %d %d ",rect.left,rect.top,rect.right,rect.bottom);	

	HBRUSH dgbrush = (HBRUSH) ::GetStockObject(BLACK_BRUSH);
	HBRUSH oldbrush = (HBRUSH)  ::SelectObject(hdcBits,dgbrush);

	::Rectangle(hdcBits, rect.left,rect.top,rect.right,rect.bottom);
	
	//Display the record information when recording
	if (recordstate==1) {


		//Ver 1.1
		COLORREF rectcolor;
		COLORREF textcolor;

		if (nColors <= 8) {

			rectcolor = RGB(255,255,255);
			textcolor = RGB(0,0,128);
		}
		else {
		
			rectcolor = RGB(225,225,225);
			textcolor = RGB(0,0,100);
		}

		RECT txRect;
		
		int xoffset =10 ;
		int yoffset =10 ;

		HFONT hFont = (HFONT) ::GetStockObject(ANSI_VAR_FONT);
		HFONT hOld_Font = (HFONT) ::SelectObject(hdcBits,hFont);

		
		COLORREF oldTextColor = SetTextColor(hdcBits, textcolor);
		COLORREF oldBkColor = SetBkColor(hdcBits,rectcolor);

		HBRUSH bkBrush = CreateSolidBrush(rectcolor);
		HBRUSH old2_Brush = (HBRUSH) ::SelectObject(hdcBits,bkBrush);


		char msgstr[100];
		//CString msgstr;
		SIZE Extent;				

		//CString fmtstr;		
		//fmtstr.LoadString(IDS_STRING_CURRFRAME);		
		//msgstr.Format(LPCTSTR(fmtstr),nCurrFrame);		
		sprintf(msgstr,"Current Frame : %d", nCurrFrame);
		GetTextExtentPoint32( hdcBits,msgstr, strlen(msgstr), &Extent); 			
		txRect = rect;
		txRect.top = yoffset -2 ;
		txRect.bottom = yoffset + Extent.cy + 4 ;		
		::Rectangle(hdcBits, txRect.left,txRect.top,txRect.right,txRect.bottom);
		TextOut(hdcBits,xoffset,yoffset, msgstr, strlen(msgstr));
		
		
		/*
		yoffset+=Extent.cy+10;
		sprintf(msgstr,"Theoretical Frame Rate  : %.2f fps", fRate);
		GetTextExtentPoint32( hdcBits,msgstr, strlen(msgstr), &Extent); 			
		txRect.top = yoffset -2 ;
		txRect.bottom = yoffset + Extent.cy + 4 ;
		::Rectangle(hdcBits, txRect.left,txRect.top,txRect.right,txRect.bottom);
		TextOut(hdcBits,xoffset,yoffset, msgstr, strlen(msgstr));
		*/

		yoffset+=Extent.cy+10;		
		//fmtstr.LoadString(IDS_STRING_TIMEELASPED);		
		//msgstr.Format(LPCTSTR(fmtstr),fTimeLength);				
		sprintf(msgstr,"Time Elasped  : %.2f sec", fTimeLength);
		GetTextExtentPoint32( hdcBits,msgstr, strlen(msgstr), &Extent); 			
		txRect.top = yoffset -2 ;
		txRect.bottom = yoffset + Extent.cy + 4 ;
		::Rectangle(hdcBits, txRect.left,txRect.top,txRect.right,txRect.bottom);
		TextOut(hdcBits,xoffset,yoffset, msgstr, strlen(msgstr));

		yoffset+=Extent.cy+10;		
		//fmtstr.LoadString(IDS_STRING_NUMCOLORS);		
		//msgstr.Format(LPCTSTR(fmtstr),nColors);				
		sprintf(msgstr,"Number of Colors  : %d bits", nColors);
		GetTextExtentPoint32( hdcBits,msgstr, strlen(msgstr), &Extent); 			
		txRect.top = yoffset -2 ;
		txRect.bottom = yoffset + Extent.cy + 4 ;
		::Rectangle(hdcBits, txRect.left,txRect.top,txRect.right,txRect.bottom);
		TextOut(hdcBits,xoffset,yoffset, msgstr, strlen(msgstr));

		yoffset+=Extent.cy+10;
		//fmtstr.LoadString(IDS_STRING_CODEC);		
		//msgstr.Format(LPCTSTR(fmtstr),LPCTSTR(strCodec));
		sprintf(msgstr,"Codec  : %s", LPCTSTR(strCodec));		
		GetTextExtentPoint32( hdcBits,msgstr, strlen(msgstr), &Extent); 			
		txRect.top = yoffset -2 ;
		txRect.bottom = yoffset + Extent.cy + 4 ;
		::Rectangle(hdcBits, txRect.left,txRect.top,txRect.right,txRect.bottom);
		TextOut(hdcBits,xoffset,yoffset, msgstr, strlen(msgstr));

		
		yoffset+=Extent.cy+10;		
		//fmtstr.LoadString(IDS_STRING_ACTUALRATE);		
		//msgstr.Format(LPCTSTR(fmtstr),fActualRate);	
		sprintf(msgstr,"Actual Input Rate  : %.2f fps", fActualRate);		
		GetTextExtentPoint32( hdcBits,msgstr, strlen(msgstr), &Extent); 			
		txRect.top = yoffset -2 ;
		txRect.bottom = yoffset + Extent.cy + 4 ;
		::Rectangle(hdcBits, txRect.left,txRect.top,txRect.right,txRect.bottom);
		TextOut(hdcBits,xoffset,yoffset, msgstr, strlen(msgstr));

		yoffset+=Extent.cy+10;
		//fmtstr.LoadString(IDS_STRING_DIMENSION);		
		//msgstr.Format(LPCTSTR(fmtstr),actualwidth,actualheight);			
		sprintf(msgstr,"Dimension  : %d X %d", actualwidth,actualheight);		
		GetTextExtentPoint32( hdcBits,msgstr, strlen(msgstr), &Extent); 			
		txRect.top = yoffset -2 ;
		txRect.bottom = yoffset + Extent.cy + 4 ;
		::Rectangle(hdcBits, txRect.left,txRect.top,txRect.right,txRect.bottom);
		TextOut(hdcBits,xoffset,yoffset, msgstr, strlen(msgstr));
	

		::SelectObject(hdcBits,old2_Brush);
		DeleteObject(bkBrush);
		
		SetBkColor(hdcBits, oldBkColor);		
		SetTextColor(hdcBits, oldTextColor);

		::SelectObject(hdcBits,hOld_Font);	

	
	}
	else {

		//Ver 1.1
		if (nColors >= 8)
			SelectObject(hdcBits,hLogoBM);


				
	}

	
	::SelectObject(hdcBits,oldbrush);



	//OffScreen Buffer
	BitBlt(hdc, 0, 0, rect.right-rect.left+1, rect.bottom-rect.top+1, hdcBits, 0, 0, SRCCOPY);              
	SelectObject(hdcBits, old_bitmap);        
	DeleteObject(hbm);
    DeleteDC(hdcBits);


	
	//ver 2.26
	//Draw Message msgRecMode
	if (recordstate==0) {		
		
		CString msgRecMode;
		//CString msgRecAVI("Record to AVI");
		CString msgRecAVI;
		msgRecAVI.LoadString(IDS_RECAVI);
 		//CString msgRecFlash("Record to SWF");
		CString msgRecFlash;
		msgRecFlash.LoadString(IDS_RECSWF);

		if (RecordingMode == ModeAVI)
		{
			msgRecMode = msgRecAVI;			
		}
		else
		{
			msgRecMode = msgRecFlash;

		}

		//LPBITMAPINFO pbmiText = GetTextBitmap(pDC, &CRect(clrect),m_factor,&m_tracker.m_rect, &m_textfont, m_textstring, NULL, NULL, rgb, m_horzalign); 
		//HBITMAP newbm = DrawResampleRGB(pDC, &CRect(clrect),m_factor, (LPBITMAPINFOHEADER) pbmiText); 
		


		int xoffset =12 ;
		int yoffset =12 ;
		RECT tmodeRect;
		SIZE Extent;

		//HPEN solidPen = (HPEN) ::GetStockObject(_PEN);
		HPEN solidPen = (HPEN) ::CreatePen(PS_SOLID,1,RGB(225,225,225));
		HPEN old2_Pen = (HPEN) ::SelectObject(hdc,solidPen);

		HBRUSH bkBrush = CreateSolidBrush(RGB(0,0,0));
		HBRUSH old2_Brush = (HBRUSH) ::SelectObject(hdc,bkBrush);

		HFONT hFont = (HFONT) ::GetStockObject(ANSI_VAR_FONT);
		//HFONT hFont = (HFONT) ::GetStockObject(SYSTEM_FONT);
		HFONT hOld_Font = (HFONT) ::SelectObject(hdc,hFont);

		COLORREF oldTextColor = SetTextColor(hdc, RGB(225,225,225));
		COLORREF oldBkColor = SetBkColor(hdc,RGB(0,0,0));


		GetTextExtentPoint32( hdc, LPCTSTR(msgRecMode), msgRecMode.GetLength(), &Extent); 			
		
		int xmove = (rect.right - rect.left + 1) - (Extent.cx) - xoffset;
		//int ymove = (rect.bottom - rect.top + 1) - (Extent.cy) - yoffset;
		int ymove = yoffset;
		//ErrMsg("xmove %d",xmove);
		tmodeRect.left= xmove;
		tmodeRect.right = tmodeRect.left + Extent.cx ;
		tmodeRect.top = ymove  ;
		tmodeRect.bottom = tmodeRect.top + Extent.cy  ;
		::Rectangle(hdc, tmodeRect.left,tmodeRect.top,tmodeRect.right,tmodeRect.bottom);
		
		::Rectangle(hdc, tmodeRect.left - 3,tmodeRect.top - 3,tmodeRect.right + 3,tmodeRect.bottom + 3);
		//::RoundRect(hdc, tmodeRect.left - 3,tmodeRect.top - 3,tmodeRect.right + 3,tmodeRect.bottom + 3,5,5);
		TextOut(hdc,tmodeRect.left,tmodeRect.top, LPCTSTR(msgRecMode), msgRecMode.GetLength());

				
		SetBkColor(hdc, oldBkColor);		
		SetTextColor(hdc, oldTextColor);
		::SelectObject(hdc,hOld_Font);	


		::SelectObject(hdc,old2_Brush);
		DeleteObject(bkBrush);

		::SelectObject(hdc,old2_Pen);
		DeleteObject(solidPen);


	}


}

BOOL CVscapView::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) 
{
	// TODO: Add your specialized code here and/or call the base class	
	BOOL result = CWnd::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
	
	//Can we put the following line here to get the m_hWnd of this CWnd ? 
	hWndGlobal = m_hWnd;


	//ver 1.2
	LoadSettings();
	

	//ver 1.2
	//InstallMyKeyHook(hWndGlobal,WM_USER_KEYSTART);
	int val = SetAdjustHotKeys();


	//ver 1.2
	initTrayIconData();
	TraySetIcon(IDR_MAINFRAMESMALL);
	TraySetToolTip("CamStudio");
	TraySetMenu(IDR_TRAYMENU);
	TrayShow();

	return result;

}

void CVscapView::OnRecord() 
{
	// TODO: Add your command handler code here
	CStatusBar* pStatus = (CStatusBar*) AfxGetApp()->m_pMainWnd->GetDescendantWindow(AFX_IDW_STATUS_BAR);
	pStatus->SetPaneText(0,"Press the Stop Button to stop recording");

	
	//Version 1.1
	if (recordpaused) {
		
		recordpaused = 0;

		//ver 1.8
		//if (recordaudio==2) 
		//	mciRecordResume(tempaudiopath);

		//Set Title Bar     
		SetTitleBar("CamStudio");
		
		return;	

	}
	recordpaused = 0;


	nActualFrame=0;
	nCurrFrame=0;
	fRate=0.0;
	fActualRate=0.0;
	fTimeLength=0.0;

	if (MouseCaptureMode==0) {

		if (fixedcapture) {

			rc.top=capturetop;
			rc.left=captureleft;
			rc.right=captureleft+capturewidth-1;
			rc.bottom=capturetop+captureheight-1;	

			
			if (rc.top<0) rc.top=0;
			if (rc.left<0) rc.left=0;
			if (rc.right>maxxScreen-1) rc.right=maxxScreen-1;
			if (rc.bottom>maxyScreen-1) rc.bottom=maxyScreen-1;

			//using protocols for MouseCaptureMode==0
			rcClip = rc;
			old_rcClip=rcClip;
			NormalizeRect(&old_rcClip);
			CopyRect(&rcUse, &old_rcClip);

			::PostMessage (hWndGlobal,WM_USER_RECORDSTART,0,(LPARAM) 0); 


		}
		else {
	
		
			rc.top=0;
			rc.left=0;
			rc.right=capturewidth-1;
			rc.bottom=captureheight-1;	

			::ShowWindow(hMouseCaptureWnd,SW_MAXIMIZE);
			::UpdateWindow(hMouseCaptureWnd);

			InitDrawShiftWindow(); //will affect rc implicity

		}

	}
	else if (MouseCaptureMode==1) {

		::ShowWindow(hMouseCaptureWnd,SW_MAXIMIZE);
		::UpdateWindow(hMouseCaptureWnd);

		InitSelectRegionWindow(); //will affect rc implicity

	}
	else if (MouseCaptureMode==2) {

		rcUse.left=0;
		rcUse.top=0;
		rcUse.right= maxxScreen-1;
		rcUse.bottom= maxyScreen-1;
		::PostMessage (hWndGlobal,WM_USER_RECORDSTART,0,(LPARAM) 0); 

	}	
	
}


void CVscapView::OnStop() 
{
	
	//Version 1.1
	if (recordstate==0) return;

	if (recordpaused) {
		
		recordpaused=0;

		//Set Title Bar     
		SetTitleBar("CamStudio");

	}

	
	OnRecordInterrupted (0, 0);
	
}



void CVscapView::OnUpdateRegionPanregion(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	if (MouseCaptureMode==0) pCmdUI->SetCheck(TRUE);
	else pCmdUI->SetCheck(FALSE);
	
}

void CVscapView::OnUpdateRegionRubber(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	if (MouseCaptureMode==1) pCmdUI->SetCheck(TRUE);
	else pCmdUI->SetCheck(FALSE);
	
}



//ver 1.6
#define MAXCOMPRESSORS 50
void CVscapView::OnFileVideooptions() 
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

    first_alpbi=captureScreenFrame(left,top,width, height,1);		

	

	num_compressor =0;
	if  (compressor_info == NULL) {
		compressor_info = (ICINFO *) calloc(MAXCOMPRESSORS,sizeof(ICINFO));
	}
	else {

		free(compressor_info);
		compressor_info = (ICINFO *) calloc(MAXCOMPRESSORS,sizeof(ICINFO));

	}	
	
	
	for(int i=0; ICInfo(ICTYPE_VIDEO, i, &compressor_info[num_compressor]); i++) {


		if (num_compressor>=MAXCOMPRESSORS) break; //maximum allows 30 compressors		
		
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
							num_compressor ++ ;				
							
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
					hic = ICOpen(compressor_info[num_compressor].fccType, compressor_info[num_compressor].fccHandler, ICMODE_QUERY);
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
	
	
	CVideoOptions vod;
	vod.DoModal();
	
	
}

void CVscapView::OnOptionsCursoroptions() 
{
	// TODO: Add your command handler code here
	CCursorOptionsDlg cod;
	cod.DoModal();

	
}


void InsertHighLight(HDC hdc,int xoffset, int yoffset)
{

	CSize fullsize;
	fullsize.cx=128;
	fullsize.cy=128;

	int highlightsize = g_highlightsize;
	COLORREF highlightcolor = g_highlightcolor;
	int highlightshape = g_highlightshape;
		
	double x1,x2,y1,y2;

	//OffScreen Buffer	
	HBITMAP hbm = NULL;
	HBITMAP old_bitmap;
	HDC hdcBits = ::CreateCompatibleDC(hdc);
    hbm = (HBITMAP) ::CreateCompatibleBitmap(hdc,fullsize.cx,fullsize.cy);    	
	old_bitmap = (HBITMAP) ::SelectObject(hdcBits,hbm);		
		
	if ((highlightshape == 0) || (highlightshape == 2)) { //circle and square

		x1 = (fullsize.cx - highlightsize)/2.0;
		x2 = (fullsize.cx + highlightsize)/2.0;
		y1 = (fullsize.cy - highlightsize)/2.0;
		y2 = (fullsize.cy + highlightsize)/2.0;
	}
	else if ((highlightshape == 1) || (highlightshape == 3)) { //ellipse and rectangle

		x1 = (fullsize.cx - highlightsize)/2.0;
		x2 = (fullsize.cx + highlightsize)/2.0;
		y1 = (fullsize.cy - highlightsize/2.0)/2.0;
		y2 = (fullsize.cy + highlightsize/2.0)/2.0;

	}

	HBRUSH ptbrush = (HBRUSH) ::GetStockObject(WHITE_BRUSH);
	HPEN nullpen = CreatePen(PS_NULL,0,0); 
	HBRUSH hlbrush = CreateSolidBrush( highlightcolor);
	
	
	HBRUSH oldbrush = (HBRUSH)  ::SelectObject(hdcBits,ptbrush);	
	HPEN oldpen = (HPEN) ::SelectObject(hdcBits,nullpen);			
	::Rectangle(hdcBits, 0,0,fullsize.cx+1,fullsize.cy+1);		
	


		//Draw the highlight
		::SelectObject(hdcBits,hlbrush);				
		
		if ((highlightshape == 0)  || (highlightshape == 1)) { //circle and ellipse
			::Ellipse(hdcBits,(int) x1,(int) y1,(int) x2,(int) y2);
		}
		else if ((highlightshape == 2) || (highlightshape == 3)) { //square and rectangle
			::Rectangle(hdcBits,(int) x1,(int) y1,(int) x2,(int) y2);
		}
		
		::SelectObject(hdcBits,oldbrush);		

	

	::SelectObject(hdcBits,oldpen);	
	DeleteObject(hlbrush);  	
	DeleteObject(nullpen);		

	//OffScreen Buffer		
	BitBlt(hdc, xoffset, yoffset, fullsize.cx, fullsize.cy, hdcBits, 0, 0, SRCAND);  	
	SelectObject(hdcBits, old_bitmap);        
	DeleteObject(hbm);
    DeleteDC(hdcBits);

	
}

void CVscapView::OnOptionsAutopan() 
{
	// TODO: Add your command handler code here
	if (autopan==0)
		autopan=1;
	else
		autopan=0;

	
}

void CVscapView::OnOptionsAtuopanspeed() 
{
	// TODO: Add your command handler code here
	CAutopanSpeed aps_dlg;
	aps_dlg.DoModal();
	
}

void CVscapView::OnUpdateOptionsAutopan(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(autopan);
	
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



void CVscapView::OnUpdateRegionFullscreen(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	if (MouseCaptureMode==2) pCmdUI->SetCheck(TRUE);
	else pCmdUI->SetCheck(FALSE);
	
	
}

void CVscapView::OnOptionsMinimizeonstart() 
{
	// TODO: Add your command handler code here
	if (minimizeOnStart==0) 
		minimizeOnStart = 1;
	else
		minimizeOnStart = 0;
	
}

void CVscapView::OnUpdateOptionsMinimizeonstart(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(minimizeOnStart);
	
}

void CVscapView::OnOptionsHideflashing() 
{
	// TODO: Add your command handler code here
	if (flashingRect==0)
		flashingRect = 1;
	else
		flashingRect = 0;
	
}

void CVscapView::OnUpdateOptionsHideflashing(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	
	//ver 1.2
	pCmdUI->SetCheck(!flashingRect);
	
}

void CVscapView::OnOptionsProgramoptionsPlayavi() 
{
	// TODO: Add your command handler code here
	if (launchPlayer==0)
		launchPlayer = 1;
	else
		launchPlayer = 0;
	
}

void CVscapView::OnUpdateOptionsProgramoptionsPlayavi(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(launchPlayer);
	
}


BOOL CVscapView::Openlink (CString link)
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


BOOL CVscapView::OpenUsingShellExecute (CString link)
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


BOOL CVscapView::
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

  

LONG CVscapView::GetRegKey (HKEY key, LPCTSTR subkey, LPTSTR retdata)
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

void CVscapView::OnHelpWebsite() 
{
	// TODO: Add your command handler code here
	//Openlink("http://www.atomixbuttons.com/vsc");
	//Openlink("http://www.rendersoftware.com");
	Openlink("http://www.rendersoftware.com/products/camstudio");
	
}

void CVscapView::OnHelpHelp() 
{
	// TODO: Add your command handler code here
	
	CString progdir,helppath;
	progdir=GetProgPath();
	helppath= progdir + "\\help.htm";
	
	Openlink(helppath);
	
	//HtmlHelp( hWndGlobal,  progdir + "\\help.chm",  HH_DISPLAY_INDEX, (DWORD)"CamStudio") ;
	
}


void CVscapView::OnPause() 
{
	// TODO: Add your command handler code here

	//return if not current recording or already in paused state
	if ((recordstate==0) || (recordpaused==1)) return;

	recordpaused=1;

	//ver 1.8 
	//if (recordaudio==2) 
	//	mciRecordPause(tempaudiopath);

	CStatusBar* pStatus = (CStatusBar*) AfxGetApp()->m_pMainWnd->GetDescendantWindow(AFX_IDW_STATUS_BAR);
	pStatus->SetPaneText(0,"Recording Paused");		

	//Set Title Bar     
	SetTitleBar("Paused");
	
}

void CVscapView::OnUpdatePause(CCmdUI* pCmdUI) 
{
	//Version 1.1
	//pCmdUI->Enable(recordstate && (!recordpaused));
	pCmdUI->Enable(!recordpaused);
	
}

void CVscapView::OnUpdateStop(CCmdUI* pCmdUI) 
{
	//Version 1.1
	//pCmdUI->Enable(recordstate);
	
}


void CVscapView::OnUpdateRecord(CCmdUI* pCmdUI) 
{
	//Version 1.1
	pCmdUI->Enable(!recordstate || recordpaused);
	
}


void CVscapView::OnHelpFaq() 
{
	// TODO: Add your command handler code here
	//Openlink("http://www.atomixbuttons.com/vsc/page5.html");
	Openlink("http://www.rendersoftware.com/products/camstudio/faq.htm");
	
}


//===============================================
// AUDIO CODE  
//===============================================
// Ver 1.1
//===============================================

void waveInErrorMsg(MMRESULT result, const char * addstr)
{
	// say error message
	char errorbuffer[500];
	waveInGetErrorText(result, errorbuffer,500);
	//ErrorMsg("WAVEIN:%x:%s %s", result, errorbuffer, addstr);
	CString msgstr;
	msgstr.Format("%s %s",errorbuffer, addstr);

	CString tstr;
	tstr.LoadString(IDS_STRING_WAVEINERR);
	MessageBox(NULL,msgstr,tstr,MB_OK | MB_ICONEXCLAMATION);



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
	m_pFile = new CSoundFile(tempaudiopath, &m_Format);	

	
	
	if  (!(m_pFile && m_pFile->IsOK()))
		//MessageBox(NULL,"Error Creating Sound File","Note",MB_OK | MB_ICONEXCLAMATION);
		MessageOut(NULL,IDS_STRING_ERRSOUND ,IDS_STRING_NOTE,MB_OK | MB_ICONEXCLAMATION);

			
	return TRUE;
}


//Initialize the tempaudiopath variable with a valid temporary path
void GetTempWavePath() {

	CString fileName("\\~temp001.wav");
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

			//MessageBox(NULL,tempaudiopath,"Uses Temp File",MB_OK);
			//fileverified = 1;
			//Try choosing another temporary filename

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
		mmReturn = ::waveInOpen( &m_hRecord, AudioDeviceID, &m_Format,(DWORD) hWndGlobal, NULL, CALLBACK_WINDOW  ); //use on message to map.....
						
		
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
			
			audioTimeInitiated = 1;
			sdwSamplesPerSec = ((LPWAVEFORMAT) &m_Format)->nSamplesPerSec;
		    sdwBytesPerSec = ((LPWAVEFORMAT) &m_Format)->nAvgBytesPerSec;

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
		Sleep(500);

		mmReturn = ::waveInStop(m_hRecord);
		if(mmReturn) waveInErrorMsg(mmReturn, "Error in StopAudioRecording() (WaveinStop)");
	

		mmReturn = ::waveInClose(m_hRecord);
		if(mmReturn) waveInErrorMsg(mmReturn, "Error in StopAudioRecording() (WaveinClose)");
	}
	
	//if(m_QueuedBuffers != 0) ErrorMsg("Still %d buffers in waveIn queue!", m_QueuedBuffers);
	if(m_QueuedBuffers != 0) 
		//MessageBox(NULL,"Audio buffers still in queue!","note", MB_OK);
		MessageOut(NULL,IDS_STRING_AUDIOBUF ,IDS_STRING_NOTE,MB_OK | MB_ICONEXCLAMATION);

	audioTimeInitiated = 0;
	
	
}


LRESULT CVscapView::OnMM_WIM_DATA(WPARAM parm1, LPARAM parm2)
{
		
	MMRESULT mmReturn = 0;
	
	LPWAVEHDR pHdr = (LPWAVEHDR) parm2;

	mmReturn = ::waveInUnprepareHeader(m_hRecord, pHdr, sizeof(WAVEHDR));
	if(mmReturn)
	{
		waveInErrorMsg(mmReturn, "in OnWIM_DATA()");
		return 0;
	}

	TRACE("WIM_DATA %4d\n", pHdr->dwBytesRecorded);
	
	if (recordstate) 	
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
			waveInErrorMsg(mmReturn, "in OnWIM_DATA()");
		}
		else // no error
		{
			// add the input buffer to the queue again
			mmReturn = ::waveInAddBuffer(m_hRecord, pHdr, sizeof(WAVEHDR));
			if(mmReturn) waveInErrorMsg(mmReturn, "in OnWIM_DATA()");
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



void DataFromSoundIn(CBuffer* buffer) 
{		
	if(m_pFile)
	{
		if(!m_pFile->Write(buffer))
		{
			//m_SoundIn.Stop();
			StopAudioRecording();
			ClearAudioFile();

			//MessageBox(NULL,"Error Writing Sound File","Note",MB_OK | MB_ICONEXCLAMATION);
			MessageOut(NULL,IDS_STRING_ERRSOUND2 ,IDS_STRING_NOTE,MB_OK | MB_ICONEXCLAMATION);

		}
	}
	 

}



void CVscapView::OnOptionsRecordaudio() 
{
		
	
	if (waveInGetNumDevs() == 0) {

		//CString msgstr;
		//msgstr.Format("Unable to detect audio input device. You need a sound card with microphone input.");
        //MessageBox(msgstr,"Note", MB_OK | MB_ICONEXCLAMATION);
		MessageOut(this->m_hWnd,IDS_STRING_NOINPUT3 ,IDS_STRING_NOTE,MB_OK | MB_ICONEXCLAMATION);	
		return;

	}
  

	if (recordaudio==1) 
		recordaudio=0;
	else
		recordaudio=1;
	
}

void CVscapView::OnUpdateOptionsRecordaudio(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(recordaudio);
	
}


//Alloc Maximum Size for Save Format pwfx
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

			//CString msgstr;
			//msgstr.Format("Metrics failed mmresult=%u!", mmresult);
            //::MessageBox(NULL,msgstr,"Note", MB_OK | MB_ICONEXCLAMATION);			

			MessageOutINT(NULL,IDS_STRING_METRICSFAILED, IDS_STRING_NOTE, MB_OK | MB_ICONEXCLAMATION,mmresult);

			return ;
		}


		pwfx = (LPWAVEFORMATEX)GlobalAllocPtr(GHND, cbwfx);
		if (NULL == pwfx)
		{

			//CString msgstr;
			//msgstr.Format("GlobalAllocPtr(%lu) failed!", cbwfx);            
			//::MessageBox(NULL,msgstr,"Note", MB_OK | MB_ICONEXCLAMATION);			

			MessageOut(NULL,IDS_STRING_GALLOC ,IDS_STRING_NOTE,MB_OK | MB_ICONEXCLAMATION);


			return ;
		}

		initial_audiosetup=1;

	}

}


//Build Recording Format to m_Format
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
	
	//1st try MPEGLAYER3
	BuildRecordingFormat();		
	MMRESULT mmr;
	if ((m_Format.nSamplesPerSec == 22050) && (m_Format.nChannels==2) && (m_Format.wBitsPerSample <= 16)) {

		pwfx->wFormatTag = WAVE_FORMAT_MPEGLAYER3;
		mmr = acmFormatSuggest(NULL, &m_Format,  pwfx, cbwfx, ACM_FORMATSUGGESTF_WFORMATTAG);	

	}
	
	if (mmr!=0) {
		
		//ver 1.6, use PCM if MP3 not available

		//Then try ADPCM
		//BuildRecordingFormat();
		//pwfx->wFormatTag = WAVE_FORMAT_ADPCM;
		//MMRESULT mmr = acmFormatSuggest(NULL, &m_Format,  pwfx, cbwfx, ACM_FORMATSUGGESTF_WFORMATTAG);


		if (mmr!=0) {

			//Use the PCM as default
			BuildRecordingFormat();
			pwfx->wFormatTag = WAVE_FORMAT_PCM;
			MMRESULT mmr = acmFormatSuggest(NULL, &m_Format,  pwfx, cbwfx, ACM_FORMATSUGGESTF_WFORMATTAG);


			if (mmr!=0) {

				bAudioCompression = FALSE;

			}


		}

	}

	
}


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

void CVscapView::OnOptionsAudioformat() 
{

	
	// TODO: Add your command handler code here		



	
	
	if (waveInGetNumDevs() == 0) {

		//CString msgstr;
		//msgstr.Format("Unable to detect audio input device. You need a sound card with microphone input.");
        //MessageBox(msgstr,"Note", MB_OK | MB_ICONEXCLAMATION);

		MessageOut(this->m_hWnd,IDS_STRING_NOINPUT3 ,IDS_STRING_NOTE,MB_OK | MB_ICONEXCLAMATION);
		return;

	}
	


	AudioFormat aod;
	aod.DoModal();

	/*
	if (interleaveUnit == MILLISECONDS) {

		double interfloat = (((double) interleaveFactor) * ((double) frames_per_second))/1000.0;
		int interint = (int) interfloat;
		if (interint<=0)
			interint = 1;
		
		CString bstr;
		bstr.Format("interleave Unit = %d",interint);
		//MessageBox(bstr,"Note",MB_OK);

	}
	*/
	
}


// Ver 1.1
// ========================================
// Merge Audio and Video File Function
// ========================================
//
// No recompression is applied to the Video File
// Optional Recompression is applied to the Audio File	
// Assuming audio_recompress_format is compatible with the existing format of the audio file
//
// If recompress audio is set to FALSE,  both audio_recompress_format and audio_format_size can be NULL
// ========================================
int Merge_Video_And_Sound_File(CString input_video_path, CString input_audio_path, CString output_avi_path, BOOL recompress_audio, LPWAVEFORMATEX audio_recompress_format, DWORD  audio_format_size, BOOL bInterleave, int interleave_factor, int interleave_unit) {	    

	PAVISTREAM            AviStream[NUMSTREAMS];      // the editable streams
	AVICOMPRESSOPTIONS    gaAVIOptions[NUMSTREAMS];   // compression options
	LPAVICOMPRESSOPTIONS  galpAVIOptions[NUMSTREAMS];	
   
	PAVIFILE pfileVideo = NULL;
	
	
	AVIFileInit();	    
	
	//Open Video and Audio Files	
	HRESULT hr = AVIFileOpen(&pfileVideo, LPCTSTR(input_video_path), OF_READ | OF_SHARE_DENY_NONE, 0L);
    if (hr != 0)
    {		
		//MessageBox(NULL,"Unable to open video file.","Note",MB_OK | MB_ICONEXCLAMATION);
		MessageOut(NULL,IDS_STRING_NOOPENVIDEO ,IDS_STRING_NOTE,MB_OK | MB_ICONEXCLAMATION);
		
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
	 	  	//MessageBox(NULL,"Unable to open video stream.","Note",MB_OK | MB_ICONEXCLAMATION);			
			MessageOut(NULL,IDS_STRING_NOOPENSREAM ,IDS_STRING_NOTE,MB_OK | MB_ICONEXCLAMATION);

			return 1;

    	}
		
		//Set editable stream number as 0
		if (CreateEditableStream(&AviStream[0], pavi) != AVIERR_OK) {
	
			AVIStreamRelease(pavi);
			AVIFileRelease(pfileVideo);
		    
			//MessageBox(NULL,"Unable to create editable video stream.","Note",MB_OK | MB_ICONEXCLAMATION);			
			MessageOut(NULL, IDS_STRING_NOCREATESTREAM ,IDS_STRING_NOTE,MB_OK | MB_ICONEXCLAMATION);

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
 	  		//MessageBox(NULL,"Unable to open audio stream.","Note",MB_OK | MB_ICONEXCLAMATION);			
			MessageOut(NULL,IDS_STRING_NOOPENAUDIO ,IDS_STRING_NOTE,MB_OK | MB_ICONEXCLAMATION);



			return 2;
   		}
			
		//Set editable stream number as 1
		if (CreateEditableStream(&AviStream[1], pavi) != AVIERR_OK) {
		
			AVIStreamRelease(pavi);
			AVIStreamRelease(AviStream[0]);
			//MessageBox(NULL,"Unable to create editable audio stream.","Note",MB_OK | MB_ICONEXCLAMATION);			
			MessageOut(NULL,IDS_STRING_NOEDITAUDIO ,IDS_STRING_NOTE,MB_OK | MB_ICONEXCLAMATION);


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
	
		//MessageBox(NULL,"Unable to verify video stream.","Note",MB_OK | MB_ICONEXCLAMATION);			
		MessageOut(NULL,IDS_STRING_NOVERIFYVIDEO ,IDS_STRING_NOTE,MB_OK | MB_ICONEXCLAMATION);
		
		AVIStreamRelease(AviStream[0]);
		AVIStreamRelease(AviStream[1]);
		return 3;	
	
	}
		
	
	if (avis[1].fccType != streamtypeAUDIO) {
	
		//MessageBox(NULL,"Unable to verify audio stream.","Note",MB_OK | MB_ICONEXCLAMATION);			
		MessageOut(NULL,IDS_STRING_NOVERIFYAUDIO ,IDS_STRING_NOTE,MB_OK | MB_ICONEXCLAMATION);

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
	
	if (interleave_unit==FRAMES)
		galpAVIOptions[0]->dwInterleaveEvery = interleave_factor;
	else {

		double interfloat = (((double) interleaveFactor) * ((double) frames_per_second))/1000.0;
		int interint = (int) interfloat;
		if (interint<=0)
			interint = 1;	
	
		galpAVIOptions[0]->dwInterleaveEvery = interint;

	}

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
	
	
	if (interleave_unit==FRAMES)
		galpAVIOptions[1]->dwInterleaveEvery = interleave_factor;
	else {

		//back here
		double interfloat = (((double) interleaveFactor) * ((double) frames_per_second))/1000.0;
		int interint = (int) interfloat;
		if (interint<=0)
			interint = 1;
	
		galpAVIOptions[1]->dwInterleaveEvery = interint;

	}
	//galpAVIOptions[1]->dwInterleaveEvery = interleave_factor;
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
				
				//MessageBox(NULL,"Unable to merge audio and video streams (1).","Note",MB_OK | MB_ICONEXCLAMATION);	
				MessageOut(NULL,IDS_STRING_NOMERGE1 ,IDS_STRING_NOTE,MB_OK | MB_ICONEXCLAMATION);				
				
				return 5;

			}

			//Succesful Merging, but with no audio recompression
			//MessageBox(NULL,"Unable to apply audio compression with the selected options. Your movie is saved without audio compression.","Note",MB_OK | MB_ICONEXCLAMATION);		
			MessageOut(NULL,IDS_STRING_NOAUDIOCOMPRESS ,IDS_STRING_NOTE,MB_OK | MB_ICONEXCLAMATION);



		} // if recompress audio retry
		else {
			
			AVISaveOptionsFree(NUMSTREAMS,galpAVIOptions);						
			AVIStreamRelease(AviStream[0]);
			AVIStreamRelease(AviStream[1]);	    		
			//MessageBox(NULL,"Unable to audio and video merge streams (2).","Note",MB_OK | MB_ICONEXCLAMATION);	
			MessageOut(NULL,IDS_STRING_NOMERGE2 ,IDS_STRING_NOTE,MB_OK | MB_ICONEXCLAMATION);

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
	
	//char    szText[300];
    //wsprintf(szText, "Compressing Audio %d%%", iProgress);


	CString szText, fmtstr;
	fmtstr.LoadString(IDS_STRING_COMPRESSINGAUDIO);
	szText.Format(LPCTSTR(fmtstr),iProgress);

	HWND mainwnd = NULL;
	mainwnd = AfxGetApp()->m_pMainWnd->m_hWnd;
	if (mainwnd)
		//::SetWindowText(mainwnd, szText);
		::SetWindowText(mainwnd, LPCTSTR(szText));
    
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




//Ver 1.2
//These functions are used by the VideoOptions dialog to manipulate the pVideoCompressParams variable
//The pVideoCompressParams variable is used in AVICompress options
void FreeVideoCompressParams() {

	if (pVideoCompressParams) {

		GlobalFreePtr(pVideoCompressParams);
		pVideoCompressParams = NULL;

	}

}


BOOL AllocVideoCompressParams(DWORD paramsSize) {

	if (pVideoCompressParams) {
		
		//Free Existing
		FreeVideoCompressParams();

	}	
	
	pVideoCompressParams = (LPVOID) GlobalAllocPtr(GHND, paramsSize);
	if (NULL == pVideoCompressParams)
	{
		//CString msgstr;
		//msgstr.Format("GlobalAllocPtr(%lu) for Video Compress Parameters failed!", paramsSize);
        //::MessageBox(NULL,msgstr,"Note", MB_OK | MB_ICONEXCLAMATION);			

		MessageOut(NULL,IDS_STRING_GALLOCVCM ,IDS_STRING_NOTE,MB_OK | MB_ICONEXCLAMATION);

		return FALSE;
	}


	
	return TRUE;

}


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
		MessageOut(NULL,IDS_STRING_FAILALLOCVCM ,IDS_STRING_NOTE,MB_OK | MB_ICONEXCLAMATION);

		return FALSE;
	}
	
	memcpy(pParamsUse,pOrg,paramsSize);	
	return TRUE;

}

void GetVideoCompressState (HIC hic , DWORD fccHandler) {

		DWORD statesize = ICGetStateSize(hic);

		if (statesize <= 0) return;

		if (AllocVideoCompressParams(statesize)) {				
		
			//ICGetState returns statesize even if pVideoCompressParams is not NULL ??								
			DWORD ret = ICGetState(hic,(LPVOID) pVideoCompressParams,statesize);	
			if (ret < 0) {		
				
				//CString reportstr;
				//reportstr.Format("Failure in getting compressor state ! Error Value = %d" , ret) ;
				//MessageBox(NULL,reportstr,"Note",MB_OK | MB_ICONEXCLAMATION);
				MessageOutINT(NULL,IDS_STRING_COMPRESSORSTATE ,IDS_STRING_NOTE,MB_OK | MB_ICONEXCLAMATION,ret);

			}
			else {

				//we store only the state for one compressor in pVideoCompressParams
				//So we need to indicate which compressor the state is referring to 
				CompressorStateIsFor = fccHandler;													
				CompressorStateSize = statesize;

			}
		}

}


void SetVideoCompressState (HIC hic , DWORD fccHandler) {

	if (CompressorStateIsFor == fccHandler) {

			if (pVideoCompressParams) {

				DWORD ret = ICSetState(hic,(LPVOID) pVideoCompressParams,CompressorStateSize);						
				//if (ret <= 0) {							
				if (ret < 0) {							
					//MessageBox(NULL, "Failure in setting compressor state !","Note",MB_OK | MB_ICONEXCLAMATION);
					MessageOut(NULL,IDS_STRING_SETCOMPRESSOR ,IDS_STRING_NOTE,MB_OK | MB_ICONEXCLAMATION);

				}

			}
	}

}



void CVscapView::OnOptionsKeyboardshortcuts() 
{
	// TODO: Add your command handler code here
	if (!keySCOpened)
	{

		keySCOpened = 1;
		Keyshortcuts kscDlg;
		kscDlg.DoModal();
		keySCOpened = 0;
		
		SetAdjustHotKeys();

	}
		
}




// Ver 1.2
void CVscapView::SaveSettings() 
{

	FILE * sFile;	
	CString setDir,setPath;
				
	
	//********************************************
	//Creating CamStudio.ini for storing text data
	//********************************************
	//
	// Once defined...the format cannot change in future versions
	// new properties can only be appended at the end
	// otherwise, the datafile will not be upward compatible with future versions

	CString fileName("\\CamStudio.ini");	
	setDir=GetProgPath();
	setPath=setDir+fileName;

	sFile = fopen(LPCTSTR(setPath),"wt");
	if (sFile == NULL) {

		//Error creating file ...do nothing...return
		return;

	}


	// ****************************
	// Dump Variables	
	// ****************************
	//Take note of those vars with printf %ld	
	float ver = (float) 2.4;

	fprintf(sFile, "[ CamStudio Settings ver%.2f -- Please do not edit ] \n\n",ver);
	fprintf(sFile, "flashingRect=%d \n",flashingRect);
	fprintf(sFile, "launchPlayer=%d \n",launchPlayer);
	fprintf(sFile, "minimizeOnStart=%d \n",minimizeOnStart);
	fprintf(sFile, "MouseCaptureMode= %d \n",MouseCaptureMode);
	fprintf(sFile, "capturewidth=%d \n",capturewidth);
	fprintf(sFile, "captureheight=%d \n",captureheight);

	fprintf(sFile, "timelapse=%d \n",timelapse);
	fprintf(sFile, "frames_per_second= %d \n",frames_per_second);
	fprintf(sFile, "keyFramesEvery= %d \n",keyFramesEvery);
	fprintf(sFile, "compquality= %d \n",compquality);
	fprintf(sFile, "compfccHandler= %ld \n",compfccHandler);

	//LPVOID pVideoCompressParams = NULL; 
	fprintf(sFile, "CompressorStateIsFor= %ld \n",CompressorStateIsFor);
	fprintf(sFile, "CompressorStateSize= %d \n",CompressorStateSize);
	
	fprintf(sFile, "g_recordcursor=%d \n",g_recordcursor);  
	fprintf(sFile, "g_customsel=%d \n",g_customsel); //Having this line means the custom cursor type cannot be re-arranged in a new order in the combo box...else previous saved settings referring to the custom type will not be correct	
	fprintf(sFile, "g_cursortype=%d \n",g_cursortype); 
	fprintf(sFile, "g_highlightcursor=%d \n",g_highlightcursor);
	fprintf(sFile, "g_highlightsize=%d \n",g_highlightsize);
	fprintf(sFile, "g_highlightshape=%d \n",g_highlightshape);

	fprintf(sFile, "g_highlightcolorR=%d \n",GetRValue(g_highlightcolor));
	fprintf(sFile, "g_highlightcolorG=%d \n",GetGValue(g_highlightcolor));
	fprintf(sFile, "g_highlightcolorB=%d \n",GetBValue(g_highlightcolor));	

	//fprintf(sFile, "savedir=%s; \n",LPCTSTR(savedir));
	//fprintf(sFile, "cursordir=%s; \n",LPCTSTR(cursordir));	
	
	fprintf(sFile, "autopan=%d \n",autopan);
	fprintf(sFile, "maxpan= %d \n",maxpan);

	//Audio Functions and Variables
	fprintf(sFile, "AudioDeviceID= %d \n",AudioDeviceID);

	//Audio Options Dialog
	//LPWAVEFORMATEX      pwfx = NULL;
	//DWORD               cbwfx;	
	fprintf(sFile, "cbwfx= %ld \n", cbwfx);
	fprintf(sFile, "recordaudio= %d \n", recordaudio);

	//Audio Formats Dialog
	fprintf(sFile, "waveinselected= %d \n", waveinselected);
	fprintf(sFile, "audio_bits_per_sample= %d \n", audio_bits_per_sample);
	fprintf(sFile, "audio_num_channels= %d \n", audio_num_channels);
	fprintf(sFile, "audio_samples_per_seconds= %d \n",  audio_samples_per_seconds);
	fprintf(sFile, "bAudioCompression= %d \n", bAudioCompression);

	fprintf(sFile, "interleaveFrames= %d \n", interleaveFrames);
	fprintf(sFile, "interleaveFactor= %d \n", interleaveFactor);		

	//Key Shortcuts
	fprintf(sFile, "keyRecordStart= %d \n",keyRecordStart);
	fprintf(sFile, "keyRecordEnd= %d \n",keyRecordEnd);
	fprintf(sFile, "keyRecordCancel= %d \n",keyRecordCancel);

	//viewtype
	fprintf(sFile, "viewtype= %d \n",viewtype);
	
	fprintf(sFile, "g_autoadjust= %d \n",g_autoadjust);
	fprintf(sFile, "g_valueadjust= %d \n",g_valueadjust);	
	
	fprintf(sFile, "savedir=%d \n",savedir.GetLength());
	fprintf(sFile, "cursordir=%d \n",cursordir.GetLength());	
	

	//Ver 1.3
	fprintf(sFile, "threadPriority=%d \n",threadPriority);	
	

	//Ver 1.5
	fprintf(sFile, "captureleft= %d \n",captureleft);
	fprintf(sFile, "capturetop= %d \n",capturetop);	
	fprintf(sFile, "fixedcapture=%d \n",fixedcapture);
	fprintf(sFile, "interleaveUnit= %d \n", interleaveUnit);


	
	//Ver 1.6
	fprintf(sFile, "tempPath_Access=%d \n",tempPath_Access);	
	fprintf(sFile, "captureTrans=%d \n",captureTrans);	
	fprintf(sFile, "specifieddir=%d \n",specifieddir.GetLength());		
	
	fprintf(sFile, "NumDev=%d \n",NumberOfMixerDevices);	
	fprintf(sFile, "SelectedDev=%d \n",SelectedMixer);	
	fprintf(sFile, "feedback_line=%d \n",feedback_line);
	fprintf(sFile, "feedback_line_info=%d \n",feedback_lineInfo);
	fprintf(sFile, "performAutoSearch=%d \n",performAutoSearch);

	
	//Ver 1.8
	fprintf(sFile, "supportMouseDrag=%d \n",supportMouseDrag);	

	//New variables, add here
	fprintf(sFile, "keyRecordStartCtrl=%d \n",keyRecordStartCtrl);
	fprintf(sFile, "keyRecordEndCtrl=%d \n",keyRecordEndCtrl);
	fprintf(sFile, "keyRecordCancelCtrl=%d \n",keyRecordCancelCtrl);

	fprintf(sFile, "keyRecordStartAlt=%d \n",keyRecordStartAlt);
	fprintf(sFile, "keyRecordEndAlt=%d \n",keyRecordEndAlt);
	fprintf(sFile, "keyRecordCancelAlt=%d \n",keyRecordCancelAlt);

	fprintf(sFile, "keyRecordStartShift=%d \n",keyRecordStartShift);
	fprintf(sFile, "keyRecordEndShift=%d \n",keyRecordEndShift);
	fprintf(sFile, "keyRecordCancelShift=%d \n",keyRecordCancelShift);

	fprintf(sFile, "keyNext=%d \n",keyNext);
	fprintf(sFile, "keyPrev=%d \n",keyPrev);
	fprintf(sFile, "keyShowLayout=%d \n",keyShowLayout);

	fprintf(sFile, "keyNextCtrl=%d \n",keyNextCtrl);
	fprintf(sFile, "keyPrevCtrl=%d \n",keyPrevCtrl);
	fprintf(sFile, "keyShowLayoutCtrl=%d \n",keyShowLayoutCtrl);

	fprintf(sFile, "keyNextAlt=%d \n",keyNextAlt );
	fprintf(sFile, "keyPrevAlt=%d \n",keyPrevAlt );
	fprintf(sFile, "keyShowLayoutAlt=%d \n",keyShowLayoutAlt);

	fprintf(sFile, "keyNextShift=%d \n",keyNextShift);
	fprintf(sFile, "keyPrevShift=%d \n",keyPrevShift);
	fprintf(sFile, "keyShowLayoutShift=%d \n",keyShowLayoutShift);

	fprintf(sFile, "shapeNameInt=%d \n",shapeNameInt);
	fprintf(sFile, "shapeNameLen=%d \n",shapeName.GetLength());

	fprintf(sFile, "layoutNameInt=%d \n",layoutNameInt);
	fprintf(sFile, "g_layoutNameLen=%d \n",g_layoutName.GetLength());

	fprintf(sFile, "useMCI=%d \n",useMCI);
	fprintf(sFile, "shiftType=%d \n",shiftType);
	fprintf(sFile, "timeshift=%d \n",timeshift);
	fprintf(sFile, "frameshift=%d \n",frameshift);


	//ver 2.26
	fprintf(sFile, "launchPropPrompt=%d \n",launchPropPrompt);
	fprintf(sFile, "launchHTMLPlayer=%d \n",launchHTMLPlayer);
	fprintf(sFile, "deleteAVIAfterUse=%d \n",deleteAVIAfterUse);	
	fprintf(sFile, "RecordingMode=%d \n",RecordingMode);	
	fprintf(sFile, "autonaming=%d \n",autonaming);	
	fprintf(sFile, "restrictVideoCodecs=%d \n",restrictVideoCodecs);	
	//fprintf(sFile, "base_nid=%d \n",base_nid);	
		

	//ver 2.40
	fprintf(sFile, "presettime=%d \n",presettime);
	fprintf(sFile, "recordpreset=%d \n",recordpreset);

	//Add new variables here

	fclose(sFile);

	//ver 1.8, 
	CString m_newfile;	
	m_newfile = GetProgPath() + "\\CamShapes.ini";
	gList.SaveShapeArray(m_newfile);

	m_newfile = GetProgPath() + "\\CamLayout.ini";
	gList.SaveLayout(m_newfile);

	if (g_cursortype==2) {

		CString cursorFileName="\\CamCursor.ini";	
		CString cursorDir=GetProgPath();
		CString cursorPath=cursorDir+cursorFileName;	

		//Note, we do not save the cursorFilePath, but instead we make a copy of the cursor file in the Prog directory
		CopyFile(g_cursorFilePath,cursorPath,FALSE);

	}
			

	
	//********************************************
	//Creating Camdata.ini for storing binary data
	//********************************************	
	FILE * tFile;
	fileName="\\Camdata.ini";
	setDir=GetProgPath();
	setPath=setDir+fileName;

	tFile = fopen(LPCTSTR(setPath),"wb");
	if (tFile == NULL) {

		//Error creating file ...do nothing...return
		return;

	}

	// ****************************
	// Dump Variables	
	// ****************************	
	//Saving Directories, put here
	if (savedir.GetLength()>0)
		fwrite( (void *) LPCTSTR(savedir), savedir.GetLength(), 1, tFile );
	
	if (cursordir.GetLength()>0)
		fwrite( (void *) LPCTSTR(cursordir), cursordir.GetLength(), 1, tFile );
	
	if (cbwfx>0)
		fwrite( (void *) pwfx, cbwfx, 1, tFile );

	if (CompressorStateSize>0)
		fwrite( (void *) pVideoCompressParams, CompressorStateSize, 1, tFile );	

	//Ver 1.6
	if (specifieddir.GetLength()>0)
		fwrite( (void *) LPCTSTR(specifieddir), specifieddir.GetLength(), 1, tFile );

	//Ver 1.8
	if (shapeName.GetLength()>0)
		fwrite( (void *) LPCTSTR(shapeName), shapeName.GetLength(), 1, tFile );

	if (g_layoutName.GetLength()>0)
		fwrite( (void *) LPCTSTR(g_layoutName), g_layoutName.GetLength(), 1, tFile );
	
	

	fclose(tFile);


}



// Ver 1.2
void CVscapView::LoadSettings() 
{

	// this can be deferred until the Create of the Screen Annotation dialog
	//if (ver>=1.799999)	{ 
	{	//attempt to load the shapes and layouts ...never mind the version
	
		CString m_newfile;		
		m_newfile = GetProgPath() + "\\CamShapes.ini";
		gList.LoadShapeArray(m_newfile);
		
		m_newfile = GetProgPath() + "\\CamLayout.ini";
		gList.LoadLayout(m_newfile);
	
	}
	
	
	FILE * sFile;
	FILE * rFile;
	CString setDir,setPath;
	
	
	//The absence of nosave.ini file indicates savesettings = 1
	CString fileName("\\NoSave.ini ");	
	setDir=GetProgPath();
	setPath=setDir+fileName;

		
	rFile = fopen(LPCTSTR(setPath),"rt");
	if (rFile == NULL) {
		savesettings = 1;
	}
	else {

		fclose(rFile);
		savesettings = 0;

	}

	//********************************************
	//Loading CamStudio.ini for storing text data
	//********************************************
	fileName="\\CamStudio.ini";	
	setDir=GetProgPath();
	setPath=setDir+fileName;

	sFile = fopen(LPCTSTR(setPath),"rt");
	if (sFile == NULL) {

		//Error creating file ...
		SuggestRecordingFormat();
		SuggestCompressFormat();
		return;

	}


	// ****************************
	// Read Variables	
	// ****************************
	
	int idata;		
	int savelen=0;
	int cursorlen=0;	
	char sdata[1000]; 	
	char tdata[1000]; 
	float ver=1.0;


	//Ver 1.6 
	int specifiedDirLength=0;
	char specdata[1000]; 	



	fscanf(sFile, "[ CamStudio Settings ver%f -- Please do not edit ] \n\n",&ver);

	//Ver 1.2
	if (ver>=1.199999)	{ 
		
		fscanf(sFile, "flashingRect=%d \n",&flashingRect);

		fscanf(sFile, "launchPlayer=%d \n",&launchPlayer);
		fscanf(sFile, "minimizeOnStart=%d \n",&minimizeOnStart);
		fscanf(sFile, "MouseCaptureMode= %d \n",&MouseCaptureMode);
		fscanf(sFile, "capturewidth=%d \n",&capturewidth);
		fscanf(sFile, "captureheight=%d \n",&captureheight);

		fscanf(sFile, "timelapse=%d \n",&timelapse);
		fscanf(sFile, "frames_per_second= %d \n",&frames_per_second);
		fscanf(sFile, "keyFramesEvery= %d \n",&keyFramesEvery);
		fscanf(sFile, "compquality= %d \n",&compquality);
		fscanf(sFile, "compfccHandler= %ld \n",&compfccHandler);

		//LPVOID pVideoCompressParams = NULL; 
		fscanf(sFile, "CompressorStateIsFor= %ld \n",&CompressorStateIsFor);
		fscanf(sFile, "CompressorStateSize= %d \n",&CompressorStateSize);
				
		fscanf(sFile, "g_recordcursor=%d \n",&g_recordcursor); 
		fscanf(sFile, "g_customsel=%d \n",&g_customsel); //Having this line means the custom cursor type cannot be re-arranged in a new order in the combo box...else previous saved settings referring to the custom type will not be correct	
		fscanf(sFile, "g_cursortype=%d \n",&g_cursortype); 		
		fscanf(sFile, "g_highlightcursor=%d \n",&g_highlightcursor);
		fscanf(sFile, "g_highlightsize=%d \n",&g_highlightsize);
		fscanf(sFile, "g_highlightshape=%d \n",&g_highlightshape);	
		
		int redv,greenv,bluev;
		fscanf(sFile, "g_highlightcolorR=%d \n",&idata);
		redv=idata;
		fscanf(sFile, "g_highlightcolorG=%d \n",&idata);
		greenv=idata;
		fscanf(sFile, "g_highlightcolorB=%d \n",&idata);	
		bluev=idata;
		g_highlightcolor = RGB(redv,greenv,bluev);


		
		fscanf(sFile, "autopan=%d \n",&autopan);
		fscanf(sFile, "maxpan= %d \n",&maxpan);

		//Audio Functions and Variables
		fscanf(sFile, "AudioDeviceID= %d \n",&AudioDeviceID);

		//Audio Options Dialog
		//LPWAVEFORMATEX      pwfx = NULL;
		//DWORD               cbwfx;	
		fscanf(sFile, "cbwfx= %ld \n", &cbwfx);
		fscanf(sFile, "recordaudio= %d \n", &recordaudio);


		//Audio Formats Dialog
		fscanf(sFile, "waveinselected= %d \n", &waveinselected);
		fscanf(sFile, "audio_bits_per_sample= %d \n", &audio_bits_per_sample);
		fscanf(sFile, "audio_num_channels= %d \n", &audio_num_channels);
		fscanf(sFile, "audio_samples_per_seconds= %d \n",  &audio_samples_per_seconds);
		fscanf(sFile, "bAudioCompression= %d \n", &bAudioCompression);

		fscanf(sFile, "interleaveFrames= %d \n", &interleaveFrames);
		fscanf(sFile, "interleaveFactor= %d \n", &interleaveFactor);		

		//Key Shortcuts
		fscanf(sFile, "keyRecordStart= %d \n",&keyRecordStart);
		fscanf(sFile, "keyRecordEnd= %d \n",&keyRecordEnd);
		fscanf(sFile, "keyRecordCancel= %d \n",&keyRecordCancel);
		
		fscanf(sFile, "viewtype= %d \n",&viewtype);

		fscanf(sFile, "g_autoadjust= %d \n",&g_autoadjust);
		fscanf(sFile, "g_valueadjust= %d \n",&g_valueadjust);

		fscanf(sFile, "savedir=%d \n",&savelen);
		fscanf(sFile, "cursordir=%d \n",&cursorlen);
		
		
		AttemptRecordingFormat();	
		
		//ver 1.4		
		//Force settings from previous version to upgrade
		if (ver<1.35) {
			
			//set auto adjust to max rate
			g_autoadjust = 1;
			g_valueadjust = 1;

			//set default compressor
			compfccHandler = mmioFOURCC('M', 'S', 'V', 'C');	
			CompressorStateIsFor = 0;
			CompressorStateSize = 0;
			compquality = 7000;	
			
			//set default audio recording format and compress format
			SuggestRecordingFormat();
			SuggestCompressFormat();
			

		}

		if (g_autoadjust) {
		
			int framerate;
			int delayms;
			int val = g_valueadjust;
			
			AutoSetRate( val, framerate, delayms);

			timelapse=delayms;
			frames_per_second = framerate;
			keyFramesEvery = framerate;

		}

		strCodec = GetCodecDescription(compfccHandler);		



		
		

	    if (g_cursortype == 1)		
		{
			DWORD customicon;
			if (g_customsel<0) g_customsel = 0;
			customicon = icon_info[g_customsel];	
					
			g_customcursor = LoadIcon(AfxGetInstanceHandle(),MAKEINTRESOURCE(customicon));
			if (g_customcursor==NULL) {
				g_cursortype = 0;

			}
			
		}
		else if (g_cursortype == 2) { //load cursor

			CString cursorFileName="\\CamCursor.ini";	
			CString cursorDir=GetProgPath();
			CString cursorPath=cursorDir+cursorFileName;	
			
			g_loadcursor = LoadCursorFromFile(cursorPath);				

			if (g_loadcursor==NULL) {
				g_cursortype = 0;
			}
			

		}


	}//ver 1.2


	//Ver 1.3
	if (ver>=1.299999)	{ 

		fscanf(sFile, "threadPriority=%d \n",&threadPriority);	

		//CString tracex;
		//tracex.Format("Thread %d ver %f",threadPriority, ver);
		//MessageBox(tracex,"Note",MB_OK);
		
	}
	

	//Ver 1.5
	if (ver>=1.499999)	{ 
		
		fscanf(sFile, "captureleft= %d \n",&captureleft);
		fscanf(sFile, "capturetop= %d \n",&capturetop);	
		fscanf(sFile, "fixedcapture=%d \n",&fixedcapture);
		fscanf(sFile, "interleaveUnit= %d \n", &interleaveUnit);		

	}
	else {

		//force interleve settings
		interleaveUnit= MILLISECONDS;
		interleaveFactor = 100;

	}


	//Ver 1.6
	if (ver>=1.599999)	{ 

		fscanf(sFile, "tempPath_Access=%d \n",&tempPath_Access);	
		fscanf(sFile, "captureTrans=%d \n",&captureTrans);	
		fscanf(sFile, "specifieddir=%d \n",&specifiedDirLength);	
		
		fscanf(sFile, "NumDev=%d \n",&NumberOfMixerDevices);	
		fscanf(sFile, "SelectedDev=%d \n",&SelectedMixer);	
		fscanf(sFile, "feedback_line=%d \n",&feedback_line);
		fscanf(sFile, "feedback_line_info=%d \n",&feedback_lineInfo);
		fscanf(sFile, "performAutoSearch=%d \n",&performAutoSearch);		
		
		onLoadSettings(recordaudio);
		
			
	}
	else {

		tempPath_Access  = USE_WINDOWS_TEMP_DIR;
		captureTrans = 1;			
		specifiedDirLength = 0;
		
		NumberOfMixerDevices=0;
		SelectedMixer=0;
		feedback_line = -1;
		feedback_lineInfo = -1;
		performAutoSearch=1;

	}


  	if (specifiedDirLength == 0) {

		int old_tempPath_Access = tempPath_Access;
		tempPath_Access  = USE_WINDOWS_TEMP_DIR;
		specifieddir = GetTempPath();
		tempPath_Access  = old_tempPath_Access;

		//Do not modify the specifiedDirLength variable, even if specifieddir is changed.  It will need to be used below

	}

	//Update Player to ver 2.0
	//Make the the modified keys do not overlap
	if (ver<1.799999)	{ 
	
		if (launchPlayer == 1)	
			launchPlayer = 3;	

		if ((keyRecordStart  == VK_MENU) || (keyRecordStart  == VK_SHIFT)  || (keyRecordStart  == VK_CONTROL) || (keyRecordStart == VK_ESCAPE)) {
			keyRecordStart = VK_F8;
			keyRecordStartCtrl = 1;
		}

		if ((keyRecordEnd  == VK_MENU) || (keyRecordEnd  == VK_SHIFT)  || (keyRecordEnd  == VK_CONTROL) || (keyRecordEnd  == VK_ESCAPE)) {
			keyRecordEnd = VK_F9;
			keyRecordEndCtrl = 1;
		}

		if ((keyRecordCancel  == VK_MENU) || (keyRecordCancel  == VK_SHIFT)  || (keyRecordCancel  == VK_CONTROL) || (keyRecordCancel  == VK_ESCAPE)) {
			keyRecordCancel = VK_F10;
			keyRecordCancelCtrl = 1;
		}
	
	}


	//Ver 1.8
	int shapeNameLen=0;
	int layoutNameLen=0;
	if (ver>=1.799999)	{ 
	
		fscanf(sFile, "supportMouseDrag=%d \n",&supportMouseDrag);	

		fscanf(sFile,& "keyRecordStartCtrl=%d \n",&keyRecordStartCtrl);
		fscanf(sFile,& "keyRecordEndCtrl=%d \n",&keyRecordEndCtrl);
		fscanf(sFile,& "keyRecordCancelCtrl=%d \n",&keyRecordCancelCtrl);

		fscanf(sFile,& "keyRecordStartAlt=%d \n",&keyRecordStartAlt);
		fscanf(sFile,& "keyRecordEndAlt=%d \n",&keyRecordEndAlt);
		fscanf(sFile,& "keyRecordCancelAlt=%d \n",&keyRecordCancelAlt);

		fscanf(sFile,& "keyRecordStartShift=%d \n",&keyRecordStartShift);
		fscanf(sFile,& "keyRecordEndShift=%d \n",&keyRecordEndShift);
		fscanf(sFile,& "keyRecordCancelShift=%d \n",&keyRecordCancelShift);

		fscanf(sFile,& "keyNext=%d \n",&keyNext);
		fscanf(sFile,& "keyPrev=%d \n",&keyPrev);
		fscanf(sFile,& "keyShowLayout=%d \n",&keyShowLayout);

		fscanf(sFile,& "keyNextCtrl=%d \n",&keyNextCtrl);
		fscanf(sFile,& "keyPrevCtrl=%d \n",&keyPrevCtrl);
		fscanf(sFile,& "keyShowLayoutCtrl=%d \n",&keyShowLayoutCtrl);

		fscanf(sFile,& "keyNextAlt=%d \n",&keyNextAlt );
		fscanf(sFile,& "keyPrevAlt=%d \n",&keyPrevAlt );
		fscanf(sFile,& "keyShowLayoutAlt=%d \n",&keyShowLayoutAlt);

		fscanf(sFile,& "keyNextShift=%d \n",&keyNextShift);
		fscanf(sFile,& "keyPrevShift=%d \n",&keyPrevShift);
		fscanf(sFile,& "keyShowLayoutShift=%d \n",&keyShowLayoutShift);	

		fscanf(sFile, "shapeNameInt=%d \n",&shapeNameInt);
		fscanf(sFile, "shapeNameLen=%d \n",&shapeNameLen);

		fscanf(sFile, "layoutNameInt=%d \n",&layoutNameInt);
		fscanf(sFile, "g_layoutNameLen=%d \n",&layoutNameLen);

		fscanf(sFile, "useMCI=%d \n",&useMCI);
		fscanf(sFile, "shiftType=%d \n",&shiftType);
		fscanf(sFile, "timeshift=%d \n",&timeshift);
		fscanf(sFile, "frameshift=%d \n",&frameshift);
	
	
	}


	//ver 2.26
	//save format is set as 2.0
	if (ver>=1.999999)	{ 
		
		fscanf(sFile, "launchPropPrompt=%d \n",&launchPropPrompt);
		fscanf(sFile, "launchHTMLPlayer=%d \n",&launchHTMLPlayer);
		fscanf(sFile, "deleteAVIAfterUse=%d \n",&deleteAVIAfterUse);	
		fscanf(sFile, "RecordingMode=%d \n",&RecordingMode);	
		fscanf(sFile, "autonaming=%d \n",&autonaming);	
		fscanf(sFile, "restrictVideoCodecs=%d \n",&restrictVideoCodecs);	
		//fscanf(sFile, "base_nid=%d \n",&base_nid);	

	}

	//ver 2.40
	if (ver>=2.399999)	{ 	
		
		fscanf(sFile, "presettime=%d \n",&presettime);
		fscanf(sFile, "recordpreset=%d \n",&recordpreset);

	}

	//new variables add here

	fclose(sFile);



	//********************************************
	//Loading Camdata.ini  binary data
	//********************************************	
	FILE * tFile;
	fileName="\\Camdata.ini";
	setDir=GetProgPath();
	setPath=setDir+fileName;

	tFile = fopen(LPCTSTR(setPath),"rb");
	if (tFile == NULL) {

		//Error creating file 
		cbwfx = 0;
		SuggestCompressFormat();
		return;		

	}

	if (ver>=1.2)	{ 

		// ****************************
		// Load Binary Data
		// ****************************	
		if ((savelen>0) && (savelen<1000)) {
				fread( (void *) sdata, savelen, 1, tFile );
				sdata[savelen]=0;
				savedir=CString(sdata);		
		}

		if ((cursorlen>0) && (cursorlen<1000)) {
				fread( (void *) tdata, cursorlen, 1, tFile );		
				tdata[cursorlen]=0;
				cursordir=CString(tdata);
		}

		if (ver > 1.35) { //if perfoming an upgrade from previous settings, do not load these additional camdata.ini information

			if (cbwfx>0) {

				AllocCompressFormat();
				int countread = fread( (void *) pwfx, cbwfx, 1, tFile );
			
				if (countread!=1) {
					cbwfx=0;
					if (pwfx) {
				
						GlobalFreePtr(pwfx);
						pwfx = NULL;
						SuggestCompressFormat();

					}	
				}	
				else {

					 AttemptCompressFormat();

				}

			}
			
			

			if (CompressorStateSize>0) {
			
				AllocVideoCompressParams(CompressorStateSize);
				fread( (void *) pVideoCompressParams, CompressorStateSize, 1, tFile );	

			}


			//ver 1.6
			if (ver>1.55) {

				//if upgrade from older file versions,  specifiedDirLength == 0 and the following code will not run
				if ((specifiedDirLength>0) && (specifiedDirLength<1000)) {
					fread( (void *) specdata, specifiedDirLength, 1, tFile );
					specdata[specifiedDirLength]=0;
					specifieddir=CString(specdata);		
				}

				//ver 1.8
				if (ver>=1.799999)	{ 

					char namedata[1000];

					if ((shapeNameLen>0) && (shapeNameLen<1000)) {
						fread( (void *) namedata, shapeNameLen, 1, tFile );
						namedata[shapeNameLen]=0;
						shapeName=CString(namedata);		
					}	

					if ((layoutNameLen>0) && (layoutNameLen<1000)) {
						fread( (void *) namedata, layoutNameLen, 1, tFile );
						namedata[layoutNameLen]=0;
						g_layoutName=CString(namedata);		
					}	

				}

			
			}// if ver >=1.55


		}// if ver >=1.35

	} //if ver>=1.2

	fclose(tFile);	



}



void CVscapView::OnOptionsProgramoptionsSavesettingsonexit() 
{
	// TODO: Add your command handler code here
	if (savesettings)
		savesettings=0;
	else
		savesettings=1;
	
}

void CVscapView::OnUpdateOptionsProgramoptionsSavesettingsonexit(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(savesettings);
	
}



//ver .12
void AttemptRecordingFormat() {
		
	WAVEINCAPS pwic;
	MMRESULT mmr = waveInGetDevCaps( AudioDeviceID ,  &pwic, sizeof(pwic) ); 
	
	if ((pwic.dwFormats) & waveinselected) { 	
	
		BuildRecordingFormat();	
	
	}
	else {	
		SuggestRecordingFormat();			
	}	
	
}

void AttemptCompressFormat() {

	bAudioCompression = TRUE;

	AllocCompressFormat();		
		
	BuildRecordingFormat();			

	//Test Compatibility 	
	MMRESULT mmr = acmFormatSuggest(NULL, &m_Format,  pwfx, cbwfx, ACM_FORMATSUGGESTF_NCHANNELS  | ACM_FORMATSUGGESTF_NSAMPLESPERSEC  | ACM_FORMATSUGGESTF_WBITSPERSAMPLE  | ACM_FORMATSUGGESTF_WFORMATTAG  );	
	if (mmr!=0) {		
		
		SuggestCompressFormat();		
		
	}
		
}	



void CVscapView::DecideSaveSettings() 
{

	CString nosaveName("\\NoSave.ini");	
	CString nosaveDir=GetProgPath();
	CString nosavePath=nosaveDir+nosaveName;		

	if (savesettings==1) {
	
		SaveSettings();		
		DeleteFile(nosavePath);

	}
	else {


		//Create the nosave.ini file if savesettings = 0;	
		FILE *rFile;
		rFile = fopen(LPCTSTR(nosavePath),"wt");
		fprintf(rFile,"savesettings = 0 \n");
		fclose(rFile);

		//Delete Settings File		
		CString setDir,setPath;
		CString fileName("\\CamStudio.ini ");	

		setDir=GetProgPath();
		setPath=setDir+fileName;

		DeleteFile(setPath);

		fileName = "\\Camdata.ini";			
		setPath=setDir+fileName;

		DeleteFile(setPath);

		fileName = "\\CamCursor.ini";			
		setPath=setDir+fileName;

		DeleteFile(setPath);
	

	}



}

///////////////////////////////////////////////////
// Ver 1.2
///////////////////////////////////////////////////
//Functions for implementing Tray Icons 
///////////////////////////////////////////////////
void TraySetIcon(HICON hIcon)
{
	ASSERT(hIcon);

	IconData.hIcon = hIcon;
	IconData.uFlags |= NIF_ICON;
}

void TraySetIcon(UINT nResourceID)
{
	ASSERT(nResourceID>0);
	HICON hIcon = 0;
	hIcon = AfxGetApp()->LoadIcon(nResourceID);
	if(hIcon)
	{
		IconData.hIcon = hIcon;
		IconData.uFlags |= NIF_ICON;
	}
	else
	{
		TRACE0("FAILED TO LOAD ICON\n");
	}
}

void TraySetIcon(LPCTSTR lpszResourceName)
{
	HICON hIcon = 0;
	hIcon = AfxGetApp()->LoadIcon(lpszResourceName);
	if(hIcon)
	{
		IconData.hIcon = hIcon;
		IconData.uFlags |= NIF_ICON;
	}
	else
	{
		TRACE0("FAILED TO LOAD ICON\n");
	}
}

void TraySetToolTip(LPCTSTR lpszToolTip)
{
	ASSERT(strlen(lpszToolTip) > 0 && strlen(lpszToolTip) < 64);

	strcpy(IconData.szTip,lpszToolTip);
	IconData.uFlags |= NIF_TIP;
}


void initTrayIconData()
{
	IconData.cbSize			= sizeof(NOTIFYICONDATA);	
	IconData.hWnd				= 0;
	IconData.uID				= 1;
	IconData.uCallbackMessage	= WM_TRAY_ICON_NOTIFY_MESSAGE;	
	IconData.hIcon				= 0;
	IconData.szTip[0]			= 0;	
	IconData.uFlags			= NIF_MESSAGE;	
	IconData.hWnd =hWndGlobal;
	
	bTrayIconVisible				= FALSE;
	DefaultMenuItem				= 0;
	bMinimizeToTray				= TRUE;

}


void finishTrayIconData()
{	
	if(IconData.hWnd && IconData.uID>0 && bTrayIconVisible)
	{
		Shell_NotifyIcon(NIM_DELETE,&IconData);
	}
}


BOOL TrayShow()
{
	BOOL bSuccess = FALSE;
	if(!bTrayIconVisible)
	{
		bSuccess = Shell_NotifyIcon(NIM_ADD,&IconData);
		if(bSuccess)
			bTrayIconVisible= TRUE;
	}
	else
	{
		TRACE0("ICON ALREADY VISIBLE");
	}
	return bSuccess;
}

BOOL TrayHide()
{
	BOOL bSuccess = FALSE;
	if(bTrayIconVisible)
	{
		bSuccess = Shell_NotifyIcon(NIM_DELETE,&IconData);
		if(bSuccess)
			bTrayIconVisible= FALSE;
	}
	else
	{
		TRACE0("ICON ALREADY HIDDEN");
	}
	return bSuccess;
}

BOOL TrayUpdate()
{
	BOOL bSuccess = FALSE;
	if(bTrayIconVisible)
	{
		bSuccess = Shell_NotifyIcon(NIM_MODIFY,&IconData);
	}
	else
	{
		TRACE0("ICON NOT VISIBLE");
	}
	return bSuccess;
}


BOOL TraySetMenu(UINT nResourceID,UINT nDefaultPos)
{
	BOOL bSuccess;
	bSuccess = TrayMenu.LoadMenu(nResourceID);
	return bSuccess;
}


BOOL TraySetMenu(LPCTSTR lpszMenuName,UINT nDefaultPos)
{
	BOOL bSuccess;
	bSuccess = TrayMenu.LoadMenu(lpszMenuName);
	return bSuccess;
}

BOOL TraySetMenu(HMENU hMenu,UINT nDefaultPos)
{
	TrayMenu.Attach(hMenu);
	return TRUE;
}

 

void OnTrayRButtonDown(CPoint pt)
{

	::SetForegroundWindow( AfxGetMainWnd()->m_hWnd ); 
	TrackPopupMenu(TrayMenu.GetSubMenu(0)->m_hMenu,TPM_BOTTOMALIGN|TPM_LEFTBUTTON|TPM_RIGHTBUTTON,pt.x,pt.y,0,AfxGetMainWnd()->m_hWnd,NULL);

	PostMessage(AfxGetMainWnd()->m_hWnd ,WM_NULL, 0, 0); 
}

void OnTrayLButtonDown(CPoint pt)
{

}

void OnTrayLButtonDblClk(CPoint pt)
{
	
	::SetForegroundWindow( AfxGetMainWnd()->m_hWnd ); 
	AfxGetMainWnd()->ShowWindow(SW_RESTORE);
	

}

void OnTrayRButtonDblClk(CPoint pt)
{
}

void OnTrayMouseMove(CPoint pt)
{
}


LRESULT CVscapView::OnTrayNotify(WPARAM wParam, LPARAM lParam) 
{ 
    UINT uID; 
    UINT uMsg; 
 
    uID = (UINT) wParam; 
    uMsg = (UINT) lParam; 
 
	if (uID != 1)
		return 0;
	
	CPoint pt;	

    switch (uMsg ) 
	{ 
	case WM_MOUSEMOVE:
		GetCursorPos(&pt);
		ClientToScreen(&pt);
		OnTrayMouseMove(pt);
		break;
	case WM_LBUTTONDOWN:
		GetCursorPos(&pt);
		ClientToScreen(&pt);
		OnTrayLButtonDown(pt);
		break;
	case WM_LBUTTONDBLCLK:
		GetCursorPos(&pt);
		ClientToScreen(&pt);
		OnTrayLButtonDblClk(pt);
		break;
	
	case WM_RBUTTONDOWN:
	case WM_CONTEXTMENU:
		GetCursorPos(&pt);
		//ClientToScreen(&pt);
		OnTrayRButtonDown(pt);
		
		break;
	case WM_RBUTTONDBLCLK:
		GetCursorPos(&pt);
		ClientToScreen(&pt);
		OnTrayRButtonDblClk(pt);
		break;
    } 
     return 0; 
 } 




void CVscapView::OnOptionsRecordingthreadpriorityNormal() 
{
	// TODO: Add your command handler code here
	threadPriority = THREAD_PRIORITY_NORMAL;
	
}

void CVscapView::OnOptionsRecordingthreadpriorityHighest() 
{
	// TODO: Add your command handler code here
	threadPriority = THREAD_PRIORITY_HIGHEST;
	
	
}

void CVscapView::OnOptionsRecordingthreadpriorityAbovenormal() 
{
	// TODO: Add your command handler code here
	threadPriority = THREAD_PRIORITY_ABOVE_NORMAL;
	
}

void CVscapView::OnOptionsRecordingthreadpriorityTimecritical() 
{
	// TODO: Add your command handler code here
	threadPriority = THREAD_PRIORITY_TIME_CRITICAL;
	
}

void CVscapView::OnUpdateOptionsRecordingthreadpriorityNormal(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(threadPriority == THREAD_PRIORITY_NORMAL);
	
}

void CVscapView::OnUpdateOptionsRecordingthreadpriorityHighest(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(threadPriority == THREAD_PRIORITY_HIGHEST);
	
}

void CVscapView::OnUpdateOptionsRecordingthreadpriorityAbovenormal(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(threadPriority == THREAD_PRIORITY_ABOVE_NORMAL);
	
}

void CVscapView::OnUpdateOptionsRecordingthreadpriorityTimecritical(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(threadPriority == THREAD_PRIORITY_TIME_CRITICAL);
	
}


CString GetCodecDescription(long fccHandler) {


	ICINFO compinfo;

	HIC hic;	
	hic = ICOpen(ICTYPE_VIDEO, fccHandler, ICMODE_QUERY);
	if (hic) {		
				
		ICGetInfo(hic, &compinfo, sizeof(ICINFO));						
		ICClose(hic);
	}

	return CString(compinfo.szDescription); 

}




void AutoSetRate(int val,int& framerate,int& delayms)
{

	if (val<=17) { //fps more than 1 per second

		
		framerate=200-((val-1)*10); //framerate 200 to 40;
		//1 corr to 200, 17 corr to 40
		
		delayms = 1000/framerate;

		/*
		//Automatically Adjust the Quality for MSVC (MS Video 1) if the framerate is too high
		int sel = ((CComboBox *) GetDlgItem(IDC_COMPRESSORS))->GetCurSel();
		if (compressor_info[sel].fccHandler==mmioFOURCC('M', 'S', 'V', 'C')) {

			int cQuality = ((CSliderCtrl *) GetDlgItem(IDC_QUALITY_SLIDER))->GetPos();
			if (cQuality<80) 
				((CSliderCtrl *) GetDlgItem(IDC_QUALITY_SLIDER))->SetPos(80);


		}
		*/

	}
	else if (val<=56) { //fps more than 1 per second
		
		framerate=(57-val); //framerate 39 to 1;
		//18 corr to 39,  56 corr to 1

		delayms = 1000/framerate;

	}
	else if (val<=86) { //assume timelapse

		framerate = 20;
		delayms = (val-56)*1000;
		
		//57 corr to 1000, 86 corr to 30000 (20 seconds)

	}
	else if (val<=99) { //assume timelapse

		framerate = 20;
		delayms = (val-86)*2000+30000;
		
		//87 corr to 30000, 99 corr to 56000 (56 seconds)

	}
	else { //val=100 , timelapse

		framerate = 20;
		delayms = 60000;

		//100 corr to 60000

	}

}


void CVscapView::OnOptionsCapturetrans() 
{
	// TODO: Add your command handler code here
	
	if (captureTrans)
		captureTrans = 0;
	else	
		captureTrans = 1;
	
}

void CVscapView::OnUpdateOptionsCapturetrans(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(captureTrans==1);

	
}

void CVscapView::OnOptionsTempdirWindows() 
{
	// TODO: Add your command handler code here
	tempPath_Access  = USE_WINDOWS_TEMP_DIR ;
	
}

void CVscapView::OnUpdateOptionsTempdirWindows(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(tempPath_Access  == USE_WINDOWS_TEMP_DIR);
	
	
}

void CVscapView::OnOptionsTempdirInstalled() 
{
	// TODO: Add your command handler code here
	tempPath_Access  = USE_INSTALLED_DIR;
	
}

void CVscapView::OnUpdateOptionsTempdirInstalled(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(tempPath_Access  == USE_INSTALLED_DIR);
	
}

void CVscapView::OnOptionsTempdirUser() 
{
	// TODO: Add your command handler code here
	tempPath_Access  = USE_USER_SPECIFIED_DIR;

	CFolderDialog cfg(specifieddir);
	int retval;
	
	retval=cfg.DoModal();

	if (retval==IDOK) {

		specifieddir = cfg.GetPathName();

	}
	
}

void CVscapView::OnUpdateOptionsTempdirUser(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(tempPath_Access  == USE_USER_SPECIFIED_DIR);
	
}



//ver 1.6
CString GetTempPath()
{

	if (tempPath_Access == USE_WINDOWS_TEMP_DIR) {
	
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
		tempPath_Access = USE_INSTALLED_DIR;
		return GetProgPath();	

	}
	else if (tempPath_Access == USE_USER_SPECIFIED_DIR)
	{

		CString tempdir;
		tempdir = specifieddir;			

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

		//set temp path to installed directory
		tempPath_Access = USE_INSTALLED_DIR;
		return GetProgPath();	


	}
	else  {

		return GetProgPath();	

	}
}




//ver 1.6 Capture waveout
//MCI functions
void mciRecordOpen()
{
	mop.dwCallback = (DWORD)hWndGlobal;
	mop.lpstrDeviceType = (LPCSTR)MCI_DEVTYPE_WAVEFORM_AUDIO;
	mop.lpstrElementName = TEXT("");

	mciSendCommand(0 , MCI_OPEN , 
		MCI_OPEN_TYPE | MCI_OPEN_TYPE_ID |
		MCI_OPEN_ELEMENT , (DWORD)&mop);

	isMciRecordOpen = 1;


}


void mciRecordStart()
{


	DWORD dwReturn;
	char  buffer[300];
	if (dwReturn = mciSendCommand(mop.wDeviceID , MCI_RECORD , 0 , 0))
	{

		mciGetErrorString(dwReturn, buffer, sizeof (buffer));
		MessageBox( NULL, buffer, "MCI_RECORD",MB_ICONEXCLAMATION | MB_OK);

	}	
   

}

void mciRecordStop(CString strFile)
{
	msp.dwCallback = (DWORD)hWndGlobal;
	msp.lpfilename = LPCTSTR(strFile);

	DWORD dwReturn;
	//char  buffer[300];

	dwReturn = mciSendCommand(mop.wDeviceID , MCI_STOP , MCI_WAIT , 0);
	/*
	if (dwReturn)
	{

		mciGetErrorString(dwReturn, buffer, sizeof (buffer));
		MessageBox( NULL, buffer, "MCI_RECORD",MB_ICONEXCLAMATION | MB_OK);

	}
	*/


	dwReturn = mciSendCommand(mop.wDeviceID , MCI_SAVE ,
			MCI_WAIT | MCI_SAVE_FILE , (DWORD)&msp);	
	
	/*
	if (dwReturn)
	{

		mciGetErrorString(dwReturn, buffer, sizeof (buffer));
		MessageBox( NULL, buffer, "MCI_RECORD",MB_ICONEXCLAMATION | MB_OK);

	}
	*/

}


void mciRecordPause(CString strFile)
{	

	//can call this only in the same thread as the one opening the device?
	
	if (isMciRecordOpen==0) return;
		
	msp.dwCallback = (DWORD)hWndGlobal;
	msp.lpfilename = LPCTSTR(strFile);	

	DWORD dwReturn;
	//char  buffer[300];
	dwReturn = mciSendCommand(mop.wDeviceID , MCI_PAUSE , MCI_WAIT , 0);	
	//dwReturn = mciSendCommand(mop.wDeviceID , MCI_STOP , MCI_WAIT , 0);
	/*
	if (dwReturn)
	{

		mciGetErrorString(dwReturn, buffer, sizeof (buffer));
		MessageBox( NULL, buffer, "MCI_RECORD",MB_ICONEXCLAMATION | MB_OK);

	}
	*/
	
	
}

void mciRecordResume(CString strFile)
{
	//can call this only in the same thread as the one opening the device?

	msp.dwCallback = (DWORD)hWndGlobal;
	msp.lpfilename = LPCTSTR(strFile);

	if (isMciRecordOpen==0) return;

	//char  buffer[300];
	DWORD dwReturn = mciSendCommand(mop.wDeviceID , MCI_RESUME , MCI_WAIT , 0);	
	/*
	if (dwReturn)
	{

		mciGetErrorString(dwReturn, buffer, sizeof (buffer));
		MessageBox( NULL, buffer, "MCI_RECORD",MB_ICONEXCLAMATION | MB_OK);

	}
	*/
	
	
}

void mciRecordClose()
{
	mciSendCommand(mop.wDeviceID , MCI_CLOSE , 0 , 0);

	isMciRecordOpen = 0;

}






void ErrMsg(char frmt[], ...) {
  
  DWORD written;
  char buf[5000];
  va_list val;
  
  va_start(val, frmt);
  wvsprintf(buf, frmt, val);

  const COORD _80x50 = {80,50};
  static BOOL startup = (AllocConsole(), SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), _80x50));
  WriteConsole(GetStdHandle(STD_OUTPUT_HANDLE), buf, lstrlen(buf), &written, 0);
}



//The setting/suggesting of format for recording Speakers is deferred until recording starts
//Default is to use best settings avalable
void mciSetWaveFormat()
{	
   

   MCI_WAVE_SET_PARMS set_parms;
   DWORD dwReturn;
   char  buffer[128];
   
   //Suggest 10 formats
   dwReturn=-1;
   for (int i=0;((i<10) && (dwReturn!=0));i++) {

	   SuggestSpeakerRecordingFormat(i);
	   BuildSpeakerRecordingFormat();
   
		// Set PCM format of recording.
	   set_parms.wFormatTag = m_FormatSpeaker.wFormatTag;
	   set_parms.wBitsPerSample = m_FormatSpeaker.wBitsPerSample;
	   set_parms.nChannels = m_FormatSpeaker.nChannels;
	   set_parms.nSamplesPerSec = m_FormatSpeaker.nSamplesPerSec;
	   set_parms.nAvgBytesPerSec =  m_FormatSpeaker.nAvgBytesPerSec;
	   set_parms.nBlockAlign = m_FormatSpeaker.nBlockAlign;

	   if (dwReturn = mciSendCommand( mop.wDeviceID, MCI_SET, MCI_WAIT |
									   MCI_WAVE_SET_FORMATTAG |
									   MCI_WAVE_SET_BITSPERSAMPLE |
									   MCI_WAVE_SET_CHANNELS |
									   MCI_WAVE_SET_SAMPLESPERSEC |

									   MCI_WAVE_SET_AVGBYTESPERSEC |
									   MCI_WAVE_SET_BLOCKALIGN,
									   (DWORD)(LPVOID)&set_parms))
	   {
		   //mciGetErrorString(dwReturn, buffer, sizeof(buffer));
		   //MessageBox( NULL, buffer, "Error setting Wave format on MCI Device",MB_ICONEXCLAMATION | MB_OK);
	   }


   }

   if (dwReturn) {

	  mciGetErrorString(dwReturn, buffer, sizeof(buffer));
	  CString tstr;
	  tstr.LoadString(IDS_STRING_MCIWAVE);
	  MessageBox( NULL, buffer, tstr,MB_ICONEXCLAMATION | MB_OK);
	  

   }   


}




void CVscapView::OnOptionsRecordaudioDonotrecordaudio() 
{
	// TODO: Add your command handler code here
	recordaudio=0;
	
}

void CVscapView::OnUpdateOptionsRecordaudioDonotrecordaudio(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(recordaudio==0);
	
}

void CVscapView::OnOptionsRecordaudioRecordfromspeakers() 
{
	// TODO: Add your command handler code here
	if (waveOutGetNumDevs() == 0) {

		//CString msgstr;
		//msgstr.Format("Unable to detect audio output device. You need a sound card with speakers attached.");
        //MessageBox(msgstr,"Note", MB_OK | MB_ICONEXCLAMATION);
		MessageOut(this->m_hWnd,IDS_STRING_NOAUDIOOUTPUT ,IDS_STRING_NOTE,MB_OK | MB_ICONEXCLAMATION);

		return;

	}

	recordaudio=2;

	useWaveout(FALSE,FALSE);


	
}

void CVscapView::OnUpdateOptionsRecordaudioRecordfromspeakers(CCmdUI* pCmdUI) 
{

	pCmdUI->SetCheck(recordaudio==2);
	
}

void CVscapView::OnOptionsRecordaudiomicrophone() 
{

	if (waveInGetNumDevs() == 0) {

		MessageOut(this->m_hWnd,IDS_STRING_NOINPUT1 ,IDS_STRING_NOTE,MB_OK | MB_ICONEXCLAMATION);


		return;

	}
  
	recordaudio=1;

	
	useWavein(TRUE,FALSE); //TRUE ==> silence mode, will not report errors
	
}

void CVscapView::OnUpdateOptionsRecordaudiomicrophone(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(recordaudio==1);

	
}




void SuggestSpeakerRecordingFormat(int i) {

	//Ordered in preference of choice
	switch (i) {

		case 0: {
			
			audio_bits_per_sample_Speaker = 16;
			audio_num_channels_Speaker = 2;
			audio_samples_per_seconds_Speaker = 44100;				
			waveinselected_Speaker = WAVE_FORMAT_4S16;
			break;
		}
		case 1: {

			audio_bits_per_sample_Speaker = 16;
			audio_num_channels_Speaker = 2;
			audio_samples_per_seconds_Speaker = 22050;				
			waveinselected_Speaker = WAVE_FORMAT_2S16;
			break;

		}
		case 2: {

			audio_bits_per_sample_Speaker = 8;
			audio_num_channels_Speaker = 2;
			audio_samples_per_seconds_Speaker = 44100;				
			waveinselected_Speaker = WAVE_FORMAT_4S08;
			break;

		}
		case 3: {

			audio_bits_per_sample_Speaker = 8;
			audio_num_channels_Speaker = 2;
			audio_samples_per_seconds_Speaker = 22050;				
			waveinselected_Speaker = WAVE_FORMAT_2S08;
			break;

		}
		case 4: {

			audio_bits_per_sample_Speaker = 16;
			audio_num_channels_Speaker = 1;
			audio_samples_per_seconds_Speaker = 44100;				
			waveinselected_Speaker = WAVE_FORMAT_4M16;
			break;

		}
		case 5: {

			audio_bits_per_sample_Speaker = 8;
			audio_num_channels_Speaker = 1;
			audio_samples_per_seconds_Speaker = 44100;				
			waveinselected_Speaker = WAVE_FORMAT_4M08;
			break;

		}
		case 6: {

			audio_bits_per_sample_Speaker = 16;
			audio_num_channels_Speaker = 1;
			audio_samples_per_seconds_Speaker = 22050;				
			waveinselected_Speaker = WAVE_FORMAT_2M16;
			break;

		}		
		case 7: {

			audio_bits_per_sample_Speaker = 16;
			audio_num_channels_Speaker = 2;
			audio_samples_per_seconds_Speaker = 11025;				
			waveinselected_Speaker = WAVE_FORMAT_1S16;
			break;

		}
		case 8: {

			audio_bits_per_sample_Speaker = 8;
			audio_num_channels_Speaker = 1;
			audio_samples_per_seconds_Speaker = 22050;				
			waveinselected_Speaker = WAVE_FORMAT_2M08;
			break;

		}		
		case 9: {

			audio_bits_per_sample_Speaker = 8;
			audio_num_channels_Speaker = 2;
			audio_samples_per_seconds_Speaker = 11025;				
			waveinselected_Speaker = WAVE_FORMAT_1S08;
			break;

		}
		default : {

			audio_bits_per_sample_Speaker = 8;
			audio_num_channels_Speaker = 1;
			audio_samples_per_seconds_Speaker = 11025;				
			waveinselected_Speaker = WAVE_FORMAT_1M08;

		}
		

	}


}



void BuildSpeakerRecordingFormat() {

	m_FormatSpeaker.wFormatTag	= WAVE_FORMAT_PCM;		
	m_FormatSpeaker.wBitsPerSample = audio_bits_per_sample_Speaker;
	m_FormatSpeaker.nSamplesPerSec = audio_samples_per_seconds_Speaker;
	m_FormatSpeaker.nChannels = audio_num_channels_Speaker;
	m_FormatSpeaker.nBlockAlign = m_FormatSpeaker.nChannels * (m_FormatSpeaker.wBitsPerSample/8);
	m_FormatSpeaker.nAvgBytesPerSec = m_FormatSpeaker.nSamplesPerSec * m_FormatSpeaker.nBlockAlign;
	m_FormatSpeaker.cbSize = 0;


}





void CVscapView::OnOptionsProgramoptionsTroubleshoot() 
{

	TroubleShoot tbsDlg;
	tbsDlg.DoModal();
	
	if ((TroubleShootVal == 1)) {
		::PostMessage(AfxGetMainWnd()->GetSafeHwnd(),WM_CLOSE,0,0);
	}
	
	
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

void CVscapView::OnOptionsProgramoptionsCamstudioplay() 
{
	// TODO: Add your command handler code here
	launchPlayer = 1;
	
}

void CVscapView::OnUpdateOptionsProgramoptionsCamstudioplay(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(launchPlayer == 1);
	
}

void CVscapView::OnOptionsProgramoptionsDefaultplay() 
{
	// TODO: Add your command handler code here
	launchPlayer = 2;
	
}


void CVscapView::OnUpdateOptionsProgramoptionsDefaultplay(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(launchPlayer == 2);
	
}

void CVscapView::OnOptionsProgramoptionsNoplay() 
{
	// TODO: Add your command handler code here
	launchPlayer = 0;
	
}

void CVscapView::OnUpdateOptionsProgramoptionsNoplay(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(launchPlayer == 0);
	
}


void CVscapView::OnHelpDonations() 
{
	// TODO: Add your command handler code here
	CString progdir,donatepath;
	progdir=GetProgPath();
	donatepath= progdir + "\\help.htm#Donations";
	
	Openlink(donatepath);
	
}


int GetOperatingSystem() 
{

	OSVERSIONINFO osv;
	osv.dwOSVersionInfoSize  = sizeof(OSVERSIONINFO);


	if (GetVersionEx(&osv))
	{
		return osv.dwMajorVersion;
	}		
	
	return 0;

}

void CVscapView::OnOptionsUsePlayer20() 
{
	// TODO: Add your command handler code here
	launchPlayer = 3;
	
}

void CVscapView::OnUpdateUsePlayer20(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(launchPlayer == 3);
	
}


	
void CVscapView::OnViewScreenannotations() 
{
	// TODO: Add your command handler code here
	if (!sadlgCreated) {
				sadlg.Create(IDD_SCREENANNOTATIONS2,NULL);
				sadlg.RefreshShapeList();
				sadlgCreated = 1;
	}

	if (sadlg.IsWindowVisible())
		sadlg.ShowWindow(SW_HIDE);
	else
		sadlg.ShowWindow(SW_RESTORE);
	
}

void CVscapView::OnUpdateViewScreenannotations(CCmdUI* pCmdUI) 
{
	
	
}

void CVscapView::OnViewVideoannotations() 
{
	// TODO: Add your command handler code here
	if (!vanWndCreated) {

		int x = (rand() % 100) + 100;
		int y = (rand() % 100) + 100;
		CRect rect;
		CString vastr("Video Annotation");
		CString m_newShapeText("Right Click to Edit Text");
				
		rect.left = x;
		rect.top = y;
		rect.right = rect.left + 160 - 1;
		rect.bottom = rect.top + 120 - 1;
		vanWnd.m_textstring = m_newShapeText;
		vanWnd.m_shapeStr = vastr;
		vanWnd.CreateTransparent(vanWnd.m_shapeStr,rect,NULL);
		vanWndCreated = 1;

	}

	

	if (vanWnd.IsWindowVisible())
	{
		vanWnd.ShowWindow(SW_HIDE);
	}
	else
	{
		
		if (vanWnd.status!=1) {
	
			MessageOut(NULL,IDS_STRING_NOWEBCAM,IDS_STRING_NOTE,MB_OK | MB_ICONEXCLAMATION);
			return;

		}

		vanWnd.OnUpdateSize();
		vanWnd.ShowWindow(SW_RESTORE);
	}

	
}


LONG audioTime(void)
{
	return timeGetTime();  	
	
}


void CVscapView::OnSetFocus(CWnd* pOldWnd) 
{
	CView::OnSetFocus(pOldWnd);
	
	
	
}



LRESULT CVscapView::OnHotKey(WPARAM wParam, LPARAM lParam)
{
    
	if (wParam==0)  //lParam>0 means keydown??
	{	
		
		
			if (recordstate==0) {			

				if (AllowNewRecordStartKey) { 
					AllowNewRecordStartKey = FALSE; //prevent the case which CamStudio presents more than one region for the user to select 
					OnRecord(); 
				}
		
			}
			else if (recordstate==1) {	
				
				
				if (recordpaused==0)
					OnPause() ; //pause if currently recording
				else
					OnRecord(); 					  

			}			
				
	}
	else if (wParam==1) 
	{	
		if (recordstate==1) {
		
			if (keyRecordEnd!=keyRecordCancel)			
				OnRecordInterrupted (keyRecordEnd,0);
			else
				OnRecordInterrupted (keyRecordCancel + 1,0);

		}
				
	}
	else if  (wParam==2)  
	{
			//wParam 		
			if (recordstate==1) 
				OnRecordInterrupted (keyRecordCancel,0);


		
	}	
	else if  (wParam==3)  
	{						

			if (!sadlgCreated) {
				sadlg.Create(IDD_SCREENANNOTATIONS2,NULL);
				sadlgCreated = 1;
			}		
			int max = gList.layoutArray.GetSize();
			if (max<=0) return 0;

			//Get Current selected
			int cursel  = sadlg.GetLayoutListSelection();
			if (cursel == -1)
				currentLayout = 0;
			else
				currentLayout = cursel + 1;

			if (currentLayout>=max)
				currentLayout=0;	

			sadlg.InstantiateLayout(currentLayout,1);				

		
	}
	else if  (wParam==4)  
	{


			if (!sadlgCreated) {
				sadlg.Create(IDD_SCREENANNOTATIONS2,NULL);
				//sadlg.RefreshLayoutList();
				sadlgCreated = 1;
			}		
			int max = gList.layoutArray.GetSize();
			if (max<=0) return 0;

			//Get Current selected
			int cursel  = sadlg.GetLayoutListSelection();
			if (cursel == -1)
				currentLayout = 0;
			else
				currentLayout = cursel - 1 ;

			if (currentLayout<0)
				currentLayout=max-1;	

			sadlg.InstantiateLayout(currentLayout,1);
		

	}
	else if  (wParam==5)  
	{


			if (!sadlgCreated) {
				sadlg.Create(IDD_SCREENANNOTATIONS2,NULL);
				sadlgCreated = 1;
			}		

			int displaynum = gList.displayArray.GetSize();
			if (displaynum>0) {
				sadlg.CloseAllWindows(1);
				return 0;
			}


			int max = gList.layoutArray.GetSize();
			if (max<=0) return 0;

			//Get Current selected
			int cursel  = sadlg.GetLayoutListSelection();
			if (cursel == -1)
				currentLayout = 0;
			else
				currentLayout = cursel;

			if ((currentLayout<0) || (currentLayout>=max))
				currentLayout=0;	

			sadlg.InstantiateLayout(currentLayout,1);			
		

	}
		


     return 1;
}


void CVscapView::OnOptionsSynchronization() 
{
	// TODO: Add your command handler code here
	if ((waveInGetNumDevs() == 0) || (waveOutGetNumDevs() == 0)) {
		
		MessageOut(this->m_hWnd,IDS_STRING_NOINPUT3 ,IDS_STRING_NOTE,MB_OK | MB_ICONEXCLAMATION);
		return;
		

	}

	CSyncDialog synDlg;
	synDlg.DoModal();
	
}


void SaveProducerCommand() 
{
	FILE * sFile;
	CString setDir,setPath;
	CString fileName;
	
	
	//********************************************
	//Saving CamProducer.ini for storing text data
	//********************************************

	fileName="\\CamStudio.Producer.command";	
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
		
	//fprintf(sFile, "[ CamStudio Flash Producer Commands ver%.2f -- Activate with -x or -b in command line mode ] \n\n",ver);
	fprintf(sFile, "[ CamStudio Flash Producer Commands ver%.2f ] \n\n",ver);

	fprintf(sFile, "launchPropPrompt=%d \n",launchPropPrompt);
	fprintf(sFile, "launchHTMLPlayer=%d \n",launchHTMLPlayer);
	fprintf(sFile, "deleteAVIAfterUse=%d \n",deleteAVIAfterUse);

	//ErrMsg( "\nvscap\n");
	//ErrMsg( "launchPropPrompt = %d \n",launchPropPrompt);
	//ErrMsg( "launchHTMLPlayer = %d \n",launchHTMLPlayer);
	//ErrMsg( "deleteAVIAfterUse = %d \n",deleteAVIAfterUse);
	
	//fflush(sFile);
	fclose(sFile);


}

void CVscapView::OnToolsSwfproducer() 
{
		CString AppDir=GetProgPath();
		CString launchPath;
		CString exefileName("\\Producer.exe ");
		launchPath=AppDir+exefileName;
				
		if (WinExec(launchPath,SW_SHOW)!=0) {

				
		}
		else {

			//MessageBox("Error launching SWF Producer!","Note",MB_OK | MB_ICONEXCLAMATION);
			MessageOut(this->m_hWnd,IDS_ERRPPRODUCER ,IDS_STRING_NOTE,MB_OK | MB_ICONEXCLAMATION);

		}
	
}

void CVscapView::OnOptionsSwfLaunchhtml() 
{
	// TODO: Add your command handler code here
	if (launchHTMLPlayer)
		launchHTMLPlayer = 0;
	else
		launchHTMLPlayer = 1;
	
}

void CVscapView::OnOptionsSwfDeleteavifile() 
{
	
	if (deleteAVIAfterUse)
		deleteAVIAfterUse = 0;
	else
		deleteAVIAfterUse = 1;
	
}

void CVscapView::OnOptionsSwfDisplayparameters() 
{
	
	if (launchPropPrompt)
		launchPropPrompt = 0;
	else
		launchPropPrompt = 1;
	
}

void CVscapView::OnUpdateOptionsSwfLaunchhtml(CCmdUI* pCmdUI) 
{
	
	pCmdUI->SetCheck(launchHTMLPlayer);
	
}

void CVscapView::OnUpdateOptionsSwfDisplayparameters(CCmdUI* pCmdUI) 
{

	pCmdUI->SetCheck(launchPropPrompt);
	
}

void CVscapView::OnUpdateOptionsSwfDeleteavifile(CCmdUI* pCmdUI) 
{

	pCmdUI->SetCheck(deleteAVIAfterUse);
	
}

void CVscapView::OnAviswf() 
{
	// TODO: Add your command handler code here
	if (RecordingMode == 0)
	{
		RecordingMode = 1;
		Invalidate();

	}
	else
	{
		RecordingMode = 0;
		Invalidate();
	}

}

BOOL CVscapView::OnEraseBkgnd(CDC* pDC) 
{
	// TODO: Add your message handler code here and/or call default
	
	//return CView::OnEraseBkgnd(pDC);
	return TRUE;
}


/*
LPBITMAPINFO GetTextBitmap(CDC *thisDC, CRect* caprect,int factor,CRect* drawtextRect, LOGFONT *drawfont, CString textstr, CPen* pPen, CBrush * pBrush, COLORREF textcolor, int horzalign) 
{

	int left =caprect->left;
	int top = caprect->top;
	int width = caprect->Width();
	int height = caprect->Height();
	int orig_width = width;
	int orig_height = height;
	
	width *= factor;
	height *= factor;

	CRect usetextRect = *drawtextRect;
	usetextRect.left *= factor;
	usetextRect.top *= factor;
	usetextRect.right *= factor;
	usetextRect.bottom *= factor;

	
	
	HDC hMemDC = ::CreateCompatibleDC(thisDC->m_hDC);     

	CDC* pMemDC = CDC::FromHandle(hMemDC);

	
	HBITMAP hbm;	
    hbm = CreateCompatibleBitmap(thisDC->m_hDC, width, height);
	HBITMAP oldbm = (HBITMAP) SelectObject(hMemDC, hbm);	 	
	
	//Get Background
	::StretchBlt(hMemDC, 0, 0, width, height, thisDC->m_hDC, left, top, orig_width,orig_height,SRCCOPY);	

	CPen* oldPen;
	CBrush* oldBrush;	
	CFont dxfont, *oldfont;
		
	
	if (drawfont) {

		LOGFONT newlogFont = *drawfont;
		newlogFont.lfWidth *= factor;
		newlogFont.lfHeight *= factor; 

		//need CreateFontIndirectW ?
		dxfont.CreateFontIndirect(&newlogFont);
		oldfont = (CFont *) pMemDC->SelectObject(&dxfont);
	}
	//if no default font is selected, can cause errors

	
	
	if (pPen)
		oldPen = pMemDC->SelectObject(pPen);

	if (pBrush)
		oldBrush = pMemDC->SelectObject(pBrush);

	int textlength = textstr.GetLength(); //get number of bytes
	

	//Draw Text

	SetBkMode(hMemDC,TRANSPARENT); 
	SetTextColor(hMemDC,textcolor);
	//DrawTextEx(hMemDC, (char *)LPCTSTR(textstr), textlength, LPRECT(usetextRect), horzalign | DT_VCENTER | DT_WORDBREAK | DT_EDITCONTROL ,  NULL);
		
	//use adaptive antialias...if size< than maxxScreen maxyScreen
	if ((versionOp>=5) && ((usetextRect.Width()>maxxScreen) || (usetextRect.Height()>maxyScreen)))
	{   //use stroke path method, less buggy
	
		BeginPath(hMemDC);
		DrawTextEx(hMemDC, (char *)LPCTSTR(textstr), textlength, LPRECT(usetextRect), horzalign | DT_VCENTER | DT_WORDBREAK  ,  NULL);
		EndPath(hMemDC);

		//CPen testpen;
		//testpen.CreatePen(PS_SOLID,0,textcolor);
		//oldPen = pMemDC->SelectObject(&testpen);	
			
		CBrush testbrush;
		testbrush.CreateSolidBrush(textcolor);
		oldBrush = pMemDC->SelectObject(&testbrush);
		//StrokeAndFillPath(hMemDC);
		FillPath(hMemDC);
		pMemDC->SelectObject(oldBrush);
		//pMemDC->SelectObject(oldPen);

		testbrush.DeleteObject();
		//testpen.DeleteObject();

	}
	else
		DrawTextEx(hMemDC, (char *)LPCTSTR(textstr), textlength, LPRECT(usetextRect), horzalign | DT_VCENTER | DT_WORDBREAK  , NULL);
	
	
	
	
	if (pBrush)
		pMemDC->SelectObject(oldBrush);
	
	if (pPen)
		pMemDC->SelectObject(oldPen);	

	
	if (drawfont) {			
		
		//no need to destroy dcfont?
		//assume the destructor of Cfont will take care of freeing dxfont resources
		pMemDC->SelectObject(oldfont);
		dxfont.DeleteObject();
	}


	SelectObject(hMemDC,oldbm);    			
	
	
	LPBITMAPINFOHEADER pBM_HEADER = (LPBITMAPINFOHEADER)GlobalLock(Bitmap2Dib(hbm, 24));

	if (pBM_HEADER == NULL) { 
		return NULL;
	}    

	pMemDC->Detach(); 

	DeleteObject(hbm);			
	DeleteDC(hMemDC);		

	return (LPBITMAPINFO) pBM_HEADER;

}

//AntiAlias 24 Bit Image
//valid factors : 1, 2, 3 
HBITMAP DrawResampleRGB(CDC *thisDC, CRect* caprect,int factor, LPBITMAPINFOHEADER expanded_bmi, int xmove, int ymove) 
{

	int bits = 24;
					 
	LONG   Width =   expanded_bmi->biWidth;
	LONG   Height =  expanded_bmi->biHeight;	
	long Rowbytes = (Width*bits+31)/32 *4;

	long reduced_width = Width/factor;
	long reduced_height = Height/factor;
	long reduced_rowbytes = (reduced_width*bits+31)/32 *4;


	if ((factor<1) || (factor>3))
		return NULL;


	//Create destination buffer	
	//long dwSize = sizeof(BITMAPINFOHEADER) + reduced_rowbytes * reduced_height * 3;


	// Allocate room for a DIB and set the LPBI fields
	LPBITMAPINFOHEADER smallbi = (LPBITMAPINFOHEADER)GlobalLock(AllocMakeDib( reduced_width, reduced_height, 24));
	if (smallbi == NULL) { 
		return NULL;
	}    	
	
	// Get the bits from the bitmap and stuff them after the LPBI	
	LPBYTE lpBits = (LPBYTE)(smallbi+1);

	//Perform the re-sampling
	long x,y,z;
	LPBYTE reduced_ptr;
	LPBYTE Ptr;

	LPBYTE reduced_rowptr = lpBits;
	LPBYTE Rowptr = (LPBYTE) (expanded_bmi + 1);
	
	//Set the pointers
	reduced_ptr = lpBits; 	
	Ptr = (LPBYTE) (expanded_bmi + 1);		

	int Ptr_incr = (factor-1)*3;
	int Row_incr = Rowbytes * factor;

	int totalval;

	for (y=0;y< reduced_height;y++) {
		
		//Set to start of each row
		reduced_ptr = reduced_rowptr;
		Ptr = Rowptr;
		
		for (x=0;x< reduced_width;x++) {			

			//Ptr_Pixel = Ptr;

			//for each RGB component
			for (z=0;z<3;z++) {

				if (factor==1)
					*reduced_ptr = *Ptr;				
				else  if (factor==2) {

					totalval = 0;
					totalval += *Ptr;
					totalval += *(Ptr + 3) ;
					totalval += *(Ptr + Rowbytes) ;
					totalval += *(Ptr + Rowbytes + 3) ;
					totalval/=4;

					if (totalval<0)
						totalval = 0;

					if (totalval>255)
						totalval = 255;

					*reduced_ptr = (BYTE) totalval;

				}
				else  if (factor==3) {

					totalval = 0;
					totalval += *Ptr;
					totalval += *(Ptr + 3) ;
					totalval += *(Ptr + 6) ;
					
					totalval += *(Ptr + Rowbytes) ;
					totalval += *(Ptr + Rowbytes + 3) ;
					totalval += *(Ptr + Rowbytes + 6) ;
					
					totalval += *(Ptr + Rowbytes + Rowbytes) ;
					totalval += *(Ptr + Rowbytes + Rowbytes + 3) ;
					totalval += *(Ptr + Rowbytes + Rowbytes + 6) ;
									
					totalval/=9;

					if (totalval<0)
						totalval = 0;

					if (totalval>255)
						totalval = 255;

					*reduced_ptr = (BYTE) totalval;
				
				} //else if factor
				

				reduced_ptr++;
				Ptr ++;			

			} //for  z

			Ptr += Ptr_incr;
			//Ptr += factor * 3; 


		} //for x
		 
		reduced_rowptr += reduced_rowbytes;
		//Rowptr += Rowbytes;
		Rowptr += Row_incr;

	}	// for y
	
	
	int ret = StretchDIBits ( thisDC->m_hDC,
								xmove, ymove,reduced_width,reduced_height,
								0, 0,reduced_width,reduced_height,                        
								lpBits, (LPBITMAPINFO)smallbi,
								DIB_RGB_COLORS,SRCCOPY);				



	

	if (smallbi)
		GlobalFreePtr(smallbi);

		
	HBITMAP newbm = NULL;
	return newbm;

}

*/


void CVscapView::OnOptionsNamingAutodate() 
{
	// TODO: Add your command handler code here
	autonaming = 1;

	
}

void CVscapView::OnUpdateOptionsNamingAutodate(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(autonaming == 1);
	
	
}

void CVscapView::OnOptionsNamingAsk() 
{
	// TODO: Add your command handler code here
	autonaming = 0;
	
}

void CVscapView::OnUpdateOptionsNamingAsk(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(autonaming == 0);
	
}

void CVscapView::OnOptionsProgramoptionsPresettime() 
{
	// TODO: Add your command handler code here
	CPresetTime prestDlg;
	prestDlg.DoModal();
	
}
