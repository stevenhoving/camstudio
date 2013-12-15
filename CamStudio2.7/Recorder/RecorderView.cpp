// RenderSoft CamStudio
//
// Copyright 2001 - 2003 RenderSoft Software & Web Publishing
//
//
// vscapView.cpp : implementation of the CRecorderView class
//
/////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "Recorder.h"

#include "RecorderView.h"
#include "RecorderDoc.h"
#include "MainFrm.h"

#include "MouseCaptureWnd.h"
#include "CamCursor.h"

#include "AutopanSpeed.h"
#include "AudioFormat.h"
#include "AutoSearchDialog.h"
#include "CursorOptionsDlg.h"
#include "FolderDialog.h"
#include "FixedRegion.h"
#include "Keyshortcuts.h"
#include "ListManager.h"
#include "PresetTime.h"
#include "ScreenAnnotations.h"
#include "SyncDialog.h"
#include "TroubleShoot.h"
#include "VideoOptions.h"
#include "ProgressDlg.h"
#include "ximage.h"
#include "soundfile.h"
#include "hook.h"		// for WM_USER_RECORDSTART_MSG
#include "MP4Converter.h"

#include "Buffer.h"
#include "CStudioLib.h"
#include "TrayIcon.h"
#include "AudioSpeakers.h"
#include "HotKey.h"
#include "Screen.h"
#include "vfw/VCM.h"
#include "vfw/ACM.h"
#include "AVI.h"
#include "MCI.h"

#include "addons/XnoteStopwatchFormat.h"
#include "addons/Camstudio4XNote.h"
#include "addons/AnnotationEffectsOptions.h"
#include "addons/EffectsOptions.h"
#include "addons/EffectsOptions2.h"

#ifdef _DEBUG
// #include <vld.h>		// Visual Leak Detector utility (In debug mode)
#endif

#include <windowsx.h>
#include <fstream>
#include <iostream>
#include <time.h>
using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
//AVI functions and #defines
#define AVIIF_KEYFRAME	0x00000010L // this frame is a key frame.
#define BUFSIZE			260
#define N_FRAMES		50
#define TEXT_HEIGHT		20

#define LPLPBI			LPBITMAPINFOHEADER *

#define BUF_SIZE 4096

//version 1.6

#if !defined(WAVE_FORMAT_MPEGLAYER3)
#define WAVE_FORMAT_MPEGLAYER3 0x0055
#endif

#define SWAP(x,y) ((x) ^= (y) ^= (x) ^= (y))

#ifndef CAPTUREBLT
#define CAPTUREBLT (DWORD)0x40000000
#endif

/////////////////////////////////////////////////////////////////////////////
// external functions
/////////////////////////////////////////////////////////////////////////////
extern void FreeWaveoutResouces();

extern BOOL useWavein(BOOL,int);
extern BOOL useWaveout(BOOL,int);
extern BOOL WaveoutUninitialize();

extern BOOL initialSaveMMMode();
extern BOOL finalRestoreMMMode();
extern BOOL onLoadSettings(int iRecordAudio);

/////////////////////////////////////////////////////////////////////////////
// external variables
/////////////////////////////////////////////////////////////////////////////
extern int iRrefreshRate;
extern CString shapeName;
extern CString strLayoutName;

/////////////////////////////////////////////////////////////////////////////
// unused variables
/////////////////////////////////////////////////////////////////////////////
#ifdef UNUSED_CODE
CTransparentWnd* transWnd;
#endif	// UNUSED_CODE

/////////////////////////////////////////////////////////////////////////////
//State variables
/////////////////////////////////////////////////////////////////////////////

//Vars used for selecting fixed /variableregion
CRect rc;			// Size:  0 .. MaxScreenSize-1
CRect rcUse;		// Size:  0 .. MaxScreenSize-1
CRect rcClip;		// Size:  0 .. MaxScreenSize-1
CRect old_rcClip;	// Size:  0 .. MaxScreenSize-1
CRect rcOffset;
CPoint ptOrigin;

//Autopan
CRect rectPanCurrent;
CRect rectPanDest;

BOOL bCapturing = FALSE;

HWND hFixedRegionWnd;

HBITMAP hLogoBM = NULL;

//Misc Vars
bool bAlreadyMCIPause = false;
bool bRecordState = false;
bool bRecordPaused = false;
UINT interruptkey = 0;
DWORD dwInitialTime = 0;
bool bInitCapture = false;
int irsmallcount = 0;
unsigned long nTotalBytesWrittenSoFar = 0UL;

// Xnote timing support. Show timing from start of recording
// Todo , All these global/local vars should become an struct or class
bool bXNoteSnapRecordingState	= false;		// This settings defines if video recording is triggered by xnote. if trggered by xnote it will stop automattically is this option is set by user.
ULONG ulXNoteStartTime			= 0UL;
ULONG ulXNoteLastSnapTime		= 0UL;
int iXNoteStartSource			= XNOTE_SOURCE_UNDEFINED;		
int iXNoteLastSnapSource		= XNOTE_SOURCE_UNDEFINED;		
int iXNoteStartWithSensor		= XNOTE_TRIGGER_UNDEFINED;		// Defines if event is manual (with key or mouse) or automatic generated (by means of RS232 sensored device or Video Motion Detection)
int iXNoteLastSnapWithSensor	= XNOTE_TRIGGER_UNDEFINED;		// Defines if event is manual (with key or mouse) or automatic generated (by means of RS232 sensored device or Video Motion Detection)

char cXNoteLastSnapTimes[128] /*= {'\0'} */;
HFILE hXnoteLogFile = NULL;
ofstream ioXnoteLogFile;
ofstream * pioXnoteLogFile = (ofstream*)0;

// Messaging
HWND hWndGlobal = NULL;

//int iTempPathAccess = USE_WINDOWS_TEMP_DIR;
//CString specifieddir;

/////////////////////////////////////////////////////////////////////////////
//Variables/Options requiring interface
/////////////////////////////////////////////////////////////////////////////
int iBits = 24;
int iDefineMode = 0; //set only in FixedRegion.cpp

int selected_compressor = -1;

//Report variables
int nActualFrame = 0;
int nCurrFrame = 0;
float fRate = 0.0;
float fActualRate = 0.0;
float fTimeLength = 0.0;
int nColors = 24;
CString sTimeLength;

//Path to temporary video avi file
CString strTempVideoAviFilePath;

//Path to temporary audio wav file
CString strTempAudioWavFilePath;

//Path to temporary xnote log fiel  (xnote=stopwatch appl. File will be used to record snaps and offsets to video file)
CString strTempXnoteLogFilePath;


// Ver 1.1
/////////////////////////////////////////////////////////////////////////////
// Audio Functions and Variables
/////////////////////////////////////////////////////////////////////////////
// The program records video and sound separately, into 2 files
// ~temp.avi and ~temp.wav, before merging these 2 file into a single avi file
// using the Merge_Video_And_Sound_File function
/////////////////////////////////////////////////////////////////////////////


HWAVEIN m_hWaveRecord;
DWORD m_ThreadID;
int m_QueuedBuffers = 0;
int iBufferSize = 1000; // number of samples
CSoundFile * pSoundFile = NULL;

//Audio Options Dialog
//LPWAVEFORMATEX pwfx = NULL;

/////////////////////////////////////////////////////////////////////////////
//ver 1.2
/////////////////////////////////////////////////////////////////////////////
//Key short-cuts variables
/////////////////////////////////////////////////////////////////////////////
//state vars
BOOL bAllowNewRecordStartKey = TRUE;

PSTR strFile;

int TroubleShootVal = 0;

//ver 1.8

CScreenAnnotationsDlg sadlg;
int bCreatedSADlg = false;

//ver 1.8
int vanWndCreated = 0;

int keySCOpened = 0;

int iAudioTimeInitiated = 0;
int sdwSamplesPerSec = 22050;
int sdwBytesPerSec = 44100;

int iCurrentLayout = 0;

sProgramOpts cProgramOpts;
sProducerOpts cProducerOpts;
sAudioFormat cAudioFormat;
sVideoOpts cVideoOpts;
sHotKeyOpts cHotKeyOpts;
sRegionOpts cRegionOpts;
CCamCursor CamCursor;
sCaptionOpts cCaptionOpts;
sTimestampOpts cTimestampOpts;
sXNoteOpts cXNoteOpts;
sWatermarkOpts cWatermarkOpts;

CString strCodec("MS Video 1");
//Files Directory
CString savedir("");

HBITMAP hSavedBitmap = NULL;

/////////////////////////////////////////////////////////////////////////////
//Function prototypes
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// ===========================================================================
//ver 1.2 
// ===========================================================================
//Key short-cuts variables
// ===========================================================================

UINT keyRecordStart = VK_F8;
UINT keyRecordEnd = VK_F9;
UINT keyRecordCancel = VK_F10;
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
int SetAdjustHotKeys();
int SetHotKeys(int succ[]);
//Region Display Functions
void DrawSelect(HDC hdc, BOOL fDraw, LPRECT lprClip);

//Misc Functions
void SetVideoCompressState (HIC hic, DWORD fccHandler);

//void CALLBACK OnMM_WIM_DATA(UINT parm1, LONG parm2);
//BOOL CALLBACK SaveCallback(int iProgress);

namespace {	// annonymous

	////////////////////////////////
	// AUDIO_CODE
	////////////////////////////////
	void ClearAudioFile();
	BOOL InitAudioRecording();
	void GetTempAudioWavPath();
	//BOOL StartAudioRecording(WAVEFORMATEX* format);
	BOOL StartAudioRecording();
	void StopAudioRecording();
	void waveInErrorMsg(MMRESULT result, const char *);
	void DataFromSoundIn(CBuffer* buffer);
	int AddInputBufferToQueue();

	////////////////////////////////
	// XNOTE_CODE
	////////////////////////////////
	void GetTempXnoteLogPath();
	BOOL OpenStreamXnoteLogFile();
	void CloseStreamXnoteLogFile();
	void WriteLineToXnoteLogFile( char* pStr);

	////////////////////////////////
	//Region Select Functions
	////////////////////////////////
	int InitDrawShiftWindow();
	int InitSelectRegionWindow();

	////////////////////////////////
	// HOTKEYS_CODE
	////////////////////////////////
	//bool UnSetHotKeys(HWND hWnd);
	//int SetHotKeys(int succ[]);

	//===============================================
	// EXPERIMENTAL CODE
	//===============================================

	////////////////////////////////
	// OBSOLETE_CODE
	////////////////////////////////
#define THIS_COULD_BE_OBSOLETE_CODE
#ifndef THIS_COULD_BE_OBSOLETE_CODE
	void SetBufferSize(int NumberOfSamples);		//  TODO Might be obsolete...!
	void GetVideoCompressState (HIC hic, DWORD fccHandler);
#endif  // THIS_COULD_BE_OBSOLETE_CODE

}	// namespace annonymous

//Functions that select audio options based on settings read

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
//ver 1.8
int UnSetHotKeys()
{

	
	UnregisterHotKey(hWndGlobal,HOTKEY_RECORD_START_OR_PAUSE);
	UnregisterHotKey(hWndGlobal,HOTKEY_RECORD_STOP);
	UnregisterHotKey(hWndGlobal,HOTKEY_RECORD_CANCELSTOP);
	UnregisterHotKey(hWndGlobal,HOTKEY_LAYOUT_KEY_NEXT);
	UnregisterHotKey(hWndGlobal,HOTKEY_LAYOUT_KEY_PREVIOUS);
	UnregisterHotKey(hWndGlobal,HOTKEY_LAYOUT_SHOW_HIDE_KEY);
	UnregisterHotKey(hWndGlobal,HOTKEY_ZOOM);
	UnregisterHotKey(hWndGlobal,HOTKEY_AUTOPAN_SHOW_HIDE_KEY);

	return 0;

}
int SetAdjustHotKeys()
{

	int succ[8];
	int ret = SetHotKeys(succ);

	return 7; // return the max value of #define for Hotkey in program???
}
int SetHotKeys(int succ[])
{

	UnSetHotKeys();
	
	
	for (int i =0 ;i<6 ;i++)
		succ[i]=0;

	int tstatus = 0;

	BOOL ret;
	int nid = 0;
	if (cHotKeyOpts.m_RecordStart.m_vKey != VK_UNDEFINED) {
		ret = RegisterHotKey(hWndGlobal,nid, cHotKeyOpts.m_RecordStart.m_fsMod, cHotKeyOpts.m_RecordStart.m_vKey);
		if (!ret) 
			succ[0] = 1;
	}

	nid++;
	
	if (cHotKeyOpts.m_RecordEnd.m_vKey != VK_UNDEFINED) {
		ret = RegisterHotKey(hWndGlobal,nid,cHotKeyOpts.m_RecordEnd.m_fsMod,cHotKeyOpts.m_RecordEnd.m_vKey);
		if (!ret) 
			succ[1] = 1;
	}

	nid++;

	if (cHotKeyOpts.m_RecordCancel.m_vKey != VK_UNDEFINED) {
		ret = RegisterHotKey(hWndGlobal,nid,cHotKeyOpts.m_RecordCancel.m_fsMod, cHotKeyOpts.m_RecordCancel.m_vKey);
		if (!ret) 
			succ[2] = 1;
	}


	nid++;

	if (cHotKeyOpts.m_Next.m_vKey != VK_UNDEFINED) {	
		ret = RegisterHotKey(hWndGlobal,nid,cHotKeyOpts.m_Next.m_fsMod,cHotKeyOpts.m_Next.m_vKey);
		if (!ret) 
			succ[3] = 1;
	}

	nid++;

	if (cHotKeyOpts.m_Prev.m_vKey != VK_UNDEFINED) {	
		ret = RegisterHotKey(hWndGlobal,nid,cHotKeyOpts.m_Prev.m_fsMod, cHotKeyOpts.m_Next.m_vKey);
		if (!ret) 
			succ[4] = 1;

	}

	nid++;

	if (cHotKeyOpts.m_ShowLayout.m_vKey != VK_UNDEFINED) {
		ret = RegisterHotKey(hWndGlobal,nid,cHotKeyOpts.m_ShowLayout.m_fsMod, cHotKeyOpts.m_ShowLayout.m_vKey);
		if (!ret) 			
			succ[5] = 1;
	}

	nid++;
	
	if(cHotKeyOpts.m_Zoom.m_vKey != VK_UNDEFINED) {
		ret = RegisterHotKey(hWndGlobal, nid, cHotKeyOpts.m_Zoom.m_fsMod, cHotKeyOpts.m_Zoom.m_vKey);
		if(!ret)
			succ[6] = 1;
	}
	
	nid++;

	if(cHotKeyOpts.m_Autopan.m_vKey != VK_UNDEFINED) {
		ret = RegisterHotKey(hWndGlobal, nid, cHotKeyOpts.m_Autopan.m_fsMod, cHotKeyOpts.m_Autopan.m_vKey);
		if(!ret)
			succ[7] = 1;
	}
	return tstatus;

}

void SetVideoCompressState (HIC hic, DWORD fccHandler)
{
	if (cVideoOpts.m_dwCompressorStateIsFor == fccHandler) {
		if (cVideoOpts.State()) {
			DWORD ret = ICSetState(hic,(LPVOID) cVideoOpts.State(), cVideoOpts.StateSize());
			//if (ret <= 0) {
			if (ret < 0) {
				//MessageBox(NULL, "Failure in setting compressor state !","Note",MB_OK | MB_ICONEXCLAMATION);
				MessageOut(NULL,IDS_STRING_SETCOMPRESSOR,IDS_STRING_NOTE,MB_OK | MB_ICONEXCLAMATION);
			}
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// CRecorderView

UINT CRecorderView::WM_USER_RECORDINTERRUPTED = ::RegisterWindowMessage(WM_USER_RECORDINTERRUPTED_MSG);
UINT CRecorderView::WM_USER_RECORDPAUSED = ::RegisterWindowMessage(WM_USER_RECORDPAUSED_MSG);
UINT CRecorderView::WM_USER_SAVECURSOR = ::RegisterWindowMessage(WM_USER_SAVECURSOR_MSG);
UINT CRecorderView::WM_USER_GENERIC = ::RegisterWindowMessage(WM_USER_GENERIC_MSG);
UINT CRecorderView::WM_USER_RECORDSTART = ::RegisterWindowMessage(WM_USER_RECORDSTART_MSG);

IMPLEMENT_DYNCREATE(CRecorderView, CView)

BEGIN_MESSAGE_MAP(CRecorderView, CView)
	//{{AFX_MSG_MAP(CRecorderView)
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SETFOCUS()
	ON_WM_ERASEBKGND()
	ON_COMMAND(ID_RECORD, OnRecord)
	ON_UPDATE_COMMAND_UI(ID_RECORD, OnUpdateRecord)
	ON_COMMAND(ID_STOP, OnStop)
	ON_UPDATE_COMMAND_UI(ID_STOP, OnUpdateStop)
	ON_COMMAND(ID_REGION_RUBBER, OnRegionRubber)
	ON_UPDATE_COMMAND_UI(ID_REGION_RUBBER, OnUpdateRegionRubber)
	ON_COMMAND(ID_REGION_PANREGION, OnRegionPanregion)
	ON_UPDATE_COMMAND_UI(ID_REGION_PANREGION, OnUpdateRegionPanregion)
	ON_COMMAND(ID_OPTIONS_AUTOPAN, OnOptionsAutopan)
	ON_UPDATE_COMMAND_UI(ID_OPTIONS_AUTOPAN, OnUpdateOptionsAutopan)
	ON_COMMAND(ID_OPTIONS_VIDEOOPTIONS, OnFileVideooptions)
	ON_COMMAND(ID_OPTIONS_CURSOROPTIONS, OnOptionsCursoroptions)
	ON_COMMAND(ID_OPTIONS_ATUOPANSPEED, OnOptionsAtuopanspeed)
	ON_COMMAND(ID_REGION_FULLSCREEN, OnRegionFullscreen)
	ON_UPDATE_COMMAND_UI(ID_REGION_FULLSCREEN, OnUpdateRegionFullscreen)
	ON_COMMAND(ID_SCREENS_SELECTSCREEN, OnRegionSelectScreen)
	ON_UPDATE_COMMAND_UI(ID_SCREENS_SELECTSCREEN, OnUpdateRegionSelectScreen)
	ON_COMMAND(ID_SCREENS_ALLSCREENS, OnRegionAllScreens)
	ON_UPDATE_COMMAND_UI(ID_SCREENS_ALLSCREENS, OnUpdateRegionAllScreens)
	ON_COMMAND(ID_HELP_WEBSITE, OnHelpWebsite)
	ON_COMMAND(ID_HELP_HELP, OnHelpHelp)
	ON_COMMAND(ID_PAUSE, OnPause)
	ON_UPDATE_COMMAND_UI(ID_PAUSE, OnUpdatePause)
	ON_COMMAND(ID_OPTIONS_RECORDAUDIO, OnOptionsRecordaudio)
	ON_UPDATE_COMMAND_UI(ID_OPTIONS_RECORDAUDIO, OnUpdateOptionsRecordaudio)
	ON_COMMAND(ID_OPTIONS_AUDIOFORMAT, OnOptionsAudioformat)
	ON_COMMAND(ID_OPTIONS_AUDIOSPEAKERS, OnOptionsAudiospeakers)
	ON_COMMAND(ID_HELP_FAQ, OnHelpFaq)
	ON_COMMAND(ID_OPTIONS_RECORDAUDIO_DONOTRECORDAUDIO, OnOptionsRecordaudioDonotrecordaudio)
	ON_UPDATE_COMMAND_UI(ID_OPTIONS_RECORDAUDIO_DONOTRECORDAUDIO, OnUpdateOptionsRecordaudioDonotrecordaudio)
	ON_COMMAND(ID_OPTIONS_RECORDAUDIO_RECORDFROMSPEAKERS, OnOptionsRecordaudioRecordfromspeakers)
	ON_UPDATE_COMMAND_UI(ID_OPTIONS_RECORDAUDIO_RECORDFROMSPEAKERS, OnUpdateOptionsRecordaudioRecordfromspeakers)
	ON_COMMAND(ID_OPTIONS_RECORDAUDIOMICROPHONE, OnOptionsRecordaudiomicrophone)
	ON_UPDATE_COMMAND_UI(ID_OPTIONS_RECORDAUDIOMICROPHONE, OnUpdateOptionsRecordaudiomicrophone)
	ON_COMMAND(ID_HELP_DONATIONS, OnHelpDonations)
	ON_COMMAND(ID_VIEW_SCREENANNOTATIONS, OnViewScreenannotations)
	ON_UPDATE_COMMAND_UI(ID_VIEW_SCREENANNOTATIONS, OnUpdateViewScreenannotations)
	ON_COMMAND(ID_VIEW_VIDEOANNOTATIONS, OnViewVideoannotations)
	ON_COMMAND(ID_OPTIONS_AUDIOOPTIONS_AUDIOVIDEOSYNCHRONIZATION, OnOptionsSynchronization)
	ON_COMMAND(ID_TOOLS_SWFPRODUCER, OnToolsSwfproducer)
	ON_COMMAND(ID_OPTIONS_SWF_LAUNCHHTML, OnOptionsSwfLaunchhtml)
	ON_COMMAND(ID_OPTIONS_SWF_DELETEAVIFILE, OnOptionsSwfDeleteavifile)
	ON_COMMAND(ID_OPTIONS_SWF_DISPLAYPARAMETERS, OnOptionsSwfDisplayparameters)
	ON_UPDATE_COMMAND_UI(ID_OPTIONS_SWF_LAUNCHHTML, OnUpdateOptionsSwfLaunchhtml)
	ON_UPDATE_COMMAND_UI(ID_OPTIONS_SWF_DISPLAYPARAMETERS, OnUpdateOptionsSwfDisplayparameters)
	ON_UPDATE_COMMAND_UI(ID_OPTIONS_SWF_DELETEAVIFILE, OnUpdateOptionsSwfDeleteavifile)
	ON_COMMAND(ID_AVISWF, OnAVISWFMP4)
	ON_COMMAND(ID_OPTIONS_NAMING_AUTODATE, OnOptionsNamingAutodate)
	ON_UPDATE_COMMAND_UI(ID_OPTIONS_NAMING_AUTODATE, OnUpdateOptionsNamingAutodate)
	ON_UPDATE_COMMAND_UI(ID_OPTIONS_LANGUAGE_ENGLISH, OnUpdateOptionsLanguageEnglish)
	ON_UPDATE_COMMAND_UI(ID_OPTIONS_LANGUAGE_GERMAN, OnUpdateOptionsLanguageGerman)
	ON_UPDATE_COMMAND_UI(ID_OPTIONS_LANGUAGE_FILIPINO, OnUpdateOptionsLanguageFilipino)
	ON_COMMAND(ID_OPTIONS_LANGUAGE_ENGLISH, OnOptionsLanguageEnglish)
	ON_COMMAND(ID_OPTIONS_LANGUAGE_GERMAN, OnOptionsLanguageFilipino)
	ON_COMMAND(ID_OPTIONS_LANGUAGE_FILIPINO, OnOptionsLanguageFilipino)
	ON_COMMAND(ID_REGION_WINDOW, OnRegionWindow)
	ON_UPDATE_COMMAND_UI(ID_REGION_WINDOW, OnUpdateRegionWindow)

	ON_COMMAND(ID_ANNOTATION_ADDSYSTEMTIMESTAMP, OnAnnotationAddsystemtimestamp)
	ON_UPDATE_COMMAND_UI(ID_ANNOTATION_ADDSYSTEMTIMESTAMP, OnUpdateAnnotationAddsystemtimestamp)
	
	ON_COMMAND(ID_ANNOTATION_ADDXNOTE, OnAnnotationAddXNote)
	ON_UPDATE_COMMAND_UI(ID_ANNOTATION_ADDXNOTE, OnUpdateAnnotationAddXNote)

	ON_COMMAND(ID_XNOTE_RECORDDURATIONLIMITMODE, OnXnoteRecordDurationLimitMode)

	ON_COMMAND(ID_ANNOTATION_ADDCAPTION, OnAnnotationAddcaption)
	ON_UPDATE_COMMAND_UI(ID_ANNOTATION_ADDCAPTION, OnUpdateAnnotationAddcaption)
	ON_COMMAND(ID_ANNOTATION_ADDWATERMARK, OnAnnotationAddwatermark)
	ON_UPDATE_COMMAND_UI(ID_ANNOTATION_ADDWATERMARK, OnUpdateAnnotationAddwatermark)
	ON_COMMAND(ID_EFFECTS_OPTIONS, OnEffectsOptions)
	ON_COMMAND(ID_OPTIONS_PROGRAMOPTIONS_PRESETTIME, OnOptionsProgramoptionsPresettime)
	ON_COMMAND(ID_OPTIONS_MINIMIZEONSTART, OnOptionsMinimizeonstart)
	ON_UPDATE_COMMAND_UI(ID_OPTIONS_MINIMIZEONSTART, OnUpdateOptionsMinimizeonstart)
	ON_COMMAND(ID_OPTIONS_HIDEFLASHING, OnOptionsHideflashing)
	ON_UPDATE_COMMAND_UI(ID_OPTIONS_HIDEFLASHING, OnUpdateOptionsHideflashing)
	ON_COMMAND(ID_OPTIONS_PROGRAMOPTIONS_SAVESETTINGSONEXIT, OnOptionsProgramoptionsSavesettingsonexit)
	ON_UPDATE_COMMAND_UI(ID_OPTIONS_PROGRAMOPTIONS_SAVESETTINGSONEXIT, OnUpdateOptionsProgramoptionsSavesettingsonexit)
	ON_COMMAND(ID_OPTIONS_CAPTURETRANS, OnOptionsCapturetrans)
	ON_UPDATE_COMMAND_UI(ID_OPTIONS_CAPTURETRANS, OnUpdateOptionsCapturetrans)
	ON_COMMAND(ID_OPTIONS_PROGRAMOPTIONS_PLAYAVI, OnOptionsProgramoptionsPlayavi)
	ON_UPDATE_COMMAND_UI(ID_OPTIONS_PROGRAMOPTIONS_PLAYAVI, OnUpdateOptionsProgramoptionsPlayavi)
	ON_COMMAND(ID_OPTIONS_PROGRAMOPTIONS_NOPLAY, OnOptionsProgramoptionsNoplay)
	ON_UPDATE_COMMAND_UI(ID_OPTIONS_PROGRAMOPTIONS_NOPLAY, OnUpdateOptionsProgramoptionsNoplay)
	ON_COMMAND(ID_OPTIONS_PROGRAMOPTIONS_DEFAULTPLAY, OnOptionsProgramoptionsDefaultplay)
	ON_UPDATE_COMMAND_UI(ID_OPTIONS_PROGRAMOPTIONS_DEFAULTPLAY, OnUpdateOptionsProgramoptionsDefaultplay)
	ON_COMMAND(ID_OPTIONS_PROGRAMOPTIONS_CAMSTUDIOPLAY, OnOptionsProgramoptionsCamstudioplay)
	ON_UPDATE_COMMAND_UI(ID_OPTIONS_PROGRAMOPTIONS_CAMSTUDIOPLAY, OnUpdateOptionsProgramoptionsCamstudioplay)
	ON_COMMAND(ID_OPTIONS_PROGRAMOPTIONS_PLAYAVIFILEWHENRECORDINGSTOPS_USECAMSTUDIOPLAYER20, OnOptionsUsePlayer20)
	ON_UPDATE_COMMAND_UI(ID_OPTIONS_PROGRAMOPTIONS_PLAYAVIFILEWHENRECORDINGSTOPS_USECAMSTUDIOPLAYER20, OnUpdateUsePlayer20)
	ON_COMMAND(ID_OPTIONS_TEMPDIR_WINDOWS, OnOptionsTempdirWindows)
	ON_UPDATE_COMMAND_UI(ID_OPTIONS_TEMPDIR_WINDOWS, OnUpdateOptionsTempdirWindows)
	ON_COMMAND(ID_OPTIONS_TEMPDIR_INSTALLED, OnOptionsTempdirInstalled)
	ON_UPDATE_COMMAND_UI(ID_OPTIONS_TEMPDIR_INSTALLED, OnUpdateOptionsTempdirInstalled)
	ON_COMMAND(ID_OPTIONS_TEMPDIR_USER, OnOptionsTempdirUser)
	ON_UPDATE_COMMAND_UI(ID_OPTIONS_TEMPDIR_USER, OnUpdateOptionsTempdirUser)

	ON_COMMAND(ID_OUTPUTDIRECTORY_USEWINDOWSTEMPORARYDIRECTORY, OnOptionsOutputDirWindows)
	ON_UPDATE_COMMAND_UI(ID_OUTPUTDIRECTORY_USEWINDOWSTEMPORARYDIRECTORY, OnUpdateOptionsOutputDirWindows)

	ON_COMMAND(ID_OUTPUTDIRECTORY_USEMYCAMSTUDIORECORDINGSDIRECTORY, OnOptionsOutputDirInstalled)
	ON_UPDATE_COMMAND_UI(ID_OUTPUTDIRECTORY_USEMYCAMSTUDIORECORDINGSDIRECTORY, OnUpdateOptionsOutputDirInstalled)

	ON_COMMAND(ID_OUTPUTDIRECTORY_USEUSERSPECIFIEDDIRECTORY, OnOptionsOutputDirUser)
	ON_UPDATE_COMMAND_UI(ID_OUTPUTDIRECTORY_USEUSERSPECIFIEDDIRECTORY, OnUpdateOptionsUser)

	ON_COMMAND(ID_OPTIONS_RECORDINGTHREADPRIORITY_NORMAL, OnOptionsRecordingthreadpriorityNormal)
	ON_UPDATE_COMMAND_UI(ID_OPTIONS_RECORDINGTHREADPRIORITY_NORMAL, OnUpdateOptionsRecordingthreadpriorityNormal)
	ON_COMMAND(ID_OPTIONS_RECORDINGTHREADPRIORITY_HIGHEST, OnOptionsRecordingthreadpriorityHighest)
	ON_UPDATE_COMMAND_UI(ID_OPTIONS_RECORDINGTHREADPRIORITY_HIGHEST, OnUpdateOptionsRecordingthreadpriorityHighest)
	ON_COMMAND(ID_OPTIONS_RECORDINGTHREADPRIORITY_ABOVENORMAL, OnOptionsRecordingthreadpriorityAbovenormal)
	ON_UPDATE_COMMAND_UI(ID_OPTIONS_RECORDINGTHREADPRIORITY_ABOVENORMAL, OnUpdateOptionsRecordingthreadpriorityAbovenormal)
	ON_COMMAND(ID_OPTIONS_RECORDINGTHREADPRIORITY_TIMECRITICAL, OnOptionsRecordingthreadpriorityTimecritical)
	ON_UPDATE_COMMAND_UI(ID_OPTIONS_RECORDINGTHREADPRIORITY_TIMECRITICAL, OnUpdateOptionsRecordingthreadpriorityTimecritical)
	ON_COMMAND(ID_OPTIONS_NAMING_ASK, OnOptionsNamingAsk)
	ON_UPDATE_COMMAND_UI(ID_OPTIONS_NAMING_ASK, OnUpdateOptionsNamingAsk)
	ON_COMMAND(ID_OPTIONS_KEYBOARDSHORTCUTS, OnOptionsKeyboardshortcuts)
	ON_COMMAND(ID_OPTIONS_PROGRAMOPTIONS_TROUBLESHOOT, OnOptionsProgramoptionsTroubleshoot)
	ON_COMMAND(ID_CAMSTUDIO4XNOTE_WEBSITE, OnCamstudio4XnoteWebsite)
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
	ON_REGISTERED_MESSAGE(CRecorderView::WM_USER_RECORDSTART, OnRecordStart)
	ON_REGISTERED_MESSAGE(CRecorderView::WM_USER_RECORDINTERRUPTED, OnRecordInterrupted)
	ON_REGISTERED_MESSAGE(CRecorderView::WM_USER_RECORDPAUSED, OnRecordPaused)
	ON_REGISTERED_MESSAGE(CRecorderView::WM_USER_SAVECURSOR, OnSaveCursor)
	ON_REGISTERED_MESSAGE(CRecorderView::WM_USER_GENERIC, OnUserGeneric)
	ON_MESSAGE(MM_WIM_DATA, OnMM_WIM_DATA)
	ON_MESSAGE(WM_HOTKEY, OnHotKey)
	ON_COMMAND(ID_HELP_CAMSTUDIOBLOG, OnHelpCamstudioblog)
	ON_BN_CLICKED(IDC_BUTTONLINK, OnBnClickedButtonlink)
	ON_WM_CAPTURECHANGED()
	ON_UPDATE_COMMAND_UI(ID_OPTIONS_AUDIOOPTIONS_AUDIOVIDEOSYNCHRONIZATION, &CRecorderView::OnUpdateOptionsAudiooptionsAudiovideosynchronization)
	END_MESSAGE_MAP()

BEGIN_EVENTSINK_MAP(CRecorderView, CView)

END_EVENTSINK_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRecorderView construction/destruction

CRecorderView::CRecorderView()
{
	// TODO: add construction code here
	m_basicMsg = NULL;
	_zoom = 1;
	_zoomDirection = -1; // zoomed out
	_zoomWhen = 0; // FIXME: I hope it is unlikely zoom start at 47 day boundary ever happen by accident
	_zoomedAt = CPoint(10,10);
	showmessage = true;
}

CRecorderView::~CRecorderView()
{
	if (m_basicMsg != NULL)
	{
		m_basicMsg->CloseWindow();
		delete m_basicMsg;
		m_basicMsg = NULL;
	}
}

BOOL CRecorderView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	// the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CRecorderView drawing

void CRecorderView::OnDraw(CDC* /*pDC*/)
{
	CRecorderDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	// TODO: add draw code for native data here
}

/////////////////////////////////////////////////////////////////////////////
// CRecorderView printing

BOOL CRecorderView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CRecorderView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CRecorderView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// CRecorderView diagnostics

#ifdef _DEBUG
void CRecorderView::AssertValid() const
{
	CView::AssertValid();
}

void CRecorderView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CRecorderDoc* CRecorderView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CRecorderDoc)));
	return (CRecorderDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CRecorderView message handlers

void CRecorderView::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	// Draw Autopan Info
	//Draw Message msgRecMode
	if (!bRecordState) {
		DisplayRecordingMsg(dc);
		return;
	}
	//Display the record information when recording
	if (bRecordState) {
		CRect rectClient;
		GetClientRect(&rectClient);

		//OffScreen Buffer
		CDC dcBits;
		dcBits.CreateCompatibleDC(&dc);
		CBitmap bitmap;

		bitmap.CreateCompatibleBitmap(&dc, rectClient.Width(), rectClient.Height());
		CBitmap *pOldBitmap = dcBits.SelectObject(&bitmap);

		//Drawing to OffScreen Buffer
		//TRACE("\nRect coords: %d %d %d %d ", rectClient.left, rectClient.top, rectClient.right, rectClient.bottom);

		//CBrush brushBlack;
		//brushBlack.CreateStockObject(BLACK_BRUSH);
		//dcBits.FillRect(&rectClient, &brushBlack);

		DisplayBackground(dcBits);
		DisplayRecordingStatistics(dcBits);
		// OffScreen Buffer
		dc.BitBlt(0, 0, rectClient.Width(), rectClient.Height(), &dcBits, 0, 0, SRCCOPY);
		dcBits.SelectObject(pOldBitmap);
	}

	//ver 2.26
	// Do not call CView::OnPaint() for painting messages
}

