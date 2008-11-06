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
#include "AnnotationEffectsOptions.h"
#include "CursorOptionsDlg.h"
#include "EffectsOptions.h"
#include "EffectsOptions2.h"
#include "FolderDialog.h"
#include "FixedRegion.h"
#include "FlashingWnd.h"
#include "Keyshortcuts.h"
#include "ListManager.h"
#include "PresetTime.h"
#include "ScreenAnnotations.h"
#include "SyncDialog.h"
#include "TroubleShoot.h"
#include "VideoOptions.h"

#include "ximage.h"
#include "soundfile.h"
#include "hook.h"		// for WM_USER_RECORDSTART_MSG

#include "Buffer.h"
#include "CStudioLib.h"
#include "TrayIcon.h"
#include "AudioSpeakers.h"
#include "HotKey.h"
#include "Screen.h"

#include <windowsx.h>

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

//version 1.6

#if !defined(WAVE_FORMAT_MPEGLAYER3)
#define WAVE_FORMAT_MPEGLAYER3 0x0055
#endif

#define SWAP(x,y) ((x) ^= (y) ^= (x) ^= (y))

#ifndef CAPTUREBLT
#define CAPTUREBLT (DWORD)0x40000000
#endif

/////////////////////////////////////////////////////////////////////////////
// external variables
/////////////////////////////////////////////////////////////////////////////

//Region Movement
extern CRect newRect;
extern int newRegionUsed;
extern int readingRegion;
extern int writingRegion;

extern int settingRegion;

extern CListManager ListManager;

extern int iRrefreshRate;

extern CString shapeName;
extern CString strLayoutName;

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
RECT rcOffset;
RECT rcClip;
RECT rcUse;
RECT old_rcClip;
BOOL bCapturing = FALSE;
POINT ptOrigin;

RECT rc;
HWND hMouseCaptureWnd;
HWND hFixedRegionWnd;

HBITMAP hLogoBM = NULL;
CFlashingWnd* pFlashingWnd = 0;

//Misc Vars
bool bRecordState = false;
int recordpaused = 0;
UINT interruptkey = 0;
DWORD dwInitialTime = 0;
bool bInitCapture = false;
int irsmallcount = 0;

// Messaging
HWND hWndGlobal = NULL;

//int iTempPathAccess = USE_WINDOWS_TEMP_DIR;
CString specifieddir;

/////////////////////////////////////////////////////////////////////////////
//Variables/Options requiring interface
/////////////////////////////////////////////////////////////////////////////
int iBits = 24;
int iDefineMode = 0; //set only in FixedRegion.cpp

ICINFO * pCompressorInfo = NULL;
int num_compressor = 0;
int selected_compressor = -1;

//Ver 1.2
//Video Compress Parameters
LPVOID pVideoCompressParams = NULL;

LPVOID pParamsUse = NULL;

//Report variables
int nActualFrame = 0;
int nCurrFrame = 0;
float fRate = 0.0;
float fActualRate = 0.0;
float fTimeLength = 0.0;
int nColors = 24;
int iActualWidth = 0;
int iActualHeight = 0;

//Path to temporary avi file
CString strTempFilePath;

//Autopan
RECT rectPanCurrent;
RECT rectPanDest;

// Ver 1.1
/////////////////////////////////////////////////////////////////////////////
// Audio Functions and Variables
/////////////////////////////////////////////////////////////////////////////
// The program records video and sound separately, into 2 files
// ~temp.avi and ~temp.wav, before merging these 2 file into a single avi file
// using the Merge_Video_And_Sound_File function
/////////////////////////////////////////////////////////////////////////////

//Path to temporary wav file
CString tempaudiopath;

HWAVEIN m_hRecord;
WAVEFORMATEX m_Format;
DWORD m_ThreadID;
int m_QueuedBuffers = 0;
int iBufferSize = 1000; // number of samples
CSoundFile * pSoundFile = NULL;

//Audio Options Dialog
LPWAVEFORMATEX pwfx = NULL;

/////////////////////////////////////////////////////////////////////////////
//ver 1.2
/////////////////////////////////////////////////////////////////////////////
//Key short-cuts variables
/////////////////////////////////////////////////////////////////////////////
//state vars
BOOL bAllowNewRecordStartKey = TRUE;

MCI_OPEN_PARMS mop;
MCI_SAVE_PARMS msp;
PSTR strFile;

WAVEFORMATEX m_FormatSpeaker;
int iAudioBitsPerSampleSpeaker = 16;
int iAudioNumChannelsSpeaker = 2;
int iAudioSamplesPerSecondsSpeaker = 44100;

int TroubleShootVal = 0;

//ver 1.8

CScreenAnnotationsDlg sadlg;
int bCreatedSADlg = false;

int isMciRecordOpen = 0;
bool bAlreadyMCIPause = false;

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
sWatermarkOpts cWatermarkOpts;

/////////////////////////////////////////////////////////////////////////////
//Function prototypes
/////////////////////////////////////////////////////////////////////////////

//Region Display Functions
void DrawSelect(HDC hdc, BOOL fDraw, LPRECT lprClip);

//Region Select Functions
int InitDrawShiftWindow();
int InitSelectRegionWindow();

//Misc Functions

void FreeVideoCompressParams();
BOOL AllocVideoCompressParams(DWORD paramsSize);
void GetVideoCompressState (HIC hic, DWORD fccHandler);
void SetVideoCompressState (HIC hic, DWORD fccHandler);
void FreeParamsUse();
BOOL MakeCompressParamsCopy(DWORD paramsSize, LPVOID pOrg);
CString strCodec("MS Video 1");
//Files Directory
CString savedir("");

BOOL StartAudioRecording(WAVEFORMATEX* format);
void waveInErrorMsg(MMRESULT result, const char *);
int AddInputBufferToQueue();
void SetBufferSize(int NumberOfSamples);
void CALLBACK OnMM_WIM_DATA(UINT parm1, LONG parm2);
void DataFromSoundIn(CBuffer* buffer);
void StopAudioRecording();
BOOL InitAudioRecording();
void ClearAudioFile();
void GetTempWavePath();
void BuildRecordingFormat();
void SuggestCompressFormat();
void SuggestRecordingFormat();
void AllocCompressFormat();
// Video/Audio Merging Functions and Definitions
int Merge_Video_And_Sound_File(CString input_video_path, CString input_audio_path, CString output_avi_path, BOOL recompress_audio, LPWAVEFORMATEX audio_recompress_format, DWORD audio_format_size, BOOL bInterleave, int interleave_factor,int interleave_unit);
BOOL CALLBACK SaveCallback(int iProgress);
//Functions that select audio options based on settings read
void AttemptRecordingFormat();
void AttemptCompressFormat();

CString GetCodecDescription(long fccHandler);
void BuildSpeakerRecordingFormat();
void SuggestSpeakerRecordingFormat(int i);
void mciRecordOpen();
void mciRecordStart();
void mciRecordStop(CString strFile);
void mciRecordClose();
void mciSetWaveFormat(); //add before mcirecord
void mciRecordPause(CString strFile);
void mciRecordResume(CString strFile);

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void AutoSetRate(int val,int& framerate,int& delayms);
void AutoSetRate(int val, int& framerate, int& delayms)
{
	if (val <= 17) {
		//fps more than 1 per second
		framerate = 200 - ((val - 1) * 10); //framerate 200 to 40;
		//1 corr to 200, 17 corr to 40

		delayms = 1000/framerate;

		////Automatically Adjust the Quality for MSVC (MS Video 1) if the framerate is too high
		//int sel = ((CComboBox *) GetDlgItem(IDC_COMPRESSORS))->GetCurSel();
		//if (pCompressorInfo[sel].fccHandler==mmioFOURCC('M', 'S', 'V', 'C')) {
		//	int cQuality = ((CSliderCtrl *) GetDlgItem(IDC_QUALITY_SLIDER))->GetPos();
		//	if (cQuality<80) {
		//		((CSliderCtrl *) GetDlgItem(IDC_QUALITY_SLIDER))->SetPos(80);
		//	}
		//}
	} else if (val <= 56) {
		//fps more than 1 per second
		framerate = (57 - val); //framerate 39 to 1;
		//18 corr to 39, 56 corr to 1

		delayms = 1000/framerate;
	} else if (val <= 86) { //assume iTimeLapse
		framerate = 20;
		delayms = (val-56) * 1000;

		//57 corr to 1000, 86 corr to 30000 (20 seconds)
	} else if (val<=99) { //assume iTimeLapse
		framerate = 20;
		delayms = (val-86) * 2000 + 30000;

		//87 corr to 30000, 99 corr to 56000 (56 seconds)
	} else {
		//val=100, iTimeLapse
		framerate = 20;
		delayms = 60000;

		//100 corr to 60000
	}
}

//#define OBSOLETE_CODE
#ifdef OBSOLETE_CODE

// See CxImage::GetTypeIdFromName
int FindFormat(const CString& ext)
{
	int type = 0;
	if (ext == "bmp")
		type = CXIMAGE_FORMAT_BMP;
#if CXIMAGE_SUPPORT_JPG
	else if (ext=="jpg"||ext=="jpeg")
		type = CXIMAGE_FORMAT_JPG;
#endif
#if CXIMAGE_SUPPORT_GIF
	else if (ext == "gif")
		type = CXIMAGE_FORMAT_GIF;
#endif
#if CXIMAGE_SUPPORT_PNG
	else if (ext == "png")
		type = CXIMAGE_FORMAT_PNG;
#endif
#if CXIMAGE_SUPPORT_MNG
	else if (ext == "mng" || ext == "jng")
		type = CXIMAGE_FORMAT_MNG;
#endif
#if CXIMAGE_SUPPORT_ICO
	else if (ext == "ico")
		type = CXIMAGE_FORMAT_ICO;
#endif
#if CXIMAGE_SUPPORT_TIF
	else if (ext == "tiff" || ext== "tif" )
		type = CXIMAGE_FORMAT_TIF;
#endif
#if CXIMAGE_SUPPORT_TGA
	else if (ext=="tga")
		type = CXIMAGE_FORMAT_TGA;
#endif
#if CXIMAGE_SUPPORT_PCX
	else if (ext=="pcx")
		type = CXIMAGE_FORMAT_PCX;
#endif
#if CXIMAGE_SUPPORT_WBMP
	else if (ext=="wbmp")
		type = CXIMAGE_FORMAT_WBMP;
#endif
#if CXIMAGE_SUPPORT_WMF
	else if (ext=="wmf"||ext=="emf")
		type = CXIMAGE_FORMAT_WMF;
#endif
#if CXIMAGE_SUPPORT_J2K
	else if (ext=="j2k"||ext=="jp2")
		type = CXIMAGE_FORMAT_J2K;
#endif
#if CXIMAGE_SUPPORT_JBG
	else if (ext=="jbg")
		type = CXIMAGE_FORMAT_JBG;
#endif
#if CXIMAGE_SUPPORT_JP2
	else if (ext=="jp2"||ext=="j2k")
		type = CXIMAGE_FORMAT_JP2;
#endif
#if CXIMAGE_SUPPORT_JPC
	else if (ext=="jpc"||ext=="j2c")
		type = CXIMAGE_FORMAT_JPC;
#endif
#if CXIMAGE_SUPPORT_PGX
	else if (ext=="pgx")
		type = CXIMAGE_FORMAT_PGX;
#endif
#if CXIMAGE_SUPPORT_RAS
	else if (ext=="ras")
		type = CXIMAGE_FORMAT_RAS;
#endif
#if CXIMAGE_SUPPORT_PNM
	else if (ext=="pnm"||ext=="pgm"||ext=="ppm")
		type = CXIMAGE_FORMAT_PNM;
#endif
	else
		type = CXIMAGE_FORMAT_UNKNOWN;

	return type;
}
#endif	// OBSOLETE_CODE
#undef OBSOLETE_CODE

//ver 1.8
int UnSetHotKeys();
int UnSetHotKeys()
{
//#pragma message("Disable UnSetHotKeys")
//	return 0;
	UnregisterHotKey(hWndGlobal,0);
	UnregisterHotKey(hWndGlobal,1);
	UnregisterHotKey(hWndGlobal,2);
	UnregisterHotKey(hWndGlobal,3);
	UnregisterHotKey(hWndGlobal,4);
	UnregisterHotKey(hWndGlobal,5);

	return 0;
}

int SetHotKeys(int succ[]);
int SetHotKeys(int succ[])
{
//#pragma message("Disable SetHotKeys")
//	return 0;

	UnSetHotKeys();

	for (int i = 0; i < 6; i++)
		succ[i] = 0;

	int nid = 0;
	UINT modf = 0;
	if (cHotKeyOpts.m_RecordStart.m_bCtrl)
		modf |= MOD_CONTROL;

	if (cHotKeyOpts.m_RecordStart.m_bShift)
		modf |= MOD_SHIFT;

	if (cHotKeyOpts.m_RecordStart.m_bAlt)
		modf |= MOD_ALT;

	if (cHotKeyOpts.m_RecordStart.m_vKey != VK_UNDEFINED) {
		BOOL ret = RegisterHotKey(hWndGlobal, nid, modf, cHotKeyOpts.m_RecordStart.m_vKey);
		if (!ret)
			succ[0] = 1;
	}

	nid++;
	modf = 0;
	if (cHotKeyOpts.m_RecordEnd.m_bCtrl)
		modf |= MOD_CONTROL;

	if (cHotKeyOpts.m_RecordEnd.m_bShift)
		modf |= MOD_SHIFT;

	if (cHotKeyOpts.m_RecordEnd.m_bAlt)
		modf |= MOD_ALT;

	if (cHotKeyOpts.m_RecordEnd.m_vKey != VK_UNDEFINED) {
		BOOL ret = RegisterHotKey(hWndGlobal, nid, modf, cHotKeyOpts.m_RecordEnd.m_vKey);
		if (!ret)
			succ[1] = 1;
	}

	nid++;
	modf = 0;
	if (cHotKeyOpts.m_RecordCancel.m_bCtrl)
		modf |= MOD_CONTROL;

	if (cHotKeyOpts.m_RecordCancel.m_bShift)
		modf |= MOD_SHIFT;

	if (cHotKeyOpts.m_RecordCancel.m_bAlt)
		modf |= MOD_ALT;

	if (cHotKeyOpts.m_RecordCancel.m_vKey != VK_UNDEFINED) {
		BOOL ret = RegisterHotKey(hWndGlobal, nid, modf, cHotKeyOpts.m_RecordCancel.m_vKey);
		if (!ret)
			succ[2] = 1;
	}

	nid++;
	modf = 0;
	if (cHotKeyOpts.m_Next.m_bCtrl)
		modf |= MOD_CONTROL;

	if (cHotKeyOpts.m_Next.m_bShift)
		modf |= MOD_SHIFT;

	if (cHotKeyOpts.m_Next.m_bAlt)
		modf |= MOD_ALT;

	if (cHotKeyOpts.m_Next.m_vKey != VK_UNDEFINED) {
		BOOL ret = RegisterHotKey(hWndGlobal, nid, modf, cHotKeyOpts.m_Next.m_vKey);
		if (!ret)
			succ[3] = 1;
	}

	nid++;
	modf = 0;
	if (cHotKeyOpts.m_Prev.m_bCtrl)
		modf |= MOD_CONTROL;

	if (cHotKeyOpts.m_Prev.m_bShift)
		modf |= MOD_SHIFT;

	if (cHotKeyOpts.m_Prev.m_bAlt)
		modf |= MOD_ALT;

	if (cHotKeyOpts.m_Prev.m_vKey != VK_UNDEFINED) {
		BOOL ret = RegisterHotKey(hWndGlobal, nid, modf, cHotKeyOpts.m_Prev.m_vKey);
		if (!ret)
			succ[4] = 1;
	}

	nid++;
	modf = 0;
	if (cHotKeyOpts.m_ShowLayout.m_bCtrl)
		modf |= MOD_CONTROL;

	if (cHotKeyOpts.m_ShowLayout.m_bShift)
		modf |= MOD_SHIFT;

	if (cHotKeyOpts.m_ShowLayout.m_bAlt)
		modf |= MOD_ALT;

	if (cHotKeyOpts.m_ShowLayout.m_vKey != VK_UNDEFINED) {
		BOOL ret = RegisterHotKey(hWndGlobal, nid, modf, cHotKeyOpts.m_ShowLayout.m_vKey);
		if (!ret)
			succ[5] = 1;
	}

	return nid;
}

int SetAdjustHotKeys();
int SetAdjustHotKeys()
{
	int succ[6];
	int ret = SetHotKeys(succ);

	CString keystr;
	CString msgstr;
	CString outstr;
	if (succ[0])
	{		
		cHotKeyOpts.m_RecordStart = sHotKeyDef(VK_UNDEFINED);
		keystr.LoadString(IDS_STRINGSPRKEY);
		msgstr.LoadString(IDS_STRING_UNSSC);
		outstr.Format(msgstr,keystr);
		//keystr = "Unable to set shortcuts for %d" + keystr;
		MessageBox(NULL, outstr, "Note", MB_OK | MB_ICONEXCLAMATION);
	}

	if (succ[1])
	{
		cHotKeyOpts.m_RecordEnd = sHotKeyDef(VK_UNDEFINED);
		keystr.LoadString(IDS_STRINGSRKEY);
		msgstr.LoadString(IDS_STRING_UNSSC);
		outstr.Format(msgstr,keystr);
		//keystr = "Unable to set shortcuts for %d" + keystr;
		MessageBox(NULL, outstr, "Note", MB_OK | MB_ICONEXCLAMATION);
	}

	if (succ[2])
	{
		cHotKeyOpts.m_RecordCancel = sHotKeyDef(VK_UNDEFINED);
		keystr.LoadString(IDS_STRINGCRKEY);
		msgstr.LoadString(IDS_STRING_UNSSC);
		outstr.Format(msgstr,keystr);
		//keystr = "Unable to set shortcuts for %d" + keystr;
		MessageBox(NULL, outstr, "Note", MB_OK | MB_ICONEXCLAMATION);
	}

	if (succ[3])
	{
		cHotKeyOpts.m_Next = sHotKeyDef(VK_UNDEFINED);
		keystr.LoadString(IDS_STRINGNLKEY);
		msgstr.LoadString(IDS_STRING_UNSSC);
		outstr.Format(msgstr,keystr);
		//keystr = "Unable to set shortcuts for %d" + keystr;
		MessageBox(NULL, outstr, "Note", MB_OK | MB_ICONEXCLAMATION);
	}

	if (succ[4])
	{
		cHotKeyOpts.m_Prev = sHotKeyDef(VK_UNDEFINED);
		keystr.LoadString(IDS_STRINGPLKEY);
		msgstr.LoadString(IDS_STRING_UNSSC);
		outstr.Format(msgstr,keystr);
		//keystr = "Unable to set shortcuts for %d" + keystr;
		MessageBox(NULL, outstr, "Note", MB_OK | MB_ICONEXCLAMATION);
	}

	if (succ[5])
	{
		cHotKeyOpts.m_ShowLayout = sHotKeyDef(VK_UNDEFINED);
		keystr.LoadString(IDS_STRINGSHLKEY);
		msgstr.LoadString(IDS_STRING_UNSSC);
		outstr.Format(msgstr,keystr);
		//keystr = "Unable to set shortcuts for %d" + keystr;
		MessageBox(NULL, outstr, "Note", MB_OK | MB_ICONEXCLAMATION);
	}

	return ret;
}

/////////////////////////////////////////////////////////////////////////////
// Utility()
/////////////////////////////////////////////////////////////////////////////

HBITMAP hSavedBitmap = NULL;

int InitSelectRegionWindow()
{
	return 0;
}

int InitDrawShiftWindow()
{
	HDC hScreenDC = ::GetDC(hMouseCaptureWnd);

	FixRectSizePos(&rc, maxxScreen, maxyScreen);

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

///////////////////////// //////////////////
/////////////// Functions //////////////////
///////////////////////// //////////////////

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
	m_hRecord = NULL;

	iBufferSize = 1000; // samples per callback

	BuildRecordingFormat();

	ClearAudioFile();

	//Create temporary wav file for audio recording
	GetTempWavePath();
	pSoundFile = new CSoundFile(tempaudiopath, &m_Format);

	if (!(pSoundFile && pSoundFile->IsOK()))
		//MessageBox(NULL,"Error Creating Sound File","Note",MB_OK | MB_ICONEXCLAMATION);
		MessageOut(NULL,IDS_STRING_ERRSOUND,IDS_STRING_NOTE,MB_OK | MB_ICONEXCLAMATION);

	return TRUE;
}

//Initialize the tempaudiopath variable with a valid temporary path
void GetTempWavePath()
{
	CString fileName("\\~temp001.wav");
	tempaudiopath = GetTempFolder (cProgramOpts.m_iTempPathAccess, specifieddir) + fileName;

	//Test the validity of writing to the file
	int fileverified = 0;
	while (!fileverified)
	{
		OFSTRUCT ofstruct;
		HFILE fhandle = OpenFile( tempaudiopath, &ofstruct, OF_SHARE_EXCLUSIVE | OF_WRITE | OF_CREATE);
		if (fhandle != HFILE_ERROR) {
			fileverified = 1;
			CloseHandle( (HANDLE) fhandle);
			DeleteFile(tempaudiopath);
		} else {
			srand( (unsigned)time( NULL));
			int randnum = rand();
			char numstr[50];
			sprintf(numstr,"%d",randnum);

			CString cnumstr(numstr);
			CString fxstr("\\~temp");
			CString exstr(".wav");
			tempaudiopath = GetTempFolder (cProgramOpts.m_iTempPathAccess, specifieddir) + fxstr + cnumstr + exstr;

			//MessageBox(NULL,tempaudiopath,"Uses Temp File",MB_OK);
			//fileverified = 1;
			//Try choosing another temporary filename
		}
	}
}

void SetBufferSize(int NumberOfSamples)
{
	iBufferSize = NumberOfSamples;
}

BOOL StartAudioRecording(WAVEFORMATEX* format)
{
	TRACE(_T("StartAudioRecording\n"));
	if (format != NULL)
		m_Format = *format;

	// open wavein device
	// use on message to map.....
	MMRESULT mmReturn = ::waveInOpen(&m_hRecord, cAudioFormat.m_uDeviceID, &m_Format,(DWORD) hWndGlobal, NULL, CALLBACK_WINDOW);
	if (mmReturn) {
		waveInErrorMsg(mmReturn, "Error in StartAudioRecording()");
		return FALSE;
	}

	// make several input buffers and add them to the input queue
	for (int i = 0; i < 3; i++) {
		AddInputBufferToQueue();
	}

	// start recording
	mmReturn = ::waveInStart(m_hRecord);
	if (mmReturn) {
		waveInErrorMsg(mmReturn, "Error in StartAudioRecording()");
		return FALSE;
	}

	iAudioTimeInitiated = 1;
	sdwSamplesPerSec = ((LPWAVEFORMAT) &m_Format)->nSamplesPerSec;
	sdwBytesPerSec = ((LPWAVEFORMAT) &m_Format)->nAvgBytesPerSec;

	return TRUE;
}

int AddInputBufferToQueue()
{
	// create the header
	LPWAVEHDR pHdr = new WAVEHDR;
	if (pHdr == NULL) {
		return NULL;
	}
	ZeroMemory(pHdr, sizeof(WAVEHDR));

	// new a buffer
	CBuffer buf(m_Format.nBlockAlign * iBufferSize, false);
	pHdr->lpData = buf.ptr.c;
	pHdr->dwBufferLength = buf.ByteLen;

	// prepare it
	MMRESULT mmReturn = ::waveInPrepareHeader(m_hRecord, pHdr, sizeof(WAVEHDR));
	if (mmReturn) {
		waveInErrorMsg(mmReturn, "in AddInputBufferToQueue()");
		// todo: leak? did pHdr get deleted?
		return m_QueuedBuffers;
	}

	// add the input buffer to the queue
	mmReturn = ::waveInAddBuffer(m_hRecord, pHdr, sizeof(WAVEHDR));
	if (mmReturn) {
		waveInErrorMsg(mmReturn, "Error in AddInputBufferToQueue()");
		// todo: leak? did pHdr get deleted?
		return m_QueuedBuffers;
	}

	// no error
	// increment the number of waiting buffers
	return m_QueuedBuffers++;
}