int CRecorderView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	hWndGlobal = m_hWnd;
	setHotKeyWindow(m_hWnd);

//#pragma message("CRecorderView::LoadSettings skipped")
	LoadSettings();
	VERIFY(0 < SetAdjustHotKeys());

	if (!CreateShiftWindow()) {
		return -1;
	}

	HDC hScreenDC = ::GetDC(NULL);
	iBits = ::GetDeviceCaps(hScreenDC, BITSPIXEL);
	nColors = iBits;
	::ReleaseDC(NULL,hScreenDC);

	hLogoBM = LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BITMAP3));

	CRect rect(0, 0, maxxScreen - 1, maxyScreen - 1);
	if (!m_FlashingWnd.CreateFlashing("Flashing", rect)) {
		return -1;
	}

	//Ver 1.2
	//strCursorDir default
	char dirx[_MAX_PATH];
	GetWindowsDirectory(dirx, _MAX_PATH);
	CString windir;
	windir.Format("%s\\cursors", dirx);
	CamCursor.Dir(windir);

	//savedir default
   //We by default save to to user's configured my videos directory.
   //Using GetProgPath instead is not compatible with Vista/Win7 since we have no write permission.
	savedir = GetMyVideoPath();
	//TRACE("## CRecorderView::OnCreate: initialSaveMMMode Default savedir=GetProgPath()=[%s]!\n", savedir);

	if (!initialSaveMMMode()) {
		TRACE("CRecorderView::OnCreate: initialSaveMMMode FAILED!\n");
		//return -1;
	}

	srand((unsigned)time(NULL));

	return 0;
}

void CRecorderView::OnDestroy()
{
	DecideSaveSettings();

	//UnSetHotKeys(hWndGlobal);
	//getHotKeyMap().clear(); // who actually cares?

	DestroyShiftWindow();

	if (pCompressorInfo != NULL) {
		delete [] pCompressorInfo;
		num_compressor = 0;
	}

	if (hSavedBitmap) {
		DeleteObject(hSavedBitmap);
		hSavedBitmap = NULL;
	}

	if (hLogoBM) {
		DeleteObject(hLogoBM);
		hLogoBM = NULL;
	}

	//if (pwfx) {
	//	GlobalFreePtr(pwfx);
	//	pwfx = NULL;
	//}

	//ver 1.6
	if (TroubleShootVal) {
		//Safety code
		if ((waveInGetNumDevs() == 0) || (waveOutGetNumDevs() == 0) || (mixerGetNumDevs() == 0)) {
			//Do nothing
		} else {
			useWavein(TRUE,FALSE);
		}
	} else {
		finalRestoreMMMode();
	}

	FreeWaveoutResouces();
	WaveoutUninitialize();

	//ver 1.8
	ListManager.FreeDisplayArray();
	ListManager.FreeShapeArray();
	ListManager.FreeLayoutArray();

	CView::OnDestroy();
}

LRESULT CRecorderView::OnRecordStart(UINT /*wParam*/, LONG /*lParam*/)
{
	TRACE("CRecorderView::OnRecordStart\n");
	CStatusBar* pStatus = (CStatusBar*) AfxGetApp()->m_pMainWnd->GetDescendantWindow(AFX_IDW_STATUS_BAR);
	pStatus->SetPaneText(0,"Press the Stop Button to stop recording");

	m_cCamera.Set(cCaptionOpts);
	m_cCamera.Set(cTimestampOpts);
	m_cCamera.Set(cXNoteOpts);
	m_cCamera.Set(cWatermarkOpts);
	m_cCamera.Set(CamCursor);

	//TRACE("## CRecorderView::OnRecordStart  m_cCamera.m_sXNote.m_ulXnoteRecordDurationLimitInMilliSec[%d]\n", m_cCamera.m_sXNote.m_ulXnoteRecordDurationLimitInMilliSec  );

	//ver 1.2
	if (cProgramOpts.m_bMinimizeOnStart)
		::PostMessage(AfxGetMainWnd()->m_hWnd,WM_SYSCOMMAND,SC_MINIMIZE,0);

	//Check validity of rc and fix it
	//FixRectSizePos(&rc, maxxScreen, maxyScreen, minxScreen, minyScreen);

	// TODO: mouse events handler should be installed only if we are interested in highlighting
	// Shall we empty the click queue? if we stop & start again we have a chance to see previous stuff
	// may be no relevant since will likely expire in normal circumstances.
	// Though if we dump events in file, it is better to clean up
	// FIXME: second parameter is never used
	// We shall wrap all that stuff in class so it is created in constructor and guaranteed to be destroed in destructor
	InstallMyHook(hWndGlobal,WM_USER_SAVECURSOR);

	bRecordState = true;
	interruptkey = 0;
	
	strCodec = GetCodecDescription(cVideoOpts.m_dwCompfccHandler);

	CWinThread * pThread = AfxBeginThread(RecordThread, this);

	//Ver 1.3
	if (pThread)
		pThread->SetThreadPriority(cProgramOpts.m_iThreadPriority);

	//Ver 1.2
	bAllowNewRecordStartKey = TRUE; //allow this only after bRecordState is set to 1
	return 0;
}


LRESULT CRecorderView::OnRecordPaused (UINT /*wParam*/, LONG /*lParam*/)
{
	// TRACE("## CRecorderView::OnRecordPaused\n");
	if (bRecordPaused) {
		return 0;
	}
	//TRACE("## CRecorderView::OnRecordPaused Tick:[%lu] Call OnPause() now\n", GetTickCount() );
	OnPause();

	return 0;
}


LRESULT CRecorderView::OnRecordInterrupted (UINT wParam, LONG /*lParam*/)
{
	UninstallMyHook(hWndGlobal);

	//Ver 1.1
	if (bRecordPaused) {
		CMainFrame * pFrame = dynamic_cast<CMainFrame *>(AfxGetMainWnd());
		pFrame->SetTitle(_T("CamStudio"));
	}

	bRecordState = false;
	
	// Reset others states (XNote) that should be reset if recirdings ends.
	//TRACE("## CRecorderView::OnRecordInterrupted , Call XNoteActionStopwatchResetParams()\n");
	XNoteActionStopwatchResetParams();

	//Store the interrupt key in case this function is triggered by a keypress
	interruptkey = wParam;

	CStatusBar* pStatus = (CStatusBar*) AfxGetApp()->m_pMainWnd->GetDescendantWindow(AFX_IDW_STATUS_BAR);
	pStatus->SetPaneText(0,"Press the Record Button to start recording");

	Invalidate();

	//ver 1.2
	::SetForegroundWindow(AfxGetMainWnd()->m_hWnd);
	AfxGetMainWnd()->ShowWindow(SW_RESTORE);

	return 0;
}

LRESULT CRecorderView::OnSaveCursor (UINT wParam, LONG /*lParam*/)
{
	//TRACE("CRecorderView::OnSaveCursor\n");
	CamCursor.Save(reinterpret_cast<HCURSOR>(wParam));
	m_cCamera.Save(reinterpret_cast<HCURSOR>(wParam));
	return 0;
}

void CRecorderView::OnRegionRubber()
{
	cRegionOpts.m_iMouseCaptureMode = CAPTURE_VARIABLE;
}

void CRecorderView::OnUpdateRegionRubber(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(cRegionOpts.isCaptureMode(CAPTURE_VARIABLE));
}

void CRecorderView::OnRegionPanregion()
{
	iDefineMode = 0;
	CFixedRegionDlg cfrdlg(this);
	if (IDOK == cfrdlg.DoModal()) {
		cRegionOpts.m_iMouseCaptureMode = CAPTURE_FIXED;
	}
	iDefineMode = 0;
}

void CRecorderView::OnUpdateRegionPanregion(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(cRegionOpts.isCaptureMode(CAPTURE_FIXED));
}

void CRecorderView::OnRegionSelectScreen()
{
	cRegionOpts.m_iMouseCaptureMode =  CAPTURE_FULLSCREEN;
}

void CRecorderView::OnUpdateRegionSelectScreen(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(cRegionOpts.isCaptureMode(CAPTURE_FULLSCREEN));
}

void CRecorderView::OnRegionFullscreen()
{
	cRegionOpts.m_iMouseCaptureMode = CAPTURE_ALLSCREENS;
}

void CRecorderView::OnUpdateRegionFullscreen(CCmdUI* pCmdUI)
{
	if (::GetSystemMetrics(SM_CMONITORS) == 1)
	{
		//Capture all screens has the same effect as full screen for
		//a system with only one monitor.
		pCmdUI->SetCheck(cRegionOpts.isCaptureMode(CAPTURE_ALLSCREENS));
	}
	else
	{
		pCmdUI->m_pMenu->RemoveMenu(ID_REGION_FULLSCREEN, MF_BYCOMMAND);
	}
}

void CRecorderView::OnRegionAllScreens()
{
	cRegionOpts.m_iMouseCaptureMode = CAPTURE_ALLSCREENS;
}

void CRecorderView::OnUpdateRegionAllScreens(CCmdUI* pCmdUI)
{
	if (::GetSystemMetrics(SM_CMONITORS) == 1)
	{
		pCmdUI->SetCheck(cRegionOpts.isCaptureMode(CAPTURE_ALLSCREENS));
		pCmdUI->m_pMenu->RemoveMenu(ID_SCREENS_SELECTSCREEN, MF_BYCOMMAND);
		pCmdUI->m_pMenu->RemoveMenu(ID_SCREENS_ALLSCREENS, MF_BYCOMMAND);
	}
	else
	{
		pCmdUI->SetCheck(cRegionOpts.isCaptureMode(CAPTURE_ALLSCREENS));
	}
}

//This function is called when the avi saving is completed
LRESULT CRecorderView::OnUserGeneric (UINT /*wParam*/, LONG /*lParam*/)
{
	CString strTmp = "";					
	CString strTargetDir = "";					// Path to target location
	CString strTargetBareFileName = "";			// Target filename without path and without extension
	CString strTargetVideoFile = "";			// Video filename initial without path but with extension
	CString strTargetAudioFile = "";			// Audio filename initial without path but with extension
	CString strTargetXnoteLogFile = "";			// Xnote log filename initial without path but with extension
	CString strTargetVideoExtension = ".avi";
	CString strTargetMP4VideoFile = "";

	//ver 1.2
	::SetForegroundWindow( AfxGetMainWnd()->m_hWnd);
	AfxGetMainWnd()->ShowWindow(SW_RESTORE);

	//ver 1.2
	// FIXME: this is not quite right. Shall be bCancelled or something
	if (interruptkey == cHotKeyOpts.m_RecordCancel.m_vKey) 
	{
		//if (interruptkey==VK_ESCAPE) {
		//Perform processing for cancel operation
		DeleteFile(strTempVideoAviFilePath);
		DeleteFile(strTempXnoteLogFilePath);
		if (!cAudioFormat.isInput(NONE))
			DeleteFile(strTempAudioWavFilePath);
		return 0;

	}

	////////////////////////////////////
	// Prepare Dialog (but do not use it here)
	////////////////////////////////////
	// To ask user for the filename, Normal thread exit

	CString strFilter;
	CString strTitle;
	CString strExtFilter;

	switch(cProgramOpts.m_iRecordingMode)
	{
	case ModeAVI:
		strFilter = _T("AVI Movie Files (*.avi)|*.avi||");
		strTitle = _T("Save AVI File");
		strExtFilter = _T("*.avi");
		break;
	case ModeFlash:
		strFilter = _T("FLASH Movie Files (*.swf)|*.swf||");
		strTitle = _T("Save SWF File");
		strExtFilter = _T("*.swf");
		break;
	case ModeMP4:
		strFilter = _T("MPEG Movie Files (*.mp4)|*.mp4||");
		strTitle = _T("Save MPEG File");
		strExtFilter = _T("*.mp4");
		break;
	}

	if(DoesDefaultOutDirExist(cProgramOpts.m_strDefaultOutDir))
	{
			strTargetDir = cProgramOpts.m_strDefaultOutDir;
	}
	else
	{
		strTargetDir = GetTempFolder(cProgramOpts.m_iOutputPathAccess, cProgramOpts.m_strSpecifiedDir, true);

	}
	CFileDialog fdlg(FALSE, strExtFilter, strExtFilter, OFN_LONGNAMES | OFN_OVERWRITEPROMPT, strFilter, this);
	fdlg.m_ofn.lpstrTitle = strTitle;

	// Savedir is a global var and therefor mostly set before.
	if (savedir == "") {
		savedir = GetMyVideoPath();
	}
	fdlg.m_ofn.lpstrInitialDir = strTargetDir;

	// I believed that savedir would always be GetProgPath(). 
	// But this test below proved that that is not alway the case
	//TRACE("## CRecorderView::OnUserGeneric() cProgramOpts.m_strSpecifiedDir=[%s]\n",cProgramOpts.m_strSpecifiedDir );
	//TRACE("## CRecorderView::OnUserGeneric , savedir    = [%s]\n", savedir );
	//TRACE("## CRecorderView::OnUserGeneric , GetProgPath= [%s]\n", GetProgPath() );
	//TRACE("## CRecorderView::OnUserGeneric , savedir = GetProgPath [%s]\n", (GetProgPath()==savedir) ? "EQUAL" : "DIFFERENT"  );

	if (/*(cProgramOpts.m_iRecordingMode == ModeAVI || cProgramOpts.m_iRecordingMode == ModeMP4) &&*/ cProgramOpts.m_bAutoNaming)
	{

		// Use local copy of the timestamp string created when recording was started for autonaming.
		// "ccyymmdd-hhmm-ss" , Timestamp still used for default temp.avi output "temp-ccyymmdd-hhmm-ss.avi"
		strTargetBareFileName.SetString( cVideoOpts.m_cStartRecordingString.GetString());
		strTargetVideoExtension = ".avi";
	} 
	else if (fdlg.DoModal() == IDOK) 
	{
		strTmp = fdlg.GetPathName();
		strTargetDir = strTmp.Left(strTmp.ReverseFind('\\'));
		// remove path info, we now have the udf defined filename
		strTmp = strTmp.Mid(strTmp.ReverseFind('\\')+1);

		// Split filename in base and extension
		int iPos = strTmp.ReverseFind('.');
		if ( iPos > 0 ) 
		{
			strTargetBareFileName = strTmp.Left(iPos);
			strTargetVideoExtension = strTmp.Mid(iPos);	// Extension with dot included
		} 
		else 
		{
			strTargetBareFileName = strTmp;
			// append always .avi if no extension is given.
			strTargetVideoExtension = ".avi";
		}
	} 
	else 
	{
		DeleteFile(strTempVideoAviFilePath);
		DeleteFile(strTempXnoteLogFilePath);
		if (!cAudioFormat.isInput(NONE)) 
		{
			DeleteFile(strTempAudioWavFilePath);
		}
		return 0;
	}
	// append always .avi as filetype when record to flash or mp4 is applicable because SWF and MP4 convertor expects as input an AVI file
	switch (cProgramOpts.m_iRecordingMode)
	{
	case ModeFlash:
		strTargetVideoExtension = ".avi";
		break;
	case ModeMP4:
		strTargetVideoExtension = ".avi";
		strTargetMP4VideoFile.Format("%s\\%s.mp4", strTargetDir, strTargetBareFileName);
		break;
	}
	//if (cProgramOpts.m_iRecordingMode == ModeFlash || cProgramOpts.m_iRecordingMode == ModeMP4) 
	//{
	//	strTargetVideoExtension = ".avi";
	//}

	// Create all the applicable targetfile names
	strTargetVideoFile    = strTargetDir + "\\" + strTargetBareFileName + strTargetVideoExtension;		// strTargetVideoFile is the same string as strNewFile in previuosly approach
	strTargetAudioFile    = strTargetDir + "\\" + strTargetBareFileName + ".wav";
	strTargetXnoteLogFile = strTargetDir + "\\" + strTargetBareFileName + ".txt";

	//TRACE("## CRecorderView::OnUserGeneric 2a # strTargetDir            :[%s]\n", strTargetDir);
	//TRACE("## 2b # strTargetBareFileName   :[%s]\n", strTargetBareFileName );
	//TRACE("## 2c # strTargetVideoExtension :[%s]\n", strTargetVideoExtension );
	//TRACE("## 3a # strTargetVideoFile      :[%s]\n", strTargetVideoFile );
	//TRACE("## 3b # strTargetAudioFile      :[%s]\n", strTargetAudioFile );
	//TRACE("## 3c # strTargetXnoteLogFile   :[%s]\n", strTargetXnoteLogFile );

	/////////////////////////////////////////////////////////
	// To be compatible with old previously solution.
	// Savedir is a global var and was always be set. (Mostly as GetProgPath()) 
	// Todo: Better solution required. Doubt that we need to set or require a global savedir always 
	/////////////////////////////////////////////////////////
	savedir = strTargetDir;

	//Ver 1.1
	if (cAudioFormat.m_iRecordAudio) 
	{
		// Check if video(!) file exists and if so, does it allow overwite
		HANDLE hfile = CreateFile( strTargetVideoFile, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hfile == INVALID_HANDLE_VALUE)
		{
			//::MessageBox(NULL,"Unable to create new file. The file may be opened by another application. Please use another filename.","Note",MB_OK | MB_ICONEXCLAMATION);
			MessageOut(m_hWnd,IDS_STRING_NOCREATEWFILE,IDS_STRING_NOTE,MB_OK | MB_ICONEXCLAMATION);
			::PostMessage(hWndGlobal,WM_USER_GENERIC,0,0);
			return 0;
		}
		CloseHandle(hfile);
		DeleteFile(strTargetVideoFile);

		//ver 1.8
		if (vanWndCreated)
		{
			if (m_vanWnd.IsWindowVisible()) 
			{
				//Otherwise, will slow down the merging significantly
				m_vanWnd.ShowWindow(SW_HIDE);
			}
		}

		
		// If video only (Do not record audio) there is no reason to merge video with audio and video.avi file could be used without further processing.
		if ( cAudioFormat.m_iRecordAudio == NONE )
		{
			TRACE("## CRecorderView::OnUserGeneric  NO Audio defined, no merge required. .\n"); 
		}

		int result = -1;

		// do we ever want to merge AV?
		bool wantMerge = true;
		cfg->lookupValue("Program.MergeAV", wantMerge);
		if (wantMerge) 
		{

			//Mergefile video with audio
			//ver 2.26 ...overwrite audio settings for Flash Mode recording ... no compression used...
			if ((cAudioFormat.isInput(SPEAKERS)) || (cAudioFormat.m_bUseMCI) || (cProgramOpts.m_iRecordingMode == ModeFlash)) 
			{
				result = MergeVideoAudio(strTempVideoAviFilePath, strTempAudioWavFilePath, strTargetVideoFile, FALSE, cAudioFormat);
			} 
			else if (cAudioFormat.isInput(MICROPHONE))
			{
				result = MergeVideoAudio(strTempVideoAviFilePath, strTempAudioWavFilePath, strTargetVideoFile, cAudioFormat.m_bCompression, cAudioFormat);
			}

		}

		//Check Results : Attempt Recovery on error
		switch (result)
		{
		case 0:		// Successful
		case 1:		// video file broken; Unable to recover
		case 3:		// this case is rare; Unable to recover
			DeleteFile(strTempVideoAviFilePath);
			DeleteFile(strTempXnoteLogFilePath);
			DeleteFile(strTempAudioWavFilePath);
			break;
		case 2:
		case 4:		// recover video file
			// video file is ok, but not audio file
			// so Move the video file as avi and ignore the audio  (Move = More or Copy+Delete)
			
			if (!MoveFile( strTempVideoAviFilePath, strTargetVideoFile))
			{
				//Although there is error copying, the temp file still remains in the temp directory and is not deleted, in case user wants a manual recover
				//MessageBox("File Creation Error. Unable to rename/copy file.","Note",MB_OK | MB_ICONEXCLAMATION);
				MessageOut(m_hWnd,IDS_STRING_MOVEFILEFAILURE,IDS_STRING_NOTE,MB_OK | MB_ICONEXCLAMATION);
				return 0;
			}

			if (!MoveFile( strTempXnoteLogFilePath, strTargetXnoteLogFile) ) 
			{
				TRACE("## CRecorderView::OnUserGeneric MoveFile strTempXnoteLogFilePath failed\n");
				// No reason to quit because we got the recording and we can continue and beacuse XnoteLog file use the same name structure as Video file the risk that logfile will fail is very small.
			}

			DeleteFile(strTempAudioWavFilePath);

			//::MessageBox(NULL,"Your AVI movie will not contain a soundtrack. CamStudio is unable to merge the video with audio.","Note",MB_OK | MB_ICONEXCLAMATION);
			MessageOut(m_hWnd, IDS_STRING_NOSOUNDTRACK, IDS_STRING_NOTE, MB_OK | MB_ICONEXCLAMATION);
			break;
		case 5:		// recover both files, but as separate files
			{
				if (!MoveFile( strTempVideoAviFilePath, strTargetAudioFile)) 
				{
					//MessageBox("File Creation Error. Unable to rename/copy video file.","Note",MB_OK | MB_ICONEXCLAMATION);
					MessageOut(m_hWnd,IDS_STRING_MOVEFILEFAILURE,IDS_STRING_NOTE,MB_OK | MB_ICONEXCLAMATION);
					return 0;
				}
	
				if (!MoveFile( strTempXnoteLogFilePath, strTargetXnoteLogFile) ) 
				{
					TRACE("## CRecorderView::OnUserGeneric MoveFile strTempXnoteLogFilePath failed\n");
					// MessageOut(m_hWnd,IDS_STRING_MOVEFILEFAILURE,IDS_STRING_NOTE,MB_OK | MB_ICONEXCLAMATION);
					// No reason to quit because we got the recording and we can continue and beacuse XnoteLog file use the same name structure as Video file the risk that logfile will fail is very small.
				}

				if (!MoveFile(strTempAudioWavFilePath,strTargetAudioFile)) 
				{
					//MessageBox("File Creation Error. Unable to rename/copy audio file.","Note",MB_OK | MB_ICONEXCLAMATION);
					MessageOut(m_hWnd,IDS_STRING_FILECREATION3,IDS_STRING_NOTE,MB_OK | MB_ICONEXCLAMATION);
					return 0;
				}


				CString tstr;
				tstr.LoadString(IDS_STRING_NOTE);
				CString msgstr;
				msgstr.LoadString(IDS_STRING_NOMERGE);
				//CString msgstr("CamStudio is unable to merge the video with audio files. Your video, audio and log files are saved separately as \n\n");

				msgstr = msgstr + strTargetVideoFile + "\n";
				msgstr = msgstr + strTargetAudioFile + "\n";
				msgstr = msgstr + strTargetXnoteLogFile;

				::MessageBox(NULL, msgstr, tstr, MB_OK | MB_ICONEXCLAMATION);
			}
			break;
		}
	} 
	else 
	{
		// TRACE("## CRecorderView::OnUserGeneric  NO Audio, just do a plain copy of temp avi to final avi\n");
		//////////////////////////////////////////////////
		// No audio, just do a plain copy of temp avi to final avi will do the job
		// MoveFile behaviour. First it will try to rename file. If not possible it will do a copy and a delete.
		//////////////////////////////////////////////////
		if ( !MoveFile(strTempVideoAviFilePath, strTargetVideoFile) ) 
		{
			// Unable to rename/copy file.
			// In case of an move problem we do nothing. Source has an unique name and not to delete the source file don't cause problems anylonger
			// The file may be opened by another application.
			// Please use another filename."
			MessageOut(m_hWnd, IDS_STRING_MOVEFILEFAILURE, IDS_STRING_NOTE, MB_OK | MB_ICONEXCLAMATION);
			//Repeat this function until success
			::PostMessage(hWndGlobal, WM_USER_GENERIC, 0, 0);
			return 0;
		}
		
		if (!MoveFile( strTempXnoteLogFilePath, strTargetXnoteLogFile) ) 
		{
			//MessageOut(m_hWnd,IDS_STRING_MOVEFILEFAILURE,IDS_STRING_NOTE,MB_OK | MB_ICONEXCLAMATION);
			// No issue to quit because we got the recording and we can continue
		}

		if (!cAudioFormat.isInput(NONE)) 
		{
			DeleteFile(strTempAudioWavFilePath);
		}
	}

	// TEST
	if (0)
	{
		CString strAVIOut;
		strAVIOut.Format(_T("%s\\FadeOut.avi"), GetProgPath());
		CamAVIFile aviFile(cVideoOpts, cAudioFormat);
		aviFile.FadeOut(strTargetVideoFile, strAVIOut);
	}

	switch(cProgramOpts.m_iRecordingMode)
	{
	case ModeAVI:
		RunViewer(strTargetVideoFile);
		break;
	case ModeFlash:
		RunProducer(strTargetVideoFile);
		break;
	case ModeMP4:
		{
			CString sMsg;
			CString sRes;
			CString sOutBareName;
			sOutBareName.Format("%s.mp4", strTargetBareFileName);
			if(ConvertToMP4(strTargetVideoFile, strTargetMP4VideoFile, sOutBareName))
			{
				if(!DeleteFile(strTargetVideoFile))
				{
					sRes.LoadString(IDS_STRING_DELETE_FAILED);
					sMsg.Format(sRes, strTargetMP4VideoFile);
					MessageBox(sMsg, "Warning", 0);
				}
			}
			else
			{
				if(!DeleteFile(strTargetVideoFile))
				{
					sRes.LoadString(IDS_STRING_DELETE_FAILED);
					sMsg.Format(sRes, strTargetVideoFile);
					MessageBox(sMsg, "Warning", 0);
				}
				if(!DeleteFile(strTargetMP4VideoFile))
				{
					sRes.LoadString(IDS_STRING_DELETE_FAILED);
					sMsg.Format(sRes, strTargetMP4VideoFile);
					MessageBox(sMsg, "Warning", 0);
				}
			}
		}

		break;
	}
	return 0;
}