void StopAudioRecording()
{
	MMRESULT mmReturn = ::waveInReset(m_hRecord);
	if (mmReturn) {
		waveInErrorMsg(mmReturn, "in Stop()");
		return;
	}
	Sleep(500);

	mmReturn = ::waveInStop(m_hRecord);
	if (mmReturn) {
		waveInErrorMsg(mmReturn, "Error in StopAudioRecording() (WaveinStop)");
	}

	mmReturn = ::waveInClose(m_hRecord);
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

//Alloc Maximum Size for Save Format pwfx
void AllocCompressFormat()
{
	int initial_audiosetup = (pwfx) ? 0 : 1;
	if (!initial_audiosetup) {
		//Do nothing....already allocated
		return;
	}
	MMRESULT mmresult = acmMetrics(NULL, ACM_METRIC_MAX_SIZE_FORMAT, &cAudioFormat.m_dwCbwFX);
	if (MMSYSERR_NOERROR != mmresult) {
		//CString msgstr;
		//msgstr.Format("Metrics failed mmresult=%u!", mmresult);
		//::MessageBox(NULL,msgstr,"Note", MB_OK | MB_ICONEXCLAMATION);

		MessageOut(NULL,IDS_STRING_METRICSFAILED, IDS_STRING_NOTE, MB_OK | MB_ICONEXCLAMATION,mmresult);
		return;
	}

	pwfx = (LPWAVEFORMATEX)GlobalAllocPtr(GHND, cAudioFormat.m_dwCbwFX);
	if (NULL == pwfx) {
		//CString msgstr;
		//msgstr.Format("GlobalAllocPtr(%lu) failed!", dwCbwFX);
		//::MessageBox(NULL,msgstr,"Note", MB_OK | MB_ICONEXCLAMATION);

		MessageOut(NULL,IDS_STRING_GALLOC,IDS_STRING_NOTE,MB_OK | MB_ICONEXCLAMATION);
		return;
	}

	initial_audiosetup = 1;
}

//Build Recording Format to m_Format
void BuildRecordingFormat()
{
	m_Format.wFormatTag		= WAVE_FORMAT_PCM;
	m_Format.wBitsPerSample = cAudioFormat.m_iBitsPerSample;
	m_Format.nSamplesPerSec = cAudioFormat.m_iSamplesPerSeconds;
	m_Format.nChannels		= cAudioFormat.m_iNumChannels;

	m_Format.nBlockAlign = m_Format.nChannels * (m_Format.wBitsPerSample/8);
	m_Format.nAvgBytesPerSec = m_Format.nSamplesPerSec * m_Format.nBlockAlign;
	m_Format.cbSize = 0;
}

//Suggest Save/Compress Format to pwfx
void SuggestRecordingFormat()
{
	WAVEINCAPS pwic;
	::ZeroMemory(&pwic, sizeof(WAVEINCAPS));
	MMRESULT mmResult = ::waveInGetDevCaps(cAudioFormat.m_uDeviceID, &pwic, sizeof(pwic));
	if (MMSYSERR_NOERROR != mmResult)
	{
		// report error
		TRACE("SuggestRecordingFormat: waveInGetDevCaps failed %d\n", mmResult);
		return; 
	}

	//Ordered in preference of choice
	if ((pwic.dwFormats) & WAVE_FORMAT_2S16) {
		cAudioFormat.m_iBitsPerSample = 16;
		cAudioFormat.m_iNumChannels = 2;
		cAudioFormat.m_iSamplesPerSeconds = 22050;
		cAudioFormat.m_dwWaveinSelected = WAVE_FORMAT_2S16;
	} else if ((pwic.dwFormats) & WAVE_FORMAT_2M08) {
		cAudioFormat.m_iBitsPerSample = 8;
		cAudioFormat.m_iNumChannels = 1;
		cAudioFormat.m_iSamplesPerSeconds = 22050;
		cAudioFormat.m_dwWaveinSelected = WAVE_FORMAT_2M08;
	} else if ((pwic.dwFormats) & WAVE_FORMAT_2S08) {
		cAudioFormat.m_iBitsPerSample = 8;
		cAudioFormat.m_iNumChannels = 2;
		cAudioFormat.m_iSamplesPerSeconds = 22050;
		cAudioFormat.m_dwWaveinSelected = WAVE_FORMAT_2S08;
	} else if ((pwic.dwFormats) & WAVE_FORMAT_2M16) {
		cAudioFormat.m_iBitsPerSample = 16;
		cAudioFormat.m_iNumChannels = 1;
		cAudioFormat.m_iSamplesPerSeconds = 22050;
		cAudioFormat.m_dwWaveinSelected = WAVE_FORMAT_2M16;
	} else if ((pwic.dwFormats) & WAVE_FORMAT_1M08) {
		cAudioFormat.m_iBitsPerSample = 8;
		cAudioFormat.m_iNumChannels = 1;
		cAudioFormat.m_iSamplesPerSeconds = 11025;
		cAudioFormat.m_dwWaveinSelected = WAVE_FORMAT_1M08;
	} else if ((pwic.dwFormats) & WAVE_FORMAT_1M16) {
		cAudioFormat.m_iBitsPerSample = 16;
		cAudioFormat.m_iNumChannels = 1;
		cAudioFormat.m_iSamplesPerSeconds = 11025;
		cAudioFormat.m_dwWaveinSelected = WAVE_FORMAT_1M16;
	} else if ((pwic.dwFormats) & WAVE_FORMAT_1S08) {
		cAudioFormat.m_iBitsPerSample = 8;
		cAudioFormat.m_iNumChannels = 2;
		cAudioFormat.m_iSamplesPerSeconds = 11025;
		cAudioFormat.m_dwWaveinSelected = WAVE_FORMAT_1S08;
	} else if ((pwic.dwFormats) & WAVE_FORMAT_1S16) {
		cAudioFormat.m_iBitsPerSample = 16;
		cAudioFormat.m_iNumChannels = 2;
		cAudioFormat.m_iSamplesPerSeconds = 11025;
		cAudioFormat.m_dwWaveinSelected = WAVE_FORMAT_1S16;
	} else if ((pwic.dwFormats) & WAVE_FORMAT_4M08) {
		cAudioFormat.m_iBitsPerSample = 8;
		cAudioFormat.m_iNumChannels = 1;
		cAudioFormat.m_iSamplesPerSeconds = 44100;
		cAudioFormat.m_dwWaveinSelected = WAVE_FORMAT_4M08;
	} else if ((pwic.dwFormats) & WAVE_FORMAT_4M16) {
		cAudioFormat.m_iBitsPerSample = 16;
		cAudioFormat.m_iNumChannels = 1;
		cAudioFormat.m_iSamplesPerSeconds = 44100;
		cAudioFormat.m_dwWaveinSelected = WAVE_FORMAT_4M16;
	} else if ((pwic.dwFormats) & WAVE_FORMAT_4S08) {
		cAudioFormat.m_iBitsPerSample = 8;
		cAudioFormat.m_iNumChannels = 2;
		cAudioFormat.m_iSamplesPerSeconds = 44100;
		cAudioFormat.m_dwWaveinSelected = WAVE_FORMAT_4S08;
	} else if ((pwic.dwFormats) & WAVE_FORMAT_4S16) {
		cAudioFormat.m_iBitsPerSample = 16;
		cAudioFormat.m_iNumChannels = 2;
		cAudioFormat.m_iSamplesPerSeconds = 44100;
		cAudioFormat.m_dwWaveinSelected = WAVE_FORMAT_4S16;
	} else {
		//Arbitrarily choose one
		cAudioFormat.m_iBitsPerSample = 8;
		cAudioFormat.m_iNumChannels = 1;
		cAudioFormat.m_iSamplesPerSeconds = 22050;
		cAudioFormat.m_dwWaveinSelected = WAVE_FORMAT_2M08;
	}

	//Build m_Format
	BuildRecordingFormat();
}

void SuggestCompressFormat()
{
	cAudioFormat.m_bCompression = TRUE;

	AllocCompressFormat();

	//1st try MPEGLAYER3
	BuildRecordingFormat();
	MMRESULT mmr;
	if ((m_Format.nSamplesPerSec == 22050) && (m_Format.nChannels==2) && (m_Format.wBitsPerSample <= 16)) {
		pwfx->wFormatTag = WAVE_FORMAT_MPEGLAYER3;
		mmr = acmFormatSuggest(NULL, &m_Format, pwfx, cAudioFormat.m_dwCbwFX, ACM_FORMATSUGGESTF_WFORMATTAG);
	}

	if (mmr!=0) {
		//ver 1.6, use PCM if MP3 not available

		//Then try ADPCM
		//BuildRecordingFormat();
		//pwfx->wFormatTag = WAVE_FORMAT_ADPCM;
		//MMRESULT mmr = acmFormatSuggest(NULL, &m_Format, pwfx, dwCbwFX, ACM_FORMATSUGGESTF_WFORMATTAG);

		if (mmr!=0) {
			//Use the PCM as default
			BuildRecordingFormat();
			pwfx->wFormatTag = WAVE_FORMAT_PCM;
			MMRESULT mmr = acmFormatSuggest(NULL, &m_Format, pwfx, cAudioFormat.m_dwCbwFX, ACM_FORMATSUGGESTF_WFORMATTAG);

			if (mmr!=0) {
				cAudioFormat.m_bCompression = FALSE;
			}
		}
	}
}

//MMRESULT IsFormatSupported(LPWAVEFORMATEX pwfx, UINT uDeviceID);
//MMRESULT IsFormatSupported(LPWAVEFORMATEX pwfx, UINT uDeviceID)
//{
//	return (waveInOpen(
//		NULL, // ptr can be NULL for query
//		uDeviceID, // the device identifier
//		pwfx, // defines requested format
//		NULL, // no callback
//		NULL, // no instance data
//		WAVE_FORMAT_QUERY)); // query only, do not open device
//}

// Ver 1.1
// ========================================
// Merge Audio and Video File Function
// ========================================
//
// No recompression is applied to the Video File
// Optional Recompression is applied to the Audio File
// Assuming audio_recompress_format is compatible with the existing format of the audio file
//
// If recompress audio is set to FALSE, both audio_recompress_format and audio_format_size can be NULL
// ========================================
int Merge_Video_And_Sound_File(CString input_video_path, CString input_audio_path, CString output_avi_path, BOOL recompress_audio, LPWAVEFORMATEX audio_recompress_format, DWORD audio_format_size, BOOL bInterleave, int interleave_factor, int interleave_unit)
{
	AVIFileInit();

	//Open Video and Audio Files
	PAVIFILE pfileVideo = NULL;
	HRESULT hr = AVIFileOpen(&pfileVideo, LPCTSTR(input_video_path), OF_READ | OF_SHARE_DENY_NONE, 0L);
	if (hr != 0) {
		//MessageBox(NULL,"Unable to open video file.","Note",MB_OK | MB_ICONEXCLAMATION);
		MessageOut(NULL,IDS_STRING_NOOPENVIDEO,IDS_STRING_NOTE,MB_OK | MB_ICONEXCLAMATION);
		return 1;
	}

	PAVISTREAM AviStream[NUMSTREAMS];					// the editable streams

	//Get Video Stream from Video File and Audio Stream from Audio File

	// ==========================================================
	// Important Assumption
	// Assume stream 0 is the correct stream in the files
	// ==========================================================
	if (pfileVideo) {
		PAVISTREAM pavi;
		if (AVIFileGetStream(pfileVideo, &pavi, streamtypeVIDEO, 0) != AVIERR_OK) {
			AVIFileRelease(pfileVideo);
			//MessageBox(NULL,"Unable to open video stream.","Note",MB_OK | MB_ICONEXCLAMATION);
			MessageOut(NULL,IDS_STRING_NOOPENSREAM,IDS_STRING_NOTE,MB_OK | MB_ICONEXCLAMATION);
			return 1;
		}

		//Set editable stream number as 0
		if (CreateEditableStream(&AviStream[0], pavi) != AVIERR_OK) {
			AVIStreamRelease(pavi);
			AVIFileRelease(pfileVideo);

			//MessageBox(NULL,"Unable to create editable video stream.","Note",MB_OK | MB_ICONEXCLAMATION);
			MessageOut(NULL, IDS_STRING_NOCREATESTREAM,IDS_STRING_NOTE,MB_OK | MB_ICONEXCLAMATION);
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
		if (AVIStreamOpenFromFile(&pavi,input_audio_path,streamtypeAUDIO,0,OF_READ | OF_SHARE_DENY_NONE,NULL)!=AVIERR_OK) {
			AVIStreamRelease(AviStream[0]);
			//MessageBox(NULL,"Unable to open audio stream.","Note",MB_OK | MB_ICONEXCLAMATION);
			MessageOut(NULL,IDS_STRING_NOOPENAUDIO,IDS_STRING_NOTE,MB_OK | MB_ICONEXCLAMATION);

			return 2;
		}

		//Set editable stream number as 1
		if (CreateEditableStream(&AviStream[1], pavi) != AVIERR_OK) {
			AVIStreamRelease(pavi);
			AVIStreamRelease(AviStream[0]);
			//MessageBox(NULL,"Unable to create editable audio stream.","Note",MB_OK | MB_ICONEXCLAMATION);
			MessageOut(NULL,IDS_STRING_NOEDITAUDIO,IDS_STRING_NOTE,MB_OK | MB_ICONEXCLAMATION);

			return 2;
		}
		AVIStreamRelease(pavi);
	}

	//Verifying streams are of the correct type
	AVISTREAMINFO avis[NUMSTREAMS];

	AVIStreamInfo(AviStream[0], &avis[0], sizeof(avis[0]));
	AVIStreamInfo(AviStream[1], &avis[1], sizeof(avis[1]));

	//Assert that the streams we are going to work with are correct in our assumption
	//such that stream 0 is video and stream 1 is audio

	if (avis[0].fccType != streamtypeVIDEO) {
		//MessageBox(NULL,"Unable to verify video stream.","Note",MB_OK | MB_ICONEXCLAMATION);
		MessageOut(NULL,IDS_STRING_NOVERIFYVIDEO,IDS_STRING_NOTE,MB_OK | MB_ICONEXCLAMATION);

		AVIStreamRelease(AviStream[0]);
		AVIStreamRelease(AviStream[1]);
		return 3;
	}

	if (avis[1].fccType != streamtypeAUDIO) {
		//MessageBox(NULL,"Unable to verify audio stream.","Note",MB_OK | MB_ICONEXCLAMATION);
		MessageOut(NULL,IDS_STRING_NOVERIFYAUDIO,IDS_STRING_NOTE,MB_OK | MB_ICONEXCLAMATION);

		AVIStreamRelease(AviStream[0]);
		AVIStreamRelease(AviStream[1]);
		return 4;
	}

	// AVISaveV code takes a pointer to compression opts
	AVICOMPRESSOPTIONS gaAVIOptions[NUMSTREAMS];		// compression options
	LPAVICOMPRESSOPTIONS galpAVIOptions[NUMSTREAMS];
	galpAVIOptions[0] = &gaAVIOptions[0];
	galpAVIOptions[1] = &gaAVIOptions[1];

	// clear options structure to zeroes
	_fmemset(galpAVIOptions[0], 0, sizeof(AVICOMPRESSOPTIONS));
	_fmemset(galpAVIOptions[1], 0, sizeof(AVICOMPRESSOPTIONS));

	//=========================================
	//Set Video Stream Compress Options
	//=========================================

	//No Recompression
	galpAVIOptions[0]->fccType = streamtypeVIDEO;
	galpAVIOptions[0]->fccHandler = 0;
	galpAVIOptions[0]->dwFlags = AVICOMPRESSF_VALID | AVICOMPRESSF_KEYFRAMES | AVICOMPRESSF_DATARATE;
	if (bInterleave) {
		galpAVIOptions[0]->dwFlags = galpAVIOptions[0]->dwFlags | AVICOMPRESSF_INTERLEAVE;
	}
	galpAVIOptions[0]->dwKeyFrameEvery = (DWORD) -1;
	galpAVIOptions[0]->dwQuality = (DWORD)ICQUALITY_DEFAULT;
	galpAVIOptions[0]->dwBytesPerSecond = 0;

	if (interleave_unit==FRAMES) {
		galpAVIOptions[0]->dwInterleaveEvery = interleave_factor;
	} else {
		double interfloat = (((double) cAudioFormat.m_iInterleaveFactor) * ((double) cVideoOpts.m_iFramesPerSecond))/1000.0;
		int interint = (int) interfloat;
		if (interint<=0) {
			interint = 1;
		}

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
	if (bInterleave) {
		galpAVIOptions[1]->dwFlags = galpAVIOptions[1]->dwFlags | AVICOMPRESSF_INTERLEAVE;
	}
	galpAVIOptions[1]->dwKeyFrameEvery = 0;
	galpAVIOptions[1]->dwQuality = 0;
	galpAVIOptions[1]->dwBytesPerSecond = 0;

	if (interleave_unit==FRAMES) {
		galpAVIOptions[1]->dwInterleaveEvery = interleave_factor;
	} else {
		//back here
		double interfloat = (((double) cAudioFormat.m_iInterleaveFactor) * ((double) cVideoOpts.m_iFramesPerSecond))/1000.0;
		int interint = (int) interfloat;
		if (interint<=0) {
			interint = 1;
		}

		galpAVIOptions[1]->dwInterleaveEvery = interint;
	}
	//galpAVIOptions[1]->dwInterleaveEvery = interleave_factor;
	//galpAVIOptions[1]->cbParms = 0;

	if (recompress_audio) {
		galpAVIOptions[1]->cbFormat = audio_format_size;
		galpAVIOptions[1]->lpFormat = GlobalAllocPtr(GHND, audio_format_size);
		memcpy( (void *) galpAVIOptions[1]->lpFormat, (void *) audio_recompress_format, audio_format_size);
	} else {
		LONG lTemp;
		AVIStreamReadFormat(AviStream[1], AVIStreamStart(AviStream[1]), NULL, &lTemp);
		galpAVIOptions[1]->cbFormat = lTemp;

		if (lTemp) {
			galpAVIOptions[1]->lpFormat = GlobalAllocPtr(GHND, lTemp);
		}
		// Use existing format as compress format
		if (galpAVIOptions[1]->lpFormat) {
			AVIStreamReadFormat(AviStream[1], AVIStreamStart(AviStream[1]),galpAVIOptions[1]->lpFormat, &lTemp);
		}
	}

	// ============================
	// Do the work! Merging
	// ============================

	//Save fccHandlers
	DWORD fccHandler[NUMSTREAMS];
	fccHandler[0] = galpAVIOptions[0]->fccHandler;
	fccHandler[1] = galpAVIOptions[1]->fccHandler;

	hr = AVISaveV(LPCTSTR(output_avi_path), NULL, (AVISAVECALLBACK) SaveCallback, NUMSTREAMS, AviStream, galpAVIOptions);
	//hr = AVISaveV(LPCTSTR(output_avi_path), NULL, (AVISAVECALLBACK) NULL, NUMSTREAMS, AviStream, galpAVIOptions);
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
			if (bInterleave) {
				galpAVIOptions[0]->dwFlags = galpAVIOptions[0]->dwFlags | AVICOMPRESSF_INTERLEAVE;
			}
			galpAVIOptions[0]->dwKeyFrameEvery = (DWORD) -1;
			galpAVIOptions[0]->dwQuality = (DWORD)ICQUALITY_DEFAULT;
			galpAVIOptions[0]->dwBytesPerSecond = 0;
			galpAVIOptions[0]->dwInterleaveEvery = interleave_factor;

			galpAVIOptions[1]->fccType = streamtypeAUDIO;
			galpAVIOptions[1]->fccHandler = 0;
			galpAVIOptions[1]->dwFlags = AVICOMPRESSF_VALID;
			if (bInterleave) {
				galpAVIOptions[1]->dwFlags = galpAVIOptions[1]->dwFlags | AVICOMPRESSF_INTERLEAVE;
			}
			galpAVIOptions[1]->dwKeyFrameEvery = 0;
			galpAVIOptions[1]->dwQuality = 0;
			galpAVIOptions[1]->dwBytesPerSecond = 0;
			galpAVIOptions[1]->dwInterleaveEvery = interleave_factor;

			//Use default audio format
			LONG lTemp;
			AVIStreamReadFormat(AviStream[1], AVIStreamStart(AviStream[1]), NULL, &lTemp);
			galpAVIOptions[1]->cbFormat = lTemp;
			if (lTemp) {
				galpAVIOptions[1]->lpFormat = GlobalAllocPtr(GHND, lTemp);
			}
			// Use existing format as compress format
			if (galpAVIOptions[1]->lpFormat) {
				AVIStreamReadFormat(AviStream[1], AVIStreamStart(AviStream[1]),galpAVIOptions[1]->lpFormat, &lTemp);
			}

			//Do the Work .... Merging
			hr = AVISaveV(LPCTSTR(output_avi_path), NULL, (AVISAVECALLBACK) NULL, NUMSTREAMS, AviStream, galpAVIOptions);

			if (hr != AVIERR_OK) {
				AVISaveOptionsFree(NUMSTREAMS,galpAVIOptions);
				AVIStreamRelease(AviStream[0]);
				AVIStreamRelease(AviStream[1]);

				//MessageBox(NULL,"Unable to merge audio and video streams (1).","Note",MB_OK | MB_ICONEXCLAMATION);
				MessageOut(NULL,IDS_STRING_NOMERGE1,IDS_STRING_NOTE,MB_OK | MB_ICONEXCLAMATION);

				return 5;
			}

			//Succesful Merging, but with no audio recompression
			//MessageBox(NULL,"Unable to apply audio compression with the selected options. Your movie is saved without audio compression.","Note",MB_OK | MB_ICONEXCLAMATION);
			MessageOut(NULL,IDS_STRING_NOAUDIOCOMPRESS,IDS_STRING_NOTE,MB_OK | MB_ICONEXCLAMATION);
		} else {
			// if recompress audio retry
			AVISaveOptionsFree(NUMSTREAMS,galpAVIOptions);
			AVIStreamRelease(AviStream[0]);
			AVIStreamRelease(AviStream[1]);
			//MessageBox(NULL,"Unable to audio and video merge streams (2).","Note",MB_OK | MB_ICONEXCLAMATION);
			MessageOut(NULL,IDS_STRING_NOMERGE2,IDS_STRING_NOTE,MB_OK | MB_ICONEXCLAMATION);

			return 5;
		}
	}

	// Restore fccHandlers
	galpAVIOptions[0]->fccHandler = fccHandler[0];
	galpAVIOptions[1]->fccHandler = fccHandler[1];

	//Set Title Bar
	HWND mainwnd = AfxGetApp()->m_pMainWnd->m_hWnd;
	if (mainwnd) {
		::SetWindowText(mainwnd,"CamStudio - Custom Build");
	}

	AVISaveOptionsFree(NUMSTREAMS,galpAVIOptions);

	// Free Editable Avi Streams
	for (int i = 0; i < NUMSTREAMS; i++) {
		if (AviStream[i]) {
			AVIStreamRelease(AviStream[i]);
			AviStream[i] = NULL;
		}
	}

	AVIFileExit();

	return 0;
}

BOOL CALLBACK SaveCallback(int iProgress)
{
	//Set Progress in Title Bar

	//char szText[300];
	//wsprintf(szText, "Compressing Audio %d%%", iProgress);

	CString szText, fmtstr;
	fmtstr.LoadString(IDS_STRING_COMPRESSINGAUDIO);
	szText.Format(LPCTSTR(fmtstr),iProgress);

	HWND mainwnd = AfxGetApp()->m_pMainWnd->m_hWnd;
	if (mainwnd) {
		//::SetWindowText(mainwnd, szText);
		::SetWindowText(mainwnd, LPCTSTR(szText));
	}

	return WinYield();
	//return FALSE;
}

//Ver 1.2
//These functions are used by the VideoOptions dialog to manipulate the pVideoCompressParams variable
//The pVideoCompressParams variable is used in AVICompress options
void FreeVideoCompressParams()
{
	if (pVideoCompressParams) {
		GlobalFreePtr(pVideoCompressParams);
		pVideoCompressParams = NULL;
	}
}

BOOL AllocVideoCompressParams(DWORD paramsSize)
{
	if (pVideoCompressParams) {
		FreeVideoCompressParams();	//Free Existing
	}

	pVideoCompressParams = (LPVOID) GlobalAllocPtr(GHND, paramsSize);
	if (NULL == pVideoCompressParams) {
		//CString msgstr;
		//msgstr.Format("GlobalAllocPtr(%lu) for Video Compress Parameters failed!", paramsSize);
		//::MessageBox(NULL,msgstr,"Note", MB_OK | MB_ICONEXCLAMATION);

		MessageOut(NULL,IDS_STRING_GALLOCVCM,IDS_STRING_NOTE,MB_OK | MB_ICONEXCLAMATION);

		return FALSE;
	}

	return TRUE;
}

void FreeParamsUse()
{
	if (pParamsUse) {
		GlobalFreePtr(pParamsUse);
		pParamsUse = NULL;
	}
}

BOOL MakeCompressParamsCopy(DWORD paramsSize, LPVOID pOrg)
{
	if (pParamsUse) {
		//Free Existing
		FreeParamsUse();
	}

	pParamsUse = (LPVOID) GlobalAllocPtr(GHND, paramsSize);
	if (NULL == pParamsUse) {
		//::MessageBox(NULL,"Failure allocating Video Params or compression","Note", MB_OK | MB_ICONEXCLAMATION);
		MessageOut(NULL,IDS_STRING_FAILALLOCVCM,IDS_STRING_NOTE,MB_OK | MB_ICONEXCLAMATION);

		return FALSE;
	}

	memcpy(pParamsUse,pOrg,paramsSize);
	return TRUE;
}

void GetVideoCompressState (HIC hic, DWORD fccHandler)
{
	DWORD statesize = ICGetStateSize(hic);
	if (statesize <= 0) {
		return;
	}

	if (AllocVideoCompressParams(statesize)) {
		//ICGetState returns statesize even if pVideoCompressParams is not NULL ??
		DWORD ret = ICGetState(hic,(LPVOID) pVideoCompressParams,statesize);
		if (ret < 0) {
			//CString reportstr;
			//reportstr.Format("Failure in getting compressor state ! Error Value = %d", ret);
			//MessageBox(NULL,reportstr,"Note",MB_OK | MB_ICONEXCLAMATION);
			MessageOut(NULL,IDS_STRING_COMPRESSORSTATE,IDS_STRING_NOTE,MB_OK | MB_ICONEXCLAMATION,ret);
		} else {
			//we store only the state for one compressor in pVideoCompressParams
			//So we need to indicate which compressor the state is referring to
			cVideoOpts.m_dwCompressorStateIsFor = fccHandler;
			cVideoOpts.m_dwCompressorStateSize = statesize;
		}
	}
}

void SetVideoCompressState (HIC hic, DWORD fccHandler)
{
	if (cVideoOpts.m_dwCompressorStateIsFor == fccHandler) {
		if (pVideoCompressParams) {
			DWORD ret = ICSetState(hic,(LPVOID) pVideoCompressParams,cVideoOpts.m_dwCompressorStateSize);
			//if (ret <= 0) {
			if (ret < 0) {
				//MessageBox(NULL, "Failure in setting compressor state !","Note",MB_OK | MB_ICONEXCLAMATION);
				MessageOut(NULL,IDS_STRING_SETCOMPRESSOR,IDS_STRING_NOTE,MB_OK | MB_ICONEXCLAMATION);
			}
		}
	}
}

//ver .12
void AttemptRecordingFormat()
{
	WAVEINCAPS pwic;
	MMRESULT mmr = waveInGetDevCaps(cAudioFormat.m_uDeviceID, &pwic, sizeof(pwic));
	if ((pwic.dwFormats) & cAudioFormat.m_dwWaveinSelected) {
		BuildRecordingFormat();
	} else {
		SuggestRecordingFormat();
	}
}

void AttemptCompressFormat()
{
	cAudioFormat.m_bCompression = TRUE;
	AllocCompressFormat();
	BuildRecordingFormat();
	//Test Compatibility
	MMRESULT mmr = acmFormatSuggest(NULL, &m_Format, pwfx, cAudioFormat.m_dwCbwFX, ACM_FORMATSUGGESTF_NCHANNELS | ACM_FORMATSUGGESTF_NSAMPLESPERSEC | ACM_FORMATSUGGESTF_WBITSPERSAMPLE | ACM_FORMATSUGGESTF_WFORMATTAG);
	if (mmr!=0) {
		SuggestCompressFormat();
	}
}

CString GetCodecDescription(long fccHandler)
{
	// TODO: initialization
	ICINFO compinfo;
	::ZeroMemory(&compinfo, sizeof(compinfo));
	compinfo.dwSize = sizeof(ICINFO);
	HIC hic = ICOpen(ICTYPE_VIDEO, fccHandler, ICMODE_QUERY);
	if (hic) {
		ICGetInfo(hic, &compinfo, sizeof(ICINFO));
		ICClose(hic);
	}

	return CString(compinfo.szDescription);
}

//ver 1.6
//ver 1.6 Capture waveout
//MCI functions
void mciRecordOpen()
{
	mop.dwCallback = (DWORD)hWndGlobal;
	mop.lpstrDeviceType = (LPCSTR)MCI_DEVTYPE_WAVEFORM_AUDIO;
	mop.lpstrElementName = TEXT("");
	mciSendCommand(0, MCI_OPEN, MCI_OPEN_TYPE | MCI_OPEN_TYPE_ID | MCI_OPEN_ELEMENT, (DWORD)&mop);
	isMciRecordOpen = 1;
}

void mciRecordStart()
{
	DWORD dwReturn = mciSendCommand(mop.wDeviceID, MCI_RECORD, 0, 0);
	if (dwReturn) {
		char buffer[300];
		mciGetErrorString(dwReturn, buffer, sizeof (buffer));
		MessageBox(NULL, buffer, "MCI_RECORD", MB_ICONEXCLAMATION | MB_OK);
	}
}

void mciRecordStop(CString strFile)
{
	msp.dwCallback = (DWORD)hWndGlobal;
	msp.lpfilename = LPCTSTR(strFile);

	DWORD dwReturn = mciSendCommand(mop.wDeviceID, MCI_STOP, MCI_WAIT, 0);
	//if (dwReturn) {
	//char buffer[300];
	//	mciGetErrorString(dwReturn, buffer, sizeof (buffer));
	//	MessageBox( NULL, buffer, "MCI_RECORD",MB_ICONEXCLAMATION | MB_OK);
	//}

	dwReturn = mciSendCommand(mop.wDeviceID, MCI_SAVE, MCI_WAIT | MCI_SAVE_FILE, (DWORD)&msp);
	//if (dwReturn) {
	//char buffer[300];
	//	mciGetErrorString(dwReturn, buffer, sizeof (buffer));
	//	MessageBox( NULL, buffer, "MCI_RECORD",MB_ICONEXCLAMATION | MB_OK);
	//}
}

void mciRecordPause(CString strFile)
{
	//can call this only in the same thread as the one opening the device?

	if (isMciRecordOpen==0)
		return;

	msp.dwCallback = (DWORD)hWndGlobal;
	msp.lpfilename = LPCTSTR(strFile);

	DWORD dwReturn = mciSendCommand(mop.wDeviceID, MCI_PAUSE, MCI_WAIT, 0);
	//if (dwReturn) {
	//char buffer[300];
	//mciGetErrorString(dwReturn, buffer, sizeof (buffer));
	//MessageBox( NULL, buffer, "MCI_RECORD",MB_ICONEXCLAMATION | MB_OK);
	//}
}

void mciRecordResume(CString strFile)
{
	//can call this only in the same thread as the one opening the device?

	msp.dwCallback = (DWORD)hWndGlobal;
	msp.lpfilename = strFile;

	if (isMciRecordOpen == 0) {
		return;
	}

	DWORD dwReturn = mciSendCommand(mop.wDeviceID, MCI_RESUME, MCI_WAIT, 0);
	//if (dwReturn) {
	//char buffer[300];
	//	mciGetErrorString(dwReturn, buffer, sizeof (buffer));
	//	MessageBox( NULL, buffer, "MCI_RECORD",MB_ICONEXCLAMATION | MB_OK);
	//}
}

void mciRecordClose()
{
	mciSendCommand(mop.wDeviceID, MCI_CLOSE, 0, 0);

	isMciRecordOpen = 0;
}

// The setting/suggesting of format for recording Speakers is deferred until recording starts
// Default is to use best settings avalable.
// TODO: 
void mciSetWaveFormat()
{
	char buffer[128];

	// Suggest 10 formats
	// TODO: wrap the mci api. 
	MCI_WAVE_SET_PARMS set_parms;
	DWORD dwReturn = -1;
	for (int i = 0; ((i < 10) && (dwReturn != 0)); i++)
	{
		SuggestSpeakerRecordingFormat(i);
		BuildSpeakerRecordingFormat();

		// Set PCM format of recording.
		::ZeroMemory(&set_parms, sizeof(set_parms));
		set_parms.wFormatTag		= m_FormatSpeaker.wFormatTag;
		set_parms.wBitsPerSample	= m_FormatSpeaker.wBitsPerSample;
		set_parms.nChannels			= m_FormatSpeaker.nChannels;
		set_parms.nSamplesPerSec	= m_FormatSpeaker.nSamplesPerSec;
		set_parms.nAvgBytesPerSec	= m_FormatSpeaker.nAvgBytesPerSec;
		set_parms.nBlockAlign		= m_FormatSpeaker.nBlockAlign;

		dwReturn = mciSendCommand(mop.wDeviceID, MCI_SET,
			MCI_WAIT
			| MCI_WAVE_SET_FORMATTAG
			| MCI_WAVE_SET_BITSPERSAMPLE
			| MCI_WAVE_SET_CHANNELS
			| MCI_WAVE_SET_SAMPLESPERSEC
			| MCI_WAVE_SET_AVGBYTESPERSEC
			| MCI_WAVE_SET_BLOCKALIGN,
			(DWORD)(LPVOID)&set_parms);
	}

	if (dwReturn) {
		mciGetErrorString(dwReturn, buffer, sizeof(buffer));
		CString tstr;
		tstr.LoadString(IDS_STRING_MCIWAVE);
		MessageBox(NULL, buffer, tstr,MB_ICONEXCLAMATION | MB_OK);
	}
}

// TODO: Put these into a collection
void SuggestSpeakerRecordingFormat(int i)
{
	//Ordered in preference of choice
	switch (i)
	{
	case 0:
		{
			iAudioBitsPerSampleSpeaker = 16;
			iAudioNumChannelsSpeaker = 2;
			iAudioSamplesPerSecondsSpeaker = 44100;
			//waveinselected_Speaker = WAVE_FORMAT_4S16;
			break;
		}
	case 1:
		{
			iAudioBitsPerSampleSpeaker = 16;
			iAudioNumChannelsSpeaker = 2;
			iAudioSamplesPerSecondsSpeaker = 22050;
			//waveinselected_Speaker = WAVE_FORMAT_2S16;
			break;
		}
	case 2:
		{
			iAudioBitsPerSampleSpeaker = 8;
			iAudioNumChannelsSpeaker = 2;
			iAudioSamplesPerSecondsSpeaker = 44100;
			//waveinselected_Speaker = WAVE_FORMAT_4S08;
			break;
		}
	case 3:
		{
			iAudioBitsPerSampleSpeaker = 8;
			iAudioNumChannelsSpeaker = 2;
			iAudioSamplesPerSecondsSpeaker = 22050;
			//waveinselected_Speaker = WAVE_FORMAT_2S08;
			break;
		}
	case 4:
		{
			iAudioBitsPerSampleSpeaker = 16;
			iAudioNumChannelsSpeaker = 1;
			iAudioSamplesPerSecondsSpeaker = 44100;
			//waveinselected_Speaker = WAVE_FORMAT_4M16;
			break;
		}
	case 5:
		{
			iAudioBitsPerSampleSpeaker = 8;
			iAudioNumChannelsSpeaker = 1;
			iAudioSamplesPerSecondsSpeaker = 44100;
			//waveinselected_Speaker = WAVE_FORMAT_4M08;
			break;
		}
	case 6:
		{
			iAudioBitsPerSampleSpeaker = 16;
			iAudioNumChannelsSpeaker = 1;
			iAudioSamplesPerSecondsSpeaker = 22050;
			//waveinselected_Speaker = WAVE_FORMAT_2M16;
			break;
		}
	case 7:
		{
			iAudioBitsPerSampleSpeaker = 16;
			iAudioNumChannelsSpeaker = 2;
			iAudioSamplesPerSecondsSpeaker = 11025;
			//waveinselected_Speaker = WAVE_FORMAT_1S16;
			break;
		}
	case 8:
		{
			iAudioBitsPerSampleSpeaker = 8;
			iAudioNumChannelsSpeaker = 1;
			iAudioSamplesPerSecondsSpeaker = 22050;
			//waveinselected_Speaker = WAVE_FORMAT_2M08;
			break;
		}
	case 9:
		{
			iAudioBitsPerSampleSpeaker = 8;
			iAudioNumChannelsSpeaker = 2;
			iAudioSamplesPerSecondsSpeaker = 11025;
			//waveinselected_Speaker = WAVE_FORMAT_1S08;
			break;
		}
	default :
		{
			iAudioBitsPerSampleSpeaker = 8;
			iAudioNumChannelsSpeaker = 1;
			iAudioSamplesPerSecondsSpeaker = 11025;
			//waveinselected_Speaker = WAVE_FORMAT_1M08;
		}
	}
}

// copy speaker format settins from global variables.
// TOSO: encapsulate WAVEFORMATEX and provide a copy op and assignment op
// for a structure that holds the global variables.

void BuildSpeakerRecordingFormat()
{
	m_FormatSpeaker.wFormatTag		= WAVE_FORMAT_PCM;
	m_FormatSpeaker.wBitsPerSample	= iAudioBitsPerSampleSpeaker;
	m_FormatSpeaker.nSamplesPerSec	= iAudioSamplesPerSecondsSpeaker;
	m_FormatSpeaker.nChannels		= iAudioNumChannelsSpeaker;
	m_FormatSpeaker.nBlockAlign		= m_FormatSpeaker.nChannels * (m_FormatSpeaker.wBitsPerSample/8);
	m_FormatSpeaker.nAvgBytesPerSec	= m_FormatSpeaker.nSamplesPerSec * m_FormatSpeaker.nBlockAlign;
	m_FormatSpeaker.cbSize			= 0;
}

//LPBITMAPINFO GetTextBitmap(CDC *thisDC, CRect* caprect,int factor,CRect* drawtextRect, LOGFONT *drawfont, CString textstr, CPen* pPen, CBrush * pBrush, COLORREF textcolor, int horzalign)
//{
//
//	int left =caprect->left;
//	int top = caprect->top;
//	int width = caprect->Width();
//	int height = caprect->Height();
//	int orig_width = width;
//	int orig_height = height;
//
//	width *= factor;
//	height *= factor;
//
//	CRect usetextRect = *drawtextRect;
//	usetextRect.left *= factor;
//	usetextRect.top *= factor;
//	usetextRect.right *= factor;
//	usetextRect.bottom *= factor;
//
//	HDC hMemDC = ::CreateCompatibleDC(thisDC->m_hDC);
//
//	CDC* pMemDC = CDC::FromHandle(hMemDC);
//
//	HBITMAP hbm;
//	hbm = CreateCompatibleBitmap(thisDC->m_hDC, width, height);
//	HBITMAP oldbm = (HBITMAP) SelectObject(hMemDC, hbm);
//
//	//Get Background
//	::StretchBlt(hMemDC, 0, 0, width, height, thisDC->m_hDC, left, top, orig_width,orig_height,SRCCOPY);
//
//	CPen* oldPen;
//	CBrush* oldBrush;
//	CFont dxfont, *oldfont;
//
//	if (drawfont) {
//
//		LOGFONT newlogFont = *drawfont;
//		newlogFont.lfWidth *= factor;
//		newlogFont.lfHeight *= factor;
//
//		//need CreateFontIndirectW ?
//		dxfont.CreateFontIndirect(&newlogFont);
//		oldfont = (CFont *) pMemDC->SelectObject(&dxfont);
//	}
//	//if no default font is selected, can cause errors
//
//	if (pPen)
//		oldPen = pMemDC->SelectObject(pPen);
//
//	if (pBrush)
//		oldBrush = pMemDC->SelectObject(pBrush);
//
//	int textlength = textstr.GetLength(); //get number of bytes
//
//	//Draw Text
//
//	SetBkMode(hMemDC,TRANSPARENT);
//	SetTextColor(hMemDC,textcolor);
//	//DrawTextEx(hMemDC, (char *)LPCTSTR(textstr), textlength, LPRECT(usetextRect), horzalign | DT_VCENTER | DT_WORDBREAK | DT_EDITCONTROL, NULL);
//
//	//use adaptive antialias...if size< than maxxScreen maxyScreen
//	if ((versionOp>=5) && ((usetextRect.Width()>maxxScreen) || (usetextRect.Height()>maxyScreen)))
//	{ //use stroke path method, less buggy
//
//		BeginPath(hMemDC);
//		DrawTextEx(hMemDC, (char *)LPCTSTR(textstr), textlength, LPRECT(usetextRect), horzalign | DT_VCENTER | DT_WORDBREAK, NULL);
//		EndPath(hMemDC);
//
//		//CPen testpen;
//		//testpen.CreatePen(PS_SOLID,0,textcolor);
//		//oldPen = pMemDC->SelectObject(&testpen);
//
//		CBrush testbrush;
//		testbrush.CreateSolidBrush(textcolor);
//		oldBrush = pMemDC->SelectObject(&testbrush);
//		//StrokeAndFillPath(hMemDC);
//		FillPath(hMemDC);
//		pMemDC->SelectObject(oldBrush);
//		//pMemDC->SelectObject(oldPen);
//
//		testbrush.DeleteObject();
//		//testpen.DeleteObject();
//
//	}
//	else
//		DrawTextEx(hMemDC, (char *)LPCTSTR(textstr), textlength, LPRECT(usetextRect), horzalign | DT_VCENTER | DT_WORDBREAK, NULL);
//
//	if (pBrush)
//		pMemDC->SelectObject(oldBrush);
//
//	if (pPen)
//		pMemDC->SelectObject(oldPen);
//
//	if (drawfont) {
//
//		//no need to destroy dcfont?
//		//assume the destructor of Cfont will take care of freeing dxfont resources
//		pMemDC->SelectObject(oldfont);
//		dxfont.DeleteObject();
//	}
//
//	SelectObject(hMemDC,oldbm);
//
//	LPBITMAPINFOHEADER pBM_HEADER = (LPBITMAPINFOHEADER)GlobalLock(Bitmap2Dib(hbm, 24));
//
//	if (pBM_HEADER == NULL) {
//		return NULL;
//	}
//
//	pMemDC->Detach();
//
//	DeleteObject(hbm);
//	DeleteDC(hMemDC);
//
//	return (LPBITMAPINFO) pBM_HEADER;
//
//}
//
////AntiAlias 24 Bit Image
////valid factors : 1, 2, 3
//HBITMAP DrawResampleRGB(CDC *thisDC, CRect* caprect,int factor, LPBITMAPINFOHEADER expanded_bmi, int xmove, int ymove)
//{
//
//	int iBits = 24;
//
//	LONG Width = expanded_bmi->biWidth;
//	LONG Height = expanded_bmi->biHeight;
//	long Rowbytes = (Width*iBits+31)/32 *4;
//
//	long reduced_width = Width/factor;
//	long reduced_height = Height/factor;
//	long reduced_rowbytes = (reduced_width*iBits+31)/32 *4;
//
//	if ((factor<1) || (factor>3))
//		return NULL;
//
//	//Create destination buffer
//	//long dwSize = sizeof(BITMAPINFOHEADER) + reduced_rowbytes * reduced_height * 3;
//
//	// Allocate room for a DIB and set the LPBI fields
//	LPBITMAPINFOHEADER smallbi = (LPBITMAPINFOHEADER)GlobalLock(AllocMakeDib( reduced_width, reduced_height, 24));
//	if (smallbi == NULL) {
//		return NULL;
//	}
//
//	// Get the iBits from the bitmap and stuff them after the LPBI
//	LPBYTE lpBits = (LPBYTE)(smallbi+1);
//
//	//Perform the re-sampling
//	long x,y,z;
//	LPBYTE reduced_ptr;
//	LPBYTE Ptr;
//
//	LPBYTE reduced_rowptr = lpBits;
//	LPBYTE Rowptr = (LPBYTE) (expanded_bmi + 1);
//
//	//Set the pointers
//	reduced_ptr = lpBits;
//	Ptr = (LPBYTE) (expanded_bmi + 1);
//
//	int Ptr_incr = (factor-1)*3;
//	int Row_incr = Rowbytes * factor;
//
//	int totalval;
//
//	for (y=0;y< reduced_height;y++) {
//
//		//Set to start of each row
//		reduced_ptr = reduced_rowptr;
//		Ptr = Rowptr;
//
//		for (x=0;x< reduced_width;x++) {
//
//			//Ptr_Pixel = Ptr;
//
//			//for each RGB component
//			for (z=0;z<3;z++) {
//
//				if (factor==1)
//					*reduced_ptr = *Ptr;
//				else if (factor==2) {
//
//					totalval = 0;
//					totalval += *Ptr;
//					totalval += *(Ptr + 3);
//					totalval += *(Ptr + Rowbytes);
//					totalval += *(Ptr + Rowbytes + 3);
//					totalval/=4;
//
//					if (totalval<0)
//						totalval = 0;
//
//					if (totalval>255)
//						totalval = 255;
//
//					*reduced_ptr = (BYTE) totalval;
//
//				}
//				else if (factor==3) {
//
//					totalval = 0;
//					totalval += *Ptr;
//					totalval += *(Ptr + 3);
//					totalval += *(Ptr + 6);
//
//					totalval += *(Ptr + Rowbytes);
//					totalval += *(Ptr + Rowbytes + 3);
//					totalval += *(Ptr + Rowbytes + 6);
//
//					totalval += *(Ptr + Rowbytes + Rowbytes);
//					totalval += *(Ptr + Rowbytes + Rowbytes + 3);
//					totalval += *(Ptr + Rowbytes + Rowbytes + 6);
//
//					totalval/=9;
//
//					if (totalval<0)
//						totalval = 0;
//
//					if (totalval>255)
//						totalval = 255;
//
//					*reduced_ptr = (BYTE) totalval;
//
//				} //else if factor
//
//				reduced_ptr++;
//				Ptr ++;
//
//			} //for z
//
//			Ptr += Ptr_incr;
//			//Ptr += factor * 3;
//
//		} //for x
//
//		reduced_rowptr += reduced_rowbytes;
//		//Rowptr += Rowbytes;
//		Rowptr += Row_incr;
//
//	} // for y
//
//	int ret = StretchDIBits ( thisDC->m_hDC,
//		xmove, ymove,reduced_width,reduced_height,
//		0, 0,reduced_width,reduced_height,
//		lpBits, (LPBITMAPINFO)smallbi,
//		DIB_RGB_COLORS,SRCCOPY);
//
//	if (smallbi)
//		GlobalFreePtr(smallbi);
//
//	HBITMAP newbm = NULL;
//	return newbm;
//
//}

/////////////////////////////////////////////////////////////////////////////
// CRecorderView

UINT CRecorderView::WM_USER_RECORDINTERRUPTED = ::RegisterWindowMessage(WM_USER_RECORDINTERRUPTED_MSG);
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
	ON_COMMAND(ID_FILE_VIDEOOPTIONS, OnFileVideooptions)
	ON_COMMAND(ID_OPTIONS_CURSOROPTIONS, OnOptionsCursoroptions)	
	ON_COMMAND(ID_OPTIONS_ATUOPANSPEED, OnOptionsAtuopanspeed)
	ON_COMMAND(ID_REGION_FULLSCREEN, OnRegionFullscreen)
	ON_UPDATE_COMMAND_UI(ID_REGION_FULLSCREEN, OnUpdateRegionFullscreen)
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
	ON_COMMAND(ID_AVISWF, OnAviswf)
	ON_COMMAND(ID_OPTIONS_NAMING_AUTODATE, OnOptionsNamingAutodate)
	ON_UPDATE_COMMAND_UI(ID_OPTIONS_NAMING_AUTODATE, OnUpdateOptionsNamingAutodate)
	ON_UPDATE_COMMAND_UI(ID_OPTIONS_LANGUAGE_ENGLISH, OnUpdateOptionsLanguageEnglish)
	ON_UPDATE_COMMAND_UI(ID_OPTIONS_LANGUAGE_GERMAN, OnUpdateOptionsLanguageGerman)
	ON_COMMAND(ID_OPTIONS_LANGUAGE_ENGLISH, OnOptionsLanguageEnglish)
	ON_COMMAND(ID_OPTIONS_LANGUAGE_GERMAN, OnOptionsLanguageGerman)
	ON_COMMAND(ID_REGION_WINDOW, OnRegionWindow)
	ON_UPDATE_COMMAND_UI(ID_REGION_WINDOW, OnUpdateRegionWindow)
	ON_COMMAND(ID_ANNOTATION_ADDSYSTEMTIMESTAMP, OnAnnotationAddsystemtimestamp)
	ON_UPDATE_COMMAND_UI(ID_ANNOTATION_ADDSYSTEMTIMESTAMP, OnUpdateAnnotationAddsystemtimestamp)
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
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
	ON_REGISTERED_MESSAGE(CRecorderView::WM_USER_RECORDSTART, OnRecordStart)
	ON_REGISTERED_MESSAGE(CRecorderView::WM_USER_RECORDINTERRUPTED, OnRecordInterrupted)
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
}

CRecorderView::~CRecorderView()
{
}

BOOL CRecorderView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	// the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CRecorderView drawing

void CRecorderView::OnDraw(CDC* pDC)
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
//#pragma message("CRecorderView::LoadSettings skipped")
	LoadSettings();
	VERIFY(0 < SetAdjustHotKeys());

	CreateShiftWindow();

	HDC hScreenDC = ::GetDC(NULL);
	iBits = ::GetDeviceCaps(hScreenDC, BITSPIXEL);
	nColors = iBits;
	::ReleaseDC(NULL,hScreenDC);

	cVideoOpts.m_dwCompfccHandler = mmioFOURCC('M', 'S', 'V', 'C');

	hLogoBM = LoadBitmap( AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BITMAP3));

	CRect rect(0, 0, maxxScreen - 1, maxyScreen - 1);
	pFlashingWnd = new CFlashingWnd;
	pFlashingWnd->CreateFlashing("Flashing", rect);

	//Ver 1.2
	//strCursorDir default
	char dirx[_MAX_PATH];
	GetWindowsDirectory(dirx, _MAX_PATH);
	CString windir;
	windir.Format("%s\\cursors", dirx);
	CamCursor.Dir(windir);

	//savedir default
	savedir=GetProgPath();

	//Ver 1.1
	//SuggestRecordingFormat();
	//SuggestCompressFormat();

	//ver 1.6
	initialSaveMMMode();

	srand( (unsigned)time( NULL));

	return 0;
}