void CRecorderView::OnRecord()
{
	CStatusBar* pStatus = (CStatusBar*) AfxGetApp()->m_pMainWnd->GetDescendantWindow(AFX_IDW_STATUS_BAR);
	pStatus->SetPaneText(0,"Press the Stop Button to stop recording");
	
	//Version 1.1
	if (bRecordPaused) {
		bRecordPaused = false;
		//ver 1.8
		//if (iRecordAudio==2)
		// mciRecordResume(strTempAudioWavFilePath);

		//Set Title Bar
		CMainFrame * pFrame = dynamic_cast<CMainFrame *>(AfxGetMainWnd());
		pFrame->SetTitle(_T("CamStudio"));

		return;
	}
	bRecordPaused = false;

	nActualFrame = 0;
	nCurrFrame = 0;
	fRate = 0.0;
	fActualRate = 0.0;
	fTimeLength = 0.0;

	// r272: How we define rect cshecked and changed because some pixel gots dropped:  100x100 => 99x99
	// rcUse is the rect that will be used in the end to define the size of the recording. 
	// BTW. rcUse is also used to store previous values.
	// A Full screen area is 0, 0 to MaxX-1, MaxY-1

	// TRACE( _T("## CRecorderView::OnRecord /CAPTURE_FIXED/ before / cRegionOpts / T=%d, L=%d, B=.. , R=.. , W=%d, H=%d \n"), cRegionOpts.m_iCaptureTop, cRegionOpts.m_iCaptureLeft, cRegionOpts.m_iCaptureWidth, cRegionOpts.m_iCaptureHeight );


	switch (cRegionOpts.m_iMouseCaptureMode)
	{
	case CAPTURE_FIXED:
		// Applicable when Option region is set as 'Fixed Region'
		rc = CRect(cRegionOpts.m_iCaptureLeft ,										 // X = Left
			       cRegionOpts.m_iCaptureTop ,										 // Y = Top
				   cRegionOpts.m_iCaptureLeft + cRegionOpts.m_iCaptureWidth,   // X = Width
				   cRegionOpts.m_iCaptureTop  + cRegionOpts.m_iCaptureHeight );  // Y = Height
		// TRACE( _T("## CRecorderView::OnRecord /CAPTURE_FIXED/ before / rc / T=%d, L=%d, B=%d, R=%d \n"), rc.top, rc.left, rc.bottom, rc.right );

		if (cRegionOpts.m_bFixedCapture) {
			// Applicable when Option region is set as 'Fixed Region' and retangle offset is fixed either.
			
			// I don't expect that code below is ever invoked...! Hence, dead code
			if (rc.top < 0) {
				TRACE( _T("## CRecorderView::OnRecord rc.top<0  [%d]\n"), rc.top);
				//rc.top = 0;
			}
			if (rc.left < 0) {
				TRACE( _T("## CRecorderView::OnRecord rc.left<0  [%d]\n"), rc.left);
				//rc.left = 0;
			}
			if (maxxScreen <= rc.right) {
				TRACE( _T("## CRecorderView::OnRecord maxxScreen<rc.right  [%d]\n"), rc.right);
				//rc.right = maxxScreen - 1;  //ok
			}
			if (maxyScreen <= rc.bottom) {
				TRACE( _T("## CRecorderView::OnRecord maxyScreen<rc.bottom<0  [%d]\n"), rc.bottom);
				//rc.bottom = maxyScreen - 1; //ok
			}

			//using protocols for iMouseCaptureMode==0
			old_rcClip = rcClip = rc;
			old_rcClip.NormalizeRect();
			rcUse = old_rcClip;
			::PostMessage (hWndGlobal, WM_USER_RECORDSTART, 0, (LPARAM) 0);
				
		} else { 
			// Applicable when Option region is set as 'Fixed Region' but retangle offset is floating.
			// Floating Retangle. Drag the retangle first to its position...

			::ShowWindow(hMouseCaptureWnd,SW_SHOW);
			::UpdateWindow(hMouseCaptureWnd);
			InitDrawShiftWindow(); //will affect rc implicity
		}
		break;
	case CAPTURE_VARIABLE: 
		// Applicable when Option region is set as 'Region'
		::ShowWindow(hMouseCaptureWnd, SW_SHOW);
		::UpdateWindow(hMouseCaptureWnd);
		InitSelectRegionWindow(); //will affect rc implicity
		break;
	case CAPTURE_ALLSCREENS: 
		// Applicable when Option region is set as 'Full Screen'
		rcUse = CRect(minxScreen, minyScreen, maxxScreen , maxyScreen);
		::PostMessage(hWndGlobal, WM_USER_RECORDSTART, 0, (LPARAM) 0);
		break;

	case CAPTURE_WINDOW:
	case CAPTURE_FULLSCREEN:
		// Applicable when Option region is set as 'Window' and as 'Full Screen'
		
		// TODO, Possible memory leak, where is the delete operation of the new below done?
		if(m_basicMsg)
		{
			delete m_basicMsg;
			m_basicMsg = NULL;
		}

		m_basicMsg = new CBasicMessage();
		m_basicMsg->Create(CBasicMessage::IDD);
		if ( cRegionOpts.m_iMouseCaptureMode == CAPTURE_WINDOW ) {
			m_basicMsg->SetText(_T("Click on window to be captured."));
		} else {
			m_basicMsg->SetText(_T("Click on screen to be captured."));
		}
		m_basicMsg->ShowWindow(SW_SHOW);
		SetCapture(); // what is this for?
		//TRACE(_T("## CRecorderView::OnRecord CAPTURE_WINDOW / after / rc / T=%d, L=%d, B=%d, R=%d \n"), rc.top, rc.left, rc.bottom, rc.right );
		//TRACE(_T("## CRecorderView::OnRecord CAPTURE_WINDOW / after / MinX=%d, MinY=%d, MaxX=%d, MaxY=%d \n"), minxScreen, minyScreen, maxxScreen, maxyScreen );

		//TRACE(_T("## CRecorderView::OnRecord CAPTURE_WINDOW / after / rc / T=%d, L=%d, B=%d, R=%d \n"), rc.top, rc.left, rc.bottom, rc.right );
		//TRACE(_T("## CRecorderView::OnRecord CAPTURE_WINDOW / after / MinX=%d, MinY=%d, MaxX=%d, MaxY=%d \n"), minxScreen, minyScreen, maxxScreen, maxyScreen );

		break;
	}
	// TRACE(_T("## CRecorderView::OnRecord / after / rc / T=%d, L=%d, B=%d, R=%d \n"), rc.top, rc.left, rc.bottom, rc.right );
	// TRACE(_T("## CRecorderView::OnRecord / after / MinX=%d, MinY=%d, MaxX=%d, MaxY=%d \n"), minxScreen, minyScreen, maxxScreen, maxyScreen );
}

void CRecorderView::OnStop()
{
	//Version 1.1
	if (!bRecordState) {
		return;
	}

	if (bRecordPaused) {
		bRecordPaused = false;

		//Set Title Bar
		CMainFrame * pFrame = dynamic_cast<CMainFrame *>(AfxGetMainWnd());
		pFrame->SetTitle(_T("CamStudio"));
	}

	// Reset Xnote Stopwatch params to defaults. Required if recording is terminated in Camstudio instead of Xnote.
	XNoteActionStopwatchResetParams();

	// Broadcast. // mlt_msk: WTF? it is not a broadcasting
	OnRecordInterrupted (0, 0);

}

//ver 1.6
#define MAXCOMPRESSORS 50
void CRecorderView::OnFileVideooptions()
{
	// Capture a frame and use it to determine compatitble compressors for user to select
	CRect rect(0, 0, 320, 200);
	LPBITMAPINFOHEADER first_alpbi = captureScreenFrame(rect) ? m_cCamera.Image() : 0;

	num_compressor = 0;
	if (pCompressorInfo) {
		delete [] pCompressorInfo;
	}
	
	// TODO, Possible memory leak, where is the delete operation of the new below done?
	pCompressorInfo = new ICINFO[MAXCOMPRESSORS];

	//for (int i = 0; ICInfo(ICTYPE_VIDEO, i, &pCompressorInfo[num_compressor]); i++) {
	for (int i = 0; ICInfo(0, i, &pCompressorInfo[num_compressor]); i++) {
		if (MAXCOMPRESSORS <= num_compressor) {
			break; //maximum allows 30 compressors
		}

		if (cVideoOpts.m_bRestrictVideoCodecs) {
			//allow only a few
			if ((pCompressorInfo[num_compressor].fccHandler == ICHANDLER_MSVC)
				|| (pCompressorInfo[num_compressor].fccHandler == ICHANDLER_MRLE)
				|| (pCompressorInfo[num_compressor].fccHandler == ICHANDLER_CVID)
				|| (pCompressorInfo[num_compressor].fccHandler == ICHANDLER_DIVX))
			{
				HIC hic = ICOpen(pCompressorInfo[num_compressor].fccType, pCompressorInfo[num_compressor].fccHandler, ICMODE_QUERY);
				if (hic) {
					if (ICERR_OK == ICCompressQuery(hic, first_alpbi, NULL)) {
						VERIFY(0 < ICGetInfo(hic, &pCompressorInfo[num_compressor], sizeof(ICINFO)));
						num_compressor++;
					}
					ICClose(hic);
				}
			}
		} else {
			//CamStudio still cannot handle VIFP
			if (pCompressorInfo[num_compressor].fccHandler != ICHANDLER_VIFP) {
				HIC hic = ICOpen(pCompressorInfo[num_compressor].fccType, pCompressorInfo[num_compressor].fccHandler, ICMODE_QUERY);
				if (hic) {
					if (ICERR_OK == ICCompressQuery(hic, first_alpbi, NULL)) {
						VERIFY(0 < ICGetInfo(hic, &pCompressorInfo[num_compressor], sizeof(ICINFO)));
						num_compressor++;
					}
					ICClose(hic);
				}
			}
		}
	}

	if (num_compressor) {
		CVideoOptionsDlg cDlg(cVideoOpts, this);
		if (IDOK == cDlg.DoModal()) {
			cVideoOpts = cDlg.Opts();
		}
	}
}

void CRecorderView::OnOptionsCursoroptions()
{
	CCursorOptionsDlg cod(CamCursor, this);
	if (IDOK == cod.DoModal()) {
		CamCursor = cod.GetOptions();
	}
}

void CRecorderView::OnOptionsAutopan()
{
	cProgramOpts.m_bAutoPan = !cProgramOpts.m_bAutoPan;
}

void CRecorderView::OnOptionsAtuopanspeed()
{
	CAutopanSpeedDlg aps_dlg;
	aps_dlg.DoModal();
}

void CRecorderView::OnUpdateOptionsAutopan(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(cProgramOpts.m_bAutoPan);
}

void CRecorderView::OnOptionsMinimizeonstart()
{
	cProgramOpts.m_bMinimizeOnStart = !cProgramOpts.m_bMinimizeOnStart;
}

void CRecorderView::OnUpdateOptionsMinimizeonstart(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(cProgramOpts.m_bMinimizeOnStart);
}

void CRecorderView::OnOptionsHideflashing()
{
	cProgramOpts.m_bFlashingRect = !cProgramOpts.m_bFlashingRect;
}

void CRecorderView::OnUpdateOptionsHideflashing(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(!cProgramOpts.m_bFlashingRect);
}

void CRecorderView::OnOptionsProgramoptionsPlayavi()
{
	cProgramOpts.m_iLaunchPlayer = (cProgramOpts.m_iLaunchPlayer) ? NO_PLAYER : CAM1_PLAYER;
}

void CRecorderView::OnUpdateOptionsProgramoptionsPlayavi(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(cProgramOpts.m_iLaunchPlayer);
}

BOOL CRecorderView::Openlink (CString link)
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

BOOL CRecorderView::OpenUsingShellExecute (CString link)
{
	LPCTSTR mode = _T ("open");
	HINSTANCE hRun = ShellExecute (GetSafeHwnd (), mode, link, NULL, NULL, SW_SHOW);
	if ((int) hRun <= HINSTANCE_ERROR) {
		TRACE ("Failed to invoke URL using ShellExecute\n");
		return FALSE;
	}
	return TRUE;
}

BOOL CRecorderView::OpenUsingRegisteredClass (CString link)
{
	TCHAR key[MAX_PATH + MAX_PATH];
	HINSTANCE result;

	if (GetRegKey (HKEY_CLASSES_ROOT, _T (".htm"), key) == ERROR_SUCCESS) {
		LPCTSTR mode;
		mode = _T ("\\shell\\open\\command");
		_tcscat (key, mode);
		if (GetRegKey (HKEY_CLASSES_ROOT, key, key) == ERROR_SUCCESS) {
			LPTSTR pos;
			pos = _tcsstr (key, _T ("\"%1\""));
			if (pos == NULL) {
				// No quotes found
				pos = strstr (key, _T ("%1")); // Check for %1, without quotes
				if (pos == NULL) {
					// No parameter at all...
					pos = key + _tcslen (key) - 1;
				} else {
					*pos = _T ('\0'); // Remove the parameter
				}
			} else {
				*pos = _T ('\0'); // Remove the parameter
			}

			strcat(pos, _T (" "));
			strcat(pos, link);
			result = (HINSTANCE) WinExec (key, SW_SHOW);
			if ((int) result <= HINSTANCE_ERROR) {
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
			} else {
				return TRUE;
			}
		}
	}
	return FALSE;
}

LONG CRecorderView::GetRegKey (HKEY key, LPCTSTR subkey, LPTSTR retdata)
{
	HKEY hkey;
	LONG retval = RegOpenKeyEx (key, subkey, 0, KEY_QUERY_VALUE, &hkey);
	if (retval == ERROR_SUCCESS) {
		long datasize = MAX_PATH;
		TCHAR data[MAX_PATH];
		RegQueryValue (hkey, NULL, data, &datasize);
		strcpy(retdata, data);
		RegCloseKey (hkey);
	}

	return retval;
}

void CRecorderView::OnHelpWebsite()
{
	//Openlink("http://www.atomixbuttons.com/vsc");
	//Openlink("http://www.rendersoftware.com");
	Openlink("http://www.camstudio.org");
}


void CRecorderView::OnCamstudio4XnoteWebsite()
{
	/*
	Hi Jan
	I'll be happy to publish new beta releases regularly when my new team (in the future ;o) compiles the code into binaries.
	Feel free to increase the size of the About box to allow you to put your text and link into it.
	XNote Reference check: that would be great.
	Maybe only show the XNote settings info in the CamStudio Recorder display area if XNote/MotionControl are connected? The less people have to try and figure out, the better ;o)
	Cheers
	Nick :o)
	*/

	//Openlink("http://www.xnotestopwatch.com");
	Openlink("http://www.jahoma.nl/timereg");
}


void CRecorderView::OnHelpHelp()
{
	CString progdir = GetProgPath();
	CString helppath = progdir + "\\help.htm";

	Openlink(helppath);

	//HtmlHelp( hWndGlobal, progdir + "\\help.chm", HH_DISPLAY_INDEX, (DWORD)"CamStudio");
}

void CRecorderView::OnPause()
{
	//TRACE ("## CRecorderView::OnPause BEGIN RecordState:[%d] RecordPaused:[%d]\n", bRecordState, bRecordPaused);

	//return if not current recording or already in paused state
	if (!bRecordState || bRecordPaused)
		return;

	bRecordPaused = true;
	//TRACE ("## CRecorderView::OnPause  STATE SWITCHED RecordPaused:[%d]\n", bRecordPaused);

	// TODO: Set flag that will switch from recording to pause to off if nothing is happening for al very long time.


	//ver 1.8
	//if (iRecordAudio==2)
	// mciRecordPause(strTempAudioWavFilePath);

	CStatusBar* pStatus = (CStatusBar*) AfxGetApp()->m_pMainWnd->GetDescendantWindow(AFX_IDW_STATUS_BAR);
	pStatus->SetPaneText(0,"Recording Paused");

	//Set Title Bar
	CMainFrame * pFrame = dynamic_cast<CMainFrame *>(AfxGetMainWnd());
	pFrame->SetTitle(_T("Paused"));
}

void CRecorderView::OnUpdatePause(CCmdUI* pCmdUI)
{
	//Version 1.1
	//pCmdUI->Enable(bRecordState && (!bRecordPaused));
	pCmdUI->Enable(!bRecordPaused);
}

void CRecorderView::OnUpdateStop(CCmdUI* /*pCmdUI*/)
{
	//Version 1.1
	//pCmdUI->Enable(bRecordState);
}

void CRecorderView::OnUpdateRecord(CCmdUI* pCmdUI)
{
	//Version 1.1
	pCmdUI->Enable(!bRecordState || bRecordPaused);
}

void CRecorderView::OnHelpFaq()
{
	//Openlink("http://www.atomixbuttons.com/vsc/page5.html");
	Openlink("http://www.camstudio.org/faq.htm");
}

LRESULT CRecorderView::OnMM_WIM_DATA(WPARAM parm1, LPARAM parm2)
{
	HANDLE hInputDev = (HANDLE) parm1;
	ASSERT(hInputDev == m_hWaveRecord);
	LPWAVEHDR pHdr = (LPWAVEHDR) parm2;
	MMRESULT mmReturn = ::waveInUnprepareHeader(m_hWaveRecord, pHdr, sizeof(WAVEHDR));
	if (mmReturn) {
		waveInErrorMsg(mmReturn, "in OnWIM_DATA()");
		return 0;
	}

	//TRACE("WIM_DATA %4d\n", pHdr->dwBytesRecorded);

	if (bRecordState) {
		CBuffer buf(pHdr->lpData, pHdr->dwBufferLength);
		if (!bRecordPaused) {
			//write only if not paused
			//Write Data to file
			DataFromSoundIn(&buf);
		}

		// reuse the buffer:
		// prepare it again
		mmReturn = ::waveInPrepareHeader(m_hWaveRecord,pHdr, sizeof(WAVEHDR));
		if (mmReturn) {
			waveInErrorMsg(mmReturn, "in OnWIM_DATA()");
		} else {
			// no error
			// add the input buffer to the queue again
			mmReturn = ::waveInAddBuffer(m_hWaveRecord, pHdr, sizeof(WAVEHDR));
			if (mmReturn) {
				waveInErrorMsg(mmReturn, "in OnWIM_DATA()");
			} else {
				return 0; // no error
			}
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

void CRecorderView::OnOptionsRecordaudio()
{
	if (waveInGetNumDevs() == 0) {
		//CString msgstr;
		//msgstr.Format("Unable to detect audio input device. You need a sound card with microphone input.");
		//MessageBox(msgstr,"Note", MB_OK | MB_ICONEXCLAMATION);
		MessageOut(m_hWnd,IDS_STRING_NOINPUT3,IDS_STRING_NOTE,MB_OK | MB_ICONEXCLAMATION);
		return;
	}

	cAudioFormat.m_iRecordAudio = (!cAudioFormat.isInput(NONE)) ? NONE : MICROPHONE;
}

void CRecorderView::OnUpdateOptionsRecordaudio(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(!cAudioFormat.isInput(NONE));
}

void CRecorderView::OnOptionsAudioformat()
{
	if (waveInGetNumDevs() == 0) {
		//CString msgstr;
		//msgstr.Format("Unable to detect audio input device. You need a sound card with microphone input.");
		//MessageBox(msgstr,"Note", MB_OK | MB_ICONEXCLAMATION);

		MessageOut(m_hWnd,IDS_STRING_NOINPUT3,IDS_STRING_NOTE,MB_OK | MB_ICONEXCLAMATION);
		return;
	}

	CAudioFormatDlg aod(cAudioFormat, this);
	if (IDOK == aod.DoModal()) {
		// update settings
		cAudioFormat = aod.Format();
	}

	//if (iInterleaveUnit == MILLISECONDS) {
	//	double interfloat = (((double) iInterleaveFactor) * ((double) iFramesPerSecond))/1000.0;
	//	int interint = (int) interfloat;
	//	if (interint<=0)
	//		interint = 1;

	//	CString bstr;
	//	bstr.Format("interleave Unit = %d",interint);
	//	//MessageBox(bstr,"Note",MB_OK);
	//}
}

void CRecorderView::OnOptionsAudiospeakers()
{
	if (waveOutGetNumDevs() == 0)
	{
		MessageOut(m_hWnd, IDS_STRING_NOAUDIOOUTPUT, IDS_STRING_NOTE, MB_OK | MB_ICONEXCLAMATION);
	}

	CAudioSpeakersDlg aos;
	aos.DoModal();
}

void CRecorderView::OnOptionsKeyboardshortcuts()
{
	if (!keySCOpened) {
		keySCOpened = 1;
		CKeyshortcutsDlg kscDlg(this);
		kscDlg.DoModal();
		keySCOpened = 0;

		SetAdjustHotKeys();
	}
}

// use CWinApp class profile API's
bool CRecorderView::SaveAppSettings()
{
	return false;
}

// Ver 1.2
void CRecorderView::SaveSettings()
{
	//********************************************
	//Creating CamStudio.ini for storing text data
	//********************************************
	//
	// Once defined...the format cannot change in future versions
	// new properties can only be appended at the end
	// otherwise, the datafile will not be upward compatible with future versions

	CString fileName("\\CamStudio.ini");
	CString setDir = GetProgPath();					// TODO : Check use of setDir .. NOT savedir ???
	CString setPath;
	setPath.Format("%s\\CamStudio.ini", (LPCSTR)GetProgPath());

#ifndef LEGACY_PROFILE_DISABLE
	FILE * sFile = fopen((LPCTSTR)setPath, "wt");
	if (sFile == NULL) {
		//Error creating file ...do nothing...return
		return;
	}

	// ****************************
	// Dump Variables
	// ****************************
	//Take note of those vars with printf %ld
	float ver = (float) 2.5;
	fprintf(sFile, "[ CamStudio Settings ver%.2f -- Please do not edit ] \n\n", ver);
	fprintf(sFile, "bFlashingRect=%d \n",cProgramOpts.m_bFlashingRect);
	fprintf(sFile, "iLaunchPlayer=%d \n",cProgramOpts.m_iLaunchPlayer);
	fprintf(sFile, "bMinimizeOnStart=%d \n",cProgramOpts.m_bMinimizeOnStart);
	fprintf(sFile, "iMouseCaptureMode= %d \n",cRegionOpts.m_iMouseCaptureMode);
	fprintf(sFile, "iCaptureWidth=%d \n",cRegionOpts.m_iCaptureWidth);
	fprintf(sFile, "iCaptureHeight=%d \n",cRegionOpts.m_iCaptureHeight);

	fprintf(sFile, "iTimeLapse=%d \n",cVideoOpts.m_iTimeLapse);
	fprintf(sFile, "iFramesPerSecond= %d \n",cVideoOpts.m_iFramesPerSecond);
	fprintf(sFile, "iKeyFramesEvery= %d \n",cVideoOpts.m_iKeyFramesEvery);
	fprintf(sFile, "iCompQuality= %d \n",cVideoOpts.m_iCompQuality);
	fprintf(sFile, "dwCompfccHandler= %ld \n",cVideoOpts.m_dwCompfccHandler);

	//LPVOID pVideoCompressParams = NULL;
	fprintf(sFile, "dwCompressorStateIsFor= %ld \n",cVideoOpts.m_dwCompressorStateIsFor);
	fprintf(sFile, "dwCompressorStateSize= %d \n",cVideoOpts.StateSize());

	fprintf(sFile, "bRecordCursor=%d \n",CamCursor.Record());
	fprintf(sFile, "iCustomSel=%d \n",CamCursor.CustomType()); //Having this line means the custom cursor type cannot be re-arranged in a new order in the combo box...else previous saved settings referring to the custom type will not be correct
	fprintf(sFile, "iCursorType=%d \n", CamCursor.Select());
	fprintf(sFile, "bHighlightCursor=%d \n",CamCursor.Highlight());
	fprintf(sFile, "iHighlightSize=%d \n",CamCursor.HighlightSize());
	fprintf(sFile, "iHighlightShape=%d \n",CamCursor.HighlightShape());
	fprintf(sFile, "bHighlightClick=%d \n",CamCursor.HighlightClick());

	fprintf(sFile, "g_highlightcolorR=%d \n",GetRValue(CamCursor.HighlightColor()));
	fprintf(sFile, "g_highlightcolorG=%d \n",GetGValue(CamCursor.HighlightColor()));
	fprintf(sFile, "g_highlightcolorB=%d \n",GetBValue(CamCursor.HighlightColor()));

	fprintf(sFile, "g_highlightclickcolorleftR=%d \n",GetRValue(CamCursor.ClickLeftColor()));
	fprintf(sFile, "g_highlightclickcolorleftG=%d \n",GetGValue(CamCursor.ClickLeftColor()));
	fprintf(sFile, "g_highlightclickcolorleftB=%d \n",GetBValue(CamCursor.ClickLeftColor()));

	fprintf(sFile, "g_highlightclickcolorrightR=%d \n",GetRValue(CamCursor.ClickRightColor()));
	fprintf(sFile, "g_highlightclickcolorrightG=%d \n",GetGValue(CamCursor.ClickRightColor()));
	fprintf(sFile, "g_highlightclickcolorrightB=%d \n",GetBValue(CamCursor.ClickRightColor()));

	//fprintf(sFile, "savedir=%s; \n",LPCTSTR(savedir));
	//fprintf(sFile, "strCursorDir=%s; \n",LPCTSTR(strCursorDir));

	fprintf(sFile, "autopan=%d \n",cProgramOpts.m_bAutoPan);
	fprintf(sFile, "iMaxPan= %d \n",cProgramOpts.m_iMaxPan);

	//Audio Functions and Variables
	fprintf(sFile, "uAudioDeviceID= %d \n",cAudioFormat.m_uDeviceID);

	//Audio Options Dialog
	//LPWAVEFORMATEX pwfx = NULL;
	//DWORD dwCbwFX;
	fprintf(sFile, "dwCbwFX= %ld \n", cAudioFormat.m_dwCbwFX);
	fprintf(sFile, "iRecordAudio= %d \n", cAudioFormat.m_iRecordAudio);

	//Audio Formats Dialog
	fprintf(sFile, "dwWaveinSelected= %d \n", cAudioFormat.m_dwWaveinSelected);
	fprintf(sFile, "iAudioBitsPerSample= %d \n", cAudioFormat.m_iBitsPerSample);
	fprintf(sFile, "iAudioNumChannels= %d \n", cAudioFormat.m_iNumChannels);
	fprintf(sFile, "iAudioSamplesPerSeconds= %d \n", cAudioFormat.m_iSamplesPerSeconds);
	fprintf(sFile, "bAudioCompression= %d \n", cAudioFormat.m_bCompression);

	fprintf(sFile, "bInterleaveFrames= %d \n", cAudioFormat.m_bInterleaveFrames);
	fprintf(sFile, "iInterleaveFactor= %d \n", cAudioFormat.m_iInterleaveFactor);

	//Key Shortcuts
	fprintf(sFile, "keyRecordStart= %d \n",cHotKeyOpts.m_RecordStart.m_vKey);
	fprintf(sFile, "keyRecordEnd= %d \n",cHotKeyOpts.m_RecordEnd.m_vKey);
	fprintf(sFile, "uKeyRecordCancel= %d \n",cHotKeyOpts.m_RecordCancel.m_vKey);

	//iViewType
	fprintf(sFile, "iViewType= %d \n",cProgramOpts.m_iViewType);

	fprintf(sFile, "bAutoAdjust= %d \n",cVideoOpts.m_bAutoAdjust);
	fprintf(sFile, "iValueAdjust= %d \n",cVideoOpts.m_iValueAdjust);

	fprintf(sFile, "savedir=%d \n", savedir.GetLength());
	// TODO: Just save the string
	fprintf(sFile, "strCursorDir=%d \n", CamCursor.Dir().GetLength());

	//Ver 1.3
	fprintf(sFile, "iThreadPriority=%d \n",cProgramOpts.m_iThreadPriority);

	//Ver 1.5
	fprintf(sFile, "iCaptureLeft= %d \n",cRegionOpts.m_iCaptureLeft);
	fprintf(sFile, "iCaptureTop= %d \n",cRegionOpts.m_iCaptureTop);
	fprintf(sFile, "bFixedCapture=%d \n",cRegionOpts.m_bFixedCapture);
	fprintf(sFile, "iInterleaveUnit= %d \n", cAudioFormat.m_iInterleavePeriod);

	//Ver 1.6
	fprintf(sFile, "iTempPathAccess=%d \n",cProgramOpts.m_iTempPathAccess);
	fprintf(sFile, "bCaptureTrans=%d \n",cProgramOpts.m_bCaptureTrans);
	fprintf(sFile, "specifieddir=%d \n",specifieddir.GetLength());

	fprintf(sFile, "NumDev=%d \n",cAudioFormat.m_iMixerDevices);
	fprintf(sFile, "SelectedDev=%d \n",cAudioFormat.m_iSelectedMixer);
	fprintf(sFile, "iFeedbackLine=%d \n",cAudioFormat.m_iFeedbackLine);
	fprintf(sFile, "feedback_line_info=%d \n",cAudioFormat.m_iFeedbackLineInfo);
	fprintf(sFile, "bPerformAutoSearch=%d \n",cAudioFormat.m_bPerformAutoSearch);

	//Ver 1.8
	fprintf(sFile, "bSupportMouseDrag=%d \n",cRegionOpts.m_bSupportMouseDrag);

	//New variables, add here
	fprintf(sFile, "keyRecordStartCtrl=%d \n",cHotKeyOpts.m_RecordStart.m_bCtrl);
	fprintf(sFile, "keyRecordEndCtrl=%d \n",cHotKeyOpts.m_RecordEnd.m_bCtrl);
	fprintf(sFile, "keyRecordCancelCtrl=%d \n",cHotKeyOpts.m_RecordCancel.m_bCtrl);

	fprintf(sFile, "keyRecordStartAlt=%d \n",cHotKeyOpts.m_RecordStart.m_bAlt);
	fprintf(sFile, "keyRecordEndAlt=%d \n",cHotKeyOpts.m_RecordEnd.m_bAlt);
	fprintf(sFile, "keyRecordCancelAlt=%d \n",cHotKeyOpts.m_RecordCancel.m_bAlt);

	fprintf(sFile, "keyRecordStartShift=%d \n",cHotKeyOpts.m_RecordStart.m_bShift);
	fprintf(sFile, "keyRecordEndShift=%d \n",cHotKeyOpts.m_RecordEnd.m_bShift);
	fprintf(sFile, "keyRecordCancelShift=%d \n",cHotKeyOpts.m_RecordCancel.m_bShift);

	fprintf(sFile, "keyNext=%d \n",cHotKeyOpts.m_Next.m_vKey);
	fprintf(sFile, "keyPrev=%d \n",cHotKeyOpts.m_Prev.m_vKey);
	fprintf(sFile, "keyShowLayout=%d \n",cHotKeyOpts.m_ShowLayout.m_vKey);

	fprintf(sFile, "keyNextCtrl=%d \n",cHotKeyOpts.m_Next.m_bCtrl);
	fprintf(sFile, "keyPrevCtrl=%d \n",cHotKeyOpts.m_Prev.m_bCtrl);
	fprintf(sFile, "keyShowLayoutCtrl=%d \n",cHotKeyOpts.m_ShowLayout.m_bCtrl);

	fprintf(sFile, "keyNextAlt=%d \n",cHotKeyOpts.m_Next.m_bAlt);
	fprintf(sFile, "keyPrevAlt=%d \n",cHotKeyOpts.m_Prev.m_bAlt);
	fprintf(sFile, "keyShowLayoutAlt=%d \n",cHotKeyOpts.m_ShowLayout.m_bAlt);

	fprintf(sFile, "keyNextShift=%d \n",cHotKeyOpts.m_Next.m_bShift);
	fprintf(sFile, "keyPrevShift=%d \n",cHotKeyOpts.m_Prev.m_bShift);
	fprintf(sFile, "keyShowLayoutShift=%d \n",cHotKeyOpts.m_ShowLayout.m_bShift);

	fprintf(sFile, "iShapeNameInt=%d \n",iShapeNameInt);
	fprintf(sFile, "shapeNameLen=%d \n",shapeName.GetLength());

	fprintf(sFile, "iLayoutNameInt=%d \n",iLayoutNameInt);
	fprintf(sFile, "g_layoutNameLen=%d \n",strLayoutName.GetLength());

	fprintf(sFile, "bUseMCI=%d \n",cAudioFormat.m_bUseMCI);
	fprintf(sFile, "iShiftType=%d \n",cVideoOpts.m_iShiftType);
	fprintf(sFile, "iTimeShift=%d \n",cVideoOpts.m_iTimeShift);
	fprintf(sFile, "iFrameShift=%d \n",iFrameShift);

	//ver 2.26
	fprintf(sFile, "bLaunchPropPrompt=%d \n",cProducerOpts.m_bLaunchPropPrompt);
	fprintf(sFile, "bLaunchHTMLPlayer=%d \n",cProducerOpts.m_bLaunchHTMLPlayer);
	fprintf(sFile, "bDeleteAVIAfterUse=%d \n",cProducerOpts.m_bDeleteAVIAfterUse);
	fprintf(sFile, "iRecordingMode=%d \n",cProgramOpts.m_iRecordingMode);
	fprintf(sFile, "bAutoNaming=%d \n",cProgramOpts.m_bAutoNaming);
	fprintf(sFile, "bRestrictVideoCodecs=%d \n",cVideoOpts.m_bRestrictVideoCodecs);
	//fprintf(sFile, "base_nid=%d \n",base_nid);

	//ver 2.40
	fprintf(sFile, "iPresetTime=%d \n",cProgramOpts.m_iPresetTime);
	fprintf(sFile, "bRecordPreset=%d \n",cProgramOpts.m_bRecordPreset);

	//Add new variables here

	// Effects
	fprintf(sFile, "bTimestampAnnotation=%d \n",cTimestampOpts.m_bAnnotation);
	fprintf(sFile, "timestampBackColor=%d \n", cTimestampOpts.m_taTimestamp.backgroundColor);
	fprintf(sFile, "timestampSelected=%d \n", cTimestampOpts.m_taTimestamp.isFontSelected);
	fprintf(sFile, "timestampPosition=%d \n", cTimestampOpts.m_taTimestamp.position);
	fprintf(sFile, "timestampTextColor=%d \n", cTimestampOpts.m_taTimestamp.textColor);
	fprintf(sFile, "timestampTextFont=%s \n", cTimestampOpts.m_taTimestamp.logfont.lfFaceName);
	fprintf(sFile, "timestampTextWeight=%d \n", cTimestampOpts.m_taTimestamp.logfont.lfWeight);
	fprintf(sFile, "timestampTextHeight=%d \n", cTimestampOpts.m_taTimestamp.logfont.lfHeight);
	fprintf(sFile, "timestampTextWidth=%d \n", cTimestampOpts.m_taTimestamp.logfont.lfWidth);

	fprintf(sFile, "bCaptionAnnotation=%d \n", cCaptionOpts.m_bAnnotation);
	fprintf(sFile, "captionBackColor=%d \n", cCaptionOpts.m_taCaption.backgroundColor);
	fprintf(sFile, "captionSelected=%d \n", cCaptionOpts.m_taCaption.isFontSelected);
	fprintf(sFile, "captionPosition=%d \n", cCaptionOpts.m_taCaption.position);
	// fprintf(sFile, "captionText=%s \n", taCaption.text);
	fprintf(sFile, "captionTextColor=%d \n", cCaptionOpts.m_taCaption.textColor);
	fprintf(sFile, "captionTextFont=%s \n", cCaptionOpts.m_taCaption.logfont.lfFaceName);
	fprintf(sFile, "captionTextWeight=%d \n", cCaptionOpts.m_taCaption.logfont.lfWeight);
	fprintf(sFile, "captionTextHeight=%d \n", cCaptionOpts.m_taCaption.logfont.lfHeight);
	fprintf(sFile, "captionTextWidth=%d \n", cCaptionOpts.m_taCaption.logfont.lfWidth);

	fprintf(sFile, "bWatermarkAnnotation=%d \n",cWatermarkOpts.m_bAnnotation);
	fprintf(sFile, "bWatermarkAnnotation=%d \n",cWatermarkOpts.m_iaWatermark.position);

	fprintf(sFile, "bXNoteAnnotation=%d \n",cXNoteOpts.m_bAnnotation);
	fprintf(sFile, "xnoteBackColor=%d \n", cXNoteOpts.m_taXNote.backgroundColor);
	fprintf(sFile, "xnoteSelected=%d \n", cXNoteOpts.m_taXNote.isFontSelected);
	fprintf(sFile, "xnotePosition=%d \n", cXNoteOpts.m_taXNote.position);
	fprintf(sFile, "xnoteTextColor=%d \n", cXNoteOpts.m_taXNote.textColor);
	fprintf(sFile, "xnoteTextFont=%s \n", cXNoteOpts.m_taXNote.logfont.lfFaceName);
	fprintf(sFile, "xnoteTextWeight=%d \n", cXNoteOpts.m_taXNote.logfont.lfWeight);
	fprintf(sFile, "xnoteTextHeight=%d \n", cXNoteOpts.m_taXNote.logfont.lfHeight);
	fprintf(sFile, "xnoteTextWidth=%d \n", cXNoteOpts.m_taXNote.logfont.lfWidth);


	fclose(sFile);
#endif	// LEGACY_PROFILE_DISABLE

	//ver 1.8,
	CString m_newfile = GetAppDataPath() + "\\CamShapes.ini";
	ListManager.SaveShapeArray(m_newfile);

	m_newfile = GetAppDataPath() + "\\CamLayout.ini";
	ListManager.SaveLayout(m_newfile);

	if (CamCursor.Select() == 2) {
		//CString cursorFileName = "\\CamCursor.ini";
		//CString cursorDir = GetProgPath();
		CString cursorPath;
		cursorPath.Format("%s\\CamCursor.ini", GetProgPath());
		//Note, we do not save the cursorFilePath, but instead we make a copy
		// of the cursor file in the Prog directory
		CopyFile(CamCursor.FileName(), cursorPath, FALSE);
	}

	//********************************************
	//Creating Camdata.ini for storing binary data
	//********************************************
	fileName = "\\Camdata.ini";
	setDir = GetAppDataPath();
	setPath = setDir+fileName;

	FILE * tFile = fopen((LPCTSTR)setPath, "wb");
	if (tFile == NULL) {
		//Error creating file ...do nothing...return
		return;
	}

	// ****************************
	// Dump Variables
	// ****************************
	//Saving Directories, put here
	if (savedir.GetLength()>0)
      {
      if (savedir == GetMyVideoPath())
         {
		   fwrite((LPCTSTR)"%CSIDL_MYVIDEO%", 15, 1, tFile);
         }
      else
         {
		   fwrite((LPCTSTR)savedir, savedir.GetLength(), 1, tFile);
         }
      }

	if (!CamCursor.Dir().IsEmpty())
		fwrite((LPCTSTR)(CamCursor.Dir()), CamCursor.Dir().GetLength(), 1, tFile);

	if (cAudioFormat.m_dwCbwFX > 0)
		fwrite(&(cAudioFormat.AudioFormat()), cAudioFormat.m_dwCbwFX, 1, tFile);

	if (0L < cVideoOpts.StateSize()) {
		fwrite(cVideoOpts.State(), cVideoOpts.StateSize(), 1, tFile);
	}

	//Ver 1.6
	if (cProgramOpts.m_strSpecifiedDir.GetLength() > 0)
		fwrite((LPCTSTR)cProgramOpts.m_strSpecifiedDir, cProgramOpts.m_strSpecifiedDir.GetLength(), 1, tFile);

	//Ver 1.8
	if (shapeName.GetLength() > 0)
		fwrite((LPCTSTR)shapeName, shapeName.GetLength(), 1, tFile);

	if (strLayoutName.GetLength() > 0)
		fwrite((LPCTSTR)strLayoutName, strLayoutName.GetLength(), 1, tFile);

	fclose(tFile);
}

// Ver 1.2
void CRecorderView::LoadSettings()
{
	// this can be deferred until the Create of the Screen Annotation dialog
	//if (ver>=1.799999) {
	{
		//attempt to load the shapes and layouts ...never mind the version
		CString m_newfile;
		m_newfile = GetAppDataPath() + "\\CamShapes.ini";
      if (!DoesFileExist(m_newfile))
         {
   		m_newfile = GetProgPath() + "\\CamShapes.ini";
         }
		ListManager.LoadShapeArray(m_newfile);

		m_newfile = GetAppDataPath() + "\\CamLayout.ini";
      if (!DoesFileExist(m_newfile))
         {
   		m_newfile = GetProgPath() + "\\CamLayout.ini";
         }
		ListManager.LoadLayout(m_newfile);
	}

#ifndef LEGACY_PROFILE_DISABLE

	//The absence of nosave.ini file indicates savesettings = 1
	CString fileName("\\NoSave.ini ");
	CString setDir = GetProgPath();
	CString setPath = setDir+fileName;

	FILE * rFile = fopen((LPCTSTR)setPath,"rt");
	if (rFile == NULL) {
		cProgramOpts.m_bSaveSettings = true;
	} else {
		fclose(rFile);
		cProgramOpts.m_bSaveSettings = false;
	}

	//********************************************
	//Loading CamStudio.ini for storing text data
	//********************************************
	fileName = "\\CamStudio.ini";
	setDir = GetProgPath();
	setPath = setDir+fileName;

	FILE * sFile = fopen((LPCTSTR)setPath,"rt");
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
	//int iSaveLen=0;
	//int iCursorLen=0;
	char sdata[1000];
	char tdata[1000];
	float ver=1.0;

	//Ver 1.6
	//int iSpecifiedDirLength=0;
	char specdata[1000];

	fscanf_s(sFile, "[ CamStudio Settings ver%f -- Please do not edit ] \n\n",&ver);

	//Ver 1.2
	if (ver>=1.199999) {
		fscanf_s(sFile, "bFlashingRect=%d \n", &cProgramOpts.m_bFlashingRect);

		fscanf_s(sFile, "iLaunchPlayer=%d \n", &cProgramOpts.m_iLaunchPlayer);
		fscanf_s(sFile, "bMinimizeOnStart=%d \n", &cProgramOpts.m_bMinimizeOnStart);
		fscanf_s(sFile, "iMouseCaptureMode= %d \n", &cRegionOpts.m_iMouseCaptureMode);
		fscanf_s(sFile, "iCaptureWidth=%d \n", &cRegionOpts.m_iCaptureWidth);
		fscanf_s(sFile, "iCaptureHeight=%d \n", &cRegionOpts.m_iCaptureHeight);

		fscanf_s(sFile, "iTimeLapse=%d \n", &cVideoOpts.m_iTimeLapse);
		fscanf_s(sFile, "iFramesPerSecond= %d \n", &cVideoOpts.m_iFramesPerSecond);
		fscanf_s(sFile, "iKeyFramesEvery= %d \n", &cVideoOpts.m_iKeyFramesEvery);
		fscanf_s(sFile, "iCompQuality= %d \n", &cVideoOpts.m_iCompQuality);
		fscanf_s(sFile, "dwCompfccHandler= %ld \n", &cVideoOpts.m_dwCompfccHandler);

		//LPVOID pVideoCompressParams = NULL;
		fscanf_s(sFile, "dwCompressorStateIsFor= %ld \n",&cVideoOpts.m_dwCompressorStateIsFor);
		fscanf_s(sFile, "dwCompressorStateSize= %d \n",&cVideoOpts.m_dwCompressorStateSize);

		int itemp = 0;
		fscanf_s(sFile, "bRecordCursor=%d \n",&itemp);
		CamCursor.Record(itemp ? true : false);
		// Having this line means the custom cursor type cannot be re-arranged
		// in a new order in the combo box...else previous saved settings
		// referring to the custom type will not be correct

		{
			int iCursorType = 0;
			fscanf_s(sFile, "iCustomSel=%d \n",&iCursorType);
			CamCursor.CustomType(iCursorType);
			fscanf_s(sFile, "iCursorType=%d \n", &iCursorType);
			CamCursor.Select(iCursorType);
			int itemp = 0;
			fscanf_s(sFile, "bHighlightCursor=%d \n",&itemp);
			CamCursor.Highlight(itemp ? true : false);
			fscanf_s(sFile, "iHighlightSize=%d \n",&itemp);
			CamCursor.HighlightSize(itemp);
			fscanf_s(sFile, "iHighlightShape=%d \n",&itemp);
			CamCursor.HighlightShape(itemp);
			fscanf_s(sFile, "bHighlightClick=%d \n",&itemp);
			CamCursor.HighlightClick(itemp ? true : false);
		}

		int redv = 0;
		int greenv = 0;
		int bluev = 0;
		fscanf_s(sFile, "g_highlightcolorR=%d \n",&idata);
		redv=idata;
		fscanf_s(sFile, "g_highlightcolorG=%d \n",&idata);
		greenv=idata;
		fscanf_s(sFile, "g_highlightcolorB=%d \n",&idata);
		bluev=idata;
		CamCursor.HighlightColor(RGB(redv,greenv,bluev));

		redv=0;greenv=0;bluev=0;
		fscanf_s(sFile, "g_highlightclickcolorleftR=%d \n",&idata);
		redv=idata;
		fscanf_s(sFile, "g_highlightclickcolorleftG=%d \n",&idata);
		greenv=idata;
		fscanf_s(sFile, "g_highlightclickcolorleftB=%d \n",&idata);
		bluev=idata;
		CamCursor.ClickLeftColor(RGB(redv,greenv,bluev));

		redv=0;greenv=0;bluev=0;
		fscanf_s(sFile, "g_highlightclickcolorrightR=%d \n",&idata);
		redv=idata;
		fscanf_s(sFile, "g_highlightclickcolorrightG=%d \n",&idata);
		greenv=idata;
		fscanf_s(sFile, "g_highlightclickcolorrightB=%d \n",&idata);
		bluev=idata;
		CamCursor.ClickRightColor(RGB(redv,greenv,bluev));

		fscanf_s(sFile, "autopan=%d \n",&cProgramOpts.m_bAutoPan);
		fscanf_s(sFile, "iMaxPan= %d \n",&cProgramOpts.m_iMaxPan);

		//Audio Functions and Variables
		fscanf_s(sFile, "uAudioDeviceID= %d \n",&cAudioFormat.m_uDeviceID);

		//Audio Options Dialog
		fscanf_s(sFile, "dwCbwFX= %ld \n", &cAudioFormat.m_dwCbwFX);
		fscanf_s(sFile, "iRecordAudio= %d \n", &cAudioFormat.m_iRecordAudio);

		//Audio Formats Dialog
		fscanf_s(sFile, "dwWaveinSelected= %d \n", &cAudioFormat.m_dwWaveinSelected);
		fscanf_s(sFile, "iAudioBitsPerSample= %d \n", &cAudioFormat.m_iBitsPerSample);
		fscanf_s(sFile, "iAudioNumChannels= %d \n", &cAudioFormat.m_iNumChannels);
		fscanf_s(sFile, "iAudioSamplesPerSeconds= %d \n", &cAudioFormat.m_iSamplesPerSeconds);
		fscanf_s(sFile, "bAudioCompression= %d \n", &cAudioFormat.m_bCompression);

		fscanf_s(sFile, "bInterleaveFrames= %d \n", &cAudioFormat.m_bInterleaveFrames);
		fscanf_s(sFile, "iInterleaveFactor= %d \n", &cAudioFormat.m_iInterleaveFactor);

		//Key Shortcuts
		fscanf_s(sFile, "keyRecordStart= %d \n",&cHotKeyOpts.m_RecordStart.m_vKey);
		fscanf_s(sFile, "keyRecordEnd= %d \n",&cHotKeyOpts.m_RecordEnd.m_vKey);
		fscanf_s(sFile, "uKeyRecordCancel= %d \n",&cHotKeyOpts.m_RecordCancel.m_vKey);

		fscanf_s(sFile, "iViewType= %d \n",&cProgramOpts.m_iViewType);

		fscanf_s(sFile, "bAutoAdjust= %d \n",&cVideoOpts.m_bAutoAdjust);
		fscanf_s(sFile, "iValueAdjust= %d \n",&cVideoOpts.m_iValueAdjust);

		fscanf_s(sFile, "savedir=%d \n",&iSaveLen);
		fscanf_s(sFile, "strCursorDir=%d \n",&iCursorLen);

		AttemptRecordingFormat();

		//ver 1.4
		//Force settings from previous version to upgrade
		if (ver<1.35) {
			//set auto adjust to max rate
			cVideoOpts.m_bAutoAdjust = 1;
			cVideoOpts.m_iValueAdjust = 1;

			//set default compressor
			cVideoOpts.m_dwCompfccHandler = ICHANDLER_MSVC;
			cVideoOpts.m_dwCompressorStateIsFor = 0;
			cVideoOpts.m_dwCompressorStateSize = 0;
			cVideoOpts.m_iCompQuality = 7000;

			//set default audio recording format and compress format
			SuggestRecordingFormat();
			SuggestCompressFormat();
		}

		if (cVideoOpts.m_bAutoAdjust)
		{
			int framerate;
			int delayms;
			int val = cVideoOpts.m_iValueAdjust;
			AutoSetRate(val, framerate, delayms);
			cVideoOpts.m_iTimeLapse = delayms;
			cVideoOpts.m_iFramesPerSecond = framerate;
			cVideoOpts.m_iKeyFramesEvery = framerate;
		}

		strCodec = GetCodecDescription(cVideoOpts.m_dwCompfccHandler);

		switch (CamCursor.Select())
		{
		case 1:
			{
				if (CamCursor.CustomType() < 0)
				{
					CamCursor.CustomType(0);
				}
				DWORD customicon = CamCursor.GetID(CamCursor.CustomType());
				if (!CamCursor.Custom(::LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(customicon))))
				{
					CamCursor.Select(0);
				}
			}
			break;
		case 2:		//load cursor
			{
				CString cursorPath;
				cursorPath.Format("%s\\CamCursor.ini", GetProgPath());
				if (!CamCursor.Load(::LoadCursorFromFile(cursorPath)))
				{
					CamCursor.Select(0);
				}
			}
			break;
		}
	}//ver 1.2

	//Ver 1.3
	if (ver >= 1.299999)
	{
		fscanf_s(sFile, "iThreadPriority=%d \n",&cProgramOpts.m_iThreadPriority);
	}

	//Ver 1.5
	if (ver >= 1.499999)
	{
		fscanf_s(sFile, "iCaptureLeft= %d \n",&cRegionOpts.m_iCaptureLeft);
		fscanf_s(sFile, "iCaptureTop= %d \n",&cRegionOpts.m_iCaptureTop);
		fscanf_s(sFile, "bFixedCapture=%d \n",&cRegionOpts.m_bFixedCapture);
		fscanf_s(sFile, "iInterleaveUnit= %d \n", &cAudioFormat.m_iInterleavePeriod);
	} else {
		//force interleve settings
		cAudioFormat.m_iInterleavePeriod = MILLISECONDS;
		cAudioFormat.m_iInterleaveFactor = 100;
	}

	//Ver 1.6
	if (ver >= 1.599999)
	{
		fscanf_s(sFile, "iTempPathAccess=%d \n",&cProgramOpts.m_iTempPathAccess);
		fscanf_s(sFile, "bCaptureTrans=%d \n",&cProgramOpts.m_bCaptureTrans);
		fscanf_s(sFile, "specifieddir=%d \n",&cProgramOpts.m_iSpecifiedDirLength);
		fscanf_s(sFile, "NumDev=%d \n",&cAudioFormat.m_iMixerDevices);
		fscanf_s(sFile, "SelectedDev=%d \n",&cAudioFormat.m_iSelectedMixer);
		fscanf_s(sFile, "iFeedbackLine=%d \n",&cAudioFormat.m_iFeedbackLine);
		fscanf_s(sFile, "feedback_line_info=%d \n",&cAudioFormat.m_iFeedbackLineInfo);
		fscanf_s(sFile, "bPerformAutoSearch=%d \n",&cAudioFormat.m_bPerformAutoSearch);

		onLoadSettings(cAudioFormat.m_iRecordAudio);
	} else
	{
		cProgramOpts.m_iTempPathAccess = USE_WINDOWS_TEMP_DIR;
		cProgramOpts.m_bCaptureTrans = true;
		cProgramOpts.m_iSpecifiedDirLength = 0;

		cAudioFormat.m_iMixerDevices = 0;
		cAudioFormat.m_iSelectedMixer = 0;
		cAudioFormat.m_iFeedbackLine = -1;
		cAudioFormat.m_iFeedbackLineInfo = -1;
		cAudioFormat.m_bPerformAutoSearch = 1;
	}

	if (cProgramOpts.m_iSpecifiedDirLength == 0)
	{
		int old_tempPath_Access = cProgramOpts.m_iTempPathAccess;
		cProgramOpts.m_iTempPathAccess = USE_WINDOWS_TEMP_DIR;
		cProgramOpts.m_strSpecifiedDir = GetTempFolder(cProgramOpts.m_iTempPathAccess, cProgramOpts.m_strSpecifiedDir);
		cProgramOpts.m_iTempPathAccess = old_tempPath_Access;

		//Do not modify the iSpecifiedDirLength variable, even if specifieddir is changed. It will need to be used below
	}

	//Update Player to ver 2.0
	//Make the the modified keys do not overlap
	if (ver < 1.799999)
	{
		if (cProgramOpts.m_iLaunchPlayer == CAM1_PLAYER)
			cProgramOpts.m_iLaunchPlayer = CAM2_PLAYER;

		if ((cHotKeyOpts.m_RecordStart.m_vKey == VK_MENU)
			|| (cHotKeyOpts.m_RecordStart.m_vKey == VK_SHIFT)
			|| (cHotKeyOpts.m_RecordStart.m_vKey == VK_CONTROL)
			|| (cHotKeyOpts.m_RecordStart.m_vKey == VK_ESCAPE)) {
			cHotKeyOpts.m_RecordStart.m_vKey = VK_F8;
			cHotKeyOpts.m_RecordStart.m_bCtrl = true;
		}

		if ((cHotKeyOpts.m_RecordEnd.m_vKey == VK_MENU)
			|| (cHotKeyOpts.m_RecordEnd.m_vKey == VK_SHIFT)
			|| (cHotKeyOpts.m_RecordEnd.m_vKey == VK_CONTROL)
			|| (cHotKeyOpts.m_RecordEnd.m_vKey == VK_ESCAPE)) {
			cHotKeyOpts.m_RecordEnd.m_vKey = VK_F9;
			cHotKeyOpts.m_RecordEnd.m_bCtrl = true;
		}

		if ((cHotKeyOpts.m_RecordCancel.m_vKey == VK_MENU)
			|| (cHotKeyOpts.m_RecordCancel.m_vKey == VK_SHIFT)
			|| (cHotKeyOpts.m_RecordCancel.m_vKey == VK_CONTROL)
			|| (cHotKeyOpts.m_RecordCancel.m_vKey == VK_ESCAPE)) {
			cHotKeyOpts.m_RecordCancel.m_vKey = VK_F10;
			cHotKeyOpts.m_RecordCancel.m_bCtrl = 1;
		}
	}

	//Ver 1.8
	int shapeNameLen = 0;
	int layoutNameLen = 0;
	if (ver >= 1.799999)
	{
		fscanf_s(sFile, "bSupportMouseDrag=%d \n",&cRegionOpts.m_bSupportMouseDrag);

		fscanf_s(sFile, "keyRecordStartCtrl=%d \n",&cHotKeyOpts.m_RecordStart.m_bCtrl);
		fscanf_s(sFile, "keyRecordEndCtrl=%d \n",&cHotKeyOpts.m_RecordEnd.m_bCtrl);
		fscanf_s(sFile, "keyRecordCancelCtrl=%d \n",&cHotKeyOpts.m_RecordCancel.m_bCtrl);

		fscanf_s(sFile, "keyRecordStartAlt=%d \n",&cHotKeyOpts.m_RecordStart.m_bAlt);
		fscanf_s(sFile, "keyRecordEndAlt=%d \n",&cHotKeyOpts.m_RecordEnd.m_bAlt);
		fscanf_s(sFile, "keyRecordCancelAlt=%d \n",&cHotKeyOpts.m_RecordCancel.m_bAlt);

		fscanf_s(sFile, "keyRecordStartShift=%d \n",&cHotKeyOpts.m_RecordStart.m_bShift);
		fscanf_s(sFile, "keyRecordEndShift=%d \n",&cHotKeyOpts.m_RecordEnd.m_bShift);
		fscanf_s(sFile, "keyRecordCancelShift=%d \n",&cHotKeyOpts.m_RecordCancel.m_bShift);

		fscanf_s(sFile, "keyNext=%d \n",&cHotKeyOpts.m_Next.m_vKey);
		fscanf_s(sFile, "keyPrev=%d \n",&cHotKeyOpts.m_Prev.m_vKey);
		fscanf_s(sFile, "keyShowLayout=%d \n",&cHotKeyOpts.m_ShowLayout.m_vKey);

		fscanf_s(sFile, "keyNextCtrl=%d \n",&cHotKeyOpts.m_Next.m_bCtrl);
		fscanf_s(sFile, "keyPrevCtrl=%d \n",&cHotKeyOpts.m_Prev.m_bCtrl);
		fscanf_s(sFile, "keyShowLayoutCtrl=%d \n",&cHotKeyOpts.m_ShowLayout.m_bCtrl);

		fscanf_s(sFile, "keyNextAlt=%d \n",&cHotKeyOpts.m_Next.m_bAlt);
		fscanf_s(sFile, "keyPrevAlt=%d \n",&cHotKeyOpts.m_Prev.m_bAlt);
		fscanf_s(sFile, "keyShowLayoutAlt=%d \n",&cHotKeyOpts.m_ShowLayout.m_bAlt);

		fscanf_s(sFile, "keyNextShift=%d \n",&cHotKeyOpts.m_Next.m_bShift);
		fscanf_s(sFile, "keyPrevShift=%d \n",&cHotKeyOpts.m_Prev.m_bShift);
		fscanf_s(sFile, "keyShowLayoutShift=%d \n",&cHotKeyOpts.m_ShowLayout.m_bShift);

		fscanf_s(sFile, "iShapeNameInt=%d \n",&iShapeNameInt);
		fscanf_s(sFile, "shapeNameLen=%d \n",&shapeNameLen);

		fscanf_s(sFile, "iLayoutNameInt=%d \n",&iLayoutNameInt);
		fscanf_s(sFile, "g_layoutNameLen=%d \n",&layoutNameLen);

		fscanf_s(sFile, "bUseMCI=%d \n",&cAudioFormat.m_bUseMCI);
		fscanf_s(sFile, "iShiftType=%d \n",&cVideoOpts.m_iShiftType);
		fscanf_s(sFile, "iTimeShift=%d \n",&cVideoOpts.m_iTimeShift);
		fscanf_s(sFile, "iFrameShift=%d \n",&iFrameShift);
	}

	//ver 2.26
	//save format is set as 2.0
	if (ver >= 1.999999)
	{
		fscanf_s(sFile, "bLaunchPropPrompt=%d \n",&cProducerOpts.m_bLaunchPropPrompt);
		fscanf_s(sFile, "bLaunchHTMLPlayer=%d \n",&cProducerOpts.m_bLaunchHTMLPlayer);
		fscanf_s(sFile, "bDeleteAVIAfterUse=%d \n",&cProducerOpts.m_bDeleteAVIAfterUse);
		fscanf_s(sFile, "iRecordingMode=%d \n",&cProgramOpts.m_iRecordingMode);
		fscanf_s(sFile, "bAutoNaming=%d \n",&cProgramOpts.m_bAutoNaming);
		fscanf_s(sFile, "bRestrictVideoCodecs=%d \n",&cVideoOpts.m_bRestrictVideoCodecs);
		//fscanf_s(sFile, "base_nid=%d \n",&base_nid);
	}

	//ver 2.40
	if (ver >= 2.399999)
	{
		fscanf_s(sFile, "iPresetTime=%d \n",&cProgramOpts.m_iPresetTime);
		fscanf_s(sFile, "bRecordPreset=%d \n",&cProgramOpts.m_bRecordPreset);
	}

	//new variables add here

	// Effects
	fscanf_s(sFile, "bTimestampAnnotation=%d \n",&cTimestampOpts.m_bAnnotation);
	fscanf_s(sFile, "timestampBackColor=%d \n", &cTimestampOpts.m_taTimestamp.backgroundColor);
	fscanf_s(sFile, "timestampSelected=%d \n", &cTimestampOpts.m_taTimestamp.isFontSelected);
	fscanf_s(sFile, "timestampPosition=%d \n", &cTimestampOpts.m_taTimestamp.position);
	fscanf_s(sFile, "timestampTextColor=%d \n", &cTimestampOpts.m_taTimestamp.textColor);
	fscanf(sFile, "timestampTextFont=%s \n", cTimestampOpts.m_taTimestamp.logfont.lfFaceName);
	fscanf_s(sFile, "timestampTextWeight=%d \n", &cTimestampOpts.m_taTimestamp.logfont.lfWeight);
	fscanf_s(sFile, "timestampTextHeight=%d \n", &cTimestampOpts.m_taTimestamp.logfont.lfHeight);
	fscanf_s(sFile, "timestampTextWidth=%d \n", &cTimestampOpts.m_taTimestamp.logfont.lfWidth);

	fscanf_s(sFile, "bCaptionAnnotation=%d \n", &cCaptionOpts.m_bAnnotation);
	fscanf_s(sFile, "captionBackColor=%d \n", &cCaptionOpts.m_taCaption.backgroundColor);
	fscanf_s(sFile, "captionSelected=%d \n", &cCaptionOpts.m_taCaption.isFontSelected);
	fscanf_s(sFile, "captionPosition=%d \n", &cCaptionOpts.m_taCaption.position);
	// fscanf_s(sFile, "captionText=%s \n", &taCaption.text);
	fscanf_s(sFile, "captionTextColor=%d \n", &cCaptionOpts.m_taCaption.textColor);
	fscanf(sFile, "captionTextFont=%s \n", cCaptionOpts.m_taCaption.logfont.lfFaceName);
	fscanf_s(sFile, "captionTextWeight=%d \n", &cCaptionOpts.m_taCaption.logfont.lfWeight);
	fscanf_s(sFile, "captionTextHeight=%d \n", &cCaptionOpts.m_taCaption.logfont.lfHeight);
	fscanf_s(sFile, "captionTextWidth=%d \n", &cCaptionOpts.m_taCaption.logfont.lfWidth);

	fscanf_s(sFile, "bWatermarkAnnotation=%d \n",&cWatermarkOpts.m_bAnnotation);
	fscanf_s(sFile, "bWatermarkAnnotation=%d \n",&cWatermarkOpts.m_iaWatermark.position);

	fclose(sFile);
#else
	AttemptRecordingFormat();
	//set default audio recording format and compress format
	// This prevents reading profile settings
	//SuggestRecordingFormat();
	//SuggestCompressFormat();
	onLoadSettings(cAudioFormat.m_iRecordAudio);

	CString fileName("");
	CString setDir("");
	CString setPath("");
	char sdata[1000];
	char tdata[1000];
	//char specdata[1000];
	int shapeNameLen = 0;
	int layoutNameLen = 0;
	float ver = 2.5;       // What kind of var is this?  (Value for version?)

#endif
	//********************************************
	//Loading Camdata.ini binary data
	//********************************************
	fileName = "\\Camdata.ini";
	setDir = GetAppDataPath();
	setPath = setDir + fileName;
	FILE * tFile = fopen(LPCTSTR(setPath),"rb");

   if (tFile == NULL) {
   	setDir = GetProgPath();
	   setPath = setDir + fileName;
	   tFile = fopen(LPCTSTR(setPath),"rb");
	}
   
   if (tFile == NULL) {
		//Error creating file
		cAudioFormat.m_dwCbwFX = 0;
		SuggestCompressFormat();
		return;
	}

	if (ver >= 1.2)
	{
		// ****************************
		// Load Binary Data
		// ****************************
		if ((iSaveLen>0) && (iSaveLen<1000)) {
			fread(sdata, iSaveLen, 1, tFile);
			sdata[iSaveLen]=0;
			savedir=CString(sdata);
		}

		if ((iCursorLen > 0) && (iCursorLen < 1000)) {
			fread(tdata, iCursorLen, 1, tFile);
			tdata[iCursorLen] = 0;
			CamCursor.Dir(CString(tdata));
		}

		if (ver > 1.35)
		{
			// if perfoming an upgrade from previous settings,
			// do not load these additional camdata.ini information
			if (0 < cAudioFormat.m_dwCbwFX) {
				//AllocCompressFormat(cAudioFormat.m_dwCbwFX);
				int countread = fread(&(cAudioFormat.AudioFormat()), cAudioFormat.m_dwCbwFX, 1, tFile);
				if (1 == countread) {
					AttemptCompressFormat();
				} else {
					cAudioFormat.DeleteAudio();
					SuggestCompressFormat();
				}
			}

			if (1L <= cVideoOpts.StateSize()) {
				fread(cVideoOpts.State(cVideoOpts.StateSize()), cVideoOpts.StateSize(), 1, tFile);
			}

			//ver 1.6
			if (ver > 1.55)
			{
// TODO, delete
#ifdef OBSOLETE_CODE
				// if upgrade from older file versions,
				// iSpecifiedDirLength == 0 and the following code will not run
				if ((cProgramOpts.m_iSpecifiedDirLength > 0) && (cProgramOpts.m_iSpecifiedDirLength < 1000)) {
					fread(specdata, cProgramOpts.m_iSpecifiedDirLength, 1, tFile);
					specdata[cProgramOpts.m_iSpecifiedDirLength]=0;
					cProgramOpts.m_strSpecifiedDir = CString(specdata);
				}
#endif // OBSOLETE_CODE

				//ver 1.8
				if (ver >= 1.799999)
				{
					char namedata[1000];
					if ((shapeNameLen > 0) && (shapeNameLen < 1000))
					{
						fread(namedata, shapeNameLen, 1, tFile);
						namedata[shapeNameLen]=0;
						shapeName = CString(namedata);
					}

					if ((layoutNameLen > 0) && (layoutNameLen < 1000))
					{
						fread(namedata, layoutNameLen, 1, tFile);
						namedata[layoutNameLen] = 0;
						strLayoutName = CString(namedata);
					}
				}
			}
		}
	}

	fclose(tFile);
}

void CRecorderView::OnOptionsProgramoptionsSavesettingsonexit()
{
	cProgramOpts.m_bSaveSettings = !cProgramOpts.m_bSaveSettings;
}

void CRecorderView::OnUpdateOptionsProgramoptionsSavesettingsonexit(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(cProgramOpts.m_bSaveSettings);
}

void CRecorderView::DecideSaveSettings()
{
	CString nosaveName("\\NoSave.ini");
	CString nosaveDir = GetAppDataPath();
	CString nosavePath = nosaveDir + nosaveName;

	if (cProgramOpts.m_bSaveSettings) {
#pragma message("CRecorderView::SaveSettings skipped")
		SaveSettings();
		DeleteFile(nosavePath);
	} else {
		// Create the nosave.ini file if savesettings = 0;
		FILE *rFile = fopen(LPCTSTR(nosavePath),"wt");
		fprintf(rFile,"savesettings = 0 \n");
		fclose(rFile);

		//Delete Settings File
		CString setDir;
		CString setPath;
		CString fileName("\\CamStudio.ini ");
		setDir = GetAppDataPath();
		setPath = setDir+fileName;

		DeleteFile(setPath);

		fileName = "\\Camdata.ini";
		setPath = setDir + fileName;

		DeleteFile(setPath);

		fileName = "\\CamCursor.ini";
		setPath = setDir + fileName;

		DeleteFile(setPath);
	}
}

void CRecorderView::OnOptionsRecordingthreadpriorityNormal()
{
	cProgramOpts.m_iThreadPriority = THREAD_PRIORITY_NORMAL;
}

void CRecorderView::OnOptionsRecordingthreadpriorityHighest()
{
	cProgramOpts.m_iThreadPriority = THREAD_PRIORITY_HIGHEST;
}

void CRecorderView::OnOptionsRecordingthreadpriorityAbovenormal()
{
	cProgramOpts.m_iThreadPriority = THREAD_PRIORITY_ABOVE_NORMAL;
}

void CRecorderView::OnOptionsRecordingthreadpriorityTimecritical()
{
	cProgramOpts.m_iThreadPriority = THREAD_PRIORITY_TIME_CRITICAL;
}

void CRecorderView::OnUpdateOptionsRecordingthreadpriorityNormal(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(cProgramOpts.m_iThreadPriority == THREAD_PRIORITY_NORMAL);
}

void CRecorderView::OnUpdateOptionsRecordingthreadpriorityHighest(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(cProgramOpts.m_iThreadPriority == THREAD_PRIORITY_HIGHEST);
}

void CRecorderView::OnUpdateOptionsRecordingthreadpriorityAbovenormal(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(cProgramOpts.m_iThreadPriority == THREAD_PRIORITY_ABOVE_NORMAL);
}

void CRecorderView::OnUpdateOptionsRecordingthreadpriorityTimecritical(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(cProgramOpts.m_iThreadPriority == THREAD_PRIORITY_TIME_CRITICAL);
}

void CRecorderView::OnOptionsCapturetrans()
{
	cProgramOpts.m_bCaptureTrans = !cProgramOpts.m_bCaptureTrans;
}

void CRecorderView::OnUpdateOptionsCapturetrans(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(cProgramOpts.m_bCaptureTrans);
}

void CRecorderView::OnOptionsTempdirWindows()
{
	cProgramOpts.m_iTempPathAccess = USE_WINDOWS_TEMP_DIR;
}

void CRecorderView::OnUpdateOptionsTempdirWindows(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(cProgramOpts.m_iTempPathAccess == USE_WINDOWS_TEMP_DIR);
}

void CRecorderView::OnOptionsTempdirInstalled()
{
	cProgramOpts.m_iTempPathAccess = USE_INSTALLED_DIR;
}

void CRecorderView::OnUpdateOptionsTempdirInstalled(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(cProgramOpts.m_iTempPathAccess == USE_INSTALLED_DIR);
}

void CRecorderView::OnOptionsTempdirUser()
{
	CFolderDialog cfg(cProgramOpts.m_strSpecifiedDir);
	if (IDOK == cfg.DoModal()) {
		cProgramOpts.m_strSpecifiedDir = cfg.GetPathName();
		cProgramOpts.m_iTempPathAccess = USE_USER_SPECIFIED_DIR;
	}
}

void CRecorderView::OnUpdateOptionsTempdirUser(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(cProgramOpts.m_iTempPathAccess == USE_USER_SPECIFIED_DIR);
}
void CRecorderView::OnOptionsOutputDirWindows()
{
	cProgramOpts.m_iOutputPathAccess = USE_WINDOWS_TEMP_DIR;
}
void CRecorderView::OnUpdateOptionsOutputDirWindows(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(cProgramOpts.m_iOutputPathAccess == USE_WINDOWS_TEMP_DIR);
}
void CRecorderView::OnOptionsOutputDirInstalled()
{
	cProgramOpts.m_iOutputPathAccess = USE_INSTALLED_DIR;
}
void CRecorderView::OnUpdateOptionsOutputDirInstalled(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(cProgramOpts.m_iOutputPathAccess == USE_INSTALLED_DIR);
}
void CRecorderView::OnOptionsOutputDirUser()
{
	CFolderDialog cfg(cProgramOpts.m_strSpecifiedDir);
	if (IDOK == cfg.DoModal()) {
		cProgramOpts.m_strSpecifiedDir = cfg.GetPathName();
		cProgramOpts.m_iOutputPathAccess = USE_USER_SPECIFIED_DIR;
	}
}
void CRecorderView::OnUpdateOptionsUser(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(cProgramOpts.m_iOutputPathAccess == USE_USER_SPECIFIED_DIR);
}
void CRecorderView::OnOptionsRecordaudioDonotrecordaudio()
{
	cAudioFormat.m_iRecordAudio = NONE;
}

void CRecorderView::OnUpdateOptionsRecordaudioDonotrecordaudio(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(cAudioFormat.isInput(NONE));
}

void CRecorderView::OnOptionsRecordaudioRecordfromspeakers()
{
	if (waveOutGetNumDevs() == 0) {
		//CString msgstr;
		//msgstr.Format("Unable to detect audio output device. You need a sound card with speakers attached.");
		//MessageBox(msgstr,"Note", MB_OK | MB_ICONEXCLAMATION);
		MessageOut(m_hWnd,IDS_STRING_NOAUDIOOUTPUT,IDS_STRING_NOTE,MB_OK | MB_ICONEXCLAMATION);
		return;
	}

	cAudioFormat.m_iRecordAudio = SPEAKERS;

	useWaveout(FALSE,FALSE);
}

void CRecorderView::OnUpdateOptionsRecordaudioRecordfromspeakers(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(cAudioFormat.isInput(SPEAKERS));
}

void CRecorderView::OnOptionsRecordaudiomicrophone()
{
	if (waveInGetNumDevs() == 0) {
		MessageOut(m_hWnd,IDS_STRING_NOINPUT1,IDS_STRING_NOTE,MB_OK | MB_ICONEXCLAMATION);

		return;

	}

	cAudioFormat.m_iRecordAudio = MICROPHONE;

	useWavein(TRUE,FALSE); //TRUE ==> silence mode, will not report errors
}

void CRecorderView::OnUpdateOptionsRecordaudiomicrophone(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(cAudioFormat.isInput(MICROPHONE));
}

void CRecorderView::OnOptionsProgramoptionsTroubleshoot()
{
	CTroubleShootDlg tbsDlg;
	if (IDOK == tbsDlg.DoModal()) {
		if ((TroubleShootVal == 1)) {
			::PostMessage(AfxGetMainWnd()->GetSafeHwnd(),WM_CLOSE,0,0);
		}
	}
}

void CRecorderView::OnOptionsProgramoptionsCamstudioplay()
{
	cProgramOpts.m_iLaunchPlayer = CAM1_PLAYER;
}

void CRecorderView::OnUpdateOptionsProgramoptionsCamstudioplay(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(cProgramOpts.m_iLaunchPlayer == CAM1_PLAYER);
}

void CRecorderView::OnOptionsProgramoptionsDefaultplay()
{
	cProgramOpts.m_iLaunchPlayer = DEFAULT_PLAYER;
}

void CRecorderView::OnUpdateOptionsProgramoptionsDefaultplay(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(cProgramOpts.m_iLaunchPlayer == DEFAULT_PLAYER);
}

void CRecorderView::OnOptionsProgramoptionsNoplay()
{
	cProgramOpts.m_iLaunchPlayer = NO_PLAYER;
}

void CRecorderView::OnUpdateOptionsProgramoptionsNoplay(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(cProgramOpts.m_iLaunchPlayer == NO_PLAYER);
}

void CRecorderView::OnHelpDonations()
{
	CString progdir,donatepath;
	progdir=GetProgPath();
	donatepath= progdir + "\\help.htm#Donations";

	Openlink(donatepath);
}

void CRecorderView::OnOptionsUsePlayer20()
{
	cProgramOpts.m_iLaunchPlayer = CAM2_PLAYER;
}

void CRecorderView::OnUpdateUsePlayer20(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(cProgramOpts.m_iLaunchPlayer == CAM2_PLAYER);
}

void CRecorderView::OnViewScreenannotations()
{
	if (!bCreatedSADlg) {
		sadlg.Create(IDD_SCREENANNOTATIONS2,NULL);
		sadlg.RefreshShapeList();
		bCreatedSADlg = true;
	}

	if (sadlg.IsWindowVisible())
		sadlg.ShowWindow(SW_HIDE);
	else
		sadlg.ShowWindow(SW_RESTORE);
}

void CRecorderView::OnUpdateViewScreenannotations(CCmdUI* /*pCmdUI*/)
{
}

void CRecorderView::OnViewVideoannotations()
{
	if (!vanWndCreated) {
		int x = (rand() % 100) + 100;
		int y = (rand() % 100) + 100;
		CRect rect;
		CString vastr("Video Annotation");
		CString m_newShapeText("Right Click to Edit Text");

		rect.left = x;
		rect.top = y;
		// TODO, Magic values here again 160,120
		rect.right = rect.left + 160 - 1;
		rect.bottom = rect.top + 120 - 1;
		m_vanWnd.TextString(m_newShapeText);
		m_vanWnd.ShapeString(vastr);
		m_vanWnd.CreateTransparent(m_vanWnd.ShapeString(), rect,NULL);
		vanWndCreated = 1;

	}

	if (m_vanWnd.IsWindowVisible()) {
		m_vanWnd.ShowWindow(SW_HIDE);
	} else {
		if (m_vanWnd.m_iStatus != 1) {
			MessageOut(NULL,IDS_STRING_NOWEBCAM,IDS_STRING_NOTE,MB_OK | MB_ICONEXCLAMATION);
			return;
		}

		m_vanWnd.OnUpdateSize();
		m_vanWnd.ShowWindow(SW_RESTORE);
	}
}

void CRecorderView::OnSetFocus(CWnd* pOldWnd)
{
	CView::OnSetFocus(pOldWnd);
}

/////////////////////////////////////////////////////////////////////////////
// OnHotKey WM_HOTKEY message handler
// The WM_HOTKEY message is posted when the user presses a hot key registered
// by the RegisterHotKey function.
//
// wParam - Specifies the identifier of the hot key that generated the message.
// lParam - The low-order word specifies the keys that were to be pressed in
// combination with the key specified by the high-order word to generate the
// WM_HOTKEY message. This word can be one or more of the following values.
//	MOD_ALT - Either ALT key was held down.
//	MOD_CONTROL - Either CTRL key was held down.
//	MOD_SHIFT - Either SHIFT key was held down.
//	MOD_WIN - Either WINDOWS key was held down.
// The high-order word specifies the virtual key code of the hot key.
//
// HOTKEY_RECORD_START_OR_PAUSE		0
// HOTKEY_RECORD_STOP				1
// HOTKEY_RECORD_CANCELSTOP			2
// HOTKEY_LAYOUT_KEY_NEXT			3
// HOTKEY_LAYOUT_KEY_PREVIOUS		4
// HOTKEY_LAYOUT_SHOW_HIDE_KEY		5
// HOTKEY_ZOOM 6
//
/////////////////////////////////////////////////////////////////////////////
LRESULT CRecorderView::OnHotKey(WPARAM wParam, LPARAM /*lParam*/)
{
	switch (wParam)
	{
	case HOTKEY_RECORD_START_OR_PAUSE:	// 0 = start recording
		if (bRecordState) {
			// pause if currently recording
			if (!bRecordPaused) {
				OnPause();
			} else {
				OnRecord();
			}
		} else {
			if (bAllowNewRecordStartKey) {
				// prevent the case which CamStudio presents more than one region
				// for the user to select
				bAllowNewRecordStartKey = FALSE;
				OnRecord();
			}
		}
		break;
	case HOTKEY_RECORD_STOP:			// 1
		if (bRecordState) {
			if (cHotKeyOpts.m_RecordEnd.m_vKey != cHotKeyOpts.m_RecordCancel.m_vKey) {
				OnRecordInterrupted(cHotKeyOpts.m_RecordEnd.m_vKey, 0);
			} else { // FIXME: something is not quite right here
				OnRecordInterrupted(cHotKeyOpts.m_RecordCancel.m_vKey + 1, 0);
			}
		}
		break;
	case HOTKEY_RECORD_CANCELSTOP:		// 2:
		if (bRecordState) {
			OnRecordInterrupted(cHotKeyOpts.m_RecordCancel.m_vKey, 0);
		}
		break;
	case HOTKEY_LAYOUT_KEY_NEXT :		// 3
		{
			if (!bCreatedSADlg) {
				sadlg.Create(IDD_SCREENANNOTATIONS2, NULL);
				//sadlg.ShowWindow(SW_SHOW);
				bCreatedSADlg = true;
			}
			int max = ListManager.layoutArray.GetSize();
			if (max <= 0)
				return 0;

			// Get Current selected
			int cursel = sadlg.GetLayoutListSelection();
			iCurrentLayout = (cursel < 0) ? 0 : cursel + 1;
			if (max <= iCurrentLayout)
				iCurrentLayout = 0;

			sadlg.InstantiateLayout(iCurrentLayout,1);
		}
		break;
	case HOTKEY_LAYOUT_KEY_PREVIOUS :  // 4
		{
			if (!bCreatedSADlg) {
				sadlg.Create(IDD_SCREENANNOTATIONS2, NULL);
				//sadlg.RefreshLayoutList();
				bCreatedSADlg = true;
			}
			int max = ListManager.layoutArray.GetSize();
			if (max <= 0) {
				return 0;
			}

			//Get Current selected
			int cursel = sadlg.GetLayoutListSelection();
			iCurrentLayout = (cursel < 0) ? 0 : cursel - 1;
			if (iCurrentLayout < 0)
				iCurrentLayout = max - 1;

			sadlg.InstantiateLayout(iCurrentLayout,1);
		}
		break;
	case HOTKEY_LAYOUT_SHOW_HIDE_KEY :  //5
		{
			if (!bCreatedSADlg) {
				sadlg.Create(IDD_SCREENANNOTATIONS2,NULL);
				sadlg.ShowWindow(SW_SHOW);
				bCreatedSADlg = true;
			}

			int displaynum = ListManager.displayArray.GetSize();
			if (displaynum>0) {
				sadlg.CloseAllWindows(1);
				return 0;
			}

			int max = ListManager.layoutArray.GetSize();
			if (max<=0)
				return 0;

			//Get Current selected
			int cursel = sadlg.GetLayoutListSelection();
			iCurrentLayout = (cursel < 0) ? 0 : cursel;
			if ((iCurrentLayout < 0) || (iCurrentLayout >= max)) {
				iCurrentLayout = 0;
			}

			sadlg.InstantiateLayout(iCurrentLayout,1);
		}
		break;
	case HOTKEY_ZOOM: // FIXME: make yet another constant
		if (_zoomWhen == 0)
		{
			if (_zoom <= 1.) // noone needs zoom < 1?? safe for float comparison though
				VERIFY(::GetCursorPos(&_zoomedAt));
			_zoomWhen = ::GetTickCount();
		}
		_zoomDirection *= -1;
		break;
	case HOTKEY_AUTOPAN_SHOW_HIDE_KEY:
		
		OnOptionsAutopan();
		showmessage = true;
		break;
	}

	return 1;
}

void CRecorderView::OnUpdateOptionsAudiooptionsAudiovideosynchronization(CCmdUI *pCmdUI)
{
	// enable if audio or video devices
	BOOL bEnable = ((0 < waveInGetNumDevs()) || (0 < waveOutGetNumDevs()));
	pCmdUI->Enable(bEnable);
}

void CRecorderView::OnOptionsSynchronization()
{
	//if ((waveInGetNumDevs() == 0) || (waveOutGetNumDevs() == 0)) {
	//	MessageOut(m_hWnd,IDS_STRING_NOINPUT3,IDS_STRING_NOTE,MB_OK | MB_ICONEXCLAMATION);
	//	return;
	//}

	CSyncDlg synDlg(cVideoOpts.m_iShiftType, cVideoOpts.m_iTimeShift, this);
	if (IDOK == synDlg.DoModal()) {
		cVideoOpts.m_iTimeShift = synDlg.m_iTimeShift;
		cVideoOpts.m_iShiftType = synDlg.m_iShiftType;
	}
}

void CRecorderView::OnToolsSwfproducer()
{
	CString AppDir = GetProgPath();
	CString launchPath;
	CString exefileName("\\Producer.exe ");
	launchPath = AppDir + exefileName;
	if (WinExec(launchPath, SW_SHOW) < HINSTANCE_ERROR) {
		//MessageBox("Error launching SWF Producer!","Note",MB_OK | MB_ICONEXCLAMATION);
		MessageOut(m_hWnd, IDS_ERRPPRODUCER, IDS_STRING_NOTE, MB_OK | MB_ICONEXCLAMATION);
	}
}

void CRecorderView::OnOptionsSwfLaunchhtml()
{
	cProducerOpts.m_bLaunchHTMLPlayer = !cProducerOpts.m_bLaunchHTMLPlayer;
}

void CRecorderView::OnOptionsSwfDeleteavifile()
{
	cProducerOpts.m_bDeleteAVIAfterUse = !cProducerOpts.m_bDeleteAVIAfterUse;
}

void CRecorderView::OnOptionsSwfDisplayparameters()
{
	cProducerOpts.m_bLaunchPropPrompt = !cProducerOpts.m_bLaunchPropPrompt;
}

void CRecorderView::OnUpdateOptionsSwfLaunchhtml(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(cProducerOpts.m_bLaunchHTMLPlayer);
}

void CRecorderView::OnUpdateOptionsSwfDisplayparameters(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(cProducerOpts.m_bLaunchPropPrompt);
}

void CRecorderView::OnUpdateOptionsSwfDeleteavifile(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(cProducerOpts.m_bDeleteAVIAfterUse);
}

void CRecorderView::OnAVISWFMP4()
{
	if(cProgramOpts.m_iRecordingMode < ModeMP4)
		cProgramOpts.m_iRecordingMode++;
	else
		cProgramOpts.m_iRecordingMode = ModeAVI;
	Invalidate();
}

BOOL CRecorderView::OnEraseBkgnd(CDC* pDC)
{
	CMainFrame * pFrame = dynamic_cast<CMainFrame *>(AfxGetMainWnd());
	if (!pFrame) {
		return CView::OnEraseBkgnd(pDC);
	}
	CDC dcBits;
	VERIFY(dcBits.CreateCompatibleDC(pDC));
	CBitmap & bitmapLogo = pFrame->Logo();
	BITMAP bitmap;
	bitmapLogo.GetBitmap(&bitmap);

	CRect rectClient;
	GetClientRect(&rectClient);
	CBitmap *pOldBitmap = dcBits.SelectObject(&bitmapLogo);
	//pDC->BitBlt(0, 0, bitmap.bmWidth, bitmap.bmHeight, &dcBits, 0, 0, SRCCOPY);
	pDC->StretchBlt(0, 0, rectClient.Width(), rectClient.Height(), &dcBits, 0, 0, bitmap.bmWidth, bitmap.bmHeight, SRCCOPY);
	dcBits.SelectObject(pOldBitmap);

	return TRUE;
}

void CRecorderView::OnOptionsNamingAutodate()
{
	// Toggle between NamingAsk and AutoUpdate
		CFolderDialog cfg(cProgramOpts.m_strDefaultOutDir);
	if (IDOK == cfg.DoModal()) {
		cProgramOpts.m_strDefaultOutDir = cfg.GetPathName();
	}
	cProgramOpts.m_bAutoNaming = true;
}

void CRecorderView::OnUpdateOptionsNamingAutodate(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(cProgramOpts.m_bAutoNaming);
}

///////////////////////////
// Option, user shall define filename
///////////////////////////
void CRecorderView::OnOptionsNamingAsk()
{
	// Toggle between NamingAsk and AutoUpdate
	cProgramOpts.m_bAutoNaming = false;
}

void CRecorderView::OnUpdateOptionsNamingAsk(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(!cProgramOpts.m_bAutoNaming);
}
void CRecorderView::OnOptionsProgramoptionsPresettime()
{
	CPresetTimeDlg prestDlg;
	prestDlg.DoModal();
}

//Multilanguage

void CRecorderView::OnUpdateOptionsLanguageEnglish(CCmdUI* pCmdUI)
{
	CRecorderApp * pApp = static_cast<CRecorderApp*>(AfxGetApp());
	pCmdUI->SetCheck(pApp->LanguageID() == 9);
}

void CRecorderView::OnUpdateOptionsLanguageGerman(CCmdUI* pCmdUI)
{
	CRecorderApp * pApp = static_cast<CRecorderApp*>(AfxGetApp());
	pCmdUI->SetCheck(pApp->LanguageID() == 7);
}

void CRecorderView::OnUpdateOptionsLanguageFilipino(CCmdUI* pCmdUI)
{
	CRecorderApp * pApp = static_cast<CRecorderApp*>(AfxGetApp());
	pCmdUI->SetCheck(pApp->LanguageID() == 2);
}

void CRecorderView::OnOptionsLanguageEnglish()
{
	CRecorderApp * pApp = static_cast<CRecorderApp*>(AfxGetApp());
	pApp->LanguageID(9);
	//AfxGetApp()->WriteProfileInt(SEC_SETTINGS, ENT_LANGID, 9);
	AfxMessageBox(IDS_RESTARTAPP);
}

void CRecorderView::OnOptionsLanguageFilipino()
{
	CRecorderApp * pApp = static_cast<CRecorderApp*>(AfxGetApp());
	pApp->LanguageID(2);
	//AfxGetApp()->WriteProfileInt(SEC_SETTINGS, ENT_LANGID, 9);
	AfxMessageBox(IDS_RESTARTAPP);

}
void CRecorderView::OnOptionsLanguageGerman()
{
	CRecorderApp * pApp = static_cast<CRecorderApp*>(AfxGetApp());
	pApp->LanguageID(7);
	//AfxGetApp()->WriteProfileInt(SEC_SETTINGS, ENT_LANGID, 7);
	AfxMessageBox( IDS_RESTARTAPP);
}

void CRecorderView::OnRegionWindow()
{
	cRegionOpts.m_iMouseCaptureMode = CAPTURE_WINDOW;
}

void CRecorderView::OnUpdateRegionWindow(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(cRegionOpts.isCaptureMode(CAPTURE_WINDOW));
}

void CRecorderView::OnCaptureChanged(CWnd *pWnd)
{
	CPoint ptMouse;
	VERIFY(GetCursorPos(&ptMouse));
	if (cRegionOpts.isCaptureMode(CAPTURE_WINDOW))
	{
		CWnd* pWnd = WindowFromPoint(ptMouse);
		if (pWnd)
		{
			//tell windows we don't need capture change events anymore
			ReleaseCapture();

			//store the windows rect into the rect used for capturing
			::GetWindowRect(pWnd->m_hWnd, &rcUse);

			//close the window to show user selection was successful
			//post message to start recording
			if(pWnd->m_hWnd != m_basicMsg->m_hWnd)
				::PostMessage (hWndGlobal,WM_USER_RECORDSTART,0,(LPARAM) 0);
		}
	}
	else if (cRegionOpts.isCaptureMode(CAPTURE_FULLSCREEN))
	{
		HMONITOR hMonitor = NULL;
		MONITORINFO mi;

		// get the nearest monitor to the mouse point
		hMonitor = MonitorFromPoint(ptMouse, MONITOR_DEFAULTTONEAREST);
		mi.cbSize = sizeof(mi);
		GetMonitorInfo(hMonitor, &mi);

		//set the rectangle used for recording to the monitor's
		CopyRect(rcUse, &mi.rcMonitor);

		//tell windows we don't need capture change events anymore
		ReleaseCapture();

		//post message to start recording
		if(pWnd->m_hWnd != m_basicMsg->m_hWnd)
			::PostMessage (hWndGlobal,WM_USER_RECORDSTART,0,(LPARAM) 0);
	}
	CView::OnCaptureChanged(pWnd);
}

void CRecorderView::OnAnnotationAddsystemtimestamp()
{
	// Why do we use a .not. here? Because we clicked the button and need to toggle checkbox now
	cTimestampOpts.m_bAnnotation = !cTimestampOpts.m_bAnnotation;
}

void CRecorderView::OnUpdateAnnotationAddsystemtimestamp(CCmdUI *pCmdUI)
{
	// Show current selection on screen
	pCmdUI->SetCheck(cTimestampOpts.m_bAnnotation);
}


void CRecorderView::OnAnnotationAddXNote()
{
	// Why do we use a .not. here? Because we clicked the button and need to toggle checkbox now
	cXNoteOpts.m_bAnnotation = !cXNoteOpts.m_bAnnotation;
	Invalidate();
}

void CRecorderView::OnUpdateAnnotationAddXNote(CCmdUI *pCmdUI)
{
	// Show current selection on screen
	pCmdUI->SetCheck(cXNoteOpts.m_bAnnotation);
	Invalidate();
}
//////////

void CRecorderView::OnCameraDelayInMilliSec()
{
	//TRACE ("## CRecorderView::OnCameraDelayInMilliSec\n");
	// Nothing is actual changed here.
}

void CRecorderView::OnUpdateCameraDelayInMilliSec(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(cXNoteOpts.m_ulXnoteCameraDelayInMilliSec );
}
//////////////

void CRecorderView::OnRecordDurationLimitInMilliSec()
{
	//TRACE ("## CRecorderView::OnRecordDurationLimitInMilliSec\n");
	// Nothing is actual changed here.
}

void CRecorderView::OnUpdateRecordDurationLimitInMilliSec(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(cXNoteOpts.m_ulXnoteRecordDurationLimitInMilliSec );
}

//////////////

// Toggle mode
void CRecorderView::OnXnoteRecordDurationLimitMode()
{
	//TRACE ("## CRecorderView::OnXnoteRecordDurationLimitMode  SWITCH States\n");

#ifndef CAMSTUDIO4XNOTE
	// For the regular Camstudio Nick asked not to show to much info 
	// To prevent that normal users might get puzzeled too much about why Xnote info string is updated we quite here.
	if ( !cXNoteOpts.m_bXnoteRemoteControlMode ) {
		// To be sure that all info on screen will be updated
		Invalidate();
		return;
	}
#endif


	// Toggle, settings for the running program are changed
	cXNoteOpts.m_bXnoteRecordDurationLimitMode = !cXNoteOpts.m_bXnoteRecordDurationLimitMode;
	
	// If we set bXNoteSnapRecordingState to FALSE the autopause check will be ignored for the current 
	// recording but only if recording is activated by a xnote start message.
	bXNoteSnapRecordingState = cXNoteOpts.m_bXnoteRecordDurationLimitMode;

	// If bXNoteSnapRecordingState is set to TRUE here an applicable reference time for autopause is required. 
	// (Otherwise recording will stop immidiatelly).
	if ( bXNoteSnapRecordingState ) {
		ulXNoteLastSnapTime = GetTickCount();
	}

	// Prepare info to show.
	CString csXnoteAutoPauseMsg;
	if ( cXNoteOpts.m_bXnoteRecordDurationLimitMode ) {
	csXnoteAutoPauseMsg.Format("Xnote Limited recording mode : On, duration %d ms",  
		(cXNoteOpts.m_bXnoteRecordDurationLimitMode) ? cXNoteOpts.m_ulXnoteRecordDurationLimitInMilliSec : 0 );
	} else {
		csXnoteAutoPauseMsg.SetString( _T("Xnote Limited recording mode : Off") );
	}

	CStatusBar* pStatus = (CStatusBar*) AfxGetApp()->m_pMainWnd->GetDescendantWindow(AFX_IDW_STATUS_BAR);
	pStatus->SetPaneText(0, _T( csXnoteAutoPauseMsg.GetString() ) );

	//Set Title Bar
	CMainFrame * pFrame = dynamic_cast<CMainFrame *>(AfxGetMainWnd());
	pFrame->SetTitle(_T( csXnoteAutoPauseMsg.GetString() ));

	// To be sure that all info on screen will be updated
	Invalidate();
}


void CRecorderView::OnAnnotationAddcaption()
{
	cCaptionOpts.m_bAnnotation = !cCaptionOpts.m_bAnnotation;
}

void CRecorderView::OnUpdateAnnotationAddcaption(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(cCaptionOpts.m_bAnnotation);
}

void CRecorderView::OnAnnotationAddwatermark()
{
	cWatermarkOpts.m_bAnnotation = !cWatermarkOpts.m_bAnnotation;
}

void CRecorderView::OnUpdateAnnotationAddwatermark(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(cWatermarkOpts.m_bAnnotation);
}

void CRecorderView::OnEffectsOptions()
{
	//TRACE("## CRecorderView::OnEffectsOptions() \n");
	CAnnotationEffectsOptionsDlg dlg(this);
	
	dlg.m_timestamp = cTimestampOpts.m_taTimestamp;

	dlg.m_xnote = cXNoteOpts.m_taXNote;  

	/*
	* The assigned var m_bXnoteDisplayCameraDelayMode, m_bXnoteDisplayCameraDelayDirection  and m_bXnoteRecordDurationLimitMode are booleans and must be assigned to a 
	* Checkbox (Button) element in the dialog. When dialog is closed (OK) checkbox state must be assigned to the bool vars again
	* This all will be done in CAnnotationEffectsOptionsDlg::DoDataExchange and CAnnotationEffectsOptionsDlg::OnBnClickedOk() 
	*/
	dlg.m_bXnoteRemoteControlMode      = cXNoteOpts.m_bXnoteRemoteControlMode; 
	dlg.m_bXnoteDisplayCameraDelayMode = cXNoteOpts.m_bXnoteDisplayCameraDelayMode; 
	dlg.m_bXnoteDisplayCameraDelayDirection = cXNoteOpts.m_bXnoteDisplayCameraDelayDirection; 
	dlg.m_ulXnoteCameraDelayInMilliSec = cXNoteOpts.m_ulXnoteCameraDelayInMilliSec; 

	dlg.m_bXnoteRecordDurationLimitMode = cXNoteOpts.m_bXnoteRecordDurationLimitMode; 
	dlg.m_ulXnoteRecordDurationLimitInMilliSec = cXNoteOpts.m_ulXnoteRecordDurationLimitInMilliSec; 

	dlg.m_caption = cCaptionOpts.m_taCaption;
	dlg.m_image = cWatermarkOpts.m_iaWatermark;

	if (IDOK == dlg.DoModal()){
		// timestamp
		cTimestampOpts.m_taTimestamp = dlg.m_timestamp;

		// xnote stopwatch
		cXNoteOpts.m_taXNote = dlg.m_xnote;
		cXNoteOpts.m_bXnoteRemoteControlMode      =  dlg.m_bXnoteRemoteControlMode;
		cXNoteOpts.m_bXnoteDisplayCameraDelayMode =  dlg.m_bXnoteDisplayCameraDelayMode;
		cXNoteOpts.m_bXnoteDisplayCameraDelayDirection =  dlg.m_bXnoteDisplayCameraDelayDirection;
		cXNoteOpts.m_ulXnoteCameraDelayInMilliSec = dlg.m_ulXnoteCameraDelayInMilliSec;
		
		cXNoteOpts.m_bXnoteRecordDurationLimitMode = dlg.m_bXnoteRecordDurationLimitMode;
		cXNoteOpts.m_ulXnoteRecordDurationLimitInMilliSec = dlg.m_ulXnoteRecordDurationLimitInMilliSec;

		//TRACE( "## CRecorderView::OnEffectsOptions  dlg.m_bXnoteRecordDurationLimitMode       =[%d]\n", dlg.m_bXnoteRecordDurationLimitMode); 
		//TRACE( "## CRecorderView::OnEffectsOptions  dlg.m_ulXnoteRecordDurationLimitInMilliSec=[%lu]\n", dlg.m_ulXnoteRecordDurationLimitInMilliSec); 

		// Caption
		cCaptionOpts.m_taCaption = dlg.m_caption;

		// Watermark
		cWatermarkOpts.m_iaWatermark = dlg.m_image;
	}
}

void CRecorderView::OnHelpCamstudioblog()
{
	//Openlink("http://www.camstudio.org/blog");
}

void CRecorderView::OnBnClickedButtonlink()
{
	//Openlink("http://www.camstudio.org/blog");
}

void CRecorderView::DisplayRecordingStatistics(CDC & srcDC)
{
	//TRACE("CRecorderView::DisplayRecordingStatistics\n");

	CFont fontANSI;
	fontANSI.CreateStockObject(ANSI_VAR_FONT);
	CFont *pOldFont = srcDC.SelectObject(&fontANSI);

	COLORREF rectcolor = (nColors <= 8) ? RGB(255, 255, 255) : RGB(225, 225, 225);
	COLORREF textcolor = (nColors <= 8) ? RGB(0, 0, 128) : RGB(0, 0, 100);
	textcolor = RGB(0, 144, 0);

	CBrush brushSolid;
	brushSolid.CreateSolidBrush(rectcolor);
	CBrush * pOldBrush = srcDC.SelectObject(&brushSolid);

	COLORREF oldTextColor = srcDC.SetTextColor(textcolor);
	COLORREF oldBkColor = srcDC.SetBkColor(rectcolor);
	int iOldBkMode = srcDC.SetBkMode(TRANSPARENT);

	CRect rectText;
	GetClientRect(&rectText);
	CSize sizeExtent = (CSize)0;

	CString csMsg;
	int xoffset = 16;
	int yoffset = 10;
	int iLines = 9;				// Number of lines of information to display
	int iLineSpacing = 6;		// Distance between two lines
	int iStartPosY = rectText.bottom;

	//////////////////////////////////
	// Prepare information lines
	//////////////////////////////////

	// First line: Start recording 
	csMsg.Format("Start recording : %s",  cVideoOpts.m_cStartRecordingString.GetString() );
	sizeExtent = srcDC.GetTextExtent(csMsg);
	iStartPosY -= (iLines * (sizeExtent.cy + iLineSpacing));
	yoffset = iStartPosY;
	rectText.top = yoffset - 2;
	rectText.bottom = yoffset + sizeExtent.cy + 4;
//	srcDC.Rectangle(&rectText);						// Do we want to draw a fancy border around text?
	srcDC.TextOut(xoffset, yoffset, csMsg);

	// Second line: Limited recording 
	csMsg.SetString ( _T( "Limited recording : " ) );
	if ( cXNoteOpts.m_bXnoteRecordDurationLimitMode ) {
		csMsg.AppendFormat("On, %lu ms.",  cXNoteOpts.m_ulXnoteRecordDurationLimitInMilliSec );
	} else {
		csMsg.Append ("Off." );
	}
	sizeExtent = srcDC.GetTextExtent(csMsg);
	yoffset += sizeExtent.cy + iLineSpacing;
	rectText.top = yoffset - 2;
	rectText.bottom = yoffset + sizeExtent.cy + 4;
	srcDC.TextOut(xoffset, yoffset, csMsg);

	// Line: Current frame 
	csMsg.Format("Current Frame : %d",  nCurrFrame);
	sizeExtent = srcDC.GetTextExtent(csMsg);
	yoffset += sizeExtent.cy + iLineSpacing;
	rectText.top = yoffset - 2;
	rectText.bottom = yoffset + sizeExtent.cy + 4;
	srcDC.TextOut(xoffset, yoffset, csMsg);

	// Line: Current file sizing
	const unsigned long MEGABYTE = (1024UL * 1024UL);
	double dMegaBtyes = nTotalBytesWrittenSoFar;
	dMegaBtyes /= MEGABYTE;

	csMsg.Format("Current File Size : %.2f Mb", dMegaBtyes);
	sizeExtent = srcDC.GetTextExtent(csMsg);
	yoffset += sizeExtent.cy + iLineSpacing;
	rectText.top = yoffset - 2;
	rectText.bottom = yoffset + sizeExtent.cy + 4;
	srcDC.TextOut(xoffset, yoffset, csMsg);

	// Line : Input rate
	csMsg.Format("Actual Input Rate : %.2f fps",  fActualRate);
	sizeExtent = srcDC.GetTextExtent(csMsg);
	yoffset += sizeExtent.cy + iLineSpacing;
	rectText.top = yoffset - 2;
	rectText.bottom = yoffset + sizeExtent.cy + 4;
	srcDC.TextOut(xoffset, yoffset, csMsg);

	// Line : Elapsed time
	//csMsg.Format("Time Elapsed : %.2f sec",  fTimeLength);
	csMsg = "Time Elapsed : " + sTimeLength;
	sizeExtent = srcDC.GetTextExtent(csMsg);
	yoffset += sizeExtent.cy + iLineSpacing;
	rectText.top = yoffset - 2;
	rectText.bottom = yoffset + sizeExtent.cy + 4;
	srcDC.TextOut(xoffset, yoffset, csMsg);

	// Line : Colors info
	csMsg.Format("Number of Colors : %d iBits",  nColors);
	sizeExtent = srcDC.GetTextExtent(csMsg);
	yoffset += sizeExtent.cy + iLineSpacing;
	rectText.top = yoffset - 2;
	rectText.bottom = yoffset + sizeExtent.cy + 4;
	srcDC.TextOut(xoffset, yoffset, csMsg);

	// Line : Codex
	csMsg.Format("Codec : %s", LPCTSTR(strCodec));
	sizeExtent = srcDC.GetTextExtent(csMsg);
	yoffset += sizeExtent.cy + iLineSpacing;
	rectText.top = yoffset - 2;
	rectText.bottom = yoffset + sizeExtent.cy + 4;
	srcDC.TextOut(xoffset, yoffset, csMsg);

	// Line 9 : Dimension,sizing
	csMsg.Format("Dimension : %u X %d",  GetDocument()->FrameWidth(), GetDocument()->FrameHeight());
	sizeExtent = srcDC.GetTextExtent(csMsg);
	yoffset += sizeExtent.cy + iLineSpacing;
	rectText.top = yoffset - 2;
	rectText.bottom = yoffset + sizeExtent.cy + 4;
	srcDC.TextOut(xoffset, yoffset, csMsg);

	//////////////////////////////////
	// Print information lines
	//////////////////////////////////
	srcDC.SelectObject(pOldFont);
	srcDC.SelectObject(pOldBrush);
	srcDC.SetTextColor(oldTextColor);
	srcDC.SetBkColor(oldBkColor);
	srcDC.SetBkMode(iOldBkMode);
}

void CRecorderView::DisplayBackground(CDC & srcDC)
{
	//TRACE("CRecorderView::DisplayBackground\n");
	//Ver 1.1
	if (8 <= nColors) {
		CRect rectClient;
		GetClientRect(&rectClient);
		CDC dcBits;
		dcBits.CreateCompatibleDC(&srcDC);
		CBitmap bitmapLogo;
		bitmapLogo.Attach(hLogoBM);

		BITMAP bitmap;
		bitmapLogo.GetBitmap(&bitmap);
		CBitmap *pOldBitmap = dcBits.SelectObject(&bitmapLogo);

		srcDC.StretchBlt(0, 0, rectClient.Width(), rectClient.Height(), &dcBits, 0, 0, bitmap.bmWidth, bitmap.bmHeight, SRCCOPY);
		dcBits.SelectObject(pOldBitmap);

		bitmapLogo.Detach();
	}
}

void CRecorderView::DisplayRecordingMsg(CDC & srcDC)
{
	//TRACE("CRecorderView::DisplayRecordingMsg\n");

	CPen penSolid;
	penSolid.CreatePen(PS_SOLID, 1, RGB(225, 225, 225));
	CPen *pOldPen = srcDC.SelectObject(&penSolid);

	CBrush brushSolid;
	brushSolid.CreateSolidBrush(RGB(0, 0, 0));
	CBrush * pOldBrush = srcDC.SelectObject(&brushSolid);

	CFont fontANSI;
	fontANSI.CreateStockObject(ANSI_VAR_FONT);
	CFont *pOldFont = srcDC.SelectObject(&fontANSI);

	COLORREF oldTextColor = srcDC.SetTextColor(RGB(225, 225, 225));
	COLORREF oldBkColor = srcDC.SetBkColor(RGB(0, 0, 0));

	CString msgRecMode;
	switch(cProgramOpts.m_iRecordingMode)
	{
	case ModeAVI:
		msgRecMode.LoadString(IDS_RECAVI);
		break;
	case ModeFlash:
		msgRecMode.LoadString(IDS_RECSWF);
		break;
	case ModeMP4:
		msgRecMode.LoadString(IDS_RECMP4);
		break;
	}
	//msgRecMode.LoadString((cProgramOpts.m_iRecordingMode == ModeAVI) ? IDS_RECAVI : IDS_RECSWF);
	CSize sizeExtent = srcDC.GetTextExtent(msgRecMode);

	CRect rectClient;
	GetClientRect(&rectClient);
	int xoffset = 12;
	int yoffset = 6;
	int xmove = rectClient.Width() - sizeExtent.cx - xoffset;
	int ymove = yoffset;

	CRect rectMode(xmove, ymove, xmove + sizeExtent.cx, ymove + sizeExtent.cy);
	srcDC.Rectangle(&rectMode);
	srcDC.Rectangle(rectMode.left - 3, rectMode.top - 3, rectMode.right + 3, rectMode.bottom + 3);
	srcDC.TextOut(rectMode.left, rectMode.top,  msgRecMode);

	//TRACE("## CRecorderView::DisplayRecordingMsg  CAMSTUDIO4XNOTE Effect:[%d]  Option:[%d]\n", cXNoteOpts.m_bAnnotation , cXNoteOpts.m_bXnoteRecordDurationLimitMode);
	// Show informational text only if Xnote stopwatch is also activated as an effect option  XNOTEANNOTATION = On

//	if (cXNoteOpts.m_bAnnotation) {
		CString msgXnoteMode;

#ifndef CAMSTUDIO4XNOTE
		if ( cXNoteOpts.m_bXnoteRemoteControlMode ) {
#endif
			if ( cXNoteOpts.m_bXnoteRecordDurationLimitMode ) {
				// IDS_XNOTE_RECORDING_DURATION_ON "Xnote:<X> Stamp:<S> Rec:Limited,"
				msgXnoteMode.LoadString( IDS_XNOTE_RECORDING_DURATION_ON );
				msgXnoteMode.AppendFormat( " %lu ms.", cXNoteOpts.m_ulXnoteRecordDurationLimitInMilliSec );
			} else {
				// IDS_XNOTE_RECORDING_DURATION_OFF "Xnote:<X> Stamp:<S> Rec:Continuously."
				msgXnoteMode.LoadString( IDS_XNOTE_RECORDING_DURATION_OFF );
			}
			msgXnoteMode.Replace("<X>", _T(cXNoteOpts.m_bXnoteRemoteControlMode ? "On" : "Off" ) );
			msgXnoteMode.Replace("<S>", _T(cXNoteOpts.m_bAnnotation ? "On" : "Off" ) );
			CSize sizeXnoteExtent = srcDC.GetTextExtent(msgXnoteMode);

			CRect rectClientXnoteMsg;
			GetClientRect(&rectClientXnoteMsg);

			CRect rectXnoteMode(xoffset, yoffset, xoffset + sizeXnoteExtent.cx, yoffset + sizeXnoteExtent.cy);
			srcDC.Rectangle(&rectXnoteMode);
			srcDC.Rectangle(rectXnoteMode.left - 3, rectXnoteMode.top - 3, rectXnoteMode.right + 3, rectXnoteMode.bottom + 3);
			srcDC.TextOut(rectXnoteMode.left, rectXnoteMode.top,  msgXnoteMode);

#ifndef CAMSTUDIO4XNOTE
		} else {
			msgXnoteMode.SetString("");
		}
#endif

//	}

	srcDC.SelectObject(pOldPen);
	srcDC.SelectObject(pOldBrush);
	srcDC.SelectObject(pOldFont);
	srcDC.SetTextColor(oldTextColor);
	srcDC.SetBkColor(oldBkColor);
}


/////////////////////////
// TODO janhgm
// If recording is on pause it would be nice if we can recover a few records from the bin.
// Gives us a better overview about the conditions when we (or a device) decided that we should start recording again.
/////////////////////////
bool CRecorderView::captureScreenFrame(const CRect& rectView, bool bDisableRect)
{
	//TRACE("CRecorderView::captureScreenFrame\n");
	//CRect rectView(left, top, left + width, top + height);

	int width = (int)rectView.Width()/_zoom; // width of being captured screen stuff
	int height = (int)rectView.Height()/_zoom; // width of being captured screen stuff
	CRect zr = _zoom==1. ? rectView : CRect(CPoint(
		min(max(rectView.left, _zoomedAt.x - width/2), rectView.right - width),
		min(max(rectView.top, _zoomedAt.y - height/2), rectView.bottom - height)),
		CSize(width, height));

	// FIXME: can be move into UpdateZoom() function
	if (_zoomWhen) // should zoom in/out
	{
		DWORD threshold = 1000; // 1 sec
		DWORD now = ::GetTickCount();
		DWORD ago = now - _zoomWhen;
		if (ago>threshold)
		{
			_zoomWhen = 0;
		} else
		{	// FIXME: change zoom from current state in case use changed mind zooming
			_zoom = 1.5 - cos(ago*3.141592/threshold)/1.9*_zoomDirection;
			if (_zoom>2.) _zoom = 2.;
			if (_zoom<1.) _zoom = 1.;
		}
	}

	CPoint pt;
	VERIFY(::GetCursorPos(&pt));
	double dist = sqrt((double)(pt.x-_zoomedAt.x)*(pt.x-_zoomedAt.x) + (pt.y-_zoomedAt.y)*(pt.y-_zoomedAt.y));

	if (cProgramOpts.m_bAutoPan)
	{	// always cursor centered
		VERIFY(::GetCursorPos(&_zoomedAt));
	} else {
		if (abs(pt.x-_zoomedAt.x) > .4*width)
			_zoomedAt.x += (pt.x - _zoomedAt.x)*cProgramOpts.m_iMaxPan/width;
		if (abs(pt.y-_zoomedAt.y) > .4*height)
			_zoomedAt.y += (pt.y - _zoomedAt.y)*cProgramOpts.m_iMaxPan/height;
	}

	// if flashing rect
	if (!bDisableRect && (cProgramOpts.m_bFlashingRect)) {
		//if (cProgramOpts.m_bAutoPan) {
		//	m_FlashingWnd.SetUpRegion(rectView, 1);
		//}
		m_FlashingWnd.SetUpRegion(zr, cProgramOpts.m_bAutoPan);
		m_FlashingWnd.DrawFlashingRect(cProgramOpts.m_bAutoPan);
		//m_FlashingWnd.PostMessage(CFlashingWnd::WM_FLASH_WINDOW, cProgramOpts.m_bAutoPan, 1L);
	}

	m_cCamera.SetView(rectView); // this set m_rectView & m_rectFrame
	// severe changes where made to CaptureFrame
	bool bResult = m_cCamera.CaptureFrame(zr);

	// if flashing rect
	if (!bDisableRect && (cProgramOpts.m_bFlashingRect)) {
		m_FlashingWnd.DrawFlashingRect(cProgramOpts.m_bAutoPan, false);
		//m_FlashingWnd.PostMessage(CFlashingWnd::WM_FLASH_WINDOW, cProgramOpts.m_bAutoPan, 0L);
	}
	return bResult;
}

UINT CRecorderView::RecordThread(LPVOID pParam)
{
	CRecorderView *pcRecorderView = reinterpret_cast<CRecorderView*>(pParam);
	if (!pcRecorderView)
		return 0;

	return pcRecorderView->RecordVideo();
}

UINT CRecorderView::RecordVideo()
{
	//TRACE("CRecorderView::RecordAVIThread\n");

	// Test the validity of writing to the file
	// Make sure the file to be created is currently not used by another application

	// Attempt to write to temp.avi file while file still exist results in a crash when old temp.avi is still in use or locked (or not freed).
	// Can be prevented if Camstudio always write to an unique file. We decided to use the standard date-timestamp (~temp-ccyymmdd-hhuu-ss.avi) to make the file unique
	// Another benefit of using this filename is that we don't always have to copy from temp to realname once recording is finished and we always new when a recording was created.

	CString csTempFolder(GetTempFolder(cProgramOpts.m_iTempPathAccess, cProgramOpts.m_strSpecifiedDir));	
	// Location where we are creating our files
	//TRACE("## CRecorderView::RecordVideo()  cProgramOpts.m_strSpecifiedDir=[%s]\n", cProgramOpts.m_strSpecifiedDir );
	//TRACE("## CRecorderView::RecordVideo()  csTempFolder=[%s]\n", csTempFolder );

	// Define a date-time tag "ccyymmdd_uumm_ss" to add to the temp.avi file.
	// (New recordings can start just after previously recording ended.)
	time_t osBinaryTime; // C run-time time (defined in <time.h>)
	time(&osBinaryTime);
	CTime ctime(osBinaryTime);

	int day = ctime.GetDay();
	int month = ctime.GetMonth();
	int year = ctime.GetYear();
	int hour = ctime.GetHour();
	int minutes = ctime.GetMinute();
	int second = ctime.GetSecond();
	// Create timestamp tag
	CString csStartTime = "";
	csStartTime.Format("%04d%02d%02d_%02d%02d_%02d",year, month, day, hour, minutes, second); // 20100528, changed dateformating to yyyymmdd_hhmm_ss
	// We will keep this tag info that tell us when the recording started for later usage. 
	cVideoOpts.m_cStartRecordingString.SetString(csStartTime);

	strTempVideoAviFilePath.Format("%s\\%s-%s.%s", (LPCSTR)csTempFolder, TEMPFILETAGINDICATOR, (LPCSTR)csStartTime, "avi" );
	// strTempXnoteLogFilePath.Format("%s\\%s-%s.%s", (LPCSTR)csTempFolder, TEMPFILETAGINDICATOR, (LPCSTR)csStartTime, "txt" );

	//TRACE("## CRecorderView::RecordAVIThread First  Temp.Avi file=[%s]\n", strTempVideoAviFilePath.GetString()  );

	srand((unsigned)time(NULL));
	bool fileverified = false;
	while (!fileverified)
	{
		OFSTRUCT ofstruct;
		HFILE hFile = OpenFile(strTempVideoAviFilePath, &ofstruct, OF_SHARE_EXCLUSIVE | OF_WRITE | OF_CREATE);
		fileverified = (hFile != HFILE_ERROR);
		if (fileverified) {
			CloseHandle((HANDLE)hFile);
			DeleteFile(strTempVideoAviFilePath);
		} else {
			strTempVideoAviFilePath.Format("%s\\%s-%s-%d.%s", (LPCSTR)csTempFolder, TEMPFILETAGINDICATOR, (LPCSTR)csStartTime, rand(), "avi" );
		}
	}
	//TRACE(_T("## CRecorderView::RecordAVIThread  Final Temp.Avi file=[%s]\n"), strTempVideoAviFilePath.GetString()  );
	//TRACE(_T("## CRecorderView::RecordVideo rcUse / T=%d, L=%d, B=%d, R=%d \n"), rcUse.top, rcUse.left, rcUse.bottom, rcUse.right );
	return RecordVideo(rcUse, cVideoOpts.m_iFramesPerSecond, strTempVideoAviFilePath) ? 0UL : 1UL;
}

/////////////////////////////////////////////////////////////////////////////
// RecordVideo
//
// The main function used in the recording of video
// Includes opening/closing avi file, initializing avi settings, capturing
// frames, applying cursor effects etc.
/////////////////////////////////////////////////////////////////////////////
//bool CRecorderView::RecordVideo(int top, int left, int width, int height, int fps, const char *szVideoFileName)
bool CRecorderView::RecordVideo(CRect rectFrame, int fps, const char *szVideoFileName)
{
	//TRACE("CRecorderView::RecordVideo (.....) \n");
	WORD wVer = HIWORD(VideoForWindowsVersion());
	if (wVer < 0x010a) {
		TRACE("CRecorderView::RecordVideo: Wrong VideoForWindowsVersion\n");
		MessageOut(NULL, IDS_STRING_VERSIONOLD , IDS_STRING_NOTE, MB_OK | MB_ICONSTOP);
		return false;
	}

	sVideoOpts SaveVideoOpts = cVideoOpts;

	// And again CS is doing a recalculation to determine width and height.
	// If sizing is determined with SetCapture() the dimensions are different as we getwith CRect(Top,Left,Bottom,Right)
	// HIER_BEN_IK
	// TRACE(_T("## CRecorderView::RecordVideo / cRegionOpts.m_iMouseCaptureMode =%d\n"), cRegionOpts.m_iMouseCaptureMode );
	switch (cRegionOpts.m_iMouseCaptureMode)
	{
	case CAPTURE_WINDOW:
	case CAPTURE_FULLSCREEN:
		if(m_basicMsg)
		{
			if(m_basicMsg->Cancelled())
			{
				bRecordState = false;
				return false;
			}
			else
				m_basicMsg->ShowWindow(SW_HIDE);
		}
		// For rects captured with SetCapture
		//TRACE(_T("## CRecorderView::RecordVideo / cRegionOpts.m_iMouseCaptureMode =%d  (Do nothing) \n"), cRegionOpts.m_iMouseCaptureMode );
		GetDocument()->FrameWidth(rectFrame.Width() );
		GetDocument()->FrameHeight(rectFrame.Height() );
		break;
	default:
		// For rects defined with Rect(top,left,bottom,right)
		//TRACE(_T("## CRecorderView::RecordVideo / cRegionOpts.m_iMouseCaptureMode =%d  (increase width anf height with one) \n"), cRegionOpts.m_iMouseCaptureMode );
		GetDocument()->FrameWidth(rectFrame.Width() );
		GetDocument()->FrameHeight(rectFrame.Height());
		break;
	}
	//TRACE(_T("## CRecorderView::RecordVideo / rectFrame / T=%d, L=%d, B=%d, R=%d / W=%d, H=%d\n"), rectFrame.top, rectFrame.left, rectFrame.bottom, rectFrame.right , rectFrame.Width(), rectFrame.Height() );
	//TRACE(_T("## CRecorderView::RecordVideo / GetDocument / W=%d, H=%d\n"), GetDocument()->FrameWidth(), GetDocument()->FrameHeight() );

	nTotalBytesWrittenSoFar = 0;

	////////////////////////////////////////////////
	// CAPTURE FIRST FRAME
	////////////////////////////////////////////////

	LPBITMAPINFOHEADER alpbi = captureScreenFrame(rectFrame) ? m_cCamera.Image() : 0;
	// TEST VALIDITY OF COMPRESSOR
	if (cVideoOpts.m_iSelectedCompressor > 0) {
		HIC hic = ::ICOpen(pCompressorInfo[cVideoOpts.m_iSelectedCompressor].fccType, pCompressorInfo[cVideoOpts.m_iSelectedCompressor].fccHandler, ICMODE_QUERY);
		if (hic) {
			int newleft = 0;
			int newtop = 0;
			int newwidth = 0;
			int newheight = 0;
			int align = 1;
			LRESULT lResult = ::ICCompressQuery(hic, alpbi, NULL);
			if (ICERR_OK != (lResult = ::ICCompressQuery(hic, alpbi, NULL))) {
				// Try adjusting width/height a little bit
				/*align = align * 2;
				if (8 < align)
				newleft = rectFrame.left;
				newtop = rectFrame.top;
				int wm = (rectFrame.Width() % align);
				if (0 < wm) {
					newwidth = rectFrame.Width() + (align - wm);
					if (maxxScreen < newwidth) {
						newwidth = rectFrame.Width() - wm;
					}
				}

				int hm = (rectFrame.Height() % align);
				if (0 < hm) {
					newheight = rectFrame.Height() + (align - hm);
					if (maxyScreen < newheight) {
						newwidth = rectFrame.Height() - hm;
					}
				}
				*/
				if(cVideoOpts.m_bRoundDown)
				{
					newwidth = (rectFrame.Width() % 2 != 0) ? rectFrame.Width() -1 : rectFrame.Width();
					newheight = (rectFrame.Height() % 2 != 0) ? rectFrame.Height() -1 : rectFrame.Height();
					CRect rectNewFrame(rectFrame.left, rectFrame.top, rectFrame.left + newwidth, rectFrame.top + newheight);

					alpbi = captureScreenFrame(rectNewFrame) ? m_cCamera.Image() : 0;
					rectFrame = rectNewFrame;
				}
			}


			lResult = ::ICCompressQuery(hic, alpbi, NULL);
			ASSERT(ICERR_OK == lResult);
			ICClose(hic);

			// if succeed with new width/height, use the new width and height
			// else if still fails == > default to MS Video 1 (MSVC)
			/*if (align == 1) {
				// Compressor has no problem with the current dimensions...so proceed
				// do nothing here
				TRACE("CRecorderView::RecordVideo : no problem with the current dimensions\n");
			} else if (align <= 8) {

				// Compressor can work if the dimensions is adjusted slightly
				rectFrame = CRect(newleft, newtop, newleft + newwidth, newtop + newheight);
				GetDocument()->FrameWidth(rectFrame.Width());
				GetDocument()->FrameHeight(rectFrame.Height());
			} else {
				cVideoOpts.m_dwCompfccHandler = ICHANDLER_MSVC;
				strCodec = CString("MS Video 1");
			}*/

		} else {
			//TRACE("CRecorderView::RecordVideo - ICOpen failed\n");
			cVideoOpts.m_dwCompfccHandler = ICHANDLER_MSVC;
			strCodec = CString("MS Video 1");
		}
	} //selected_compressor


	// IV50
	if (cVideoOpts.m_dwCompfccHandler == ICHANDLER_IV50) { //Still Can't Handle Indeo 5.04
		cVideoOpts.m_dwCompfccHandler = ICHANDLER_MSVC;
		strCodec = CString("MS Video 1");
	}

	////////////////////////////////////////////////
	// Set Up Flashing Rect
	////////////////////////////////////////////////
	if (cProgramOpts.m_bFlashingRect) {
		m_FlashingWnd.SetUpRegion(rectFrame, cProgramOpts.m_bAutoPan);
		m_FlashingWnd.ShowWindow(SW_SHOW);
	}

	////////////////////////////////////////////////
	// INIT AVI USING FIRST FRAME
	////////////////////////////////////////////////
	::AVIFileInit();

	// Open the movie file for writing....
	char szTitle[BUFSIZE];
	strcpy_s(szTitle, "AVI Movie");

	// There are a few routine in this code that use a 'goto error' routine for exception handling.
	// All var's that are checked in this error handling block must be innitialisated before to prevent warning and, worse case, abnormal endings.
	PAVIFILE 	pfile = 0;
	PAVISTREAM 	ps = NULL;
	PAVISTREAM 	psCompressed = NULL;
	
	
	HRESULT hr = ::AVIFileOpen(&pfile, szVideoFileName, OF_WRITE | OF_CREATE, NULL);
	if (hr != AVIERR_OK) {
		TRACE("CRecorderView::RecordVideo: VideoAviFileOpen error\n");
		CAVI::OnError(hr);
		goto error;
	}

	// Fill in the header for the video stream....
	// The video stream will run in 15ths of a second....
	AVISTREAMINFO strhdr;
	::ZeroMemory(&strhdr, sizeof(AVISTREAMINFO));
	strhdr.fccType                = streamtypeVIDEO;// stream type
	//strhdr.fccHandler             = dwCompfccHandler;
	strhdr.fccHandler             = 0;
	strhdr.dwScale                = 1;
	strhdr.dwRate                 = fps;
	strhdr.dwSuggestedBufferSize  = alpbi->biSizeImage;
	// rectangle for stream
	SetRect(&strhdr.rcFrame, 0, 0, (int) alpbi->biWidth, (int) alpbi->biHeight);

	// And create the stream;
	/*PAVISTREAM*/ ps = NULL;
	hr = AVIFileCreateStream(pfile, &ps, &strhdr);
	if (hr != AVIERR_OK) {
		TRACE("CRecorderView::RecordVideo: AVIFileCreateStream error\n");
		CAVI::OnError(hr);
		goto error;
	}

	AVICOMPRESSOPTIONS opts;
	AVICOMPRESSOPTIONS FAR * aopts[1] = {&opts};
	memset(&opts, 0, sizeof(opts));
	aopts[0]->fccType			= streamtypeVIDEO;
	aopts[0]->fccHandler		= cVideoOpts.m_dwCompfccHandler;
	aopts[0]->dwKeyFrameEvery	= cVideoOpts.m_iKeyFramesEvery;		// keyframe rate
	aopts[0]->dwQuality			= cVideoOpts.m_iCompQuality;        // compress quality 0-10, 000
	aopts[0]->dwBytesPerSecond	= 0;		// bytes per second
	aopts[0]->dwFlags			= AVICOMPRESSF_VALID | AVICOMPRESSF_KEYFRAMES;    // flags
	aopts[0]->lpFormat			= 0x0;                         // save format
	aopts[0]->cbFormat			= 0;
	aopts[0]->dwInterleaveEvery = 0;			// for non-video streams only

	//ver 2.26
	if (cProgramOpts.m_iRecordingMode == ModeFlash) {
		//Internally adjust codec to MSVC 100 Quality
		aopts[0]->fccHandler = ICHANDLER_MSVC;	 //msvc
		strCodec = CString("MS Video 1");
		aopts[0]->dwQuality = 10000;
	} else {
		//Ver 1.2
		//
		if ((cVideoOpts.m_dwCompfccHandler != 0) && (cVideoOpts.m_dwCompfccHandler == cVideoOpts.m_dwCompressorStateIsFor)) {
			// make a copy of the pVideoCompressParams just in case after compression,
			// this variable become messed up
			aopts[0]->lpParms = cVideoOpts.State();
			aopts[0]->cbParms = cVideoOpts.StateSize();
		}
	}

	//The 1 here indicates only 1 stream
	//if (!AVISaveOptions(NULL, 0, 1, &ps, (LPAVICOMPRESSOPTIONS *) &aopts))
	//        goto error;

	/*PAVISTREAM*/ psCompressed = NULL;
	hr = AVIMakeCompressedStream(&psCompressed, ps, &opts, NULL);
	if (AVIERR_OK != hr) {
		TRACE("CRecorderView::RecordVideo: AVIMakeCompressedStream error\n");
		CAVI::OnError(hr);
		goto error;
	}

	hr = AVIStreamSetFormat(psCompressed, 0, alpbi, alpbi->biSize + alpbi->biClrUsed * sizeof(RGBQUAD));
	if (hr != AVIERR_OK) {
		CAVI::OnError(hr);
		goto error;
	}

	alpbi = NULL;

	if (cProgramOpts.m_bAutoPan) {
		rectPanCurrent = rectFrame;
		rectPanCurrent.right--;
		rectPanCurrent.bottom--;
	}

	//////////////////////////////////////////////
	// Recording Audio
	//////////////////////////////////////////////
	if ((cAudioFormat.isInput(SPEAKERS)) || (cAudioFormat.m_bUseMCI)) {
		mciRecordOpen(hWndGlobal);
		mciSetWaveFormat();
		mciRecordStart();

		//if (iShiftType == 1)
		//{
		//	mci::mciRecordPause(strTempAudioWavFilePath);
		//	unshifted = 1;
		//}
	} else if (!cAudioFormat.isInput(NONE)) {
		InitAudioRecording();
		StartAudioRecording();
	}

	//////////////////////////////////////////////
	// Xnote or MotionDetection log file
	// Store some Camstudio info as well.
	//////////////////////////////////////////////
	//OpenStreamXnoteLogFile();

	// Document which version and release of Camstudio is used.
	char cTmp[128] = "";
	//sprintf( cTmp, "<recorder><version>%s</version><release>%s</release></recorder>\n", CURRENT_VERSION_NUMBER, CURRENT_HG_RELEASE_NUMBER );
	//WriteLineToXnoteLogFile( cTmp );

	if (cVideoOpts.m_iShiftType == AUDIOFIRST) {
		Sleep(cVideoOpts.m_iTimeShift);
	}

	bInitCapture = true;
	nCurrFrame = 0;
	nActualFrame = 0;
	dwInitialTime = ::timeGetTime();
	DWORD timeexpended = 0;
	DWORD frametime = 0;
	DWORD oldframetime = 0;
	//////////////////////////////////////////////
	// WRITING FRAMES
	//////////////////////////////////////////////
	long divx = 0L;
	long oldsec = 0L;
	while (bRecordState)
	{
		if (!bInitCapture)
		{
			timeexpended = timeGetTime() - dwInitialTime;
		} 
		else 
		{
			frametime = 0;
			timeexpended = 0;
		}
		// Autopan
		if (cProgramOpts.m_bAutoPan && (rectFrame.Width() < maxxScreen) && (rectFrame.Height() < maxyScreen))
		{
			CPoint xPoint;
			GetCursorPos(&xPoint);

			int extleft = (rectPanCurrent.Width()*1)/4 + rectPanCurrent.left;
			int extright = (rectPanCurrent.Width()*3)/4 + rectPanCurrent.left;
			int exttop = (rectPanCurrent.Height()*1)/4 + rectPanCurrent.top;
			int extbottom = (rectPanCurrent.Height()*3)/4 + rectPanCurrent.top;

			if (xPoint.x < extleft ) {
				//need to pan left
				rectPanDest.left = xPoint.x - rectFrame.Width()/2;
				rectPanDest.right = rectPanDest.left + rectFrame.Width() - 1;
				if (rectPanDest.left < 0)  {
					rectPanDest.left = 0;
					rectPanDest.right = rectPanDest.left + rectFrame.Width() - 1;
				}
			} else if (extright < xPoint.x) {
				//need to pan right
				rectPanDest.left = xPoint.x - rectFrame.Width()/2;
				rectPanDest.right = rectPanDest.left +  rectFrame.Width() - 1;
				if (maxxScreen <= rectPanDest.right) {
					rectPanDest.right = maxxScreen - 1;
					rectPanDest.left  = rectPanDest.right - rectFrame.Width() + 1;
				}
			} else {
				rectPanDest.right = rectPanCurrent.right;
				rectPanDest.left  = rectPanCurrent.left;
			}

			if (xPoint.y < exttop) { //need to pan up
				rectPanDest.top = xPoint.y - rectFrame.Height()/2;
				rectPanDest.bottom = rectPanDest.top + rectFrame.Height() - 1;
				if (rectPanDest.top < 0)  {
					rectPanDest.top = 0;
					rectPanDest.bottom = rectPanDest.top + rectFrame.Height() - 1;
				}
			} else if (extbottom < xPoint.y) { //need to pan down
				rectPanDest.top = xPoint.y - rectFrame.Height()/2;
				rectPanDest.bottom = rectPanDest.top + rectFrame.Height() - 1;
				if (maxyScreen <= rectPanDest.bottom) {
					rectPanDest.bottom = maxyScreen - 1;
					rectPanDest.top  = rectPanDest.bottom - rectFrame.Height() + 1;
				}
			} else {
				rectPanDest.top = rectPanCurrent.top;
				rectPanDest.bottom  = rectPanCurrent.bottom;
			}

			// Determine Pan Values
			int xdiff = rectPanDest.left - rectPanCurrent.left;
			int ydiff = rectPanDest.top - rectPanCurrent.top;
			if (abs(xdiff) < cProgramOpts.m_iMaxPan) {
				rectPanCurrent.left += xdiff;
			} else if (xdiff < 0) {
				rectPanCurrent.left -= cProgramOpts.m_iMaxPan;
			} else {
				rectPanCurrent.left += cProgramOpts.m_iMaxPan;
			}

			if (abs(ydiff) < cProgramOpts.m_iMaxPan) {
				rectPanCurrent.top += ydiff;
			} else  if (ydiff<0) {
				rectPanCurrent.top -= cProgramOpts.m_iMaxPan;
			} else {
				rectPanCurrent.top += cProgramOpts.m_iMaxPan;
			}

			rectPanCurrent.right = rectPanCurrent.left + rectFrame.Width() - 1;
			rectPanCurrent.bottom =  rectPanCurrent.top + rectFrame.Height() - 1;

			CRect rectPanFrame(rectPanCurrent);
			rectPanFrame.right++;
			rectPanFrame.bottom++;
			alpbi = captureScreenFrame(rectPanFrame, false)
				? m_cCamera.Image()
				: 0;
			if(showmessage)
			{
				DisplayAutopanInfo(rectPanFrame);
				showmessage = false;
			}
		} 
		else 
		{
			//ver 1.8
			//moving region

			if (m_FlashingWnd.NewRegionUsed()) 
			{
				// Happens when flashing region was moved
				// it seems unlikely will be called when setting new region explicitly in the code. good for zoom!
				//TRACE(_T("rectFrame != newRect %s\n"), (rectFrame != newRect) ? _T("TRUE") : _T("FALSE"));
				//TRACE(_T("rectFrame.Width: %d\nNewRect.Width: %d\n"), rectFrame.Width(), newRect.Width());
				TRACE(_T("rectFrame != newRect %s\n"), (rectFrame != m_FlashingWnd.Rect()) ? _T("TRUE") : _T("FALSE"));
				TRACE(_T("rectFrame.Width: %d\nNewRect.Width: %d\n"), rectFrame.Width(), m_FlashingWnd.Rect().Width());
				//rectFrame = newRect;
				rectFrame = m_FlashingWnd.Rect();
				rectFrame.bottom++;
				rectFrame.right++;
				m_FlashingWnd.NewRegionUsed(false);
				//width and height unchanged
			}
			//rectFrame = CRect(left, top, left + width, top + height);
			alpbi = captureScreenFrame(rectFrame, false) ? m_cCamera.Image() : 0;
			if(showmessage)
			{
				DisplayAutopanInfo(rectFrame);
				showmessage = false;
			}
		}

		if (!bInitCapture) 
		{
			if (1000 < cVideoOpts.m_iTimeLapse)
			{
				frametime++;
			} 
			else 
			{
				frametime = (DWORD) (((double) timeexpended /1000.0 ) * (double) (1000.0/cVideoOpts.m_iTimeLapse));
			}
		} 
		else 
		{
			bInitCapture = false;
		}
		int seconds = 0.0;
		int minutes = 0;
		int hours = 0;
		hours = timeexpended / 1000 / 60 / 60 % 60;
		minutes = timeexpended / 1000 / 60 % 60;
		seconds = timeexpended / 1000 % 60; 
		sTimeLength.Format("%d hrs %d mins %d secs", hours, minutes, seconds);
		fTimeLength = ((float) timeexpended) /((float) 1000.0);

		if (cProgramOpts.m_bRecordPreset) 
		{
			if (cProgramOpts.m_iPresetTime <= int(fTimeLength)) 
			{
				::PostMessage(hWndGlobal, WM_USER_RECORDINTERRUPTED, 0, 0);
			}

			//CString msgStr;
			//msgStr.Format("%.2f %d", fTimeLength, iPresetTime);
			//MessageBox(NULL, msgStr, "N", MB_OK);
			//or should we post messages
		}

		//if ((iShiftType == 1) && (unshifted))
		//{
		//	cc++;
		//	unsigned long thistime = timeGetTime();
		//	int diffInTime = thistime - dwInitialTime;
		//	if (diffInTime >= iTimeShift)
		//	{
		//		ErrMsg("cc %d diffInTime %d", cc-1, diffInTime);
		//		if ((iRecordAudio == 2) || (bUseMCI))
		//		{
		//			mci::mciRecordResume(strTempAudioWavFilePath);
		//			unshifted = 0;
		//		}
		//	}
		//}

	
		if ( ( frametime == 0 ) || ( oldframetime < frametime ) ) 
		{
			//ver 1.8
			//if (iShiftType == 1) {
			//	if (frametime == 0) {
			//		//Discard .. do nothing
			//	} else {
			//		//writr old frame time instead
			//		hr = AVIStreamWrite(psCompressed, // stream pointer
			//			oldframetime, // time of this frame
			//			1, // number to write
			//			(LPBYTE) alpbi +				// pointer to data
			//			alpbi->biSize +
			//			alpbi->biClrUsed * sizeof(RGBQUAD),
			//			alpbi->biSizeImage, // size of this frame
			//			//AVIIF_KEYFRAME, // flags....
			//			0, //Dependent n previous frame, not key frame
			//			NULL,
			//			NULL);
			//	}
			//} else {

			// if frametime repeats (frametime == oldframetime)
			// ...the avistreamwrite will cause an error
			LONG lSampWritten = 0L;
			LONG lBytesWritten = 0L;
			hr = ::AVIStreamWrite(psCompressed, frametime, 1, (LPBYTE) alpbi + alpbi->biSize + alpbi->biClrUsed * sizeof(RGBQUAD), alpbi->biSizeImage, 0, &lSampWritten, &lBytesWritten);
			//}
			if (hr != AVIERR_OK) 
			{
				TRACE("CRecorderView::RecordVideo: AVIStreamWrite error\n");
				CAVI::OnError(hr);
				break;
			}

			//nTotalBytesWrittenSoFar += alpbi->biSizeImage;
			nTotalBytesWrittenSoFar += lBytesWritten;
			nActualFrame++;
			nCurrFrame = frametime;
			fRate = ((float) nCurrFrame)/fTimeLength;
			fActualRate = ((float) nActualFrame)/fTimeLength;

			// Update recording stats every half a second
			divx = timeexpended / 500;
			if (divx != oldsec) 
			{
				oldsec = divx;
				::InvalidateRect(hWndGlobal, NULL, FALSE);
			}

			//free memory
			alpbi = NULL;

			oldframetime = frametime;
		} // if frametime is different

		//Version 1.1
		int haveslept = 0;
		int pausecounter = 0;
		// local variable is initialized but not referenced
		//int pauseremainder = 0;
		int pauseindicator = 1;
		DWORD timestartpause = 0;
		DWORD timeendpause	= 0;
		DWORD timedurationpause = 0;
		while (bRecordPaused) 
		{
			if (!haveslept) 
			{
				timestartpause = timeGetTime();
			}

			//Flash Pause Indicator in Title Bar
			pausecounter++;
			if ((pausecounter % 8) == 0) 
			{
				//if after every 400 milliseconds (8 * 50)
				CMainFrame * pFrame = dynamic_cast<CMainFrame *>(AfxGetMainWnd());

// TODO - Found this crash behaviour while I developed CAMSTUDIO4XNOTE but it looks like to be something that was still in previously releases. (before r245)
// TRACE("CRecorderView: if Pause selected, statement skippped, pFrame->SetTitle(_T("")), causes error\n");
				if (pauseindicator) 
				{
#define CAMSTUDIO_IGNORE_THIS
#ifndef CAMSTUDIO_IGNORE_THIS
					pFrame->SetTitle(_T(""));     // This still gives an error before r245 (when build using vs2008)...!				
#endif
					pauseindicator = 0;
				} else {
#ifndef CAMSTUDIO_IGNORE_THIS
					pFrame->SetTitle(_T("Paused"));
#endif
#undef CAMSTUDIO_IGNORE_THIS
					pauseindicator = 1;
				}
			}

			if (cAudioFormat.isInput(SPEAKERS) || cAudioFormat.m_bUseMCI) 
			{
				if (bAlreadyMCIPause == 0) 
				{
					mciRecordPause(hWndGlobal, strTempAudioWavFilePath);
					bAlreadyMCIPause = true;
				}
			}

			//do nothing.. wait
			::Sleep(50); //Sleep for 50

			haveslept = 1;
		}

		//Version 1.1
		if (haveslept) 
		{
			if (cAudioFormat.isInput(SPEAKERS) || cAudioFormat.m_bUseMCI)
			{
				if (bAlreadyMCIPause) 
				{
					bAlreadyMCIPause = false;
					mciRecordResume(hWndGlobal, strTempAudioWavFilePath);
				}
					timeendpause = timeGetTime();
					timedurationpause =  timeendpause - timestartpause;
					dwInitialTime = dwInitialTime + timedurationpause;
					continue;
			}
		}
		else 
		{
			//introduce time lapse
			//maximum lapse when bRecordState changes will be less than 100 milliseconds
			int no_iteration = cVideoOpts.m_iTimeLapse/50;
			int remainlapse = cVideoOpts.m_iTimeLapse - no_iteration * 50;
			for (int j = 0; j < no_iteration; j++) {
				::Sleep(50); //Sleep for 50 milliseconds many times
				if (!bRecordState) {
					break;
				}
			}
			if (bRecordState) 
			{
				::Sleep(remainlapse);
			}
		}
	} //for loop

error:

	// Now close the file

	if (cProgramOpts.m_bFlashingRect) {
		m_FlashingWnd.ShowWindow(SW_HIDE);
	}

	//Ver 1.2
	//
	if ((cVideoOpts.m_dwCompfccHandler == cVideoOpts.m_dwCompressorStateIsFor) && (cVideoOpts.m_dwCompfccHandler != 0)) {
		// Detach pParamsUse from AVICOMPRESSOPTIONS so AVISaveOptionsFree will not free it
		// (we will free it ourselves)
		aopts[0]->lpParms = 0;
		aopts[0]->cbParms = 0;
	}

	::AVISaveOptionsFree(1, (LPAVICOMPRESSOPTIONS FAR *) &aopts);

	//////////////////////////////////////////////
	// Recording Audio
	//////////////////////////////////////////////
	if (cAudioFormat.isInput(SPEAKERS) || cAudioFormat.m_bUseMCI) {
		GetTempAudioWavPath();
		mciRecordStop(hWndGlobal, strTempAudioWavFilePath);
		mciRecordClose();
		//restoreWave();
	} else if (!cAudioFormat.isInput(NONE)) {
		StopAudioRecording();
		ClearAudioFile();
	}

	//////////////////////////////////////////////
	// Close Camstudio, XNote or MotionDetection log file
	//////////////////////////////////////////////
	CloseStreamXnoteLogFile();		


	if (pfile) {
		::AVIFileClose(pfile);
	}

	if (ps) {
		::AVIStreamClose(ps);
	}

	if (psCompressed) {
		::AVIStreamClose(psCompressed);
	}

	::AVIFileExit();

	if (hr != NOERROR) 	{
		::PostMessage(hWndGlobal, WM_USER_RECORDINTERRUPTED, 0, 0);

		//char *ErrorBuffer; // This really is a pointer - not reserved space!
		//FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | 	FORMAT_MESSAGE_FROM_SYSTEM, NULL, GetLastError(), MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US), (LPTSTR)&ErrorBuffer, 0, NULL);
		//CString reasonstr(ErrorBuffer);
		//CString errorstr("File Creation Error. Unable to rename file.\n\n");
		//CString reportstr;
		//reportstr = errorstr + reasonstr;
		//MessageBox(NULL, reportstr, "Note", MB_OK | MB_ICONEXCLAMATION);

		if (cVideoOpts.m_dwCompfccHandler != ICHANDLER_MSVC) {
			//if (IDYES == MessageBox(NULL, "Error recording AVI file using current compressor. Use default compressor ? ", "Note", MB_YESNO | MB_ICONEXCLAMATION)) {
			if (IDYES == MessageOut(NULL, IDS_STRING_ERRAVIDEFAULT, IDS_STRING_NOTE, MB_YESNO | MB_ICONQUESTION	)) {
				cVideoOpts.m_dwCompfccHandler = ICHANDLER_MSVC;
				strCodec = "MS Video 1";
				::PostMessage(hWndGlobal, WM_USER_RECORDSTART, 0, 0);
			}
		} else {
			//MessageBox(NULL, "Error Creating AVI File", "Error", MB_OK | MB_ICONEXCLAMATION);
			MessageOut(NULL, IDS_STRING_ERRCREATEAVI , IDS_STRING_NOTE, MB_OK | MB_ICONEXCLAMATION);
		}

		cVideoOpts = SaveVideoOpts;
		return true;
	}

	cVideoOpts = SaveVideoOpts;
	//Save the file on success
	::PostMessage(hWndGlobal, WM_USER_GENERIC, 0, 0);

	TRACE("CRecorderView::RecordVideo: Success end\n");
	return true;
}

void CRecorderView::SaveProducerCommand()
{
	// TODO: Why the artificial long file name here???
	// How about "Producer.ini" ?
	//Saving CamStudio.Producer.command.ini for storing text data

	CString strProfile;
	strProfile.Format(_T("%s\\CamStudio.Producer.command"), GetAppDataPath());

	CString strSection = _T("CamStudio Flash Producer Commands");
	CString strValue;

	CString strKey = _T("LaunchPropPrompt");
	strValue.Format(_T("%d"), cProducerOpts.m_bLaunchPropPrompt);
	::WritePrivateProfileString(strSection, strKey, strValue, strProfile);

	strKey = _T("LaunchHTMLPlayer");
	strValue.Format(_T("%d"), cProducerOpts.m_bLaunchHTMLPlayer);
	::WritePrivateProfileString(strSection, strKey, strValue, strProfile);

	strKey = _T("DeleteAVIAfterUse");
	strValue.Format(_T("%d"), cProducerOpts.m_bDeleteAVIAfterUse);
	::WritePrivateProfileString(strSection, strKey, strValue, strProfile);

#ifdef OBSOLETE_CODE
	CString fileName = "\\CamStudio.Producer.command";
	CString setDir = GetProgPath();
	CString setPath = setDir + fileName;

	FILE * sFile = fopen((LPCTSTR)(setPath), "wt");
	if (sFile == NULL) {
		return;
	}

	// ****************************
	// Write Variables
	// ****************************

	float ver=1.0;

	//fprintf(sFile, "[ CamStudio Flash Producer Commands ver%.2f -- Activate with -x or -b in command line mode ] \n\n",ver);
	fprintf(sFile, "[ CamStudio Flash Producer Commands ver%.2f ] \n\n",ver);
	fprintf(sFile, "bLaunchPropPrompt=%d \n", cProducerOpts.m_bLaunchPropPrompt);
	fprintf(sFile, "bLaunchHTMLPlayer=%d \n", cProducerOpts.m_bLaunchHTMLPlayer);
	fprintf(sFile, "bDeleteAVIAfterUse=%d \n", cProducerOpts.m_bDeleteAVIAfterUse);

	fclose(sFile);
#endif
}

bool CRecorderView::RunViewer(const CString& strNewFile)
{
	//Launch the player
	if (cProgramOpts.m_iLaunchPlayer == CAM1_PLAYER) {
		CString AppDir = GetProgPath();
		CString exefileName("\\player.exe ");
		CString launchPath = AppDir + exefileName + strNewFile;
		if (WinExec(launchPath, SW_SHOW) < HINSTANCE_ERROR) {
			//MessageBox("Error launching avi player!","Note",MB_OK | MB_ICONEXCLAMATION);
			MessageOut(m_hWnd,IDS_STRING_ERRPLAYER,IDS_STRING_NOTE,MB_OK | MB_ICONEXCLAMATION);
		}
	} else if (cProgramOpts.m_iLaunchPlayer == DEFAULT_PLAYER) {
		if (Openlink(strNewFile)) {
		} else {
			//MessageBox("Error launching avi player!","Note",MB_OK | MB_ICONEXCLAMATION);
			MessageOut(m_hWnd,IDS_STRING_ERRDEFAULTPLAYER,IDS_STRING_NOTE,MB_OK | MB_ICONEXCLAMATION);
		}
	} else if (cProgramOpts.m_iLaunchPlayer == CAM2_PLAYER) {
		CString AppDir = GetProgPath();
		CString launchPath;
		CString exefileName("\\Playerplus.exe ");		// Changed from Playplus.exe to Playerplus.exe
		launchPath = AppDir + exefileName + strNewFile;
		if (WinExec(launchPath,SW_SHOW) < HINSTANCE_ERROR) {
			//MessageBox("Error launching avi player!","Note",MB_OK | MB_ICONEXCLAMATION);
			MessageOut(m_hWnd,IDS_STRING_ERRPLAYER,IDS_STRING_NOTE,MB_OK | MB_ICONEXCLAMATION);
		}
	}
	return true;
}

bool CRecorderView::RunProducer(const CString& strNewFile)
{
	//ver 2.26
	SaveProducerCommand();

	//Sleep(2000);
	STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ZeroMemory( &si, sizeof(si) );
    si.cb = sizeof(si);
    ZeroMemory( &pi, sizeof(pi) );

	CString AppDir = GetProgPath();
	CString exefileName("\\producer.exe\" -x ");
	//CString exefileName("\\producer.exe -b ");
	CString quote = "\"";
	//CString launchPath = AppDir + exefileName + strNewFile;
	CString launchPath = quote + AppDir + exefileName + quote + strNewFile + quote;

	TRACE("CRecorderView::OnUserGeneric: %s\n", (LPCTSTR)launchPath);

	if (WinExec(launchPath, SW_SHOW) < HINSTANCE_ERROR) {
		MessageBox("Error launching SWF Producer!","Note",MB_OK | MB_ICONEXCLAMATION);
	}

	return true;
}

long CRecorderView::GetAVILengthTime(const CString & sAVIFile)
{
	long lRet = 0;
	HRESULT hr;
	PAVIFILE pFile;
	hr = AVIFileOpen(&pFile, sAVIFile, 0, 0L);
	if(hr == AVIERR_OK)
	{
		PAVISTREAM  pavi;
		if(AVIFileGetStream(pFile, &pavi, streamtypeAUDIO , 0) == AVIERR_OK)
		{
			lRet = AVIStreamLengthTime(pavi);
		}
		AVIFileRelease(pFile);
	}
	return lRet;
}
bool CRecorderView::ConvertToMP4(const CString& sInputAVI, const CString& sOutputMP4, const CString& sOutBareName)
{
	ConversionResult res;

	CMP4Converter *pConv = new CMP4Converter();
	if(pConv)
	{
		pConv->ConvertAVItoMP4(sInputAVI, sOutputMP4);
		CProgressDlg *pProgDlg = new CProgressDlg();
		if(pProgDlg)
		{
			pProgDlg->Create(this);
			CString msgx,msgout;
			msgx.LoadString(IDS_STRING_GENERATING);
			msgout.Format(msgx,sOutBareName);
			((CStatic *) pProgDlg->GetDlgItem(IDC_CONVERSIONTEXT))->SetWindowText(msgout);

			int nProgress = 0;
			time_t timer;
			long lTimeStart = time(&timer);
			long lCurrentTime = 0;
			while(pConv->Converting())
			{
				lCurrentTime = time(&timer);
				if(pProgDlg->CheckCancelButton())
				{
					pConv->CancelConversion();
					break;
				}
				if(lCurrentTime-lTimeStart >= rand() % pProgDlg->MaxProg() + pProgDlg->MinProg() && nProgress < pProgDlg->FakeMax())
				{
					nProgress = nProgress + rand() % pProgDlg->MaxProg() + pProgDlg->MinProg();
					pProgDlg->SetPos(nProgress);
					lTimeStart = time(&timer);
				}
			}
			while(nProgress < pProgDlg->RealMax())
			{
				lCurrentTime = time(&timer);
				if(pProgDlg->CheckCancelButton())
				{
					res = CANCELLED;
					break;
				}
				nProgress++;
				pProgDlg->SetPos(nProgress);
				lTimeStart = time(&timer);
			}
			res = pConv->Status();
			delete(pProgDlg);
		}
		delete pConv;
	}

	return (res == SUCCESS) ? true : false;
}

bool CRecorderView::GetRecordState()
{
	return bRecordState;
}

bool CRecorderView::GetPausedState()
{
	return bRecordPaused;
}

VOID CRecorderView::XNoteSetRecordingInPauseMode(void)
{
	//TRACE("## XNoteSetRecordingInPauseMode / BEGIN ##\n" );
	
	//return if not current recording or already in paused state
	if (!bRecordState || bRecordPaused)
		return;
  
	// RecordDurationLimitMode applicable
	if ( cXNoteOpts.m_bXnoteRecordDurationLimitMode ) {
		if ( bXNoteSnapRecordingState == true ) {
			if ( (GetTickCount() - ulXNoteLastSnapTime) > (ULONG) cXNoteOpts.m_ulXnoteRecordDurationLimitInMilliSec ){
				bXNoteSnapRecordingState = false;
				
				/* TRACE("## CRecorderView::XNoteSetRecordingInPauseMode / PostMessage WM_USER_RECORDPAUSED bool:[%d] difftime[%d][%d] ##\n",
					cXNoteOpts.m_bXnoteRecordDurationLimitMode,
					cXNoteOpts.m_ulXnoteRecordDurationLimitInMilliSec ,
					GetTickCount() - ulXNoteLastSnapTime ); */
					
				// Post message that will put recording on hold with the next incoming frame
				::PostMessage(hWndGlobal, WM_USER_RECORDPAUSED, 0, 0);
			}
		}
	}
	return;
}


/////////////////////////////////////////////////////////////////////////////
// XNoteActionStopwatchResetParams
// Reset all used params for Xnote that nust be reset after a recording is or should terminate.
//
void CRecorderView::XNoteActionStopwatchResetParams(void)
{
		// Reset
		// Once CamStudio recording session shall terminate, Xnote stopwatch params must be reset. 
		// TODO, put all gelobal params in stuct or class.
		ulXNoteStartTime = 0UL;
		ulXNoteLastSnapTime = 0UL;

		iXNoteStartSource			= XNOTE_SOURCE_UNDEFINED;		
		iXNoteLastSnapSource		= XNOTE_SOURCE_UNDEFINED;		
		iXNoteStartWithSensor		= XNOTE_TRIGGER_UNDEFINED;
		iXNoteLastSnapWithSensor	= XNOTE_TRIGGER_UNDEFINED;

		bXNoteSnapRecordingState = false;
		strcpy(cXNoteLastSnapTimes,"");

		cXNoteOpts.m_ulStartXnoteTickCounter = 0UL;
		cXNoteOpts.m_ulSnapXnoteTickCounter = 0UL;
		cXNoteOpts.m_cXnoteStartEntendedInfo.SetString("");
}


/////////////////////////////////////////////////////////////////////////////
// CRecorderView
// Processes actions innitiated by XNote WindowsMessages. (Xnote is a stopwatch application. http://www.xnotestopwatch.com/)
// Allows that external program can work with Camstudio and can instruct CamStudio when to start recording, to pause and to terminate recording.

VOID CRecorderView::XNoteProcessWinMessage(int iActionID , int iSensorID , int iSourceID , ULONG lXnoteTimeInMilliSeconds )
{
	if ( !cXNoteOpts.m_bXnoteRemoteControlMode ) {
		TRACE(_T("## CRecorderView::XNoteProcessWinMessage RemoteControl OFF : iActionID:[%d], iSensorID:[%d], time:[%lu], GetRecordState:[%d], GetPausedState:[%d]\n"), iActionID, iSensorID, lXnoteTimeInMilliSeconds, GetRecordState(), GetPausedState() );
		return;
	}

	DWORD dwCurrTickCount =  GetTickCount();
//	int nStrLength = 0;
	int nStrLengthNew = 0;
	int nStrLengthSnaps = 0;

	// Stringc to create output to display in recording until next snap occurs.
	//  (Declaration outside switch due to C2360
	char cTmp[128] = "";
	char cTmpBuffXNoteTimeStamp[128] = "";

	// Todo, replace magic numbers is switch with defined names
	switch (iActionID)
	{
	case XNOTE_ACTION_STOPWATCH_START:
		// Start, (Clock is counting)
		//TRACE(_T("## CRecorderView::XNoteProcessWinMessage: iActionID:[%d], iSensorID:[%d], time:[%lu], GetRecordState:[%d], GetPausedState:[%d]\n"), iActionID, iSensorID, lXnoteTimeInMilliSeconds, GetRecordState(), GetPausedState() );
		//TRACE("## CRecorderView::XNoteProcessWinMessage START Tick:[%lu] XNote:[%lu]\n", GetTickCount(), lXnoteTimeInMilliSeconds);

		if ( ulXNoteStartTime == 0UL) {
			bXNoteSnapRecordingState = true;

			iXNoteStartWithSensor = iSensorID;
			iXNoteStartSource	  = iSourceID;		

			ulXNoteStartTime = dwCurrTickCount;
			ulXNoteLastSnapTime = dwCurrTickCount;
			cXNoteOpts.m_ulStartXnoteTickCounter = ULONG(dwCurrTickCount); 
			cXNoteOpts.m_ulSnapXnoteTickCounter = ULONG(dwCurrTickCount); ;
			cXNoteOpts.m_cSnapXnoteTimesString.SetString( "" );

			// Create first info line what and how is start triggered
			CXnoteStopwatchFormat::FormatXnoteExtendedInfoSourceSensor( cTmp, iSourceID, iSensorID );
			cXNoteOpts.m_cXnoteStartEntendedInfo.SetString( cTmp );
		}

		// Don't call OnRecord if CamStudio is still recording unless it is paused
		if ( !GetRecordState() ||
			 ( GetRecordState() && GetPausedState()	) ){
			//TRACE(_T("## CRecorderView::XNoteProcessWinMessage: iActionID:[%d], iSensorID:[%d], Call OnRecord()..\n"), iActionID, iSensorID);
			OnRecord();
		}
		break;

	case XNOTE_ACTION_STOPWATCH_STOP:
		// Stop, 
		// Xnote Stopwatch Clock is on hold but here we do our own counting)
		// So Camstudio counting will continues and show different times than Xnote Stopwatch..! 

		//TRACE(_T("## CRecorderView::XNoteProcessWinMessage: iActionID:[%d], iSensorID:[%d], time:[%lu], GetRecordState:[%d], GetPausedState:[%d]\n"), iActionID, iSensorID, lXnoteTimeInMilliSeconds, GetRecordState(), GetPausedState() );
		
		// OnPause will first check that recording is active, no extra checks required here
		
		//TRACE(_T("## CRecorderView::XNoteProcessWinMessage: iActionID:[%d], iSensorID:[%d], Call OnPause()..\n"), iActionID, iSensorID);
		OnPause();
		break;

	case XNOTE_ACTION_STOPWATCH_SNAP:
		// Snap, 
		//TRACE(_T("## CRecorderView::XNoteProcessWinMessage: iActionID:[%d] iSensorID:[%d], time:[%lu], GetRecordState:[%d], GetPausedState:[%d]\n"), iActionID, iSensorID, lXnoteTimeInMilliSeconds, GetRecordState(), GetPausedState() );
		//TRACE("## CRecorderView::XNoteProcessWinMessage SNAP Tick:[%lu] XNote:[%lu]\n", GetTickCount(), lXnoteTimeInMilliSeconds);

		// Any snap should be recorded. This are the events that are important and must be registrated.
		// Because we don't have the correct Xnote time yet (todo) we shall use the current TickCount. 
		// Penalty is a minor difference in snaptime as registrated by xnote and our snaptime ( 15-20 ms)

		bXNoteSnapRecordingState = true;

		iXNoteStartWithSensor	= iSensorID;
		iXNoteLastSnapSource	= iSourceID;

		ulXNoteLastSnapTime = dwCurrTickCount;
		cXNoteOpts.m_ulSnapXnoteTickCounter = dwCurrTickCount;

		// CXnoteStopwatchFormat::FormatXnoteDelayedTimeString( cTmp, cXNoteOpts.m_ulStartXnoteTickCounter, dwCurrTickCount ,0 , false );

		sprintf( cTmp, "%s\n", cXNoteOpts.m_cXnoteStartEntendedInfo );
		// Show Xnote values instead of own calcuated value
		CXnoteStopwatchFormat::FormatXnoteDelayedTimeString( cTmp, 0, lXnoteTimeInMilliSeconds ,0 , false, false );
		// Add info about how the snap is created.
		CXnoteStopwatchFormat::FormatXnoteInfoSourceSensor( cTmp, iSourceID, iSensorID );

		// For onScreen reporting of snaptime we only need info about the last seconds  (00.000aa).
		nStrLengthNew = max (0, strlen(cTmp)-(6+2) );
		// And previous snaps can be truncated. How much snasps to show should be a user option (TODO)
		nStrLengthSnaps = max (0, strlen(cXNoteLastSnapTimes)- min(2,strlen(cXNoteLastSnapTimes)/(6+2+1))*(6+2+1) );
		
		sprintf( cTmpBuffXNoteTimeStamp, "%s%s ", &cXNoteLastSnapTimes[nStrLengthSnaps], &cTmp[nStrLengthNew] );
		strcpy(cXNoteLastSnapTimes,cTmpBuffXNoteTimeStamp);

		cXNoteOpts.m_cSnapXnoteTimesString.SetString( cXNoteLastSnapTimes );

		// Write snap timevalue (in ms) to the with the video recording started log file.
		// Use XML tags because others logs might be written is files in the near future.
		sprintf( cTmp, "<xnote> <snap> %09lu <\\snap> <frame> <\\frame> <\\xnote>\n", lXnoteTimeInMilliSeconds );
		WriteLineToXnoteLogFile( cTmp );

		// If recording but paused onRecord must called for release
		if ( GetRecordState() && GetPausedState() ) {
			//TRACE(_T("## CRecorderView::XNoteProcessWinMessage: iActionID:[%d] iSensorID:[%d], Call OnRecord()..\n"), iActionID, iSensorID);
			OnRecord();
		}
		break;

	case XNOTE_ACTION_STOPWATCH_RESET:

		// Call OnStop. OnStop checks is Camstudio is still in recording mode first.
		XNoteActionStopwatchResetParams();		// Also Include in OnStop buit only if recording is still active
		OnStop();
		break;

	case XNOTE_ACTION_MOTIONDETECTOR_ALERT:
		// Initiated not by Xnote but by MotionDetection application
		// Call OnRecord if CamStudio is still recording but currently in pause mode
		if ( GetRecordState() && GetPausedState() ){

			// Reset some values to make sure that recording will start again it and stops if duration times out.
			bXNoteSnapRecordingState = true;
			ulXNoteLastSnapTime = dwCurrTickCount;
			cXNoteOpts.m_ulSnapXnoteTickCounter = dwCurrTickCount;

			//TRACE(_T("## CRecorderView::XNoteProcessWinMessage: iActionID:[%d] iSensorID:[%d], Motion detected, call OnRecord()..\n"), iActionID, iSensorID);
			OnRecord();
		}
		break;

	default:
		TRACE(_T("## CRecorderView::XNoteProcessWinMessage: iActionID:[%d] iSensorID:[%d]. Xnote or MotionDetector action not assigned.\n"), iActionID, iSensorID);
		break;
	}
}

void CRecorderView::DisplayAutopanInfo(CRect rc)
{
	if(bRecordState){
		CRect rectDraw(rc);
		HDC hdc = ::GetDC(hFixedRegionWnd);
		HBRUSH newbrush = (HBRUSH) CreateHatchBrush(HS_CROSS, RGB(0,0,0));
		HBRUSH oldbrush = (HBRUSH) SelectObject(hdc, newbrush);
		HFONT newfont;
		newfont =  CreateFont(16,0,0,0,FW_DONTCARE,FALSE,FALSE,FALSE,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,
			CLIP_DEFAULT_PRECIS, 0, VARIABLE_PITCH, NULL);
		HFONT oldfont = (HFONT) SelectObject(hdc, newfont);
		CString strmessage;
		strmessage.LoadString((cProgramOpts.m_bAutoPan == true ) ? IDS_STRING_AUTOPAN_ENABLED : IDS_STRING_AUTOPAN_DISABLED );
		//m_Loc.LoadString((cProgramOpts.m_bAutoPan == true ) ? IDS_STRING_AUTOPAN_ENABLED : IDS_STRING_AUTOPAN_DISABLED );
		//strmessage.Format("%d - %d -%d - %d", rectDraw.left, rectDraw.top, rectDraw.right, rectDraw.bottom);
		SIZE sExtent;
		DWORD dw = GetTextExtentPoint(hdc, (LPCTSTR)strmessage, strmessage.GetLength(), &sExtent);
		VERIFY(0 != dw);
		COLORREF oldtextcolor = SetTextColor(hdc,RGB(255,255,255));
		COLORREF oldbkcolor = SetBkColor(hdc,RGB(0, 0, 0));
		int dx = sExtent.cx;
		int dy = sExtent.cy;
		int x = rectDraw.right - dx;
		int y = rectDraw.bottom - dy;
		Rectangle(hdc, x -3, y -3, x + dx + 2, y + dy + 1);
		ExtTextOut(hdc, x, y, 0, NULL, (LPCTSTR)strmessage, strmessage.GetLength(), NULL);
		SetBkColor(hdc,oldbkcolor);
		SetTextColor(hdc,oldtextcolor);
		SetBkMode(hdc,OPAQUE);
		::ReleaseDC(hFixedRegionWnd, hdc);
		Sleep(500);
	}
}
namespace {	// annonymous

void DataFromSoundIn(CBuffer* buffer)
{
	if (pSoundFile) {
		if (!pSoundFile->Write(buffer)) {
			//m_SoundIn.Stop();
			StopAudioRecording();
			ClearAudioFile();

			//MessageBox(NULL,"Error Writing Sound File","Note",MB_OK | MB_ICONEXCLAMATION);
			MessageOut(NULL,IDS_STRING_ERRSOUND2,IDS_STRING_NOTE,MB_OK | MB_ICONEXCLAMATION);
		}
	}
}

int AddInputBufferToQueue()
{
	// create the header
	// TODO, Possible memory leak, where is the delete operation of the new below done?
	LPWAVEHDR pHdr = new WAVEHDR;
	if (pHdr == NULL) {
		return NULL;
	}
	ZeroMemory(pHdr, sizeof(WAVEHDR));

	// new a buffer
	CBuffer buf(cAudioFormat.AudioFormat().nBlockAlign * iBufferSize, false);
	pHdr->lpData = buf.ptr.c;
	pHdr->dwBufferLength = buf.ByteLen;

	// prepare it
	MMRESULT mmReturn = ::waveInPrepareHeader(m_hWaveRecord, pHdr, sizeof(WAVEHDR));
	if (mmReturn) {
		waveInErrorMsg(mmReturn, "in AddInputBufferToQueue()");
		// todo: leak? did pHdr get deleted?
		return m_QueuedBuffers;
	}

	// add the input buffer to the queue
	mmReturn = ::waveInAddBuffer(m_hWaveRecord, pHdr, sizeof(WAVEHDR));
	if (mmReturn) {
		waveInErrorMsg(mmReturn, "Error in AddInputBufferToQueue()");
		// todo: leak? did pHdr get deleted?
		return m_QueuedBuffers;
	}

	// no error
	// increment the number of waiting buffers
	return m_QueuedBuffers++;
}

//===============================================
// AUDIO CODE
//===============================================

void waveInErrorMsg(MMRESULT result, const char * addstr)
{
	// say error message
	char errorbuffer[500];
	waveInGetErrorText(result, errorbuffer,500);
	CString msgstr;
	msgstr.Format("%s %s", errorbuffer, addstr);

	CString tstr;
	tstr.LoadString(IDS_STRING_WAVEINERR);
	MessageBox(NULL, msgstr, tstr, MB_OK | MB_ICONEXCLAMATION);
}

//Delete the pSoundFile variable and close existing audio file
void ClearAudioFile()
{
	if (pSoundFile) {
		delete pSoundFile;	// will close output file
		pSoundFile = NULL;
	}
}

BOOL InitAudioRecording()
{
	m_ThreadID = ::GetCurrentThreadId();
	m_QueuedBuffers = 0;
	m_hWaveRecord = NULL;

	iBufferSize = 1000; // samples per callback

	cAudioFormat.BuildRecordingFormat();

	ClearAudioFile();

	//Create temporary wav file for audio recording
	GetTempAudioWavPath();
	// TODO, Possible memory leak, where is the delete operation of the new below done?
	pSoundFile = new CSoundFile(strTempAudioWavFilePath, &(cAudioFormat.AudioFormat()));

	if (!(pSoundFile && pSoundFile->IsOK()))
		//MessageBox(NULL,"Error Creating Sound File","Note",MB_OK | MB_ICONEXCLAMATION);
		MessageOut(NULL,IDS_STRING_ERRSOUND,IDS_STRING_NOTE,MB_OK | MB_ICONEXCLAMATION);

	return TRUE;
}

//Initialize the strTempAudioWavFilePath variable with a valid temporary path
void GetTempAudioWavPath()
{
	CString csTempFolder(GetTempFolder(cProgramOpts.m_iTempPathAccess, cProgramOpts.m_strSpecifiedDir));	

	//CString fileName;
	//fileName.Format("\\%s001.wav", TEMPFILETAGINDICATOR );
	//strTempAudioWavFilePath = GetTempFolder (cProgramOpts.m_iTempPathAccess, cProgramOpts.m_strSpecifiedDir) + fileName;

	strTempAudioWavFilePath.Format("%s\\%s-%s.%s", (LPCSTR)csTempFolder, TEMPFILETAGINDICATOR, (LPCSTR)cVideoOpts.m_cStartRecordingString , "wav" );

	//Test the validity of writing to the file
	int fileverified = 0;
	while (!fileverified)
	{
		OFSTRUCT ofstruct;
		HFILE fhandle = OpenFile( strTempAudioWavFilePath, &ofstruct, OF_SHARE_EXCLUSIVE | OF_WRITE | OF_CREATE);
		if (fhandle != HFILE_ERROR) {
			fileverified = 1;
			CloseHandle( (HANDLE) fhandle);
			DeleteFile(strTempAudioWavFilePath);
		} else {
			srand( (unsigned)time( NULL));
			int randnum = rand();
			char numstr[50];
			sprintf(numstr,"%d",randnum);

			CString cnumstr(numstr);
			// CString fxstr;  
			// fxstr.Format("\\%s", TEMPFILETAGINDICATOR );
			// CString exstr(".wav");
			// strTempAudioWavFilePath = GetTempFolder (cProgramOpts.m_iTempPathAccess, cProgramOpts.m_strSpecifiedDir) + fxstr + cnumstr + exstr;
			strTempAudioWavFilePath.Format("%s\\%s-%s-%s.%s", (LPCSTR)csTempFolder, TEMPFILETAGINDICATOR, (LPCSTR)cVideoOpts.m_cStartRecordingString , cnumstr , "wav" );

			//MessageBox(NULL,strTempAudioWavFilePath,"Uses Temp File",MB_OK);
			//fileverified = 1;
			//Try choosing another temporary filename
		}
	}
}

BOOL StartAudioRecording()
{
	TRACE(_T("StartAudioRecording\n"));

	// open wavein device
	// use on message to map.....
	MMRESULT mmReturn = ::waveInOpen(&m_hWaveRecord, cAudioFormat.m_uDeviceID, &(cAudioFormat.AudioFormat()),(DWORD) hWndGlobal, NULL, CALLBACK_WINDOW);
	if (mmReturn) {
		waveInErrorMsg(mmReturn, "Error in StartAudioRecording()");
		return FALSE;
	}

	// make several input buffers and add them to the input queue
	for (int i = 0; i < 3; i++) {
		AddInputBufferToQueue();
	}

	// start recording
	mmReturn = ::waveInStart(m_hWaveRecord);
	if (mmReturn) {
		waveInErrorMsg(mmReturn, "Error in StartAudioRecording()");
		return FALSE;
	}

	iAudioTimeInitiated = 1;
	sdwSamplesPerSec = cAudioFormat.AudioFormat().nSamplesPerSec;
	sdwBytesPerSec = cAudioFormat.AudioFormat().nAvgBytesPerSec;

	return TRUE;
}

void StopAudioRecording()
{
	MMRESULT mmReturn = ::waveInReset(m_hWaveRecord);
	if (mmReturn) {
		waveInErrorMsg(mmReturn, "in Stop()");
		return;
	}
	Sleep(500);

	mmReturn = ::waveInStop(m_hWaveRecord);
	if (mmReturn) {
		waveInErrorMsg(mmReturn, "Error in StopAudioRecording() (WaveinStop)");
	}

	mmReturn = ::waveInClose(m_hWaveRecord);
	if (mmReturn) {
		waveInErrorMsg(mmReturn, "Error in StopAudioRecording() (WaveinClose)");
	}

	//if (m_QueuedBuffers != 0) ErrorMsg("Still %d buffers in waveIn queue!", m_QueuedBuffers);
	if (m_QueuedBuffers != 0) {
		//MessageBox(NULL,"Audio buffers still in queue!","note", MB_OK);
		MessageOut(NULL,IDS_STRING_AUDIOBUF,IDS_STRING_NOTE,MB_OK | MB_ICONEXCLAMATION);
	}

	iAudioTimeInitiated = 0;
}


//===============================================
// REGION CODE
//===============================================

int InitSelectRegionWindow()
{
	return 0;
}

int InitDrawShiftWindow()
{
	//MessageBox(NULL,"sdg","",0);
	HDC hScreenDC = ::GetDC(hMouseCaptureWnd);

	//FixRectSizePos(&rc, maxxScreen, maxyScreen, minxScreen, minyScreen);

	rcClip.left = rc.left;
	rcClip.top = rc.top;
	rcClip.right = rc.right;
	rcClip.bottom = rc.bottom;
	DrawSelect(hScreenDC, TRUE, &rcClip);

	old_rcClip = rcClip;

	//Set Curosr at the centre of the clip rectangle
	POINT ptOrigin;
	ptOrigin.x = (rcClip.right + rcClip.left)/2;
	ptOrigin.y = (rcClip.top + rcClip.bottom)/2;

	rcOffset.left	= rcClip.left - ptOrigin.x;
	rcOffset.top	= rcClip.top - ptOrigin.y;
	rcOffset.right	= rcClip.right - ptOrigin.x;
	rcOffset.bottom	= rcClip.bottom - ptOrigin.y;

	::ReleaseDC(hMouseCaptureWnd,hScreenDC);

	return 0;
}



//===============================================
// XNOTE or MOTION DETECTOR stopwatch supporting CODE
//===============================================


//Delete the hXnoteLogFile variable and close existing log file
void CloseStreamXnoteLogFile()
{
	if ( ioXnoteLogFile.is_open() ) {
		ioXnoteLogFile.close();
		pioXnoteLogFile = NULL;
	}

}

BOOL OpenStreamXnoteLogFile()
{
	CloseStreamXnoteLogFile();

	//Create temporary Xnote Log file for event and log recording
	GetTempXnoteLogPath();
	
	// If file exist is will be overwritten and re created.
	ioXnoteLogFile.open( strTempXnoteLogFilePath, ios::out );
	if ( ! ioXnoteLogFile.is_open() )
	{
		TRACE("## Opening strTempXnoteLogFilePath failed..!\n");
	}

	return TRUE;
}

void WriteLineToXnoteLogFile( char* pStr )
{
	//TRACE("## WriteLineToXnoteLogFile pStr=[%s]\n",pStr);
	if ( ioXnoteLogFile.is_open() ) {
		ioXnoteLogFile << pStr ;
	}
}



//Initialize the strTempXnoteLogFilePath variable with a valid temporary path
void GetTempXnoteLogPath()
{
	CString csTempFolder(GetTempFolder(cProgramOpts.m_iTempPathAccess, cProgramOpts.m_strSpecifiedDir));	
	// TODO csTempFolder does not contain the temp folder but returns the target folder. Hence the function name is not correct..!
	// TRACE( _T("## RecorderView.cpp GetTempXnoteLogPath() / m_iTempPathAccess=[%d]  m_strSpecifiedDir=[%s] csTempFolder=[%s]\n"),cProgramOpts.m_iTempPathAccess, cProgramOpts.m_strSpecifiedDir, csTempFolder );

	strTempXnoteLogFilePath.Format("%s\\%s-%s.%s", (LPCSTR)csTempFolder, TEMPFILETAGINDICATOR, (LPCSTR)cVideoOpts.m_cStartRecordingString , "txt" );
	// TRACE( _T("## GetTempXnoteLogPath() / strTempXnoteLogFilePath=[%s] \n"), strTempXnoteLogFilePath.GetString() );

	//Test the validity of writing to this file  (Using the old way with FILE instead of ofstream)
	int fileverified = 0;
	while (!fileverified)
	{
		OFSTRUCT ofstruct;
		HFILE fhandle = OpenFile( strTempXnoteLogFilePath, &ofstruct, OF_SHARE_EXCLUSIVE | OF_WRITE | OF_CREATE);
		if (fhandle != HFILE_ERROR) {
			fileverified = 1;
			CloseHandle( (HANDLE) fhandle);
			DeleteFile(strTempXnoteLogFilePath);
		} else {
			// Try to open a file with a random number attached till we got a file that can be used.
			srand( (unsigned)time( NULL));
			int randnum = rand();
			char numstr[50];
			sprintf(numstr,"%d",randnum);

			CString cnumstr(numstr);
			strTempXnoteLogFilePath.Format("%s\\%s-%s-%s.%s", (LPCSTR)csTempFolder, TEMPFILETAGINDICATOR, (LPCSTR)cVideoOpts.m_cStartRecordingString , cnumstr , "txt" );
		}
	}
}

//===============================================
// EXPERIMENTAL CODE
//===============================================

	// Nothing currently...!

//===============================================
// OBSOLETE CODE
// Not used or used and called from dead code branches only
//===============================================

#ifndef THIS_COULD_BE_OBSOLETE_CODE
void SetBufferSize(int NumberOfSamples)
{
	iBufferSize = NumberOfSamples;
}

void GetVideoCompressState (HIC hic, DWORD fccHandler)
{
	DWORD statesize = ICGetStateSize(hic);
	if (statesize < 1L) {
		return;
	}

	if (cVideoOpts.State(statesize)) {
		//ICGetState returns statesize even if pVideoCompressParams is not NULL ??
		DWORD ret = ICGetState(hic, cVideoOpts.State(), cVideoOpts.StateSize());
		if (ret < 0) {
			//CString reportstr;
			//reportstr.Format("Failure in getting compressor state ! Error Value = %d", ret);
			//MessageBox(NULL,reportstr,"Note",MB_OK | MB_ICONEXCLAMATION);
			MessageOut(NULL,IDS_STRING_COMPRESSORSTATE,IDS_STRING_NOTE,MB_OK | MB_ICONEXCLAMATION,ret);
		} else {
			//we store only the state for one compressor in pVideoCompressParams
			//So we need to indicate which compressor the state is referring to
			cVideoOpts.m_dwCompressorStateIsFor = fccHandler;
		}
	}
}

#endif  //THIS_COULD_BE_OBSOLETE_CODE



}	// namespace annonymous