void CRecorderView::OnDestroy()
{
	CView::OnDestroy();

	DecideSaveSettings();

	UnSetHotKeys();

	DestroyShiftWindow();

	if (pCompressorInfo != NULL) {
		free(pCompressorInfo);
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

	if (pFlashingWnd) {
		pFlashingWnd->DestroyWindow();
		delete pFlashingWnd;
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
}

int tdata = 0;	// TODO: delete when thread fixed
LRESULT CRecorderView::OnRecordStart(UINT wParam, LONG lParam)
{
	TRACE("CRecorderView::OnRecordStart\n");
	CStatusBar* pStatus = (CStatusBar*) AfxGetApp()->m_pMainWnd->GetDescendantWindow(AFX_IDW_STATUS_BAR);
	pStatus->SetPaneText(0,"Press the Stop Button to stop recording");

	m_cCamera.Set(cCaptionOpts);
	m_cCamera.Set(cTimestampOpts);
	m_cCamera.Set(cWatermarkOpts);
	m_cCamera.Set(CamCursor);	

	//ver 1.2
	if (cProgramOpts.m_bMinimizeOnStart)
		::PostMessage(AfxGetMainWnd()->m_hWnd,WM_SYSCOMMAND,SC_MINIMIZE,0);

	//Check validity of rc and fix it
	FixRectSizePos(&rc, maxxScreen, maxyScreen);

	InstallMyHook(hWndGlobal,WM_USER_SAVECURSOR);

	bRecordState = true;
	interruptkey = 0;

	//CWinThread * pThread = AfxBeginThread(RecordAVIThread, &tdata);
	CWinThread * pThread = AfxBeginThread(RecordAVIThread, this);

	//Ver 1.3
	if (pThread)
		pThread->SetThreadPriority(cProgramOpts.m_iThreadPriority);

	//Ver 1.2
	bAllowNewRecordStartKey = TRUE; //allow this only after bRecordState is set to 1

	return 0;
}

LRESULT CRecorderView::OnRecordInterrupted (UINT wParam, LONG lParam)
{
	UninstallMyHook(hWndGlobal);

	//Ver 1.1
	if (recordpaused) {
		recordpaused = 0;
		SetTitleBar("CamStudio");
	}

	bRecordState = false;

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

LRESULT CRecorderView::OnSaveCursor (UINT wParam, LONG lParam)
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

void CRecorderView::OnRegionFullscreen()
{
	cRegionOpts.m_iMouseCaptureMode = CAPTURE_FULLSCREEN;
}

void CRecorderView::OnUpdateRegionFullscreen(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(cRegionOpts.isCaptureMode(CAPTURE_FULLSCREEN));
}

//This function is called when the avi saving is completed
LRESULT CRecorderView::OnUserGeneric (UINT wParam, LONG lParam)
{
	//ver 1.2
	::SetForegroundWindow( AfxGetMainWnd()->m_hWnd);
	AfxGetMainWnd()->ShowWindow(SW_RESTORE);

	//ver 1.2
	if (interruptkey == cHotKeyOpts.m_RecordCancel.m_vKey) {
		//if (interruptkey==VK_ESCAPE) {
		//Perform processing for cancel operation
		DeleteFile(strTempFilePath);
		if (!cAudioFormat.isInput(NONE))
			DeleteFile(tempaudiopath);
		return 0;

	}

	//Normal thread exit
	//Prompt the user for the filename

	char szFilter[100];
	char szTitle[100];
	char extFilter[30];

	//ver 2.26
	if (cProgramOpts.m_iRecordingMode == ModeAVI) {
		strcpy_s(szFilter,"AVI Movie Files (*.avi)|*.avi||");
		strcpy_s(szTitle,"Save AVI File");
		strcpy_s(extFilter,"*.avi");
	} else {
		strcpy_s(szFilter,"FLASH Movie Files (*.swf)|*.swf||");
		strcpy_s(szTitle,"Save SWF File");
		strcpy_s(extFilter,"*.swf");
	}

	CFileDialog fdlg(FALSE,extFilter,extFilter,OFN_LONGNAMES,szFilter,this);

	//static char BASED_CODE szFilter[] = "AVI Movie Files (*.avi)|*.avi||";
	//static char szTitle[]="Save AVI File";
	//CFileDialog fdlg(FALSE,"*.avi","*.avi",OFN_LONGNAMES,szFilter,this);

	fdlg.m_ofn.lpstrTitle=szTitle;

	if (savedir=="")
		savedir=GetProgPath();

	fdlg.m_ofn.lpstrInitialDir = savedir;

	CString m_newfile;
	CString m_newfileTitle;

	if ((cProgramOpts.m_iRecordingMode == ModeAVI) && cProgramOpts.m_bAutoNaming) {
		savedir=GetProgPath();

		//ErrMsg("hereh");

		time_t osBinaryTime; // C run-time time (defined in <time.h>)
		time( &osBinaryTime);

		CTime ctime(osBinaryTime);

		int day = ctime.GetDay();
		int month = ctime.GetMonth();
		int year = ctime.GetYear();
		int hour = ctime.GetHour();
		int minutes = ctime.GetMinute();
		int second = ctime.GetSecond();

		CString filestr;
		//filestr.Format("%d%d%d_%d%d",day,month,year,hour,minutes);
		filestr.Format("%d%d%d_%d%d_%d",day,month,year,hour,minutes,second);

		fdlg.m_ofn.lpstrInitialDir = savedir;

		m_newfile = savedir + "\\" + filestr + ".avi";
		m_newfileTitle = savedir + "\\" + filestr + ".avi";
		m_newfileTitle=m_newfileTitle.Left(m_newfileTitle.ReverseFind('\\'));
	} else {
		if (fdlg.DoModal() == IDOK) {
			m_newfile = fdlg.GetPathName();

			m_newfileTitle = fdlg.GetPathName();
			m_newfileTitle=m_newfileTitle.Left(m_newfileTitle.ReverseFind('\\'));
			savedir = m_newfileTitle;
		} else {
			DeleteFile(strTempFilePath);
			if (!cAudioFormat.isInput(NONE)) {
				DeleteFile(tempaudiopath);
			}
			return 0;
		}
	}

	//ver 2.26
	if (cProgramOpts.m_iRecordingMode == ModeFlash) {
		int lenx = m_newfile.GetLength();
		if (((m_newfile.GetAt(lenx-1) == 'f') || (m_newfile.GetAt(lenx-1) == 'F'))
			&& ((m_newfile.GetAt(lenx-2) == 'w') || (m_newfile.GetAt(lenx-2) == 'W'))
			&& ((m_newfile.GetAt(lenx-3) == 's') || (m_newfile.GetAt(lenx-3) == 'S'))
			&& (m_newfile.GetAt(lenx-4) == '.'))
		{
			m_newfile.SetAt(lenx-1,'i');
			m_newfile.SetAt(lenx-2,'v');
			m_newfile.SetAt(lenx-3,'a');
			m_newfile.SetAt(lenx-4,'.');
		} else {
			m_newfile += ".avi";
		}
	}

	//Ver 1.1
	if (cAudioFormat.m_iRecordAudio) {
		//Check if file exists and if so, does it allow overwite
		HANDLE hfile = CreateFile(m_newfile, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hfile == INVALID_HANDLE_VALUE) {
			//::MessageBox(NULL,"Unable to create new file. The file may be opened by another application. Please use another filename.","Note",MB_OK | MB_ICONEXCLAMATION);
			MessageOut(m_hWnd,IDS_STRING_NOCREATEWFILE,IDS_STRING_NOTE,MB_OK | MB_ICONEXCLAMATION);
			::PostMessage(hWndGlobal,WM_USER_GENERIC,0,0);
			return 0;
		}
		CloseHandle(hfile);
		DeleteFile(m_newfile);

		//ver 1.8
		if (vanWndCreated) {
			if (m_vanWnd.IsWindowVisible()) {
				//Otherwise, will slow down the merging significantly
				m_vanWnd.ShowWindow(SW_HIDE);
			}
		}

		int result;
		//Mergefile video with audio
		//if (iRecordAudio==2) {
		//if ((iRecordAudio==2) || (bUseMCI)) {
		//ver 2.26 ...overwrite audio settings for Flash Moe recording ... no compression used...
		if ((cAudioFormat.isInput(SPEAKERS)) || (cAudioFormat.m_bUseMCI) || (cProgramOpts.m_iRecordingMode == ModeFlash)) {
			result = Merge_Video_And_Sound_File(strTempFilePath, tempaudiopath, m_newfile, FALSE, pwfx, cAudioFormat.m_dwCbwFX,cAudioFormat.m_bInterleaveFrames,cAudioFormat.m_iInterleaveFactor, cAudioFormat.m_iInterleavePeriod);
		} else if (cAudioFormat.isInput(MICROPHONE)) {
			result = Merge_Video_And_Sound_File(strTempFilePath, tempaudiopath, m_newfile, cAudioFormat.m_bCompression, pwfx, cAudioFormat.m_dwCbwFX,cAudioFormat.m_bInterleaveFrames,cAudioFormat.m_iInterleaveFactor, cAudioFormat.m_iInterleavePeriod);
		}

		//Check Results : Attempt Recovery on error
		if (result==0) {
			//Successful
			DeleteFile(strTempFilePath);
			DeleteFile(tempaudiopath);
		} else if (result==1) { //video file broken
			//Unable to recover
			DeleteFile(strTempFilePath);
			DeleteFile(tempaudiopath);
		} else if (result==3) { //this case is rare
			//Unable to recover
			DeleteFile(strTempFilePath);
			DeleteFile(tempaudiopath);
		} else if ((result==2) || (result==4)) { //recover video file
			//video file is ok, but not audio file
			//so copy the video file as avi and ignore the audio
			if (!CopyFile( strTempFilePath,m_newfile,FALSE)) {
				//Although there is error copying, the temp file still remains in the temp directory and is not deleted, in case user wants a manual recover
				//MessageBox("File Creation Error. Unable to rename/copy file.","Note",MB_OK | MB_ICONEXCLAMATION);
				MessageOut(m_hWnd,IDS_STRING_FILECREATIONERROR,IDS_STRING_NOTE,MB_OK | MB_ICONEXCLAMATION);
				return 0;
			}
			DeleteFile(strTempFilePath);
			DeleteFile(tempaudiopath);

			//::MessageBox(NULL,"Your AVI movie will not contain a soundtrack. CamStudio is unable to merge the video with audio.","Note",MB_OK | MB_ICONEXCLAMATION);
			MessageOut(m_hWnd,IDS_STRING_NOSOUNDTRACK,IDS_STRING_NOTE,MB_OK | MB_ICONEXCLAMATION);
		} else if (result == 5) {
			//recover both files, but as separate files
			CString m_audioext(".wav");
			CString m_audiofile = m_newfile + m_audioext;

			if (!CopyFile( strTempFilePath,m_newfile,FALSE)) {
				//MessageBox("File Creation Error. Unable to rename/copy video file.","Note",MB_OK | MB_ICONEXCLAMATION);
				MessageOut(m_hWnd,IDS_STRINGFILECREATION2,IDS_STRING_NOTE,MB_OK | MB_ICONEXCLAMATION);
				return 0;
			}
			DeleteFile(strTempFilePath);

			if (!CopyFile(tempaudiopath,m_audiofile,FALSE)) {
				//MessageBox("File Creation Error. Unable to rename/copy audio file.","Note",MB_OK | MB_ICONEXCLAMATION);
				MessageOut(m_hWnd,IDS_STRING_FILECREATION3,IDS_STRING_NOTE,MB_OK | MB_ICONEXCLAMATION);
				return 0;
			}
			DeleteFile(tempaudiopath);

			CString tstr,msgstr;
			tstr.LoadString(IDS_STRING_NOTE);
			msgstr.LoadString(IDS_STRING_NOMERGE);
			//CString msgstr("CamStudio is unable to merge the video with audio files. Your video and audio files are saved separately as \n\n");

			msgstr = msgstr + m_newfile + "\n";
			msgstr = msgstr + m_audiofile;
			::MessageBox(NULL,msgstr,tstr,MB_OK | MB_ICONEXCLAMATION);
		} //if result
	} else {
		//no audio, just do a plain copy of temp avi to final avi
		if (!CopyFile( strTempFilePath,m_newfile,FALSE)) {
			//Ver 1.1
			//DeleteFile(m_newfile);
			//MessageBox("File Creation Error. Unable to rename/copy file. The file may be opened by another application. Please use another filename.","Note",MB_OK | MB_ICONEXCLAMATION);
			MessageOut(m_hWnd,IDS_STRING_FILECREATION4,IDS_STRING_NOTE,MB_OK | MB_ICONEXCLAMATION);
			//Repeat this function until success
			::PostMessage(hWndGlobal,WM_USER_GENERIC,0,0);
			return 0;
		}
		DeleteFile(strTempFilePath);
		if (!cAudioFormat.isInput(NONE)) {
			DeleteFile(tempaudiopath);
		}
	}

	//ver 2.26
	if (cProgramOpts.m_iRecordingMode == ModeAVI) {
		//Launch the player
		if (cProgramOpts.m_iLaunchPlayer == CAM1_PLAYER) {
			CString AppDir = GetProgPath();
			CString exefileName("\\player.exe ");
			CString launchPath = AppDir + exefileName + m_newfile;
			if (WinExec(launchPath,SW_SHOW)!=0) {
			} else {
				//MessageBox("Error launching avi player!","Note",MB_OK | MB_ICONEXCLAMATION);
				MessageOut(m_hWnd,IDS_STRING_ERRPLAYER,IDS_STRING_NOTE,MB_OK | MB_ICONEXCLAMATION);
			}
		} else if (cProgramOpts.m_iLaunchPlayer == DEFAULT_PLAYER) {
			if (Openlink(m_newfile)) {
			} else {
				//MessageBox("Error launching avi player!","Note",MB_OK | MB_ICONEXCLAMATION);
				MessageOut(m_hWnd,IDS_STRING_ERRDEFAULTPLAYER,IDS_STRING_NOTE,MB_OK | MB_ICONEXCLAMATION);
			}
		} else if (cProgramOpts.m_iLaunchPlayer == CAM2_PLAYER) {
			CString AppDir=GetProgPath();
			CString launchPath;
			CString exefileName("\\Playplus.exe ");
			launchPath=AppDir+exefileName+m_newfile;

			if (WinExec(launchPath,SW_SHOW)!=0) {
			} else {
				//MessageBox("Error launching avi player!","Note",MB_OK | MB_ICONEXCLAMATION);
				MessageOut(m_hWnd,IDS_STRING_ERRPLAYER,IDS_STRING_NOTE,MB_OK | MB_ICONEXCLAMATION);
			}
		}
	} else {
		//CString swfname;
		//swfname = m_newfile;
		//int lenx = m_newfile.GetLength();
		//if (((m_newfile.GetAt(lenx-1) == 'i') || (m_newfile.GetAt(lenx-1) == 'I')) &&
		//	((m_newfile.GetAt(lenx-2) == 'v') || (m_newfile.GetAt(lenx-2) == 'V')) &&
		//	((m_newfile.GetAt(lenx-3) == 'a') || (m_newfile.GetAt(lenx-3) == 'A')) &&
		//	(m_newfile.GetAt(lenx-4) == '.'))
		//{
		//	m_newfile.SetAt(lenx-1,'f');
		//	m_newfile.SetAt(lenx-2,'w');
		//	m_newfile.SetAt(lenx-3,'s');
		//	m_newfile.SetAt(lenx-4,'.');
		//}

		//ver 2.26
		SaveProducerCommand();

		//Sleep(2000);

		CString AppDir = GetProgPath();
		CString exefileName("\\producer.exe -x ");
		CString launchPath = AppDir + exefileName + m_newfile;
		if (WinExec(launchPath,SW_SHOW) != 0) {
		} else {
			MessageBox("Error launching SWF Producer!","Note",MB_OK | MB_ICONEXCLAMATION);
			//MessageOut(m_hWnd,IDS_STRING_ERRPRODUCER,IDS_STRING_NOTE,MB_OK | MB_ICONEXCLAMATION);
		}
	}

	return 0;
}

void CRecorderView::OnRecord()
{
	CStatusBar* pStatus = (CStatusBar*) AfxGetApp()->m_pMainWnd->GetDescendantWindow(AFX_IDW_STATUS_BAR);
	pStatus->SetPaneText(0,"Press the Stop Button to stop recording");

	//Version 1.1
	if (recordpaused) {
		recordpaused = 0;
		//ver 1.8
		//if (iRecordAudio==2)
		// mciRecordResume(tempaudiopath);

		//Set Title Bar
		SetTitleBar("CamStudio");

		return;
	}
	recordpaused = 0;

	nActualFrame = 0;
	nCurrFrame = 0;
	fRate = 0.0;
	fActualRate = 0.0;
	fTimeLength = 0.0;

	switch (cRegionOpts.m_iMouseCaptureMode)
	{
	case 0:
		if (cRegionOpts.m_bFixedCapture) {
			rc.top = cRegionOpts.m_iCaptureTop;
			rc.left = cRegionOpts.m_iCaptureLeft;
			rc.right = cRegionOpts.m_iCaptureLeft + cRegionOpts.m_iCaptureWidth - 1;
			rc.bottom = cRegionOpts.m_iCaptureTop + cRegionOpts.m_iCaptureHeight - 1;

			if (rc.top < 0)
				rc.top = 0;
			if (rc.left < 0)
				rc.left = 0;
			if (rc.right > maxxScreen - 1)
				rc.right = maxxScreen -1;
			if (rc.bottom > maxyScreen - 1)
				rc.bottom = maxyScreen - 1;

			//using protocols for iMouseCaptureMode==0
			rcClip = rc;
			old_rcClip = rcClip;
			NormalizeRect(&old_rcClip);
			CopyRect(&rcUse, &old_rcClip);
			::PostMessage (hWndGlobal, WM_USER_RECORDSTART, 0, (LPARAM) 0);
		} else {
			rc.top = 0;
			rc.left = 0;
			rc.right = cRegionOpts.m_iCaptureWidth - 1;
			rc.bottom = cRegionOpts.m_iCaptureHeight - 1;

			::ShowWindow(hMouseCaptureWnd,SW_MAXIMIZE);
			::UpdateWindow(hMouseCaptureWnd);

			InitDrawShiftWindow(); //will affect rc implicity
		}
		break;
	case 1:
		::ShowWindow(hMouseCaptureWnd, SW_MAXIMIZE);
		::UpdateWindow(hMouseCaptureWnd);
		InitSelectRegionWindow(); //will affect rc implicity
		break;
	case 2:
		rcUse.left = 0;
		rcUse.top = 0;
		rcUse.right = maxxScreen - 1;
		rcUse.bottom = maxyScreen - 1;
		::PostMessage(hWndGlobal, WM_USER_RECORDSTART, 0, (LPARAM) 0);
		break;
	case 3:
		// window
		AfxMessageBox("Click on Window to be captured");
		SetCapture();
		break;
	}
}

void CRecorderView::OnStop()
{
	//Version 1.1
	if (!bRecordState) {
		return;
	}

	if (recordpaused) {
		recordpaused = 0;

		//Set Title Bar
		SetTitleBar(_T("CamStudio"));
	}

	OnRecordInterrupted (0, 0);
}

//ver 1.6
#define MAXCOMPRESSORS 50
void CRecorderView::OnFileVideooptions()
{
	//COMPVARS compVars;
	//compVars.cbSize = sizeof(COMPVARS); // validate it
	//compVars.dwFlags = 0;

	int top = 0;
	int left = 0;
	int width = 320;
	int height = 200;

	// Capture a frame and use it to determine compatitble compressors for user to select
	LPBITMAPINFOHEADER first_alpbi = captureScreenFrame(left, top, width, height, 1);

	num_compressor = 0;
	if (pCompressorInfo == NULL) {
		pCompressorInfo = (ICINFO *) calloc(MAXCOMPRESSORS, sizeof(ICINFO));
	} else {
		free(pCompressorInfo);
		pCompressorInfo = (ICINFO *) calloc(MAXCOMPRESSORS, sizeof(ICINFO));
	}

	for (int i = 0; ICInfo(ICTYPE_VIDEO, i, &pCompressorInfo[num_compressor]); i++) {
		if (num_compressor >= MAXCOMPRESSORS) {
			break; //maximum allows 30 compressors
		}

		if (cVideoOpts.m_bRestrictVideoCodecs) {
			//allow only a few
			if ((pCompressorInfo[num_compressor].fccHandler == mmioFOURCC('m', 's', 'v', 'c'))
				|| (pCompressorInfo[num_compressor].fccHandler == mmioFOURCC('m', 'r', 'l', 'e'))
				|| (pCompressorInfo[num_compressor].fccHandler == mmioFOURCC('c', 'v', 'i', 'd'))
				|| (pCompressorInfo[num_compressor].fccHandler == mmioFOURCC('d', 'i', 'v', 'x')))
			{
				HIC hic = ICOpen(pCompressorInfo[num_compressor].fccType, pCompressorInfo[num_compressor].fccHandler, ICMODE_QUERY);
				if (hic) {
					if (ICERR_OK == ICCompressQuery(hic, first_alpbi, NULL)) {
						ICGetInfo(hic, &pCompressorInfo[num_compressor], sizeof(ICINFO));
						num_compressor ++;
					}
					ICClose(hic);
				}
			}
		} else {
			//CamStudio still cannot handle VIFP
			if (pCompressorInfo[num_compressor].fccHandler != mmioFOURCC('v', 'i', 'f', 'p')) {
				HIC hic = ICOpen(pCompressorInfo[num_compressor].fccType, pCompressorInfo[num_compressor].fccHandler, ICMODE_QUERY);
				if (hic) {
					if (ICERR_OK == ICCompressQuery(hic, first_alpbi, NULL)) {
						ICGetInfo(hic, &pCompressorInfo[num_compressor], sizeof(ICINFO));
						num_compressor ++;
					}
					ICClose(hic);
				}
			}
		}
	}

	FreeFrame(first_alpbi);

	CVideoOptionsDlg vod;
	vod.DoModal();
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

void CRecorderView::OnHelpHelp()
{
	CString progdir = GetProgPath();
	CString helppath = progdir + "\\help.htm";

	Openlink(helppath);

	//HtmlHelp( hWndGlobal, progdir + "\\help.chm", HH_DISPLAY_INDEX, (DWORD)"CamStudio");
}

void CRecorderView::OnPause()
{
	//return if not current recording or already in paused state
	if (!bRecordState || (recordpaused==1))
		return;

	recordpaused=1;

	//ver 1.8
	//if (iRecordAudio==2)
	// mciRecordPause(tempaudiopath);

	CStatusBar* pStatus = (CStatusBar*) AfxGetApp()->m_pMainWnd->GetDescendantWindow(AFX_IDW_STATUS_BAR);
	pStatus->SetPaneText(0,"Recording Paused");

	//Set Title Bar
	SetTitleBar("Paused");
}

void CRecorderView::OnUpdatePause(CCmdUI* pCmdUI)
{
	//Version 1.1
	//pCmdUI->Enable(bRecordState && (!recordpaused));
	pCmdUI->Enable(!recordpaused);
}

void CRecorderView::OnUpdateStop(CCmdUI* pCmdUI)
{
	//Version 1.1
	//pCmdUI->Enable(bRecordState);
}

void CRecorderView::OnUpdateRecord(CCmdUI* pCmdUI)
{
	//Version 1.1
	pCmdUI->Enable(!bRecordState || recordpaused);
}

void CRecorderView::OnHelpFaq()
{
	//Openlink("http://www.atomixbuttons.com/vsc/page5.html");
	Openlink("http://www.camstudio.org/faq.htm");
}

LRESULT CRecorderView::OnMM_WIM_DATA(WPARAM parm1, LPARAM parm2)
{
	HANDLE hInputDev = (HANDLE) parm1;
	ASSERT(hInputDev == m_hRecord);
	LPWAVEHDR pHdr = (LPWAVEHDR) parm2;
	MMRESULT mmReturn = ::waveInUnprepareHeader(m_hRecord, pHdr, sizeof(WAVEHDR));
	if (mmReturn) {
		waveInErrorMsg(mmReturn, "in OnWIM_DATA()");
		return 0;
	}

	//TRACE("WIM_DATA %4d\n", pHdr->dwBytesRecorded);

	if (bRecordState) {
		CBuffer buf(pHdr->lpData, pHdr->dwBufferLength);
		if (!recordpaused) {
			//write only if not paused
			//Write Data to file
			DataFromSoundIn(&buf);
		}

		// reuse the buffer:
		// prepare it again
		mmReturn = ::waveInPrepareHeader(m_hRecord,pHdr, sizeof(WAVEHDR));
		if (mmReturn) {
			waveInErrorMsg(mmReturn, "in OnWIM_DATA()");
		} else {
			// no error
			// add the input buffer to the queue again
			mmReturn = ::waveInAddBuffer(m_hRecord, pHdr, sizeof(WAVEHDR));
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
	CString setDir = GetProgPath();
	CString setPath;
	setPath.Format("%s\\CamStudio.ini", (LPCSTR)GetProgPath());

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
	fprintf(sFile, "dwCompressorStateSize= %d \n",cVideoOpts.m_dwCompressorStateSize);

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

	fclose(sFile);

	//ver 1.8,
	CString m_newfile = GetProgPath() + "\\CamShapes.ini";
	ListManager.SaveShapeArray(m_newfile);

	m_newfile = GetProgPath() + "\\CamLayout.ini";
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
	setDir = GetProgPath();
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
		fwrite((LPCTSTR)savedir, savedir.GetLength(), 1, tFile);

	if (!CamCursor.Dir().IsEmpty())
		fwrite((LPCTSTR)(CamCursor.Dir()), CamCursor.Dir().GetLength(), 1, tFile);

	if (cAudioFormat.m_dwCbwFX > 0)
		fwrite(pwfx, cAudioFormat.m_dwCbwFX, 1, tFile);

	if (pVideoCompressParams && (cVideoOpts.m_dwCompressorStateSize > 0)) {
		fwrite(pVideoCompressParams, cVideoOpts.m_dwCompressorStateSize, 1, tFile);
	} else {
		cVideoOpts.m_dwCompressorStateSize = 0;
	}

	//Ver 1.6
	if (specifieddir.GetLength() > 0)
		fwrite((LPCTSTR)specifieddir, specifieddir.GetLength(), 1, tFile);

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
		m_newfile = GetProgPath() + "\\CamShapes.ini";
		ListManager.LoadShapeArray(m_newfile);

		m_newfile = GetProgPath() + "\\CamLayout.ini";
		ListManager.LoadLayout(m_newfile);

	}

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
			cVideoOpts.m_dwCompfccHandler = mmioFOURCC('M', 'S', 'V', 'C');
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
		specifieddir = GetTempFolder(cProgramOpts.m_iTempPathAccess, specifieddir);
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

	//********************************************
	//Loading Camdata.ini binary data
	//********************************************
	fileName = "\\Camdata.ini";
	setDir = GetProgPath();
	setPath = setDir + fileName;
	FILE * tFile = fopen(LPCTSTR(setPath),"rb");
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
			fread( (void *) sdata, iSaveLen, 1, tFile);
			sdata[iSaveLen]=0;
			savedir=CString(sdata);
		}

		if ((iCursorLen > 0) && (iCursorLen < 1000)) {
			fread( (void *) tdata, iCursorLen, 1, tFile);
			tdata[iCursorLen] = 0;
			CamCursor.Dir(CString(tdata));
		}

		if (ver > 1.35)
		{
			// if perfoming an upgrade from previous settings,
			// do not load these additional camdata.ini information
			if (cAudioFormat.m_dwCbwFX > 0) {
				AllocCompressFormat();
				int countread = fread( (void *) pwfx, cAudioFormat.m_dwCbwFX, 1, tFile);
				if (countread != 1) {
					cAudioFormat.m_dwCbwFX = 0;
					if (pwfx)
					{
						GlobalFreePtr(pwfx);
						pwfx = NULL;
						SuggestCompressFormat();
					}
				} else {
					AttemptCompressFormat();
				}
			}

			if (cVideoOpts.m_dwCompressorStateSize > 0)
			{
				AllocVideoCompressParams(cVideoOpts.m_dwCompressorStateSize);
				fread( (void *) pVideoCompressParams, cVideoOpts.m_dwCompressorStateSize, 1, tFile);
			}

			//ver 1.6
			if (ver > 1.55)
			{
				// if upgrade from older file versions,
				// iSpecifiedDirLength == 0 and the following code will not run
				if ((cProgramOpts.m_iSpecifiedDirLength > 0) && (cProgramOpts.m_iSpecifiedDirLength < 1000)) {
					fread( (void *) specdata, cProgramOpts.m_iSpecifiedDirLength, 1, tFile);
					specdata[cProgramOpts.m_iSpecifiedDirLength]=0;
					specifieddir=CString(specdata);
				}

				//ver 1.8
				if (ver >= 1.799999)
				{
					char namedata[1000];
					if ((shapeNameLen > 0) && (shapeNameLen < 1000))
					{
						fread( (void *) namedata, shapeNameLen, 1, tFile);
						namedata[shapeNameLen]=0;
						shapeName = CString(namedata);
					}

					if ((layoutNameLen > 0) && (layoutNameLen < 1000))
					{
						fread( (void *) namedata, layoutNameLen, 1, tFile);
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
	CString nosaveDir = GetProgPath();
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
		setDir = GetProgPath();
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
	CFolderDialog cfg(specifieddir);
	if (IDOK == cfg.DoModal()) {
		specifieddir = cfg.GetPathName();
		cProgramOpts.m_iTempPathAccess = USE_USER_SPECIFIED_DIR;
	}
}

void CRecorderView::OnUpdateOptionsTempdirUser(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(cProgramOpts.m_iTempPathAccess == USE_USER_SPECIFIED_DIR);
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

void CRecorderView::OnUpdateViewScreenannotations(CCmdUI* pCmdUI)
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
		rect.right = rect.left + 160 - 1;
		rect.bottom = rect.top + 120 - 1;
		m_vanWnd.m_textstring = m_newShapeText;
		m_vanWnd.m_shapeStr = vastr;
		m_vanWnd.CreateTransparent(m_vanWnd.m_shapeStr,rect,NULL);
		vanWndCreated = 1;

	}

	if (m_vanWnd.IsWindowVisible())
	{
		m_vanWnd.ShowWindow(SW_HIDE);
	}
	else
	{
		if (m_vanWnd.status!=1) {
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
// wParam - Specifies the identifier of the hot key that generated the message. 
// lParam - The low-order word specifies the keys that were to be pressed in
// combination with the key specified by the high-order word to generate the
// WM_HOTKEY message. This word can be one or more of the following values.
//	MOD_ALT - Either ALT key was held down.
//	MOD_CONTROL - Either CTRL key was held down.
//	MOD_SHIFT - Either SHIFT key was held down.
//	MOD_WIN - Either WINDOWS key was held down. 
// The high-order word specifies the virtual key code of the hot key. 
/////////////////////////////////////////////////////////////////////////////
LRESULT CRecorderView::OnHotKey(WPARAM wParam, LPARAM lParam)
{
	switch (wParam)
	{
	case 0:
		if (!bRecordState) {
			if (bAllowNewRecordStartKey) {
				//prevent the case which CamStudio presents more than one region for the user to select
				bAllowNewRecordStartKey = FALSE;
				OnRecord();
			}
		} else if (bRecordState) {
			// pause if currently recording
			if (recordpaused == 0) {
				OnPause();
			} else {
				OnRecord();
			}
		}
		break;
	case 1:
		if (bRecordState) {
			if (cHotKeyOpts.m_RecordEnd.m_vKey != cHotKeyOpts.m_RecordCancel.m_vKey) {
				OnRecordInterrupted(cHotKeyOpts.m_RecordEnd.m_vKey, 0);
			} else {
				OnRecordInterrupted(cHotKeyOpts.m_RecordCancel.m_vKey + 1, 0);
			}
		}
		break;
	case 2:
		if (bRecordState) {
			OnRecordInterrupted(cHotKeyOpts.m_RecordCancel.m_vKey, 0);
		}
		break;
	case 3:
		{
			if (!bCreatedSADlg) {
				sadlg.Create(IDD_SCREENANNOTATIONS2, NULL);
				bCreatedSADlg = true;
			}
			int max = ListManager.layoutArray.GetSize();
			if (max <= 0)
				return 0;

			// Get Current selected
			int cursel = sadlg.GetLayoutListSelection();
			iCurrentLayout = (cursel < 0) ? 0 : cursel + 1;
			if (iCurrentLayout >= max)
				iCurrentLayout = 0;

			sadlg.InstantiateLayout(iCurrentLayout,1);
		}
		break;
	case 4:
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
	case 5:
		{
			if (!bCreatedSADlg) {
				sadlg.Create(IDD_SCREENANNOTATIONS2,NULL);
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
	CString AppDir=GetProgPath();
	CString launchPath;
	CString exefileName("\\Producer.exe ");
	launchPath=AppDir+exefileName;

	if (WinExec(launchPath,SW_SHOW)!=0) {
	}
	else {
		//MessageBox("Error launching SWF Producer!","Note",MB_OK | MB_ICONEXCLAMATION);
		MessageOut(m_hWnd,IDS_ERRPPRODUCER,IDS_STRING_NOTE,MB_OK | MB_ICONEXCLAMATION);
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

void CRecorderView::OnAviswf()
{
	cProgramOpts.m_iRecordingMode = (cProgramOpts.m_iRecordingMode == ModeAVI) ? ModeFlash : ModeAVI;
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
	cProgramOpts.m_bAutoNaming = true;
}

void CRecorderView::OnUpdateOptionsNamingAutodate(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(cProgramOpts.m_bAutoNaming);
}

void CRecorderView::OnOptionsNamingAsk()
{
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

void CRecorderView::OnOptionsLanguageEnglish()
{
	CRecorderApp * pApp = static_cast<CRecorderApp*>(AfxGetApp());
	pApp->LanguageID(9);
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
	CPoint point;
	VERIFY(GetCursorPos(&point));
	ScreenToClient(&point);
	ClientToScreen(&point);
	CWnd* wnd = WindowFromPoint(point);
	HWND hWnd = NULL;
	CWnd* wnd2 = NULL;
	if (wnd)
	{
		hWnd = wnd->m_hWnd;
		wnd2 = wnd;

		//HWND desktop = ::GetDesktopWindow();
		//if ((wnd->GetParent() == NULL) || (wnd->GetParent()->m_hWnd == desktop)){
		//	hWnd = wnd->m_hWnd;
		//	wnd2 = wnd;
		//}
		//else{
		//	hWnd = wnd->GetTopLevelParent()->m_hWnd;
		//	wnd2 = wnd->GetTopLevelParent();
		//}

		ReleaseCapture();

		CView::OnCaptureChanged(pWnd);

		//::GetClientRect(hWnd, &rcUse);
		::GetWindowRect(hWnd, &rcUse);
		//::ClientToScreen(&rcUse);

		::PostMessage (hWndGlobal,WM_USER_RECORDSTART,0,(LPARAM) 0);
	} else {
		ReleaseCapture();
		CView::OnCaptureChanged(pWnd);
	}
}

void CRecorderView::OnAnnotationAddsystemtimestamp()
{
	cTimestampOpts.m_bAnnotation = !cTimestampOpts.m_bAnnotation;
}

void CRecorderView::OnUpdateAnnotationAddsystemtimestamp(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(cTimestampOpts.m_bAnnotation);
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
	CAnnotationEffectsOptionsDlg dlg(this);
	dlg.m_timestamp = cTimestampOpts.m_taTimestamp;
	dlg.m_caption = cCaptionOpts.m_taCaption;
	dlg.m_image = cWatermarkOpts.m_iaWatermark;
	if (IDOK == dlg.DoModal()){
		cTimestampOpts.m_taTimestamp = dlg.m_timestamp;
		cCaptionOpts.m_taCaption = dlg.m_caption;
		cWatermarkOpts.m_iaWatermark = dlg.m_image;
	}
}

void CRecorderView::OnHelpCamstudioblog()
{
	Openlink("http://www.camstudio.org/blog");
}

void CRecorderView::OnBnClickedButtonlink()
{
	Openlink("http://www.camstudio.org/blog");
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

	CString csMsg;
	csMsg.Format("Current Frame : %d",  nCurrFrame);
	CSize sizeExtent = srcDC.GetTextExtent(csMsg);

	CRect rectText;
	GetClientRect(&rectText);
	int xoffset = 40;
	int yoffset = 10;
	int iLines = 6;
	int iStartPosY = rectText.bottom;
	iStartPosY -= (iLines * (sizeExtent.cy + 10));
	yoffset = iStartPosY;

	rectText.top = yoffset - 2;
	rectText.bottom = yoffset + sizeExtent.cy + 4;
//	srcDC.Rectangle(&rectText);
	srcDC.TextOut(xoffset, yoffset, csMsg);

	csMsg.Format("Actual Input Rate : %.2f fps",  fActualRate);
	sizeExtent = srcDC.GetTextExtent(csMsg);
	yoffset += sizeExtent.cy + 10;
	rectText.top = yoffset - 2;
	rectText.bottom = yoffset + sizeExtent.cy + 4;
//	srcDC.Rectangle(&rectText);
	srcDC.TextOut(xoffset, yoffset, csMsg);

	csMsg.Format("Time Elasped : %.2f sec",  fTimeLength);
	sizeExtent = srcDC.GetTextExtent(csMsg);
	yoffset += sizeExtent.cy + 10;
	rectText.top = yoffset - 2;
	rectText.bottom = yoffset + sizeExtent.cy + 4;
//	srcDC.Rectangle(&rectText);
	srcDC.TextOut(xoffset, yoffset, csMsg);

	csMsg.Format("Number of Colors : %d iBits",  nColors);
	sizeExtent = srcDC.GetTextExtent(csMsg);
	yoffset += sizeExtent.cy + 10;
	rectText.top = yoffset - 2;
	rectText.bottom = yoffset + sizeExtent.cy + 4;
//	srcDC.Rectangle(&rectText);
	srcDC.TextOut(xoffset, yoffset, csMsg);

	csMsg.Format("Codec : %s",  LPCTSTR(strCodec));
	sizeExtent = srcDC.GetTextExtent(csMsg);
	yoffset += sizeExtent.cy + 10;
	rectText.top = yoffset - 2;
	rectText.bottom = yoffset + sizeExtent.cy + 4;
//	srcDC.Rectangle(&rectText);
	srcDC.TextOut(xoffset, yoffset, csMsg);

	csMsg.Format("Dimension : %d X %d",  iActualWidth, iActualHeight);
	sizeExtent = srcDC.GetTextExtent(csMsg);
	yoffset += sizeExtent.cy + 10;
	rectText.top = yoffset - 2;
	rectText.bottom = yoffset + sizeExtent.cy + 4;
//	srcDC.Rectangle(&rectText);
	srcDC.TextOut(xoffset, yoffset, csMsg);

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
	msgRecMode.LoadString((cProgramOpts.m_iRecordingMode == ModeAVI) ? IDS_RECAVI : IDS_RECSWF);

	CSize sizeExtent = srcDC.GetTextExtent(msgRecMode);

	CRect rectClient;
	GetClientRect(&rectClient);
	int xoffset = 12;
	int yoffset = 12;
	int xmove = rectClient.Width() - sizeExtent.cx - xoffset;
	int ymove = yoffset;

	CRect rectMode(xmove, ymove, xmove + sizeExtent.cx, ymove + sizeExtent.cy);
	srcDC.Rectangle(&rectMode);
	srcDC.Rectangle(rectMode.left - 3, rectMode.top - 3, rectMode.right + 3, rectMode.bottom + 3);
	srcDC.TextOut(rectMode.left, rectMode.top,  msgRecMode);

	srcDC.SelectObject(pOldPen);
	srcDC.SelectObject(pOldBrush);
	srcDC.SelectObject(pOldFont);
	srcDC.SetTextColor(oldTextColor);
	srcDC.SetBkColor(oldBkColor);
}

LPBITMAPINFOHEADER CRecorderView::captureScreenFrame(int left, int top, int width, int height, int tempDisableRect)
{
//	TRACE("CRecorderView::captureScreenFrame\n");
	CRect rectView(left, top, left + width, top + height);

	// if flashing rect
	if (cProgramOpts.m_bFlashingRect && !tempDisableRect) {
		if (cProgramOpts.m_bAutoPan) {
			pFlashingWnd->SetUpRegion(rectView.left, rectView.top, rectView.Width(), rectView.Height(), 1);
		}
		pFlashingWnd->DrawFlashingRect(TRUE, cProgramOpts.m_bAutoPan);
	}

	m_cCamera.SetView(rectView);
	m_cCamera.CaptureFrame(rectView);

	//if flashing rect
	if (cProgramOpts.m_bFlashingRect && !tempDisableRect) {
		pFlashingWnd->DrawFlashingRect(FALSE, cProgramOpts.m_bAutoPan);
	}
	return m_cCamera.Image();
}

void CRecorderView::FreeFrame(LPBITMAPINFOHEADER alpbi)
{
	if (!alpbi)
		return;

#pragma message("CRecorderView::FreeFrame: ignore")

	//GlobalFreePtr(alpbi);
	alpbi = 0;
}


UINT CRecorderView::RecordAVIThread(LPVOID pParam)
{
	CRecorderView *pcRecorderView = reinterpret_cast<CRecorderView*>(pParam);
	if (!pcRecorderView)
		return 0;

	return pcRecorderView->RecordAVI();
}

UINT CRecorderView::RecordAVI()
{
	TRACE("CRecorderView::RecordAVIThread\n");
	//Test the validity of writing to the file
	//Make sure the file to be created is currently not used by another application
	CString csTempFolder(GetTempFolder(cProgramOpts.m_iTempPathAccess, specifieddir));
	strTempFilePath.Format("%s\\temp.avi", (LPCSTR)csTempFolder);

	srand( (unsigned)time( NULL));
	bool fileverified = false;
	while (!fileverified)
	{
		OFSTRUCT ofstruct;
		HFILE hFile = OpenFile(strTempFilePath, &ofstruct, OF_SHARE_EXCLUSIVE | OF_WRITE | OF_CREATE);
		fileverified = (hFile != HFILE_ERROR);
		if (fileverified) {
			CloseHandle((HANDLE)hFile);
			DeleteFile(strTempFilePath);
		} else {
			strTempFilePath.Format("%s\\temp%d.avi", (LPCSTR)csTempFolder, rand());
		}
	}

	int top = rcUse.top;
	int left = rcUse.left;
	int width = rcUse.right - rcUse.left+1;
	int height = rcUse.bottom - rcUse.top + 1;
	int fps = cVideoOpts.m_iFramesPerSecond;

	RecordVideo(top, left, width, height, fps, strTempFilePath);

	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// RecordVideo
//
// The main function used in the recording of video
// Includes opening/closing avi file, initializing avi settings, capturing
// frames, applying cursor effects etc.
/////////////////////////////////////////////////////////////////////////////
int CRecorderView::RecordVideo(int top, int left, int width, int height, int fps, const char *szFileName)
{
	TRACE("CRecorderView::RecordVideo\n");
	WORD wVer = HIWORD(VideoForWindowsVersion());
	if (wVer < 0x010a) {
		TRACE("CRecorderView::RecordVideo: Wrong VideoForWindowsVersion\n");
		MessageOut(NULL, IDS_STRING_VERSIONOLD , IDS_STRING_NOTE, MB_OK | MB_ICONSTOP);
		return FALSE;
	}

	iActualWidth = width;
	iActualHeight = height;

	////////////////////////////////////////////////
	// CAPTURE FIRST FRAME
	////////////////////////////////////////////////
	LPBITMAPINFOHEADER alpbi = captureScreenFrame(left, top, width, height, 1);

	////////////////////////////////////////////////
	// TEST VALIDITY OF COMPRESSOR
	//////////////////////////////////////////////////

	if (cVideoOpts.m_iSelectedCompressor > 0) {
		HIC hic = ICOpen(pCompressorInfo[cVideoOpts.m_iSelectedCompressor].fccType, pCompressorInfo[cVideoOpts.m_iSelectedCompressor].fccHandler, ICMODE_QUERY);
		if (hic) {
			int newleft;
			int newtop;
			int newwidth;
			int newheight;
			int align = 1;
			while (ICERR_OK != ICCompressQuery(hic, alpbi, NULL)) {
				// Try adjusting width/height a little bit
				align = align * 2;
				if (align > 8)
					break;

				newleft = left;
				newtop = top;
				int wm = (width % align);
				if (wm > 0) {
					newwidth = width + (align - wm);
					if (newwidth>maxxScreen) {
						newwidth = width - wm;
					}
				}

				int hm = (height % align);
				if (hm > 0) {
					newheight = height + (align - hm);
					if (newheight>maxyScreen) {
						newwidth = height - hm;
					}
				}

				if (alpbi) {
					FreeFrame(alpbi);
				}
				alpbi = captureScreenFrame(newleft, newtop, newwidth, newheight, 1);
			}

			//if succeed with new width/height, use the new width and height
			//else if still fails == > default to MS Video 1 (MSVC)
			if (align == 1) {
				//Compressor has no problem with the current dimensions...so proceed
				//do nothing here
			} else if  (align <= 8) {
				//Compressor can work if the dimensions is adjusted slightly
				left = newleft;
				top = newtop;
				width = newwidth;
				height = newheight;

				iActualWidth = newwidth;
				iActualHeight = newheight;
			} else {
				cVideoOpts.m_dwCompfccHandler = mmioFOURCC('M', 'S', 'V', 'C');
				strCodec = CString("MS Video 1");
			}

			ICClose(hic);
		} else {
			cVideoOpts.m_dwCompfccHandler = mmioFOURCC('M', 'S', 'V', 'C');
			strCodec = CString("MS Video 1");
			//MessageBox(NULL, "hic default", "note", MB_OK);
		}
	} //selected_compressor

	//Special Cases
	//DIVX
	//if (dwCompfccHandler == mmioFOURCC('D', 'I', 'V', 'X')) { //Still Can't Handle DIVX
	//	dwCompfccHandler = mmioFOURCC('M', 'S', 'V', 'C');
	//	strCodec = CString("MS Video 1");
	//}

	//IV50
	if (cVideoOpts.m_dwCompfccHandler == mmioFOURCC('I', 'V', '5', '0')) { //Still Can't Handle Indeo 5.04
		cVideoOpts.m_dwCompfccHandler = mmioFOURCC('M', 'S', 'V', 'C');
		strCodec = CString("MS Video 1");
	}

	////////////////////////////////////////////////
	// Set Up Flashing Rect
	////////////////////////////////////////////////
	if (cProgramOpts.m_bFlashingRect) {
		pFlashingWnd->SetUpRegion(left, top, width, height, cProgramOpts.m_bAutoPan);
		pFlashingWnd->ShowWindow(SW_SHOW);
	}

	////////////////////////////////////////////////
	// INIT AVI USING FIRST FRAME
	////////////////////////////////////////////////
	AVIFileInit();

	// Open the movie file for writing....
	char szTitle[BUFSIZE];
	strcpy_s(szTitle, "AVI Movie");

	PAVIFILE pfile = NULL;
	HRESULT hr;
	hr = AVIFileOpen(&pfile, szFileName, OF_WRITE | OF_CREATE, NULL);
	if (hr != AVIERR_OK) {
		TRACE("CRecorderView::RecordVideo: AVIFileOpen error\n");
		goto error;
	}

	// Fill in the header for the video stream....
	// The video stream will run in 15ths of a second....
	AVISTREAMINFO strhdr;
	_fmemset(&strhdr, 0, sizeof(strhdr));
	strhdr.fccType                = streamtypeVIDEO;// stream type

	//strhdr.fccHandler             = dwCompfccHandler;
	strhdr.fccHandler             = 0;

	strhdr.dwScale                = 1;
	strhdr.dwRate                 = fps;
	strhdr.dwSuggestedBufferSize  = alpbi->biSizeImage;
	// rectangle for stream
	SetRect(&strhdr.rcFrame, 0, 0, (int) alpbi->biWidth, (int) alpbi->biHeight);

	// And create the stream;
	PAVISTREAM ps = NULL;
	hr = AVIFileCreateStream(pfile, &ps, &strhdr);
	if (hr != AVIERR_OK) {
		TRACE("CRecorderView::RecordVideo: AVIFileCreateStream error\n");
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
		aopts[0]->fccHandler = mmioFOURCC('M', 'S', 'V', 'C');	 //msvc
		strCodec = CString("MS Video 1");
		aopts[0]->dwQuality = 10000;
	} else {
		//Ver 1.2
		//
		if ((cVideoOpts.m_dwCompfccHandler != 0) && (cVideoOpts.m_dwCompfccHandler == cVideoOpts.m_dwCompressorStateIsFor)) {
			// make a copy of the pVideoCompressParams just in case after compression,
			// this variable become messed up
			if (MakeCompressParamsCopy(cVideoOpts.m_dwCompressorStateSize, pVideoCompressParams)) {
				aopts[0]->lpParms = pParamsUse;
				aopts[0]->cbParms = cVideoOpts.m_dwCompressorStateSize;
			}
		}
	}

	//The 1 here indicates only 1 stream
	//if (!AVISaveOptions(NULL, 0, 1, &ps, (LPAVICOMPRESSOPTIONS *) &aopts))
	//        goto error;

	PAVISTREAM psCompressed = NULL;
	hr = AVIMakeCompressedStream(&psCompressed, ps, &opts, NULL);
	if (hr != AVIERR_OK) {
		TRACE("CRecorderView::RecordVideo: AVIMakeCompressedStream error\n");
		goto error;
	}

	hr = AVIStreamSetFormat(psCompressed, 0, alpbi, alpbi->biSize + alpbi->biClrUsed * sizeof(RGBQUAD));
	if (hr != AVIERR_OK) {
		goto error;
	}

	FreeFrame(alpbi);
	alpbi=NULL;

	if (cProgramOpts.m_bAutoPan) {
		rectPanCurrent.left = left;
		rectPanCurrent.top = top;
		rectPanCurrent.right = left + width - 1;
		rectPanCurrent.bottom = top + height - 1;
	}

	//////////////////////////////////////////////
	// Recording Audio
	//////////////////////////////////////////////
	if ((cAudioFormat.isInput(SPEAKERS)) || (cAudioFormat.m_bUseMCI)) {
		mciRecordOpen();
		mciSetWaveFormat();
		mciRecordStart();

		//if (iShiftType == 1)
		//{
		//	mci::mciRecordPause(tempaudiopath);
		//	unshifted = 1;
		//}
	} else if (!cAudioFormat.isInput(NONE)) {
		InitAudioRecording();
		StartAudioRecording(&m_Format);
	}

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
		if (!bInitCapture) {
			timeexpended = timeGetTime() - dwInitialTime;
		} else {
			frametime = 0;
			timeexpended = 0;
		}

		//Autopan
		if (cProgramOpts.m_bAutoPan && (width < maxxScreen) && (height < maxyScreen)) {
			POINT xPoint;
			GetCursorPos(&xPoint);

			int extleft = ((rectPanCurrent.right - rectPanCurrent.left)*1)/4 + rectPanCurrent.left;
			int extright = ((rectPanCurrent.right - rectPanCurrent.left)*3)/4 + rectPanCurrent.left;
			int exttop = ((rectPanCurrent.bottom - rectPanCurrent.top)*1)/4 + rectPanCurrent.top;
			int extbottom = ((rectPanCurrent.bottom - rectPanCurrent.top)*3)/4 + rectPanCurrent.top;

			if (xPoint.x  < extleft ) { //need to pan left
				rectPanDest.left = xPoint.x - width/2;
				rectPanDest.right = rectPanDest.left +  width - 1;
				if (rectPanDest.left < 0)  {
					rectPanDest.left = 0;
					rectPanDest.right = rectPanDest.left +  width - 1;
				}
			} else if (xPoint.x  > extright ) { //need to pan right
				rectPanDest.left = xPoint.x - width/2;
				rectPanDest.right = rectPanDest.left +  width - 1;
				if (rectPanDest.right >= maxxScreen) {
					rectPanDest.right = maxxScreen - 1;
					rectPanDest.left  = rectPanDest.right - width + 1;
				}
			} else {
				rectPanDest.right = rectPanCurrent.right;
				rectPanDest.left  = rectPanCurrent.left;
			}

			if (xPoint.y  < exttop ) { //need to pan up
				rectPanDest.top = xPoint.y - height/2;
				rectPanDest.bottom = rectPanDest.top +  height - 1;
				if (rectPanDest.top < 0)  {
					rectPanDest.top = 0;
					rectPanDest.bottom = rectPanDest.top +  height - 1;
				}
			} else if (xPoint.y  > extbottom ) { //need to pan down
				rectPanDest.top = xPoint.y - height/2;
				rectPanDest.bottom = rectPanDest.top +  height - 1;
				if (rectPanDest.bottom >= maxyScreen) {
					rectPanDest.bottom = maxyScreen - 1;
					rectPanDest.top  = rectPanDest.bottom - height + 1;
				}
			} else {
				rectPanDest.top = rectPanCurrent.top;
				rectPanDest.bottom  = rectPanCurrent.bottom;
			}

			//Determine Pan Values
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

			rectPanCurrent.right = rectPanCurrent.left + width - 1;
			rectPanCurrent.bottom =  rectPanCurrent.top + height - 1;

			alpbi = captureScreenFrame(rectPanCurrent.left, rectPanCurrent.top, width, height, 0);
		} else {
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

			alpbi = captureScreenFrame(left, top, width, height, 0);
		}

		if (!bInitCapture) {
			if (cVideoOpts.m_iTimeLapse > 1000) {
				frametime++;
			} else {
				frametime = (DWORD) (((double) timeexpended /1000.0 ) * (double) (1000.0/cVideoOpts.m_iTimeLapse));
			}
		} else {
			bInitCapture = false;
		}

		fTimeLength = ((float) timeexpended) /((float) 1000.0);

		if (cProgramOpts.m_bRecordPreset) {
			if (int(fTimeLength) >= cProgramOpts.m_iPresetTime) {
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
		//			mci::mciRecordResume(tempaudiopath);
		//			unshifted = 0;
		//		}
		//	}
		//}

		if ((frametime == 0) || (frametime > oldframetime)) {
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
			hr = AVIStreamWrite(psCompressed, frametime, 1, (LPBYTE) alpbi + alpbi->biSize + alpbi->biClrUsed * sizeof(RGBQUAD), alpbi->biSizeImage, 0, NULL, NULL);
			//}

			if (hr != AVIERR_OK) {
				break;
			}

			nActualFrame++;
			nCurrFrame = frametime;
			fRate = ((float) nCurrFrame)/fTimeLength;
			fActualRate = ((float) nActualFrame)/fTimeLength;

			//Update recording stats every half a second
			divx = timeexpended / 500;
			if (divx != oldsec) {
				oldsec = divx;
				::InvalidateRect(hWndGlobal, NULL, FALSE);
			}

			//free memory
			FreeFrame(alpbi);
			alpbi = NULL;

			oldframetime = frametime;
		} // if frametime is different

		//Version 1.1
		int haveslept = 0;
		int pausecounter = 0;
		// local variable is initialized but not referenced
		//int pauseremainder = 0;
		int pauseindicator = 1;
		DWORD timestartpause;
		DWORD timeendpause;
		DWORD timedurationpause;
		while (recordpaused) {
			if (!haveslept) {
				timestartpause = timeGetTime();
			}

			//Flash Pause Indicator in Title Bar
			pausecounter++;
			if ((pausecounter % 8) == 0) {
				//if after every 400 milliseconds (8 * 50)
				if (pauseindicator) {
					SetTitleBar("");
					pauseindicator = 0;
				} else {
					SetTitleBar("Paused");
					pauseindicator = 1;
				}
			}

			if (cAudioFormat.isInput(SPEAKERS) || cAudioFormat.m_bUseMCI) {
				if (bAlreadyMCIPause == 0) {
					mciRecordPause(tempaudiopath);
					bAlreadyMCIPause = true;
				}
			}

			//do nothing.. wait
			::Sleep(50); //Sleep for 50

			haveslept = 1;
		}

		//Version 1.1
		if (haveslept) {
			if (cAudioFormat.isInput(SPEAKERS) || cAudioFormat.m_bUseMCI) {
				if (bAlreadyMCIPause) {
					mciRecordResume(tempaudiopath);
					bAlreadyMCIPause = false;
				}
			}

			timeendpause = timeGetTime();
			timedurationpause =  timeendpause - timestartpause;

			//CString msgstr;
			//msgstr.Format("timestartpause %ld\ntimeendpause %ld\ntimedurationpause %ld", timeendpause, timeendpause, timedurationpause);
			//MessageBox(NULL, msgstr, "Note", MB_OK);

			dwInitialTime = dwInitialTime + timedurationpause;
		} else {
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
			if (bRecordState) {
				::Sleep(remainlapse);
			}
		}
	} //for loop

error:

	// Now close the file

	if (cProgramOpts.m_bFlashingRect) {
		pFlashingWnd->ShowWindow(SW_HIDE);
	}

	//Ver 1.2
	//
	if ((cVideoOpts.m_dwCompfccHandler == cVideoOpts.m_dwCompressorStateIsFor) && (cVideoOpts.m_dwCompfccHandler != 0)) {
		//Detach pParamsUse from AVICOMPRESSOPTIONS so AVISaveOptionsFree will not free it
		//(we will free it ourselves)

		//Setting this is no harm even if pParamsUse is not attached to lpParams
		aopts[0]->lpParms			 = 0;
		aopts[0]->cbParms			 = 0;
	}

	AVISaveOptionsFree(1, (LPAVICOMPRESSOPTIONS FAR *) &aopts);

	//////////////////////////////////////////////
	// Recording Audio
	//////////////////////////////////////////////
	if (cAudioFormat.isInput(SPEAKERS) || cAudioFormat.m_bUseMCI) {
		GetTempWavePath();
		mciRecordStop(tempaudiopath);
		mciRecordClose();
		//restoreWave();
	} else if (!cAudioFormat.isInput(NONE)) {
		StopAudioRecording();
		ClearAudioFile();
	}

	if (pfile) {
		AVIFileClose(pfile);
	}

	if (ps) {
		AVIStreamClose(ps);
	}

	if (psCompressed) {
		AVIStreamClose(psCompressed);
	}

	AVIFileExit();

	if (hr != NOERROR) 	{
		::PostMessage(hWndGlobal, WM_USER_RECORDINTERRUPTED, 0, 0);

		//char *ErrorBuffer; // This really is a pointer - not reserved space!
		//FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | 	FORMAT_MESSAGE_FROM_SYSTEM, NULL, GetLastError(), MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US), (LPTSTR)&ErrorBuffer, 0, NULL);
		//CString reasonstr(ErrorBuffer);
		//CString errorstr("File Creation Error. Unable to rename file.\n\n");
		//CString reportstr;
		//reportstr = errorstr + reasonstr;
		//MessageBox(NULL, reportstr, "Note", MB_OK | MB_ICONEXCLAMATION);

		if (cVideoOpts.m_dwCompfccHandler != mmioFOURCC('M', 'S', 'V', 'C')) {
			//if (IDYES == MessageBox(NULL, "Error recording AVI file using current compressor. Use default compressor ? ", "Note", MB_YESNO | MB_ICONEXCLAMATION)) {
			if (IDYES == MessageOut(NULL, IDS_STRING_ERRAVIDEFAULT, IDS_STRING_NOTE, MB_YESNO | MB_ICONQUESTION	)) {
				cVideoOpts.m_dwCompfccHandler = mmioFOURCC('M', 'S', 'V', 'C');
				strCodec = "MS Video 1";
				::PostMessage(hWndGlobal, WM_USER_RECORDSTART, 0, 0);
			}
		} else {
			//MessageBox(NULL, "Error Creating AVI File", "Error", MB_OK | MB_ICONEXCLAMATION);
			MessageOut(NULL, IDS_STRING_ERRCREATEAVI , IDS_STRING_NOTE, MB_OK | MB_ICONEXCLAMATION);
		}

		return 0;
	}

	//Save the file on success
	::PostMessage(hWndGlobal, WM_USER_GENERIC, 0, 0);

	TRACE("CRecorderView::RecordVideo: Success end\n");
	return 0;
}

void CRecorderView::SaveProducerCommand()
{
	//********************************************
	//Saving CamProducer.ini for storing text data
	//********************************************

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

	//ErrMsg( "\nvscap\n");
	//ErrMsg( "bLaunchPropPrompt = %d \n",bLaunchPropPrompt);
	//ErrMsg( "bLaunchHTMLPlayer = %d \n",bLaunchHTMLPlayer);
	//ErrMsg( "bDeleteAVIAfterUse = %d \n",bDeleteAVIAfterUse);

	//fflush(sFile);
	fclose(sFile);
}
