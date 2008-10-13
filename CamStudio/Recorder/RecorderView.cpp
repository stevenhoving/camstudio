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

#include "RecorderDoc.h"
#include "RecorderView.h"
#include "MainFrm.h"

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

//extern int viewtype;
//Link to customized icon info
extern DWORD icon_info[];
//extern int NumberOfMixerDevices;
//extern int SelectedMixer;

//extern int feedback_line;
//extern int feedback_lineInfo;

//Region Movement
extern CRect newRect;
extern int newRegionUsed;
extern int readingRegion;
extern int writingRegion;

extern int settingRegion;
extern int capturingRegion;

extern CListManager gList;

extern int useMCI;
extern int g_refreshRate;

extern int shapeNameInt;
extern CString shapeName;
extern int layoutNameInt;
extern CString g_layoutName;

/////////////////////////////////////////////////////////////////////////////
// external functions
/////////////////////////////////////////////////////////////////////////////
extern void FreeWaveoutResouces();

extern BOOL useWavein(BOOL,int);
extern BOOL useWaveout(BOOL,int);
extern BOOL WaveoutUninitialize();

extern BOOL initialSaveMMMode();
extern BOOL finalRestoreMMMode();
extern BOOL onLoadSettings(int recordaudio);

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

//Multilanguage

int languageID;

//Vars used for selecting fixed /variableregion
RECT rcOffset;
RECT rcClip;
RECT rcUse;
RECT old_rcClip;
BOOL bCapturing = FALSE;
POINT ptOrigin;

RECT rc;
HWND hMouseCaptureWnd;
HWND hWnd_FixedRegion;

HBITMAP hLogoBM = NULL;
CFlashingWnd* pFlashingWnd = 0;

//Misc Vars
int recordstate = 0;
int recordpaused = 0;
UINT interruptkey = 0;
int tdata = 0;
DWORD initialtime = 0;
int initcapture = 0;
int irsmallcount = 0;

// Messaging
HWND hWndGlobal = NULL;

//int tempPath_Access = USE_WINDOWS_TEMP_DIR;
CString specifieddir;

/////////////////////////////////////////////////////////////////////////////
//Variables/Options requiring interface
/////////////////////////////////////////////////////////////////////////////
int bits = 24;
//int flashingRect = 1;
//int launchPlayer = 3;
//int minimizeOnStart = 0;
//int MouseCaptureMode = 0;
//int capturewidth = 320;
//int captureheight = 240;
int DefineMode = 0; //set only in FixedRegion.cpp

//Video Options and Compressions
//int timelapse = 5;
//int frames_per_second = 200;
//int keyFramesEvery = 200;
//int compquality = 7000;
//DWORD compfccHandler = 0;

ICINFO * compressor_info = NULL;
int num_compressor = 0;
int selected_compressor = -1;

//Ver 1.2
//Video Compress Parameters
LPVOID pVideoCompressParams = NULL;
//DWORD CompressorStateIsFor = 0;
//DWORD CompressorStateSize = 0;

LPVOID pParamsUse = NULL;

//Report variables
int nActualFrame = 0;
int nCurrFrame = 0;
float fRate = 0.0;
float fActualRate = 0.0;
float fTimeLength = 0.0;
int nColors = 24;
int actualwidth = 0;
int actualheight = 0;

//Path to temporary avi file
CString tempfilepath;

//Autopan
//int autopan = 0;
//int maxpan = 20;
RECT panrect_current;
RECT panrect_dest;

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
//int recordaudio = 0;

//Audio Recording Variables
//UINT AudioDeviceID = WAVE_MAPPER;

HWAVEIN m_hRecord;
WAVEFORMATEX m_Format;
DWORD m_ThreadID;
int m_QueuedBuffers = 0;
int m_BufferSize = 1000; // number of samples
CSoundFile * pSoundFile = NULL;

//Audio Options Dialog
LPWAVEFORMATEX pwfx = NULL;
//DWORD cbwfx;

//Audio Formats Dialog
//DWORD waveinselected = WAVE_FORMAT_2S16;
//int audio_bits_per_sample = 16;
//int audio_num_channels = 2;
//int audio_samples_per_seconds = 22050;
//BOOL bAudioCompression = TRUE;

//BOOL interleaveFrames = TRUE;
//int interleaveFactor = 100;
//int interleaveUnit = MILLISECONDS;

/////////////////////////////////////////////////////////////////////////////
//ver 1.2
/////////////////////////////////////////////////////////////////////////////
//Key short-cuts variables
/////////////////////////////////////////////////////////////////////////////
//state vars
BOOL AllowNewRecordStartKey = TRUE;
int savesettings = 1;

//Enhanced video options
//int g_autoadjust = 1;
//int g_valueadjust = 1;

//Cursor Path, used for copying cursor file
CString g_cursorFilePath;

//ver 1.3
//int threadPriority = THREAD_PRIORITY_NORMAL;

//ver 1.5
//int captureleft = 100;
//int capturetop = 100;
//int fixedcapture = 0;

//int captureTrans = 1;

MCI_OPEN_PARMS mop;
MCI_SAVE_PARMS msp;
PSTR strFile;

WAVEFORMATEX m_FormatSpeaker;
DWORD waveinselected_Speaker = WAVE_FORMAT_4S16;
int audio_bits_per_sample_Speaker = 16;
int audio_num_channels_Speaker = 2;
int audio_samples_per_seconds_Speaker = 44100;

CAutoSearchDialog asd;
int asdCreated = FALSE;

int TroubleShootVal = 0;
//int performAutoSearch = 1;

//ver 1.8
int supportMouseDrag = 1;

CScreenAnnotations sadlg;
int sadlgCreated = 0;

CMenu contextmenu;

int isMciRecordOpen = 0;
int alreadyMCIPause = 0;

//ver 1.8
int vanWndCreated = 0;
//CVideoWnd m_vanWnd;

int keySCOpened = 0;

int audioTimeInitiated = 0;
int sdwSamplesPerSec = 22050;
int sdwBytesPerSec = 44100;

int timeshift = 100;
int frameshift = 0;
int shiftType = 0; // 0 : no shift, 1 : delayAudio, 2: delayVideo

int currentLayout = 0;

//ver 2.26 Vscap Interface
#define ModeAVI 0
#define ModeFlash 1
int RecordingMode = 0;

int launchPropPrompt = 0;
int launchHTMLPlayer = 1;
int deleteAVIAfterUse = 1;

int autonaming = 0;
int restrictVideoCodecs = 0;
//int base_nid = 0; //for keyboard shotcuts

//LPBITMAPINFO GetTextBitmap(CDC *thisDC, CRect* caprect,int factor,CRect* drawtextRect, LOGFONT* drawfont, CString textstr, CPen* pPen, CBrush * pBrush, COLORREF textcolor, int horzalign);
//HBITMAP DrawResampleRGB(CDC *thisDC, CRect* caprect,int factor, LPBITMAPINFOHEADER expanded_bmi, int xmove, int ymove);
//extern HANDLE AllocMakeDib( int reduced_width, int reduced_height, UINT bits);

int timestampAnnotation = 0;
int captionAnnotation = 0;
int watermarkAnnotation = 0;

TextAttributes caption = {TOP_LEFT, "ScreenCam", RGB(0, 0, 0), RGB(0xff, 0xff, 0xff), 0, 0};
TextAttributes timestamp = {TOP_LEFT, "", RGB(0, 0, 0), RGB(0xff, 0xff, 0xff), 0, 0};
ImageAttributes watermark = {TOP_LEFT, ""};

int presettime = 60;
int recordpreset = 0;

/////////////////////////////////////////////////////////////////////////////
//Function prototypes
/////////////////////////////////////////////////////////////////////////////

//Region Display Functions
void DrawSelect(HDC hdc, BOOL fDraw, LPRECT lprClip);

//Region Select Functions
long WINAPI MouseCaptureWndProc(HWND hWnd, UINT wMessage, WPARAM wParam, LPARAM lParam);
int InitDrawShiftWindow();
int InitSelectRegionWindow();
int CreateShiftWindow();
int DestroyShiftWindow();

//Misc Functions
void InsertHighLight(HDC hdc,int xoffset, int yoffset);

void InsertText(HDC hdc,int xoffset, int yoffset, LPCTSTR lpString);
void InsertText(HDC hdc, RECT& brect, TextAttributes& textattributes);
void InsertImage(HDC hdc, RECT& brect, ImageAttributes& imageattributes);

void FreeVideoCompressParams();
BOOL AllocVideoCompressParams(DWORD paramsSize);
void GetVideoCompressState (HIC hic, DWORD fccHandler);
void SetVideoCompressState (HIC hic, DWORD fccHandler);
void FreeParamsUse();
BOOL MakeCompressParamsCopy(DWORD paramsSize, LPVOID pOrg);
CString strCodec("MS Video 1");
//Files Directory
CString savedir("");
CString cursordir("");
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
void AutoSetRate(int val,int& framerate,int& delayms);
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

// Mouse Capture functions
// Cursor variables
HCURSOR hSavedCursor = NULL;
HCURSOR g_loadcursor = NULL;
HCURSOR g_customcursor = NULL;

//int g_recordcursor = 1;
//int g_customsel = 0;
//int g_cursortype = 0;
//int g_highlightcursor = 0;
//int g_highlightsize = 64;
//int g_highlightshape = 0;
//int g_highlightclick = 0;

//COLORREF g_highlightcolor = RGB(255,255,125);
//COLORREF g_highlightclickcolorleft = RGB(255,0,0);
//COLORREF g_highlightclickcolorright = RGB(0,0,255);

HCURSOR FetchCursorHandle();
HCURSOR FetchCursorHandle()
{
	switch (g_cursortype)
	{
	case 0:
		if (hSavedCursor == NULL)
			hSavedCursor = GetCursor();
		return hSavedCursor;
	case 1:
		return g_customcursor;
	default:
		return g_loadcursor;
	}
}

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
	if (keyRecordStartCtrl)
		modf |= MOD_CONTROL;

	if (keyRecordStartShift)
		modf |= MOD_SHIFT;

	if (keyRecordStartAlt)
		modf |= MOD_ALT;

	if (keyRecordStart != 100000) {
		BOOL ret = RegisterHotKey(hWndGlobal, nid, modf, keyRecordStart);
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
		BOOL ret = RegisterHotKey(hWndGlobal, nid, modf, keyRecordEnd);
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
		BOOL ret = RegisterHotKey(hWndGlobal, nid, modf, keyRecordCancel);
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
		BOOL ret = RegisterHotKey(hWndGlobal, nid, modf, keyNext);
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
		BOOL ret = RegisterHotKey(hWndGlobal, nid, modf, keyPrev);
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
		BOOL ret = RegisterHotKey(hWndGlobal, nid, modf, keyShowLayout);
		if (!ret)
			succ[5] = 1;
	}

	int tstatus = 0;	// TODO: this is a little silly
	return tstatus;
}

int SetAdjustHotKeys();
int SetAdjustHotKeys()
{
	int succ[6];
	int ret = SetHotKeys(succ);

	CString keystr;
	CString msgstr;
	CString outstr;
	if (succ[0]) {
		keyRecordStart = 100000;
		keyRecordStartCtrl = 0;
		keyRecordStartShift = 0;
		keyRecordStartAlt = 0;

		keystr.LoadString(IDS_STRINGSPRKEY);

		msgstr.LoadString(IDS_STRING_UNSSC);
		outstr.Format(msgstr,keystr);
		//keystr = "Unable to set shortcuts for %d" + keystr;
		MessageBox(NULL, outstr, "Note", MB_OK | MB_ICONEXCLAMATION);
	}

	if (succ[1]) {
		keyRecordEnd = 100000;
		keyRecordEndCtrl = 0;
		keyRecordEndShift = 0;
		keyRecordEndAlt = 0;

		keystr.LoadString(IDS_STRINGSRKEY);

		msgstr.LoadString(IDS_STRING_UNSSC);
		outstr.Format(msgstr,keystr);
		//keystr = "Unable to set shortcuts for %d" + keystr;
		MessageBox(NULL, outstr, "Note", MB_OK | MB_ICONEXCLAMATION);
	}

	if (succ[2]) {
		keyRecordCancel = 100000;
		keyRecordCancelCtrl = 0;
		keyRecordCancelShift = 0;
		keyRecordCancelAlt = 0;

		keystr.LoadString(IDS_STRINGCRKEY);

		msgstr.LoadString(IDS_STRING_UNSSC);
		outstr.Format(msgstr,keystr);
		//keystr = "Unable to set shortcuts for %d" + keystr;
		MessageBox(NULL, outstr, "Note", MB_OK | MB_ICONEXCLAMATION);
	}

	if (succ[3]) {
		keyNext = 100000;
		keyNextCtrl = 0;
		keyNextShift = 0;
		keyNextAlt = 0;

		keystr.LoadString(IDS_STRINGNLKEY);

		msgstr.LoadString(IDS_STRING_UNSSC);
		outstr.Format(msgstr,keystr);
		//keystr = "Unable to set shortcuts for %d" + keystr;
		MessageBox(NULL, outstr, "Note", MB_OK | MB_ICONEXCLAMATION);
	}

	if (succ[4]) {
		keyPrev = 100000;
		keyPrevCtrl = 0;
		keyPrevShift = 0;
		keyPrevAlt = 0;

		keystr.LoadString(IDS_STRINGPLKEY);

		msgstr.LoadString(IDS_STRING_UNSSC);
		outstr.Format(msgstr,keystr);
		//keystr = "Unable to set shortcuts for %d" + keystr;
		MessageBox(NULL, outstr, "Note", MB_OK | MB_ICONEXCLAMATION);
	}

	if (succ[5]) {
		keyShowLayout = 100000;
		keyShowLayoutCtrl = 0;
		keyShowLayoutShift = 0;
		keyShowLayoutAlt = 0;

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

HBITMAP savedBitmap = NULL;
/////////////////////////////////////////////////////////////////////////////
//
// DrawSelect
//
// Draws the selected clip rectangle with its dimensions on the DC
//
/////////////////////////////////////////////////////////////////////////////

void DrawSelect(HDC hdc, BOOL fDraw, LPRECT lprClip)
{
	RECT rectDraw = *lprClip;
	if (IsRectEmpty(&rectDraw)) {
		return;
	}

	// If a rectangular clip region has been selected, draw it
	HBRUSH newbrush = (HBRUSH) CreateHatchBrush(HS_BDIAGONAL, RGB(0,0,100));
	HBRUSH oldbrush = (HBRUSH) SelectObject(hdc, newbrush);

	//PatBlt SRCINVERT regardless fDraw is TRUE or FALSE
	const int DINV = 3;
	PatBlt(hdc, rectDraw.left, rectDraw.top, rectDraw.right-rectDraw.left, DINV, PATINVERT);
	PatBlt(hdc, rectDraw.left, rectDraw.bottom-DINV, DINV, -(rectDraw.bottom-rectDraw.top-2*DINV), PATINVERT);
	PatBlt(hdc, rectDraw.right-DINV, rectDraw.top+DINV, DINV, rectDraw.bottom-rectDraw.top-2*DINV, PATINVERT);
	PatBlt(hdc, rectDraw.right, rectDraw.bottom-DINV, -(rectDraw.right-rectDraw.left), DINV, PATINVERT);

	SelectObject(hdc,oldbrush);
	DeleteObject(newbrush);

	HDC hdcBits = CreateCompatibleDC(hdc);
	HFONT newfont = (HFONT) GetStockObject(ANSI_VAR_FONT);
	HFONT oldfont = (HFONT) SelectObject(hdc, newfont);

	char sz[80];
	wsprintf(sz, "Left : %d Top : %d Width : %d Height : %d", rectDraw.left, rectDraw.top, rectDraw.right - rectDraw.left+1, rectDraw.bottom - rectDraw.top+1);
	int len = lstrlen(sz);
	SIZE sExtent;
	DWORD dw = GetTextExtentPoint(hdc, sz, len, &sExtent);

	int dx = sExtent.cx;
	int dy = sExtent.cy;
	int x = rectDraw.left +10;
	int y = (rectDraw.top < (dy + DINV + 2))
		? rectDraw.bottom + DINV + 2
		: rectDraw.top - dy - DINV - 2;

	if (fDraw) {
		//Save Original Picture
		SaveBitmapCopy(savedBitmap, hdc, hdcBits, x - 4, y - 4, dx + 8, dy + 8);

		//Text
		COLORREF oldtextcolor = SetTextColor(hdc,RGB(0,0,0));
		COLORREF oldbkcolor = SetBkColor(hdc,RGB(255,255,255));
		SetBkMode(hdc,TRANSPARENT);

		RoundRect(hdc, x - 4, y - 4, x + dx + 4, y + dy + 4, 10, 10);

		SetBkMode(hdc,OPAQUE);

		ExtTextOut(hdc, x, y, 0, NULL, sz, len, NULL);
		SetBkColor(hdc,oldbkcolor);
		SetTextColor(hdc,oldtextcolor);
		SelectObject(hdc, oldfont);
	} else {
		RestoreBitmapCopy(savedBitmap, hdc, hdcBits, x - 4, y - 4, dx + 8, dy + 8);
	}

	//Icon
	if ((rectDraw.right-rectDraw.left-10 > 35) && (rectDraw.bottom-rectDraw.top-10 > dy + 40)) {
		HBITMAP hbv = LoadBitmap( AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BITMAP1));
		HBITMAP old_bitmap = (HBITMAP) SelectObject(hdcBits, hbv);
		BitBlt(hdc, rectDraw.left+10, rectDraw.bottom-42, 30, 32,hdcBits, 0,0, SRCINVERT);
		SelectObject(hdcBits,old_bitmap);
		DeleteObject(hbv);
	}

	DeleteDC(hdcBits);
}

/////////////////////////////////////////////////////////////////////////////
//
// MouseCaptureWndProc()
//
/////////////////////////////////////////////////////////////////////////////
long WINAPI MouseCaptureWndProc(HWND hWnd, UINT wMessage, WPARAM wParam, LPARAM lParam)
{
	switch (wMessage)
	{
	default:
		TRACE("MouseCaptureWndProc : skip message\n");
		break;
	case WM_MOUSEMOVE:
		{
			TRACE("MouseCaptureWndProc : WM_MOUSEMOVE\n");
			if (MouseCaptureMode == 0) {
				//Fixed Region
				POINT pt;
				GetCursorPos(&pt);

				rcClip.left = pt.x + rcOffset.left; // Update rect with new mouse info
				rcClip.top = pt.y + rcOffset.top;
				rcClip.right = pt.x + rcOffset.right;
				rcClip.bottom = pt.y + rcOffset.bottom;

				if (rcClip.left < 0) {
					rcClip.left = 0;
					rcClip.right = (rc.right - rc.left);
				}
				if (rcClip.top < 0) {
					rcClip.top = 0;
					rcClip.bottom = (rc.bottom - rc.top);
				}
				if (rcClip.right > maxxScreen - 1) {
					rcClip.right = maxxScreen - 1;
					rcClip.left = maxxScreen - 1 - (rc.right - rc.left);
				}
				if (rcClip.bottom > maxyScreen - 1) {
					rcClip.bottom = maxyScreen - 1;
					rcClip.top = maxyScreen - 1 - (rc.bottom - rc.top);
				}

				if (!isRectEqual(old_rcClip,rcClip)) {
					HDC hScreenDC = GetDC(hWnd);
					DrawSelect(hScreenDC, FALSE, &old_rcClip); // erase old rubber-band
					DrawSelect(hScreenDC, TRUE, &rcClip); // new rubber-band
					ReleaseDC(hWnd,hScreenDC);
				} // if old

				old_rcClip = rcClip;
			} else if (MouseCaptureMode == 1) { //Variable Region
				if (bCapturing) {
					POINT pt;
					GetCursorPos(&pt);

					HDC hScreenDC = GetDC(hWnd);

					DrawSelect(hScreenDC, FALSE, &rcClip); // erase old rubber-band

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
		}

	case WM_LBUTTONUP:
		{
			TRACE("MouseCaptureWndProc : WM_LBUTTONUP\n");

			if (MouseCaptureMode == 0) {
				//erase final
				HDC hScreenDC = GetDC(hWnd);
				DrawSelect(hScreenDC, FALSE, &old_rcClip);
				old_rcClip = rcClip;
				ReleaseDC(hWnd,hScreenDC);
			} else if (MouseCaptureMode == 1) {
				NormalizeRect(&rcClip);
				old_rcClip = rcClip;
				bCapturing = FALSE;
			}

			ShowWindow(hWnd, SW_HIDE);

			if (!IsRectEmpty(&old_rcClip))
			{
				NormalizeRect(&old_rcClip);
				CopyRect(&rcUse, &old_rcClip);
				if (DefineMode == 0)
				{
					TRACE("MouseCaptureWndProc: CRecorderView::WM_USER_RECORDSTART\n");
					::PostMessage (hWndGlobal, CRecorderView::WM_USER_RECORDSTART, 0, (LPARAM) 0);
				}
				else
				{
					TRACE("MouseCaptureWndProc: WM_USER_REGIONUPDATE\n");
					::PostMessage (hWnd_FixedRegion, WM_USER_REGIONUPDATE, 0, (LPARAM) 0);
				}
			}
		}

	case WM_LBUTTONDOWN:
		{
			TRACE("MouseCaptureWndProc : WM_LBUTTONDOWN\n");
			// User pressed left button, initialize selection
			// Set origin to current mouse position (in window coords)

			if (MouseCaptureMode == 1) {
				POINT pt;
				GetCursorPos(&pt);
				ptOrigin = pt;
				rcClip.left = rcClip.right = pt.x;
				rcClip.top = rcClip.bottom = pt.y;
				NormalizeRect(&rcClip); // Make sure it is a normal rect
				HDC hScreenDC = GetDC(hWnd);
				DrawSelect(hScreenDC, TRUE, &rcClip); // Draw the rubber-band box
				ReleaseDC(hWnd,hScreenDC);

				bCapturing = TRUE;
			}
		}

	case WM_RBUTTONDOWN:
		{
			TRACE("MouseCaptureWndProc : WM_RBUTTONDOWN\n");
			if (MouseCaptureMode == 0) {
				//Cancel the operation
				//erase final
				HDC hScreenDC = GetDC(hWnd);
				DrawSelect(hScreenDC, FALSE, &old_rcClip);
				ReleaseDC(hWnd,hScreenDC);

				//Cancel the operation
				ShowWindow(hWnd,SW_HIDE);

				//ver 1.2
				AllowNewRecordStartKey = TRUE;
			}
		}

	case WM_KEYDOWN:
		{
			TRACE("MouseCaptureWndProc : WM_KEYDOWN\n");
			int nVirtKey = (int) wParam; // virtual-key code
			int lKeyData = lParam; // key data

			//ver 1.2
			if (nVirtKey == (int) keyRecordCancel) { //Cancel the operation
				//if (nVirtKey==VK_ESCAPE) {
				if (MouseCaptureMode == 0) {
					//erase final
					HDC hScreenDC = GetDC(hWnd);
					DrawSelect(hScreenDC, FALSE, &old_rcClip);
					ReleaseDC(hWnd,hScreenDC);
				} else if (MouseCaptureMode == 1) {
					NormalizeRect(&rcClip);
					old_rcClip = rcClip;
					if (bCapturing) {
						bCapturing = FALSE;
					}
				}

				ShowWindow(hWnd,SW_HIDE);

				//ver 1.2
				AllowNewRecordStartKey = TRUE;
			} //VK_ESCAPE (keyRecordCancel)
		}
	}

	return DefWindowProc(hWnd, wMessage, wParam, lParam);
}

// Code For Creating a Window for Specifying Region
// A borderless, invisible window used only for capturing mouse input for the whole screen
int CreateShiftWindow()
{
	HINSTANCE hInstance = AfxGetInstanceHandle();
	HICON hcur = ::LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICONCROSSHAIR));

	WNDCLASS wndclass;
	wndclass.style = 0;
	wndclass.lpfnWndProc = (WNDPROC)MouseCaptureWndProc;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = hInstance;
	wndclass.hIcon = LoadIcon(hInstance, "WINCAP");
	wndclass.lpszMenuName = NULL;
	wndclass.lpszClassName = (LPSTR) "ShiftRegionWindow";
	wndclass.hCursor = hcur;
	wndclass.hbrBackground = NULL;

	if (!RegisterClass(&wndclass))
		return 0;

	HDC hScreenDC = ::GetDC(NULL);
	maxxScreen = ::GetDeviceCaps(hScreenDC, HORZRES);
	maxyScreen = ::GetDeviceCaps(hScreenDC, VERTRES);
	::ReleaseDC(NULL,hScreenDC);

	hMouseCaptureWnd = ::CreateWindowEx(WS_EX_TOPMOST, "ShiftRegionWindow", "Title", WS_POPUP, 0, 0, maxxScreen, maxyScreen, NULL, NULL, hInstance, NULL);
	TRACE("CreateShiftWindow : %s\n", ::IsWindow(hMouseCaptureWnd) ? "SUCCEEDED" : "FAIL");

	return 0;
}

int InitSelectRegionWindow()
{
	return 0;
}

int InitDrawShiftWindow()
{
	HDC hScreenDC = ::GetDC(hMouseCaptureWnd);

	FixRectSizePos(&rc, maxxScreen, maxyScreen);

	rcClip.left=rc.left;
	rcClip.top=rc.top;
	rcClip.right=rc.right;
	rcClip.bottom=rc.bottom;
	DrawSelect(hScreenDC, TRUE, &rcClip);

	old_rcClip = rcClip;

	//Set Curosr at the centre of the clip rectangle
	POINT ptOrigin;
	ptOrigin.x=(rcClip.right+rcClip.left)/2;
	ptOrigin.y=(rcClip.top+rcClip.bottom)/2;

	rcOffset.left=rcClip.left-ptOrigin.x;
	rcOffset.top=rcClip.top-ptOrigin.y;
	rcOffset.right=rcClip.right-ptOrigin.x;
	rcOffset.bottom=rcClip.bottom-ptOrigin.y;

	::ReleaseDC(hMouseCaptureWnd,hScreenDC);

	return 0;
}

int DestroyShiftWindow()
{
	if (hMouseCaptureWnd)
		::DestroyWindow(hMouseCaptureWnd);
	return 0;
}

///////////////////////// //////////////////
/////////////// Functions //////////////////
///////////////////////// //////////////////

void InsertHighLight(HDC hdc,int xoffset, int yoffset)
{
	CSize fullsize;
	fullsize.cx=128;
	fullsize.cy=128;

	int highlightsize = g_highlightsize;
	COLORREF highlightcolor = g_highlightcolor;
	if (g_highlightclick==1) {
		SHORT tmpShort = -999;
		tmpShort=GetKeyState(VK_LBUTTON);
		if (tmpShort!=0 && tmpShort!=1) {
			highlightcolor=g_highlightclickcolorleft;
			highlightsize *= 1.5;
		}
		tmpShort=GetKeyState(VK_RBUTTON);
		if (tmpShort!=0 && tmpShort!=1) {
			highlightcolor=g_highlightclickcolorright;
			highlightsize *= 1.5;
		}
	}

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
	} else if ((highlightshape == 1) || (highlightshape == 3)) { //ellipse and rectangle
		x1 = (fullsize.cx - highlightsize)/2.0;
		x2 = (fullsize.cx + highlightsize)/2.0;
		y1 = (fullsize.cy - highlightsize/2.0)/2.0;
		y2 = (fullsize.cy + highlightsize/2.0)/2.0;
	}

	HBRUSH ptbrush = (HBRUSH) ::GetStockObject(WHITE_BRUSH);
	HPEN nullpen = CreatePen(PS_NULL,0,0);
	HBRUSH hlbrush = CreateSolidBrush( highlightcolor);

	HBRUSH oldbrush = (HBRUSH) ::SelectObject(hdcBits,ptbrush);
	HPEN oldpen = (HPEN) ::SelectObject(hdcBits,nullpen);
	::Rectangle(hdcBits, 0,0,fullsize.cx+1,fullsize.cy+1);
	//CString xxx;
	//xxx.Format("%d",tmpShort);
	//CRect r=CRect(0,0,fullsize.cx+1,fullsize.cy+1);
	//::DrawText(hdcBits,xxx,xxx.GetLength(),r,DT_SINGLELINE);

	//Draw the highlight
	::SelectObject(hdcBits,hlbrush);

	if ((highlightshape == 0) || (highlightshape == 1)) { //circle and ellipse
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

void InsertText(HDC hdc, RECT& brect, TextAttributes& textattributes)
{
	RECT rect;
	SIZE size, full_size;
	UINT uFormat;

	size_t length = ::strlen(textattributes.text);

	//OffScreen Buffer
	HBITMAP hbm = NULL;
	HBITMAP old_bitmap;
	HDC hdcBits = ::CreateCompatibleDC(hdc);

	HFONT hf;
	HFONT old_font;
	if (textattributes.isFontSelected){
		hf = ::CreateFontIndirect(&textattributes.logfont);
		old_font = SelectFont(hdcBits, hf);
	}

	::GetTextExtentPoint32(hdcBits, textattributes.text, length, &size);
	full_size.cx = size.cx + 10;
	full_size.cy = size.cy + 10;
	RECT mRect;
	mRect.top = 0;
	mRect.left = 0;
	mRect.bottom = mRect.top + full_size.cy;
	mRect.right = mRect.left + full_size.cx;
	//full_size.cx = 2 * size.cx;
	//full_size.cy = 2 * size.cy;
	switch(textattributes.position)
	{
	case TOP_LEFT:
		rect.left = 0;
		rect.top = 0;
		break;
	case TOP_CENTER:
		rect.left = (brect.right - brect.left - full_size.cx) / 2;
		rect.top = 0;
		break;
	case TOP_RIGHT:
		rect.left = brect.right - full_size.cx;
		rect.top = 0;
		break;
	case CENTER_LEFT:
		rect.left = 0;
		rect.top = (brect.bottom - brect.top - full_size.cy) / 2;
		break;
	case CENTER_CENTER:
		rect.left = (brect.right - brect.left - full_size.cx) / 2;
		rect.top = (brect.bottom - brect.top - full_size.cy) / 2;
		break;
	case CENTER_RIGHT:
		rect.left = brect.right - full_size.cx;
		rect.top = (brect.bottom - brect.top - full_size.cy) / 2;
		break;
	case BOTTOM_LEFT:
		rect.left = 0;
		rect.top = brect.bottom - full_size.cy;
		break;
	case BOTTOM_CENTER:
		rect.left = (brect.right - brect.left - full_size.cx) / 2;
		rect.top = brect.bottom - full_size.cy;
		break;
	case BOTTOM_RIGHT:
		rect.left = brect.right - full_size.cx;
		rect.top = brect.bottom - full_size.cy;
		break;
	default:
		rect.left = 0;
		rect.top = 0;
		break;
	}
	rect.right = rect.left + full_size.cx;
	rect.bottom = rect.top + full_size.cy;

	hbm = (HBITMAP) ::CreateCompatibleBitmap(hdc, full_size.cx,full_size.cy);
	old_bitmap = (HBITMAP) ::SelectObject(hdcBits,hbm);

	HBRUSH ptbrush = ::CreateSolidBrush(textattributes.backgroundColor);//GetStockBrush(BLACK_BRUSH);
	HBRUSH oldbrush = SelectBrush(hdcBits,ptbrush);
	::Rectangle(hdcBits, 0,0,full_size.cx,full_size.cy);
	SelectBrush(hdcBits,oldbrush);
	DeleteBrush(ptbrush);

	uFormat = DT_CENTER | DT_SINGLELINE | DT_VCENTER;

	COLORREF old_bk_color = ::GetBkColor(hdcBits);
	COLORREF old_txt_color = ::GetTextColor(hdcBits);
	::SetBkColor(hdcBits, textattributes.backgroundColor);
	::SetTextColor(hdcBits, textattributes.textColor);
	::DrawTextEx(hdcBits, LPTSTR((LPCTSTR)textattributes.text), length, &mRect, uFormat, NULL);
	::SetTextColor(hdcBits, old_txt_color);
	::SetBkColor(hdcBits, old_bk_color);

	//OffScreen Buffer
	BitBlt(hdc, rect.left, rect.top, full_size.cx, full_size.cy, hdcBits, 0, 0, SRCCOPY);

	SelectObject(hdcBits, old_bitmap);
	DeleteObject(hbm);

	if (textattributes.isFontSelected){
		SelectFont(hdcBits, old_font);
		DeleteFont(hf);
	}

	DeleteDC(hdcBits);
}

void InsertImage(HDC hdc, RECT& brect, ImageAttributes& imageattributes)
{
	static CxImage image;
	static CString imageName = "";

	// load
	if (imageName != imageattributes.text) {
		image.Destroy();

		CString extin(FindExtension(imageattributes.text));
		extin.MakeLower();
		int typein = FindFormat(extin);
		if (typein == CXIMAGE_FORMAT_UNKNOWN) {
			return;
		}

		if (!image.Load(imageattributes.text, typein)){
			return;
		}

		imageName = imageattributes.text;// cache

	} else {
		// image is cached
	}

	RECT rect;
	SIZE size, full_size;

	size.cx = image.GetWidth();
	size.cy = image.GetHeight();

	full_size.cx = size.cx;
	full_size.cy = size.cy;
	RECT mRect;
	mRect.top = 0;
	mRect.left = 0;
	mRect.bottom = mRect.top + full_size.cy;
	mRect.right = mRect.left + full_size.cx;
	//full_size.cx = 2 * size.cx;
	//full_size.cy = 2 * size.cy;
	switch(imageattributes.position)
	{
	case TOP_LEFT:
		rect.left = 0;
		rect.top = 0;
		break;
	case TOP_CENTER:
		rect.left = (brect.right - brect.left - full_size.cx) / 2;
		rect.top = 0;
		break;
	case TOP_RIGHT:
		rect.left = brect.right - full_size.cx;
		rect.top = 0;
		break;
	case CENTER_LEFT:
		rect.left = 0;
		rect.top = (brect.bottom - brect.top - full_size.cy) / 2;
		break;
	case CENTER_CENTER:
		rect.left = (brect.right - brect.left - full_size.cx) / 2;
		rect.top = (brect.bottom - brect.top - full_size.cy) / 2;
		break;
	case CENTER_RIGHT:
		rect.left = brect.right - full_size.cx;
		rect.top = (brect.bottom - brect.top - full_size.cy) / 2;
		break;
	case BOTTOM_LEFT:
		rect.left = 0;
		rect.top = brect.bottom - full_size.cy;
		break;
	case BOTTOM_CENTER:
		rect.left = (brect.right - brect.left - full_size.cx) / 2;
		rect.top = brect.bottom - full_size.cy;
		break;
	case BOTTOM_RIGHT:
		rect.left = brect.right - full_size.cx;
		rect.top = brect.bottom - full_size.cy;
		break;
	default:
		rect.left = 0;
		rect.top = 0;
		break;
	}
	rect.right = rect.left + full_size.cx;
	rect.bottom = rect.top + full_size.cy;

	image.Draw(hdc, rect);

	////OffScreen Buffer
	//HDC hdcBits = ::CreateCompatibleDC(hdc);
	//HBITMAP hbm = image.MakeBitmap(hdcBits);
	////hbm = (HBITMAP) ::CreateCompatibleBitmap(hdc, full_size.cx,full_size.cy);
	//HBITMAP old_bitmap = (HBITMAP) ::SelectObject(hdcBits,hbm);
	//BitBlt(hdc, rect.left, rect.top, full_size.cx, full_size.cy, hdcBits, 0, 0, SRCCOPY);
	//SelectObject(hdcBits, old_bitmap);
	//DeleteObject(hbm);
	//DeleteDC(hdcBits);
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

	m_BufferSize = 1000; // samples per callback

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
	tempaudiopath = GetTempPath (tempPath_Access, specifieddir) + fileName;

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
			tempaudiopath = GetTempPath (tempPath_Access, specifieddir) + fxstr + cnumstr + exstr;

			//MessageBox(NULL,tempaudiopath,"Uses Temp File",MB_OK);
			//fileverified = 1;
			//Try choosing another temporary filename
		}
	}
}

void SetBufferSize(int NumberOfSamples)
{
	m_BufferSize = NumberOfSamples;
}

BOOL StartAudioRecording(WAVEFORMATEX* format)
{
	if (format != NULL)
		m_Format = *format;

	// open wavein device
	//use on message to map.....
	MMRESULT mmReturn = ::waveInOpen(&m_hRecord, AudioDeviceID, &m_Format,(DWORD) hWndGlobal, NULL, CALLBACK_WINDOW);
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

	audioTimeInitiated = 1;
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
	CBuffer buf(m_Format.nBlockAlign * m_BufferSize, false);
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

	audioTimeInitiated = 0;
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
	MMRESULT mmresult = acmMetrics(NULL, ACM_METRIC_MAX_SIZE_FORMAT, &cbwfx);
	if (MMSYSERR_NOERROR != mmresult) {
		//CString msgstr;
		//msgstr.Format("Metrics failed mmresult=%u!", mmresult);
		//::MessageBox(NULL,msgstr,"Note", MB_OK | MB_ICONEXCLAMATION);

		MessageOut(NULL,IDS_STRING_METRICSFAILED, IDS_STRING_NOTE, MB_OK | MB_ICONEXCLAMATION,mmresult);
		return;
	}

	pwfx = (LPWAVEFORMATEX)GlobalAllocPtr(GHND, cbwfx);
	if (NULL == pwfx) {
		//CString msgstr;
		//msgstr.Format("GlobalAllocPtr(%lu) failed!", cbwfx);
		//::MessageBox(NULL,msgstr,"Note", MB_OK | MB_ICONEXCLAMATION);

		MessageOut(NULL,IDS_STRING_GALLOC,IDS_STRING_NOTE,MB_OK | MB_ICONEXCLAMATION);
		return;
	}

	initial_audiosetup = 1;
}

//Build Recording Format to m_Format
void BuildRecordingFormat()
{
	m_Format.wFormatTag = WAVE_FORMAT_PCM;
	m_Format.wBitsPerSample = audio_bits_per_sample;
	m_Format.nSamplesPerSec = audio_samples_per_seconds;
	m_Format.nChannels = audio_num_channels;
	m_Format.nBlockAlign = m_Format.nChannels * (m_Format.wBitsPerSample/8);
	m_Format.nAvgBytesPerSec = m_Format.nSamplesPerSec * m_Format.nBlockAlign;
	m_Format.cbSize = 0;
}

//Suggest Save/Compress Format to pwfx
void SuggestRecordingFormat()
{
	WAVEINCAPS pwic;
	MMRESULT mmr = waveInGetDevCaps( AudioDeviceID, &pwic, sizeof(pwic));

	//Ordered in preference of choice
	if ((pwic.dwFormats) & WAVE_FORMAT_2S16) {
		audio_bits_per_sample = 16;
		audio_num_channels = 2;
		audio_samples_per_seconds = 22050;
		waveinselected = WAVE_FORMAT_2S16;
	} else if ((pwic.dwFormats) & WAVE_FORMAT_2M08) {
		audio_bits_per_sample = 8;
		audio_num_channels = 1;
		audio_samples_per_seconds = 22050;
		waveinselected = WAVE_FORMAT_2M08;
	} else if ((pwic.dwFormats) & WAVE_FORMAT_2S08) {
		audio_bits_per_sample = 8;
		audio_num_channels = 2;
		audio_samples_per_seconds = 22050;
		waveinselected = WAVE_FORMAT_2S08;
	} else if ((pwic.dwFormats) & WAVE_FORMAT_2M16) {
		audio_bits_per_sample = 16;
		audio_num_channels = 1;
		audio_samples_per_seconds = 22050;
		waveinselected = WAVE_FORMAT_2M16;
	} else if ((pwic.dwFormats) & WAVE_FORMAT_1M08) {
		audio_bits_per_sample = 8;
		audio_num_channels = 1;
		audio_samples_per_seconds = 11025;
		waveinselected = WAVE_FORMAT_1M08;
	} else if ((pwic.dwFormats) & WAVE_FORMAT_1M16) {
		audio_bits_per_sample = 16;
		audio_num_channels = 1;
		audio_samples_per_seconds = 11025;
		waveinselected = WAVE_FORMAT_1M16;
	} else if ((pwic.dwFormats) & WAVE_FORMAT_1S08) {
		audio_bits_per_sample = 8;
		audio_num_channels = 2;
		audio_samples_per_seconds = 11025;
		waveinselected = WAVE_FORMAT_1S08;
	} else if ((pwic.dwFormats) & WAVE_FORMAT_1S16) {
		audio_bits_per_sample = 16;
		audio_num_channels = 2;
		audio_samples_per_seconds = 11025;
		waveinselected = WAVE_FORMAT_1S16;
	} else if ((pwic.dwFormats) & WAVE_FORMAT_4M08) {
		audio_bits_per_sample = 8;
		audio_num_channels = 1;
		audio_samples_per_seconds = 44100;
		waveinselected = WAVE_FORMAT_4M08;
	} else if ((pwic.dwFormats) & WAVE_FORMAT_4M16) {
		audio_bits_per_sample = 16;
		audio_num_channels = 1;
		audio_samples_per_seconds = 44100;
		waveinselected = WAVE_FORMAT_4M16;
	} else if ((pwic.dwFormats) & WAVE_FORMAT_4S08) {
		audio_bits_per_sample = 8;
		audio_num_channels = 2;
		audio_samples_per_seconds = 44100;
		waveinselected = WAVE_FORMAT_4S08;
	} else if ((pwic.dwFormats) & WAVE_FORMAT_4S16) {
		audio_bits_per_sample = 16;
		audio_num_channels = 2;
		audio_samples_per_seconds = 44100;
		waveinselected = WAVE_FORMAT_4S16;
	} else {
		//Arbitrarily choose one
		audio_bits_per_sample = 8;
		audio_num_channels = 1;
		audio_samples_per_seconds = 22050;
		waveinselected = WAVE_FORMAT_2M08;
	}

	//Build m_Format
	BuildRecordingFormat();
}

void SuggestCompressFormat()
{
	bAudioCompression = TRUE;

	AllocCompressFormat();

	//1st try MPEGLAYER3
	BuildRecordingFormat();
	MMRESULT mmr;
	if ((m_Format.nSamplesPerSec == 22050) && (m_Format.nChannels==2) && (m_Format.wBitsPerSample <= 16)) {
		pwfx->wFormatTag = WAVE_FORMAT_MPEGLAYER3;
		mmr = acmFormatSuggest(NULL, &m_Format, pwfx, cbwfx, ACM_FORMATSUGGESTF_WFORMATTAG);
	}

	if (mmr!=0) {
		//ver 1.6, use PCM if MP3 not available

		//Then try ADPCM
		//BuildRecordingFormat();
		//pwfx->wFormatTag = WAVE_FORMAT_ADPCM;
		//MMRESULT mmr = acmFormatSuggest(NULL, &m_Format, pwfx, cbwfx, ACM_FORMATSUGGESTF_WFORMATTAG);

		if (mmr!=0) {
			//Use the PCM as default
			BuildRecordingFormat();
			pwfx->wFormatTag = WAVE_FORMAT_PCM;
			MMRESULT mmr = acmFormatSuggest(NULL, &m_Format, pwfx, cbwfx, ACM_FORMATSUGGESTF_WFORMATTAG);

			if (mmr!=0) {
				bAudioCompression = FALSE;
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
		double interfloat = (((double) interleaveFactor) * ((double) frames_per_second))/1000.0;
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
		double interfloat = (((double) interleaveFactor) * ((double) frames_per_second))/1000.0;
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
			CompressorStateIsFor = fccHandler;
			CompressorStateSize = statesize;
		}
	}
}

void SetVideoCompressState (HIC hic, DWORD fccHandler)
{
	if (CompressorStateIsFor == fccHandler) {
		if (pVideoCompressParams) {
			DWORD ret = ICSetState(hic,(LPVOID) pVideoCompressParams,CompressorStateSize);
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
	MMRESULT mmr = waveInGetDevCaps(AudioDeviceID, &pwic, sizeof(pwic));
	if ((pwic.dwFormats) & waveinselected) {
		BuildRecordingFormat();
	} else {
		SuggestRecordingFormat();
	}
}

void AttemptCompressFormat()
{
	bAudioCompression = TRUE;
	AllocCompressFormat();
	BuildRecordingFormat();
	//Test Compatibility
	MMRESULT mmr = acmFormatSuggest(NULL, &m_Format, pwfx, cbwfx, ACM_FORMATSUGGESTF_NCHANNELS | ACM_FORMATSUGGESTF_NSAMPLESPERSEC | ACM_FORMATSUGGESTF_WBITSPERSAMPLE | ACM_FORMATSUGGESTF_WFORMATTAG);
	if (mmr!=0) {
		SuggestCompressFormat();
	}
}

CString GetCodecDescription(long fccHandler)
{
	// TODO: initialization
	ICINFO compinfo;
	HIC hic = ICOpen(ICTYPE_VIDEO, fccHandler, ICMODE_QUERY);
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

		////Automatically Adjust the Quality for MSVC (MS Video 1) if the framerate is too high
		//int sel = ((CComboBox *) GetDlgItem(IDC_COMPRESSORS))->GetCurSel();
		//if (compressor_info[sel].fccHandler==mmioFOURCC('M', 'S', 'V', 'C')) {
		//	int cQuality = ((CSliderCtrl *) GetDlgItem(IDC_QUALITY_SLIDER))->GetPos();
		//	if (cQuality<80) {
		//		((CSliderCtrl *) GetDlgItem(IDC_QUALITY_SLIDER))->SetPos(80);
		//	}
		//}
	} else if (val<=56) { //fps more than 1 per second
		framerate=(57-val); //framerate 39 to 1;
		//18 corr to 39, 56 corr to 1

		delayms = 1000/framerate;
	} else if (val<=86) { //assume timelapse
		framerate = 20;
		delayms = (val-56)*1000;

		//57 corr to 1000, 86 corr to 30000 (20 seconds)
	} else if (val<=99) { //assume timelapse
		framerate = 20;
		delayms = (val-86)*2000+30000;

		//87 corr to 30000, 99 corr to 56000 (56 seconds)
	} else { //val=100, timelapse
		framerate = 20;
		delayms = 60000;

		//100 corr to 60000
	}
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
		MessageBox( NULL, buffer, "MCI_RECORD",MB_ICONEXCLAMATION | MB_OK);
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
	msp.lpfilename = LPCTSTR(strFile);

	if (isMciRecordOpen==0) {
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

//The setting/suggesting of format for recording Speakers is deferred until recording starts
//Default is to use best settings avalable
void mciSetWaveFormat()
{
	MCI_WAVE_SET_PARMS set_parms;
	DWORD dwReturn;
	char buffer[128];

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
		set_parms.nAvgBytesPerSec = m_FormatSpeaker.nAvgBytesPerSec;
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

void SuggestSpeakerRecordingFormat(int i)
{
	//Ordered in preference of choice
	switch (i)
	{
	case 0:
		{
			audio_bits_per_sample_Speaker = 16;
			audio_num_channels_Speaker = 2;
			audio_samples_per_seconds_Speaker = 44100;
			waveinselected_Speaker = WAVE_FORMAT_4S16;
			break;
		}
	case 1:
		{
			audio_bits_per_sample_Speaker = 16;
			audio_num_channels_Speaker = 2;
			audio_samples_per_seconds_Speaker = 22050;
			waveinselected_Speaker = WAVE_FORMAT_2S16;
			break;
		}
	case 2:
		{
			audio_bits_per_sample_Speaker = 8;
			audio_num_channels_Speaker = 2;
			audio_samples_per_seconds_Speaker = 44100;
			waveinselected_Speaker = WAVE_FORMAT_4S08;
			break;
		}
	case 3:
		{
			audio_bits_per_sample_Speaker = 8;
			audio_num_channels_Speaker = 2;
			audio_samples_per_seconds_Speaker = 22050;
			waveinselected_Speaker = WAVE_FORMAT_2S08;
			break;
		}
	case 4:
		{
			audio_bits_per_sample_Speaker = 16;
			audio_num_channels_Speaker = 1;
			audio_samples_per_seconds_Speaker = 44100;
			waveinselected_Speaker = WAVE_FORMAT_4M16;
			break;
		}
	case 5:
		{
			audio_bits_per_sample_Speaker = 8;
			audio_num_channels_Speaker = 1;
			audio_samples_per_seconds_Speaker = 44100;
			waveinselected_Speaker = WAVE_FORMAT_4M08;
			break;
		}
	case 6:
		{
			audio_bits_per_sample_Speaker = 16;
			audio_num_channels_Speaker = 1;
			audio_samples_per_seconds_Speaker = 22050;
			waveinselected_Speaker = WAVE_FORMAT_2M16;
			break;
		}
	case 7:
		{
			audio_bits_per_sample_Speaker = 16;
			audio_num_channels_Speaker = 2;
			audio_samples_per_seconds_Speaker = 11025;
			waveinselected_Speaker = WAVE_FORMAT_1S16;
			break;
		}
	case 8:
		{
			audio_bits_per_sample_Speaker = 8;
			audio_num_channels_Speaker = 1;
			audio_samples_per_seconds_Speaker = 22050;
			waveinselected_Speaker = WAVE_FORMAT_2M08;
			break;
		}
	case 9:
		{
			audio_bits_per_sample_Speaker = 8;
			audio_num_channels_Speaker = 2;
			audio_samples_per_seconds_Speaker = 11025;
			waveinselected_Speaker = WAVE_FORMAT_1S08;
			break;
		}
	default :
		{
			audio_bits_per_sample_Speaker = 8;
			audio_num_channels_Speaker = 1;
			audio_samples_per_seconds_Speaker = 11025;
			waveinselected_Speaker = WAVE_FORMAT_1M08;
		}
	}
}

void BuildSpeakerRecordingFormat()
{
	m_FormatSpeaker.wFormatTag = WAVE_FORMAT_PCM;
	m_FormatSpeaker.wBitsPerSample = audio_bits_per_sample_Speaker;
	m_FormatSpeaker.nSamplesPerSec = audio_samples_per_seconds_Speaker;
	m_FormatSpeaker.nChannels = audio_num_channels_Speaker;
	m_FormatSpeaker.nBlockAlign = m_FormatSpeaker.nChannels * (m_FormatSpeaker.wBitsPerSample/8);
	m_FormatSpeaker.nAvgBytesPerSec = m_FormatSpeaker.nSamplesPerSec * m_FormatSpeaker.nBlockAlign;
	m_FormatSpeaker.cbSize = 0;
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
//	int bits = 24;
//
//	LONG Width = expanded_bmi->biWidth;
//	LONG Height = expanded_bmi->biHeight;
//	long Rowbytes = (Width*bits+31)/32 *4;
//
//	long reduced_width = Width/factor;
//	long reduced_height = Height/factor;
//	long reduced_rowbytes = (reduced_width*bits+31)/32 *4;
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
//	// Get the bits from the bitmap and stuff them after the LPBI
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
	ON_COMMAND(ID_OPTIONS_RECORDAUDIO, OnOptionsRecordaudio)
	ON_UPDATE_COMMAND_UI(ID_OPTIONS_RECORDAUDIO, OnUpdateOptionsRecordaudio)
	ON_COMMAND(ID_OPTIONS_AUDIOFORMAT, OnOptionsAudioformat)
	ON_COMMAND(ID_OPTIONS_AUDIOSPEAKERS, OnOptionsAudiospeakers)
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
	ON_COMMAND(ID_OPTIONS_NAMING_ASK, OnOptionsNamingAsk)
	ON_UPDATE_COMMAND_UI(ID_OPTIONS_NAMING_ASK, OnUpdateOptionsNamingAsk)
	ON_COMMAND(ID_OPTIONS_PROGRAMOPTIONS_PRESETTIME, OnOptionsProgramoptionsPresettime)
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
	if (recordstate == 0) {
		DisplayRecordingMsg(dc);
		return;
	}
	//Display the record information when recording
	if (recordstate == 1) {
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

	// TODO: Add your specialized creation code here
	hWndGlobal = m_hWnd;
	VERIFY(0 == SetAdjustHotKeys());
	LoadSettings();

	CreateShiftWindow();

	HDC hScreenDC = ::GetDC(NULL);
	bits = ::GetDeviceCaps(hScreenDC, BITSPIXEL);
	nColors = bits;
	::ReleaseDC(NULL,hScreenDC);

	compfccHandler = mmioFOURCC('M', 'S', 'V', 'C');

	hLogoBM = LoadBitmap( AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BITMAP3));

	CRect rect(0, 0, maxxScreen - 1, maxyScreen - 1);
	pFlashingWnd = new CFlashingWnd;
	pFlashingWnd->CreateFlashing("Flashing", rect);

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

	srand( (unsigned)time( NULL));

	return 0;
}

void CRecorderView::OnDestroy()
{
	CView::OnDestroy();

	// TODO: Add your message handler code here

	//ver 1.2
	DecideSaveSettings();

	//ver 1.2
	//UninstallMyKeyHook(hWndGlobal);
	UnSetHotKeys();

	DestroyShiftWindow();

	if (compressor_info != NULL) {
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
	gList.FreeDisplayArray();
	gList.FreeShapeArray();
	gList.FreeLayoutArray();
}

LRESULT CRecorderView::OnRecordStart(UINT wParam, LONG lParam)
{
	TRACE("CRecorderView::OnRecordStart\n");
	CStatusBar* pStatus = (CStatusBar*) AfxGetApp()->m_pMainWnd->GetDescendantWindow(AFX_IDW_STATUS_BAR);
	pStatus->SetPaneText(0,"Press the Stop Button to stop recording");

	//if (minimizeOnStart == 1)
	// AfxGetMainWnd()->ShowWindow(SW_MINIMIZE);

	//ver 1.2
	if (minimizeOnStart == 1)
		::PostMessage(AfxGetMainWnd()->m_hWnd,WM_SYSCOMMAND,SC_MINIMIZE,0);

	//Check validity of rc and fix it
	FixRectSizePos(&rc, maxxScreen, maxyScreen);

	InstallMyHook(hWndGlobal,WM_USER_SAVECURSOR);

	recordstate = 1;
	interruptkey = 0;

	CWinThread * pThread = AfxBeginThread(RecordAVIThread, &tdata);

	//Ver 1.3
	if (pThread)
		pThread->SetThreadPriority(threadPriority);

	//Ver 1.2
	AllowNewRecordStartKey = TRUE; //allow this only after recordstate is set to 1

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

	recordstate = 0;

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
	hSavedCursor = (HCURSOR) wParam;

	return 0;
}

void CRecorderView::OnRegionRubber()
{
	MouseCaptureMode=1;
}

void CRecorderView::OnRegionPanregion()
{
	DefineMode = 0;

	CFixedRegion cfrdlg;
	cfrdlg.DoModal();

	MouseCaptureMode = 0;
	DefineMode = 0;
}

void CRecorderView::OnRegionFullscreen()
{
	MouseCaptureMode=2;
}

//This function is called when the avi saving is completed
LRESULT CRecorderView::OnUserGeneric (UINT wParam, LONG lParam)
{
	//ver 1.2
	::SetForegroundWindow( AfxGetMainWnd()->m_hWnd);
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
	if (RecordingMode == ModeAVI) {
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

	if ((RecordingMode == ModeAVI) && (autonaming > 0)) {
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
			DeleteFile(tempfilepath);
			if (recordaudio) {
				DeleteFile(tempaudiopath);
			}
			return 0;
		}
	}

	//ver 2.26
	if (RecordingMode == ModeFlash) {
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
	if (recordaudio) {
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
		//if (recordaudio==2) {
		//if ((recordaudio==2) || (useMCI)) {
		//ver 2.26 ...overwrite audio settings for Flash Moe recording ... no compression used...
		if ((recordaudio == 2) || (useMCI) || (RecordingMode == ModeFlash)) {
			result = Merge_Video_And_Sound_File(tempfilepath, tempaudiopath, m_newfile, FALSE, pwfx, cbwfx,interleaveFrames,interleaveFactor, interleaveUnit);
		} else if (recordaudio==1) {
			result = Merge_Video_And_Sound_File(tempfilepath, tempaudiopath, m_newfile, bAudioCompression, pwfx, cbwfx,interleaveFrames,interleaveFactor, interleaveUnit);
		}

		//Check Results : Attempt Recovery on error
		if (result==0) {
			//Successful
			DeleteFile(tempfilepath);
			DeleteFile(tempaudiopath);
		} else if (result==1) { //video file broken
			//Unable to recover
			DeleteFile(tempfilepath);
			DeleteFile(tempaudiopath);
		} else if (result==3) { //this case is rare
			//Unable to recover
			DeleteFile(tempfilepath);
			DeleteFile(tempaudiopath);
		} else if ((result==2) || (result==4)) { //recover video file
			//video file is ok, but not audio file
			//so copy the video file as avi and ignore the audio
			if (!CopyFile( tempfilepath,m_newfile,FALSE)) {
				//Although there is error copying, the temp file still remains in the temp directory and is not deleted, in case user wants a manual recover
				//MessageBox("File Creation Error. Unable to rename/copy file.","Note",MB_OK | MB_ICONEXCLAMATION);
				MessageOut(m_hWnd,IDS_STRING_FILECREATIONERROR,IDS_STRING_NOTE,MB_OK | MB_ICONEXCLAMATION);
				return 0;
			}
			DeleteFile(tempfilepath);
			DeleteFile(tempaudiopath);

			//::MessageBox(NULL,"Your AVI movie will not contain a soundtrack. CamStudio is unable to merge the video with audio.","Note",MB_OK | MB_ICONEXCLAMATION);
			MessageOut(m_hWnd,IDS_STRING_NOSOUNDTRACK,IDS_STRING_NOTE,MB_OK | MB_ICONEXCLAMATION);
		} else if (result == 5) {
			//recover both files, but as separate files
			CString m_audioext(".wav");
			CString m_audiofile = m_newfile + m_audioext;

			if (!CopyFile( tempfilepath,m_newfile,FALSE)) {
				//MessageBox("File Creation Error. Unable to rename/copy video file.","Note",MB_OK | MB_ICONEXCLAMATION);
				MessageOut(m_hWnd,IDS_STRINGFILECREATION2,IDS_STRING_NOTE,MB_OK | MB_ICONEXCLAMATION);
				return 0;
			}
			DeleteFile(tempfilepath);

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
		if (!CopyFile( tempfilepath,m_newfile,FALSE)) {
			//Ver 1.1
			//DeleteFile(m_newfile);
			//MessageBox("File Creation Error. Unable to rename/copy file. The file may be opened by another application. Please use another filename.","Note",MB_OK | MB_ICONEXCLAMATION);
			MessageOut(m_hWnd,IDS_STRING_FILECREATION4,IDS_STRING_NOTE,MB_OK | MB_ICONEXCLAMATION);
			//Repeat this function until success
			::PostMessage(hWndGlobal,WM_USER_GENERIC,0,0);
			return 0;
		}
		DeleteFile(tempfilepath);
		if (recordaudio) {
			DeleteFile(tempaudiopath);
		}
	}

	//ver 2.26
	if (RecordingMode == ModeAVI) {
		//Launch the player
		if (launchPlayer == 1) {
			CString AppDir = GetProgPath();
			CString exefileName("\\player.exe ");
			CString launchPath = AppDir + exefileName + m_newfile;
			if (WinExec(launchPath,SW_SHOW)!=0) {
			} else {
				//MessageBox("Error launching avi player!","Note",MB_OK | MB_ICONEXCLAMATION);
				MessageOut(m_hWnd,IDS_STRING_ERRPLAYER,IDS_STRING_NOTE,MB_OK | MB_ICONEXCLAMATION);
			}
		} else if (launchPlayer == 2) {
			if (Openlink(m_newfile)) {
			} else {
				//MessageBox("Error launching avi player!","Note",MB_OK | MB_ICONEXCLAMATION);
				MessageOut(m_hWnd,IDS_STRING_ERRDEFAULTPLAYER,IDS_STRING_NOTE,MB_OK | MB_ICONEXCLAMATION);
			}
		} else if (launchPlayer == 3) {
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
	// TODO: Add your command handler code here
	CStatusBar* pStatus = (CStatusBar*) AfxGetApp()->m_pMainWnd->GetDescendantWindow(AFX_IDW_STATUS_BAR);
	pStatus->SetPaneText(0,"Press the Stop Button to stop recording");

	//Version 1.1
	if (recordpaused) {
		recordpaused = 0;
		//ver 1.8
		//if (recordaudio==2)
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

	switch (MouseCaptureMode)
	{
	case 0:
		if (fixedcapture) {
			rc.top = capturetop;
			rc.left = captureleft;
			rc.right = captureleft + capturewidth - 1;
			rc.bottom = capturetop + captureheight - 1;

			if (rc.top < 0)
				rc.top = 0;
			if (rc.left < 0)
				rc.left = 0;
			if (rc.right > maxxScreen - 1)
				rc.right = maxxScreen -1 ;
			if (rc.bottom > maxyScreen - 1)
				rc.bottom = maxyScreen - 1;

			//using protocols for MouseCaptureMode==0
			rcClip = rc;
			old_rcClip = rcClip;
			NormalizeRect(&old_rcClip);
			CopyRect(&rcUse, &old_rcClip);
			::PostMessage (hWndGlobal, WM_USER_RECORDSTART, 0, (LPARAM) 0);
		} else {
			rc.top = 0;
			rc.left = 0;
			rc.right = capturewidth-1;
			rc.bottom = captureheight-1;

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
	if (recordstate == 0) {
		return;
	}

	if (recordpaused) {
		recordpaused = 0;

		//Set Title Bar
		SetTitleBar("CamStudio");
	}

	OnRecordInterrupted (0, 0);
}

void CRecorderView::OnUpdateRegionPanregion(CCmdUI* pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(0 == MouseCaptureMode);
}

void CRecorderView::OnUpdateRegionRubber(CCmdUI* pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(1 == MouseCaptureMode);
}

//ver 1.6
#define MAXCOMPRESSORS 50
void CRecorderView::OnFileVideooptions()
{
	//Capture a frame and use it to determine compatitble compressors for user to select

	LPBITMAPINFOHEADER first_alpbi = NULL;

	COMPVARS compVars;
	compVars.cbSize = sizeof(COMPVARS); // validate it
	compVars.dwFlags = 0;

	int top = 0;
	int left = 0;
	int width = 320;
	int height = 200;

	first_alpbi=captureScreenFrame(left,top,width, height,1);

	num_compressor =0;
	if (compressor_info == NULL) {
		compressor_info = (ICINFO *) calloc(MAXCOMPRESSORS,sizeof(ICINFO));
	} else {
		free(compressor_info);
		compressor_info = (ICINFO *) calloc(MAXCOMPRESSORS,sizeof(ICINFO));
	}

	for(int i=0; ICInfo(ICTYPE_VIDEO, i, &compressor_info[num_compressor]); i++) {
		if (num_compressor>=MAXCOMPRESSORS) {
			break; //maximum allows 30 compressors
		}

		HIC hic;

		if (restrictVideoCodecs) {
			//allow only a few
			if ((compressor_info[num_compressor].fccHandler==mmioFOURCC('m', 's', 'v', 'c'))
				|| (compressor_info[num_compressor].fccHandler==mmioFOURCC('m', 'r', 'l', 'e'))
				|| (compressor_info[num_compressor].fccHandler==mmioFOURCC('c', 'v', 'i', 'd'))
				|| (compressor_info[num_compressor].fccHandler==mmioFOURCC('d', 'i', 'v', 'x')))
			{
				hic = ICOpen(compressor_info[num_compressor].fccType, compressor_info[num_compressor].fccHandler, ICMODE_QUERY);
				if (hic) {
					if (ICERR_OK==ICCompressQuery(hic, first_alpbi, NULL)) {
						ICGetInfo(hic, &compressor_info[num_compressor], sizeof(ICINFO));
						num_compressor ++;
					}
					ICClose(hic);
				}
			}
		} else {
			//CamStudio still cannot handle VIFP
			if (compressor_info[num_compressor].fccHandler != mmioFOURCC('v', 'i', 'f', 'p')) {
				hic = ICOpen(compressor_info[num_compressor].fccType, compressor_info[num_compressor].fccHandler, ICMODE_QUERY);
				if (hic) {
					if (ICERR_OK==ICCompressQuery(hic, first_alpbi, NULL)) {
						ICGetInfo(hic, &compressor_info[num_compressor], sizeof(ICINFO));
						num_compressor ++;
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

void CRecorderView::OnOptionsCursoroptions()
{
	// TODO: Add your command handler code here
	CCursorOptionsDlg cod;
	cod.DoModal();
}

void CRecorderView::OnOptionsAutopan()
{
	// TODO: Add your command handler code here
	autopan = (autopan) ? 0 : 1;
}

void CRecorderView::OnOptionsAtuopanspeed()
{
	// TODO: Add your command handler code here
	CAutopanSpeed aps_dlg;
	aps_dlg.DoModal();
}

void CRecorderView::OnUpdateOptionsAutopan(CCmdUI* pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(autopan);
}

void CRecorderView::OnUpdateRegionFullscreen(CCmdUI* pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(MouseCaptureMode == 2);
}

void CRecorderView::OnOptionsMinimizeonstart()
{
	// TODO: Add your command handler code here
	minimizeOnStart = (minimizeOnStart == 0) ? 1 : 0;
}

void CRecorderView::OnUpdateOptionsMinimizeonstart(CCmdUI* pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(minimizeOnStart);
}

void CRecorderView::OnOptionsHideflashing()
{
	flashingRect = !flashingRect;
}

void CRecorderView::OnUpdateOptionsHideflashing(CCmdUI* pCmdUI)
{
	// TODO: Add your command update UI handler code here

	//ver 1.2
	pCmdUI->SetCheck(!flashingRect);
}

void CRecorderView::OnOptionsProgramoptionsPlayavi()
{
	// TODO: Add your command handler code here
	if (launchPlayer==0)
		launchPlayer = 1;
	else
		launchPlayer = 0;
}

void CRecorderView::OnUpdateOptionsProgramoptionsPlayavi(CCmdUI* pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(launchPlayer);
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
	// TODO: Add your command handler code here
	//Openlink("http://www.atomixbuttons.com/vsc");
	//Openlink("http://www.rendersoftware.com");
	Openlink("http://www.camstudio.org");
}

void CRecorderView::OnHelpHelp()
{
	// TODO: Add your command handler code here

	CString progdir,helppath;
	progdir=GetProgPath();
	helppath= progdir + "\\help.htm";

	Openlink(helppath);

	//HtmlHelp( hWndGlobal, progdir + "\\help.chm", HH_DISPLAY_INDEX, (DWORD)"CamStudio");
}

void CRecorderView::OnPause()
{
	// TODO: Add your command handler code here

	//return if not current recording or already in paused state
	if ((recordstate==0) || (recordpaused==1))
		return;

	recordpaused=1;

	//ver 1.8
	//if (recordaudio==2)
	// mciRecordPause(tempaudiopath);

	CStatusBar* pStatus = (CStatusBar*) AfxGetApp()->m_pMainWnd->GetDescendantWindow(AFX_IDW_STATUS_BAR);
	pStatus->SetPaneText(0,"Recording Paused");

	//Set Title Bar
	SetTitleBar("Paused");
}

void CRecorderView::OnUpdatePause(CCmdUI* pCmdUI)
{
	//Version 1.1
	//pCmdUI->Enable(recordstate && (!recordpaused));
	pCmdUI->Enable(!recordpaused);
}

void CRecorderView::OnUpdateStop(CCmdUI* pCmdUI)
{
	//Version 1.1
	//pCmdUI->Enable(recordstate);
}

void CRecorderView::OnUpdateRecord(CCmdUI* pCmdUI)
{
	//Version 1.1
	pCmdUI->Enable(!recordstate || recordpaused);
}

void CRecorderView::OnHelpFaq()
{
	// TODO: Add your command handler code here
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

	TRACE("WIM_DATA %4d\n", pHdr->dwBytesRecorded);

	if (recordstate) {
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

	recordaudio = (recordaudio) ? 0 : 1;
}

void CRecorderView::OnUpdateOptionsRecordaudio(CCmdUI* pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(recordaudio);
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

	AudioFormat aod;
	aod.DoModal();

	//if (interleaveUnit == MILLISECONDS) {
	//	double interfloat = (((double) interleaveFactor) * ((double) frames_per_second))/1000.0;
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

	AudioSpeakers aos;
	aos.DoModal();
}

void CRecorderView::OnOptionsKeyboardshortcuts()
{
	// TODO: Add your command handler code here
	if (!keySCOpened) {
		keySCOpened = 1;
		Keyshortcuts kscDlg;
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
	fprintf(sFile, "g_highlightclick=%d \n",g_highlightclick);

	fprintf(sFile, "g_highlightcolorR=%d \n",GetRValue(g_highlightcolor));
	fprintf(sFile, "g_highlightcolorG=%d \n",GetGValue(g_highlightcolor));
	fprintf(sFile, "g_highlightcolorB=%d \n",GetBValue(g_highlightcolor));

	fprintf(sFile, "g_highlightclickcolorleftR=%d \n",GetRValue(g_highlightclickcolorleft));
	fprintf(sFile, "g_highlightclickcolorleftG=%d \n",GetGValue(g_highlightclickcolorleft));
	fprintf(sFile, "g_highlightclickcolorleftB=%d \n",GetBValue(g_highlightclickcolorleft));

	fprintf(sFile, "g_highlightclickcolorrightR=%d \n",GetRValue(g_highlightclickcolorright));
	fprintf(sFile, "g_highlightclickcolorrightG=%d \n",GetGValue(g_highlightclickcolorright));
	fprintf(sFile, "g_highlightclickcolorrightB=%d \n",GetBValue(g_highlightclickcolorright));

	//fprintf(sFile, "savedir=%s; \n",LPCTSTR(savedir));
	//fprintf(sFile, "cursordir=%s; \n",LPCTSTR(cursordir));

	fprintf(sFile, "autopan=%d \n",autopan);
	fprintf(sFile, "maxpan= %d \n",maxpan);

	//Audio Functions and Variables
	fprintf(sFile, "AudioDeviceID= %d \n",AudioDeviceID);

	//Audio Options Dialog
	//LPWAVEFORMATEX pwfx = NULL;
	//DWORD cbwfx;
	fprintf(sFile, "cbwfx= %ld \n", cbwfx);
	fprintf(sFile, "recordaudio= %d \n", recordaudio);

	//Audio Formats Dialog
	fprintf(sFile, "waveinselected= %d \n", waveinselected);
	fprintf(sFile, "audio_bits_per_sample= %d \n", audio_bits_per_sample);
	fprintf(sFile, "audio_num_channels= %d \n", audio_num_channels);
	fprintf(sFile, "audio_samples_per_seconds= %d \n", audio_samples_per_seconds);
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

	fprintf(sFile, "keyNextAlt=%d \n",keyNextAlt);
	fprintf(sFile, "keyPrevAlt=%d \n",keyPrevAlt);
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
	//Multilanguage
	fprintf(sFile, "language=%d \n",languageID);

	// Effects
	fprintf(sFile, "timestampAnnotation=%d \n",timestampAnnotation);
	fprintf(sFile, "timestampBackColor=%d \n", timestamp.backgroundColor);
	fprintf(sFile, "timestampSelected=%d \n", timestamp.isFontSelected);
	fprintf(sFile, "timestampPosition=%d \n", timestamp.position);
	fprintf(sFile, "timestampTextColor=%d \n", timestamp.textColor);
	fprintf(sFile, "timestampTextFont=%s \n", timestamp.logfont.lfFaceName);
	fprintf(sFile, "timestampTextWeight=%d \n", timestamp.logfont.lfWeight);
	fprintf(sFile, "timestampTextHeight=%d \n", timestamp.logfont.lfHeight);
	fprintf(sFile, "timestampTextWidth=%d \n", timestamp.logfont.lfWidth);

	fprintf(sFile, "captionAnnotation=%d \n", captionAnnotation);
	fprintf(sFile, "captionBackColor=%d \n", caption.backgroundColor);
	fprintf(sFile, "captionSelected=%d \n", caption.isFontSelected);
	fprintf(sFile, "captionPosition=%d \n", caption.position);
	// fprintf(sFile, "captionText=%s \n", caption.text);
	fprintf(sFile, "captionTextColor=%d \n", caption.textColor);
	fprintf(sFile, "captionTextFont=%s \n", caption.logfont.lfFaceName);
	fprintf(sFile, "captionTextWeight=%d \n", caption.logfont.lfWeight);
	fprintf(sFile, "captionTextHeight=%d \n", caption.logfont.lfHeight);
	fprintf(sFile, "captionTextWidth=%d \n", caption.logfont.lfWidth);

	fprintf(sFile, "watermarkAnnotation=%d \n",watermarkAnnotation);
	fprintf(sFile, "watermarkAnnotation=%d \n",watermark.position);

	fclose(sFile);

	//ver 1.8,
	CString m_newfile = GetProgPath() + "\\CamShapes.ini";
	gList.SaveShapeArray(m_newfile);

	m_newfile = GetProgPath() + "\\CamLayout.ini";
	gList.SaveLayout(m_newfile);

	if (g_cursortype==2) {
		CString cursorFileName = "\\CamCursor.ini";
		CString cursorDir = GetProgPath();
		CString cursorPath = cursorDir + cursorFileName;

		//Note, we do not save the cursorFilePath, but instead we make a copy of the cursor file in the Prog directory
		CopyFile(g_cursorFilePath,cursorPath,FALSE);
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
		fwrite( (void *) LPCTSTR(savedir), savedir.GetLength(), 1, tFile);

	if (cursordir.GetLength()>0)
		fwrite( (void *) LPCTSTR(cursordir), cursordir.GetLength(), 1, tFile);

	if (cbwfx>0)
		fwrite( (void *) pwfx, cbwfx, 1, tFile);

	if (CompressorStateSize > 0)
		fwrite( (void *) pVideoCompressParams, CompressorStateSize, 1, tFile);

	//Ver 1.6
	if (specifieddir.GetLength()>0)
		fwrite( (void *) LPCTSTR(specifieddir), specifieddir.GetLength(), 1, tFile);

	//Ver 1.8
	if (shapeName.GetLength()>0)
		fwrite( (void *) LPCTSTR(shapeName), shapeName.GetLength(), 1, tFile);

	if (g_layoutName.GetLength()>0)
		fwrite( (void *) LPCTSTR(g_layoutName), g_layoutName.GetLength(), 1, tFile);

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
		gList.LoadShapeArray(m_newfile);

		m_newfile = GetProgPath() + "\\CamLayout.ini";
		gList.LoadLayout(m_newfile);

	}

	//The absence of nosave.ini file indicates savesettings = 1
	CString fileName("\\NoSave.ini ");
	CString setDir = GetProgPath();
	CString setPath = setDir+fileName;

	FILE * rFile = fopen((LPCTSTR)setPath,"rt");
	if (rFile == NULL) {
		savesettings = 1;
	} else {
		fclose(rFile);
		savesettings = 0;
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
	//int savelen=0;
	//int cursorlen=0;
	char sdata[1000];
	char tdata[1000];
	float ver=1.0;

	//Ver 1.6
	//int specifiedDirLength=0;
	char specdata[1000];

	fscanf_s(sFile, "[ CamStudio Settings ver%f -- Please do not edit ] \n\n",&ver);

	//Ver 1.2
	if (ver>=1.199999) {
		fscanf_s(sFile, "flashingRect=%d \n", &flashingRect);

		fscanf_s(sFile, "launchPlayer=%d \n", &launchPlayer);
		fscanf_s(sFile, "minimizeOnStart=%d \n", &minimizeOnStart);
		fscanf_s(sFile, "MouseCaptureMode= %d \n", &MouseCaptureMode);
		fscanf_s(sFile, "capturewidth=%d \n", &capturewidth);
		fscanf_s(sFile, "captureheight=%d \n", &captureheight);

		fscanf_s(sFile, "timelapse=%d \n", &timelapse);
		fscanf_s(sFile, "frames_per_second= %d \n", &frames_per_second);
		fscanf_s(sFile, "keyFramesEvery= %d \n", &keyFramesEvery);
		fscanf_s(sFile, "compquality= %d \n", &compquality);
		fscanf_s(sFile, "compfccHandler= %ld \n", &compfccHandler);

		//LPVOID pVideoCompressParams = NULL;
		fscanf_s(sFile, "CompressorStateIsFor= %ld \n",&CompressorStateIsFor);
		fscanf_s(sFile, "CompressorStateSize= %d \n",&CompressorStateSize);

		fscanf_s(sFile, "g_recordcursor=%d \n",&g_recordcursor);
		fscanf_s(sFile, "g_customsel=%d \n",&g_customsel); //Having this line means the custom cursor type cannot be re-arranged in a new order in the combo box...else previous saved settings referring to the custom type will not be correct
		fscanf_s(sFile, "g_cursortype=%d \n",&g_cursortype);
		fscanf_s(sFile, "g_highlightcursor=%d \n",&g_highlightcursor);
		fscanf_s(sFile, "g_highlightsize=%d \n",&g_highlightsize);
		fscanf_s(sFile, "g_highlightshape=%d \n",&g_highlightshape);
		fscanf_s(sFile, "g_highlightclick=%d \n",&g_highlightclick);

		int redv = 0;
		int greenv = 0;
		int bluev = 0;
		fscanf_s(sFile, "g_highlightcolorR=%d \n",&idata);
		redv=idata;
		fscanf_s(sFile, "g_highlightcolorG=%d \n",&idata);
		greenv=idata;
		fscanf_s(sFile, "g_highlightcolorB=%d \n",&idata);
		bluev=idata;
		g_highlightcolor = RGB(redv,greenv,bluev);

		redv=0;greenv=0;bluev=0;
		fscanf_s(sFile, "g_highlightclickcolorleftR=%d \n",&idata);
		redv=idata;
		fscanf_s(sFile, "g_highlightclickcolorleftG=%d \n",&idata);
		greenv=idata;
		fscanf_s(sFile, "g_highlightclickcolorleftB=%d \n",&idata);
		bluev=idata;
		g_highlightclickcolorleft = RGB(redv,greenv,bluev);

		redv=0;greenv=0;bluev=0;
		fscanf_s(sFile, "g_highlightclickcolorrightR=%d \n",&idata);
		redv=idata;
		fscanf_s(sFile, "g_highlightclickcolorrightG=%d \n",&idata);
		greenv=idata;
		fscanf_s(sFile, "g_highlightclickcolorrightB=%d \n",&idata);
		bluev=idata;
		g_highlightclickcolorright = RGB(redv,greenv,bluev);

		fscanf_s(sFile, "autopan=%d \n",&autopan);
		fscanf_s(sFile, "maxpan= %d \n",&maxpan);

		//Audio Functions and Variables
		fscanf_s(sFile, "AudioDeviceID= %d \n",&AudioDeviceID);

		//Audio Options Dialog
		fscanf_s(sFile, "cbwfx= %ld \n", &cbwfx);
		fscanf_s(sFile, "recordaudio= %d \n", &recordaudio);

		//Audio Formats Dialog
		fscanf_s(sFile, "waveinselected= %d \n", &waveinselected);
		fscanf_s(sFile, "audio_bits_per_sample= %d \n", &audio_bits_per_sample);
		fscanf_s(sFile, "audio_num_channels= %d \n", &audio_num_channels);
		fscanf_s(sFile, "audio_samples_per_seconds= %d \n", &audio_samples_per_seconds);
		fscanf_s(sFile, "bAudioCompression= %d \n", &bAudioCompression);

		fscanf_s(sFile, "interleaveFrames= %d \n", &interleaveFrames);
		fscanf_s(sFile, "interleaveFactor= %d \n", &interleaveFactor);

		//Key Shortcuts
		fscanf_s(sFile, "keyRecordStart= %d \n",&keyRecordStart);
		fscanf_s(sFile, "keyRecordEnd= %d \n",&keyRecordEnd);
		fscanf_s(sFile, "keyRecordCancel= %d \n",&keyRecordCancel);

		fscanf_s(sFile, "viewtype= %d \n",&viewtype);

		fscanf_s(sFile, "g_autoadjust= %d \n",&g_autoadjust);
		fscanf_s(sFile, "g_valueadjust= %d \n",&g_valueadjust);

		fscanf_s(sFile, "savedir=%d \n",&savelen);
		fscanf_s(sFile, "cursordir=%d \n",&cursorlen);

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

		if (g_cursortype == 1) {
			DWORD customicon;
			if (g_customsel<0) {
				g_customsel = 0;
			}
			customicon = icon_info[g_customsel];

			g_customcursor = LoadIcon(AfxGetInstanceHandle(),MAKEINTRESOURCE(customicon));
			if (g_customcursor==NULL) {
				g_cursortype = 0;
			}
		} else if (g_cursortype == 2) { //load cursor
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
	if (ver>=1.299999) {
		fscanf_s(sFile, "threadPriority=%d \n",&threadPriority);

		//CString tracex;
		//tracex.Format("Thread %d ver %f",threadPriority, ver);
		//MessageBox(tracex,"Note",MB_OK);
	}

	//Ver 1.5
	if (ver>=1.499999) {
		fscanf_s(sFile, "captureleft= %d \n",&captureleft);
		fscanf_s(sFile, "capturetop= %d \n",&capturetop);
		fscanf_s(sFile, "fixedcapture=%d \n",&fixedcapture);
		fscanf_s(sFile, "interleaveUnit= %d \n", &interleaveUnit);
	} else {
		//force interleve settings
		interleaveUnit= MILLISECONDS;
		interleaveFactor = 100;
	}

	//Ver 1.6
	if (ver>=1.599999) {
		fscanf_s(sFile, "tempPath_Access=%d \n",&tempPath_Access);
		fscanf_s(sFile, "captureTrans=%d \n",&captureTrans);
		fscanf_s(sFile, "specifieddir=%d \n",&specifiedDirLength);
		fscanf_s(sFile, "NumDev=%d \n",&NumberOfMixerDevices);
		fscanf_s(sFile, "SelectedDev=%d \n",&SelectedMixer);
		fscanf_s(sFile, "feedback_line=%d \n",&feedback_line);
		fscanf_s(sFile, "feedback_line_info=%d \n",&feedback_lineInfo);
		fscanf_s(sFile, "performAutoSearch=%d \n",&performAutoSearch);

		onLoadSettings(recordaudio);
	} else {
		tempPath_Access = USE_WINDOWS_TEMP_DIR;
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
		tempPath_Access = USE_WINDOWS_TEMP_DIR;
		specifieddir = GetTempPath(tempPath_Access, specifieddir);
		tempPath_Access = old_tempPath_Access;

		//Do not modify the specifiedDirLength variable, even if specifieddir is changed. It will need to be used below
	}

	//Update Player to ver 2.0
	//Make the the modified keys do not overlap
	if (ver<1.799999) {
		if (launchPlayer == 1)
			launchPlayer = 3;

		if ((keyRecordStart == VK_MENU) || (keyRecordStart == VK_SHIFT) || (keyRecordStart == VK_CONTROL) || (keyRecordStart == VK_ESCAPE)) {
			keyRecordStart = VK_F8;
			keyRecordStartCtrl = 1;
		}

		if ((keyRecordEnd == VK_MENU) || (keyRecordEnd == VK_SHIFT) || (keyRecordEnd == VK_CONTROL) || (keyRecordEnd == VK_ESCAPE)) {
			keyRecordEnd = VK_F9;
			keyRecordEndCtrl = 1;
		}

		if ((keyRecordCancel == VK_MENU) || (keyRecordCancel == VK_SHIFT) || (keyRecordCancel == VK_CONTROL) || (keyRecordCancel == VK_ESCAPE)) {
			keyRecordCancel = VK_F10;
			keyRecordCancelCtrl = 1;
		}
	}

	//Ver 1.8
	int shapeNameLen=0;
	int layoutNameLen=0;
	if (ver>=1.799999) {
		fscanf_s(sFile, "supportMouseDrag=%d \n",&supportMouseDrag);

		fscanf_s(sFile, "keyRecordStartCtrl=%d \n",&keyRecordStartCtrl);
		fscanf_s(sFile, "keyRecordEndCtrl=%d \n",&keyRecordEndCtrl);
		fscanf_s(sFile, "keyRecordCancelCtrl=%d \n",&keyRecordCancelCtrl);

		fscanf_s(sFile, "keyRecordStartAlt=%d \n",&keyRecordStartAlt);
		fscanf_s(sFile, "keyRecordEndAlt=%d \n",&keyRecordEndAlt);
		fscanf_s(sFile, "keyRecordCancelAlt=%d \n",&keyRecordCancelAlt);

		fscanf_s(sFile, "keyRecordStartShift=%d \n",&keyRecordStartShift);
		fscanf_s(sFile, "keyRecordEndShift=%d \n",&keyRecordEndShift);
		fscanf_s(sFile, "keyRecordCancelShift=%d \n",&keyRecordCancelShift);

		fscanf_s(sFile, "keyNext=%d \n",&keyNext);
		fscanf_s(sFile, "keyPrev=%d \n",&keyPrev);
		fscanf_s(sFile, "keyShowLayout=%d \n",&keyShowLayout);

		fscanf_s(sFile, "keyNextCtrl=%d \n",&keyNextCtrl);
		fscanf_s(sFile, "keyPrevCtrl=%d \n",&keyPrevCtrl);
		fscanf_s(sFile, "keyShowLayoutCtrl=%d \n",&keyShowLayoutCtrl);

		fscanf_s(sFile, "keyNextAlt=%d \n",&keyNextAlt);
		fscanf_s(sFile, "keyPrevAlt=%d \n",&keyPrevAlt);
		fscanf_s(sFile, "keyShowLayoutAlt=%d \n",&keyShowLayoutAlt);

		fscanf_s(sFile, "keyNextShift=%d \n",&keyNextShift);
		fscanf_s(sFile, "keyPrevShift=%d \n",&keyPrevShift);
		fscanf_s(sFile, "keyShowLayoutShift=%d \n",&keyShowLayoutShift);

		fscanf_s(sFile, "shapeNameInt=%d \n",&shapeNameInt);
		fscanf_s(sFile, "shapeNameLen=%d \n",&shapeNameLen);

		fscanf_s(sFile, "layoutNameInt=%d \n",&layoutNameInt);
		fscanf_s(sFile, "g_layoutNameLen=%d \n",&layoutNameLen);

		fscanf_s(sFile, "useMCI=%d \n",&useMCI);
		fscanf_s(sFile, "shiftType=%d \n",&shiftType);
		fscanf_s(sFile, "timeshift=%d \n",&timeshift);
		fscanf_s(sFile, "frameshift=%d \n",&frameshift);
	}

	//ver 2.26
	//save format is set as 2.0
	if (ver>=1.999999) {
		fscanf_s(sFile, "launchPropPrompt=%d \n",&launchPropPrompt);
		fscanf_s(sFile, "launchHTMLPlayer=%d \n",&launchHTMLPlayer);
		fscanf_s(sFile, "deleteAVIAfterUse=%d \n",&deleteAVIAfterUse);
		fscanf_s(sFile, "RecordingMode=%d \n",&RecordingMode);
		fscanf_s(sFile, "autonaming=%d \n",&autonaming);
		fscanf_s(sFile, "restrictVideoCodecs=%d \n",&restrictVideoCodecs);
		//fscanf_s(sFile, "base_nid=%d \n",&base_nid);
	}

	//ver 2.40
	if (ver>=2.399999) {
		fscanf_s(sFile, "presettime=%d \n",&presettime);
		fscanf_s(sFile, "recordpreset=%d \n",&recordpreset);
	}

	//new variables add here
	//Multilanguage
	fscanf_s(sFile, "language=%d \n",&languageID);

	// Effects
	fscanf_s(sFile, "timestampAnnotation=%d \n",&timestampAnnotation);
	fscanf_s(sFile, "timestampBackColor=%d \n", &timestamp.backgroundColor);
	fscanf_s(sFile, "timestampSelected=%d \n", &timestamp.isFontSelected);
	fscanf_s(sFile, "timestampPosition=%d \n", &timestamp.position);
	fscanf_s(sFile, "timestampTextColor=%d \n", &timestamp.textColor);
	fscanf(sFile, "timestampTextFont=%s \n", timestamp.logfont.lfFaceName);
	fscanf_s(sFile, "timestampTextWeight=%d \n", &timestamp.logfont.lfWeight);
	fscanf_s(sFile, "timestampTextHeight=%d \n", &timestamp.logfont.lfHeight);
	fscanf_s(sFile, "timestampTextWidth=%d \n", &timestamp.logfont.lfWidth);

	fscanf_s(sFile, "captionAnnotation=%d \n", &captionAnnotation);
	fscanf_s(sFile, "captionBackColor=%d \n", &caption.backgroundColor);
	fscanf_s(sFile, "captionSelected=%d \n", &caption.isFontSelected);
	fscanf_s(sFile, "captionPosition=%d \n", &caption.position);
	// fscanf_s(sFile, "captionText=%s \n", &caption.text);
	fscanf_s(sFile, "captionTextColor=%d \n", &caption.textColor);
	fscanf(sFile, "captionTextFont=%s \n", caption.logfont.lfFaceName);
	fscanf_s(sFile, "captionTextWeight=%d \n", &caption.logfont.lfWeight);
	fscanf_s(sFile, "captionTextHeight=%d \n", &caption.logfont.lfHeight);
	fscanf_s(sFile, "captionTextWidth=%d \n", &caption.logfont.lfWidth);

	fscanf_s(sFile, "watermarkAnnotation=%d \n",&watermarkAnnotation);
	fscanf_s(sFile, "watermarkAnnotation=%d \n",&watermark.position);

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
		cbwfx = 0;
		SuggestCompressFormat();
		return;
	}

	if (ver>=1.2) {
		// ****************************
		// Load Binary Data
		// ****************************
		if ((savelen>0) && (savelen<1000)) {
			fread( (void *) sdata, savelen, 1, tFile);
			sdata[savelen]=0;
			savedir=CString(sdata);
		}

		if ((cursorlen>0) && (cursorlen<1000)) {
			fread( (void *) tdata, cursorlen, 1, tFile);
			tdata[cursorlen]=0;
			cursordir=CString(tdata);
		}

		if (ver > 1.35) { //if perfoming an upgrade from previous settings, do not load these additional camdata.ini information
			if (cbwfx>0) {
				AllocCompressFormat();
				int countread = fread( (void *) pwfx, cbwfx, 1, tFile);
				if (countread!=1) {
					cbwfx=0;
					if (pwfx) {
						GlobalFreePtr(pwfx);
						pwfx = NULL;
						SuggestCompressFormat();
					}
				} else {
					AttemptCompressFormat();
				}
			}

			if (CompressorStateSize>0) {
				AllocVideoCompressParams(CompressorStateSize);
				fread( (void *) pVideoCompressParams, CompressorStateSize, 1, tFile);
			}

			//ver 1.6
			if (ver>1.55) {
				//if upgrade from older file versions, specifiedDirLength == 0 and the following code will not run
				if ((specifiedDirLength>0) && (specifiedDirLength<1000)) {
					fread( (void *) specdata, specifiedDirLength, 1, tFile);
					specdata[specifiedDirLength]=0;
					specifieddir=CString(specdata);
				}

				//ver 1.8
				if (ver>=1.799999) {
					char namedata[1000];

					if ((shapeNameLen>0) && (shapeNameLen<1000)) {
						fread( (void *) namedata, shapeNameLen, 1, tFile);
						namedata[shapeNameLen]=0;
						shapeName=CString(namedata);
					}

					if ((layoutNameLen>0) && (layoutNameLen<1000)) {
						fread( (void *) namedata, layoutNameLen, 1, tFile);
						namedata[layoutNameLen]=0;
						g_layoutName=CString(namedata);
					}
				}

			}// if ver >=1.55

		}// if ver >=1.35

	} //if ver>=1.2

	fclose(tFile);
}

void CRecorderView::OnOptionsProgramoptionsSavesettingsonexit()
{
	// TODO: Add your command handler code here
	savesettings = (savesettings) ? 0 : 1;
}

void CRecorderView::OnUpdateOptionsProgramoptionsSavesettingsonexit(CCmdUI* pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(savesettings);
}

void CRecorderView::DecideSaveSettings()
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

void CRecorderView::OnOptionsRecordingthreadpriorityNormal()
{
	// TODO: Add your command handler code here
	threadPriority = THREAD_PRIORITY_NORMAL;
}

void CRecorderView::OnOptionsRecordingthreadpriorityHighest()
{
	// TODO: Add your command handler code here
	threadPriority = THREAD_PRIORITY_HIGHEST;
}

void CRecorderView::OnOptionsRecordingthreadpriorityAbovenormal()
{
	// TODO: Add your command handler code here
	threadPriority = THREAD_PRIORITY_ABOVE_NORMAL;
}

void CRecorderView::OnOptionsRecordingthreadpriorityTimecritical()
{
	// TODO: Add your command handler code here
	threadPriority = THREAD_PRIORITY_TIME_CRITICAL;
}

void CRecorderView::OnUpdateOptionsRecordingthreadpriorityNormal(CCmdUI* pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(threadPriority == THREAD_PRIORITY_NORMAL);
}

void CRecorderView::OnUpdateOptionsRecordingthreadpriorityHighest(CCmdUI* pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(threadPriority == THREAD_PRIORITY_HIGHEST);
}

void CRecorderView::OnUpdateOptionsRecordingthreadpriorityAbovenormal(CCmdUI* pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(threadPriority == THREAD_PRIORITY_ABOVE_NORMAL);
}

void CRecorderView::OnUpdateOptionsRecordingthreadpriorityTimecritical(CCmdUI* pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(threadPriority == THREAD_PRIORITY_TIME_CRITICAL);
}

void CRecorderView::OnOptionsCapturetrans()
{
	// TODO: Add your command handler code here

	if (captureTrans)
		captureTrans = 0;
	else
		captureTrans = 1;
}

void CRecorderView::OnUpdateOptionsCapturetrans(CCmdUI* pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(captureTrans==1);
}

void CRecorderView::OnOptionsTempdirWindows()
{
	// TODO: Add your command handler code here
	tempPath_Access = USE_WINDOWS_TEMP_DIR;
}

void CRecorderView::OnUpdateOptionsTempdirWindows(CCmdUI* pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(tempPath_Access == USE_WINDOWS_TEMP_DIR);
}

void CRecorderView::OnOptionsTempdirInstalled()
{
	// TODO: Add your command handler code here
	tempPath_Access = USE_INSTALLED_DIR;
}

void CRecorderView::OnUpdateOptionsTempdirInstalled(CCmdUI* pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(tempPath_Access == USE_INSTALLED_DIR);
}

void CRecorderView::OnOptionsTempdirUser()
{
	// TODO: Add your command handler code here
	tempPath_Access = USE_USER_SPECIFIED_DIR;

	CFolderDialog cfg(specifieddir);
	int retval;

	retval=cfg.DoModal();

	if (retval==IDOK) {
		specifieddir = cfg.GetPathName();

	}
}

void CRecorderView::OnUpdateOptionsTempdirUser(CCmdUI* pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(tempPath_Access == USE_USER_SPECIFIED_DIR);
}

void CRecorderView::OnOptionsRecordaudioDonotrecordaudio()
{
	// TODO: Add your command handler code here
	recordaudio = 0;
}

void CRecorderView::OnUpdateOptionsRecordaudioDonotrecordaudio(CCmdUI* pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(recordaudio==0);
}

void CRecorderView::OnOptionsRecordaudioRecordfromspeakers()
{
	// TODO: Add your command handler code here
	if (waveOutGetNumDevs() == 0) {
		//CString msgstr;
		//msgstr.Format("Unable to detect audio output device. You need a sound card with speakers attached.");
		//MessageBox(msgstr,"Note", MB_OK | MB_ICONEXCLAMATION);
		MessageOut(m_hWnd,IDS_STRING_NOAUDIOOUTPUT,IDS_STRING_NOTE,MB_OK | MB_ICONEXCLAMATION);
		return;
	}

	recordaudio=2;

	useWaveout(FALSE,FALSE);
}

void CRecorderView::OnUpdateOptionsRecordaudioRecordfromspeakers(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(recordaudio==2);
}

void CRecorderView::OnOptionsRecordaudiomicrophone()
{
	if (waveInGetNumDevs() == 0) {
		MessageOut(m_hWnd,IDS_STRING_NOINPUT1,IDS_STRING_NOTE,MB_OK | MB_ICONEXCLAMATION);

		return;

	}

	recordaudio=1;

	useWavein(TRUE,FALSE); //TRUE ==> silence mode, will not report errors
}

void CRecorderView::OnUpdateOptionsRecordaudiomicrophone(CCmdUI* pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(recordaudio==1);
}

void CRecorderView::OnOptionsProgramoptionsTroubleshoot()
{
	TroubleShoot tbsDlg;
	tbsDlg.DoModal();

	if ((TroubleShootVal == 1)) {
		::PostMessage(AfxGetMainWnd()->GetSafeHwnd(),WM_CLOSE,0,0);
	}
}

void CRecorderView::OnOptionsProgramoptionsCamstudioplay()
{
	// TODO: Add your command handler code here
	launchPlayer = 1;
}

void CRecorderView::OnUpdateOptionsProgramoptionsCamstudioplay(CCmdUI* pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(launchPlayer == 1);
}

void CRecorderView::OnOptionsProgramoptionsDefaultplay()
{
	// TODO: Add your command handler code here
	launchPlayer = 2;
}

void CRecorderView::OnUpdateOptionsProgramoptionsDefaultplay(CCmdUI* pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(launchPlayer == 2);
}

void CRecorderView::OnOptionsProgramoptionsNoplay()
{
	// TODO: Add your command handler code here
	launchPlayer = 0;
}

void CRecorderView::OnUpdateOptionsProgramoptionsNoplay(CCmdUI* pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(launchPlayer == 0);
}

void CRecorderView::OnHelpDonations()
{
	// TODO: Add your command handler code here
	CString progdir,donatepath;
	progdir=GetProgPath();
	donatepath= progdir + "\\help.htm#Donations";

	Openlink(donatepath);
}

void CRecorderView::OnOptionsUsePlayer20()
{
	// TODO: Add your command handler code here
	launchPlayer = 3;
}

void CRecorderView::OnUpdateUsePlayer20(CCmdUI* pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(launchPlayer == 3);
}

void CRecorderView::OnViewScreenannotations()
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

void CRecorderView::OnUpdateViewScreenannotations(CCmdUI* pCmdUI)
{
}

void CRecorderView::OnViewVideoannotations()
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
		if (recordstate == 0) {
			if (AllowNewRecordStartKey) {
				//prevent the case which CamStudio presents more than one region for the user to select
				AllowNewRecordStartKey = FALSE;
				OnRecord();
			}
		} else if (recordstate == 1) {
			// pause if currently recording
			if (recordpaused == 0) {
				OnPause();
			} else {
				OnRecord();
			}
		}
		break;
	case 1:
		if (recordstate == 1) {
			if (keyRecordEnd != keyRecordCancel) {
				OnRecordInterrupted(keyRecordEnd, 0);
			} else {
				OnRecordInterrupted(keyRecordCancel + 1, 0);
			}
		}
		break;
	case 2:
		if (recordstate == 1) {
			OnRecordInterrupted(keyRecordCancel, 0);
		}
		break;
	case 3:
		{
			if (!sadlgCreated) {
				sadlg.Create(IDD_SCREENANNOTATIONS2,NULL);
				sadlgCreated = 1;
			}
			int max = gList.layoutArray.GetSize();
			if (max<=0)
				return 0;

			//Get Current selected
			int cursel = sadlg.GetLayoutListSelection();
			if (cursel == -1)
				currentLayout = 0;
			else
				currentLayout = cursel + 1;

			if (currentLayout>=max)
				currentLayout=0;

			sadlg.InstantiateLayout(currentLayout,1);
		}
		break;
	case 4:
		{
			if (!sadlgCreated) {
				sadlg.Create(IDD_SCREENANNOTATIONS2,NULL);
				//sadlg.RefreshLayoutList();
				sadlgCreated = 1;
			}
			int max = gList.layoutArray.GetSize();
			if (max<=0) return 0;

			//Get Current selected
			int cursel = sadlg.GetLayoutListSelection();
			if (cursel == -1) {
				currentLayout = 0;
			} else {
				currentLayout = cursel - 1;
			}

			if (currentLayout<0)
				currentLayout=max-1;

			sadlg.InstantiateLayout(currentLayout,1);
		}
		break;
	case 5:
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
			if (max<=0)
				return 0;

			//Get Current selected
			int cursel = sadlg.GetLayoutListSelection();
			if (cursel == -1) {
				currentLayout = 0;
			} else {
				currentLayout = cursel;
			}

			if ((currentLayout<0) || (currentLayout>=max)) {
				currentLayout=0;
			}

			sadlg.InstantiateLayout(currentLayout,1);
		}
	}

	return 1;
}

void CRecorderView::OnOptionsSynchronization()
{
	// TODO: Add your command handler code here
	if ((waveInGetNumDevs() == 0) || (waveOutGetNumDevs() == 0)) {
		MessageOut(m_hWnd,IDS_STRING_NOINPUT3,IDS_STRING_NOTE,MB_OK | MB_ICONEXCLAMATION);
		return;
	}

	CSyncDialog synDlg;
	synDlg.DoModal();
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
	// TODO: Add your command handler code here
	launchHTMLPlayer = (launchHTMLPlayer) ? 0 : 1;
}

void CRecorderView::OnOptionsSwfDeleteavifile()
{
	deleteAVIAfterUse = (deleteAVIAfterUse) ? 0 : 1;
}

void CRecorderView::OnOptionsSwfDisplayparameters()
{
	launchPropPrompt = (launchPropPrompt) ? 0 : 1;
}

void CRecorderView::OnUpdateOptionsSwfLaunchhtml(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(launchHTMLPlayer);
}

void CRecorderView::OnUpdateOptionsSwfDisplayparameters(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(launchPropPrompt);
}

void CRecorderView::OnUpdateOptionsSwfDeleteavifile(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(deleteAVIAfterUse);
}

void CRecorderView::OnAviswf()
{
	// TODO: Add your command handler code here
	RecordingMode = (RecordingMode == 0) ? 1 : 0;
	Invalidate();
}

BOOL CRecorderView::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default
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
	// TODO: Add your command handler code here
	autonaming = 1;
}

void CRecorderView::OnUpdateOptionsNamingAutodate(CCmdUI* pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(autonaming == 1);
}

void CRecorderView::OnOptionsNamingAsk()
{
	// TODO: Add your command handler code here
	autonaming = 0;
}

void CRecorderView::OnUpdateOptionsNamingAsk(CCmdUI* pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(autonaming == 0);
}

void CRecorderView::OnOptionsProgramoptionsPresettime()
{
	// TODO: Add your command handler code here
	CPresetTime prestDlg;
	prestDlg.DoModal();
}

//Multilanguage

void CRecorderView::OnUpdateOptionsLanguageEnglish(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(languageID==9);
}

void CRecorderView::OnUpdateOptionsLanguageGerman(CCmdUI* pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(languageID==7);
}

void CRecorderView::OnOptionsLanguageEnglish()
{
	// TODO: Add your command handler code here
	languageID=9;
	AfxGetApp()->WriteProfileInt( SEC_SETTINGS, ENT_LANGID, 9);
	AfxMessageBox( IDS_RESTARTAPP);
}

void CRecorderView::OnOptionsLanguageGerman()
{
	// TODO: Add your command handler code here

	languageID=7;
	AfxGetApp()->WriteProfileInt( SEC_SETTINGS, ENT_LANGID, 7);
	AfxMessageBox( IDS_RESTARTAPP);
}

void CRecorderView::OnRegionWindow()
{
	// TODO: Add your command handler code here
	MouseCaptureMode = 3;
}

void CRecorderView::OnUpdateRegionWindow(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck((MouseCaptureMode==3) ? TRUE : FALSE);
}

void CRecorderView::OnCaptureChanged(CWnd *pWnd)
{
	// TODO: Add your message handler code here
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
	// TODO: Add your command handler code here
	timestampAnnotation = !timestampAnnotation;
}

void CRecorderView::OnUpdateAnnotationAddsystemtimestamp(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(timestampAnnotation);
}

void CRecorderView::OnAnnotationAddcaption()
{
	// TODO: Add your command handler code here
	captionAnnotation = !captionAnnotation;
}

void CRecorderView::OnUpdateAnnotationAddcaption(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(captionAnnotation);
}

void CRecorderView::OnAnnotationAddwatermark()
{
	// TODO: Add your command handler code here
	watermarkAnnotation = !watermarkAnnotation;
}

void CRecorderView::OnUpdateAnnotationAddwatermark(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(watermarkAnnotation);
}

void CRecorderView::OnEffectsOptions()
{
	// TODO: Add your command handler code here
	CAnnotationEffectsOptions dlg;
	dlg.m_timestamp = timestamp;
	dlg.m_caption = caption;
	dlg.m_image = watermark;
	if (dlg.DoModal() == IDOK){
		timestamp = dlg.m_timestamp;
		caption = dlg.m_caption;
		watermark = dlg.m_image;
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

	csMsg.Format("Number of Colors : %d bits",  nColors);
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

	csMsg.Format("Dimension : %d X %d",  actualwidth, actualheight);
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
	msgRecMode.LoadString((RecordingMode == ModeAVI) ? IDS_RECAVI : IDS_RECSWF);

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
	TRACE("CRecorderView::captureScreenFrame\n");
	HDC hScreenDC = ::GetDC(NULL);

	//if flashing rect
	if (flashingRect && !tempDisableRect) {
		if (autopan) {
			pFlashingWnd->SetUpRegion(left, top, width, height, 1);
		}
		pFlashingWnd->DrawFlashingRect(TRUE, (autopan) ? 1 : 0);
	}

	HDC hMemDC = ::CreateCompatibleDC(hScreenDC);
	HBITMAP hbm = ::CreateCompatibleBitmap(hScreenDC, width, height);
	HBITMAP oldbm = (HBITMAP) ::SelectObject(hMemDC, hbm);

	//ver 1.6
	DWORD bltFlags = SRCCOPY;
	if (captureTrans && (4 < versionOp))
		bltFlags |= CAPTUREBLT;
	BitBlt(hMemDC, 0, 0, width, height, hScreenDC, left, top, bltFlags);

	RECT rect;
	rect.left = 0;
	rect.top = 0;
	rect.right = rect.left + width;
	rect.bottom = rect.top + height;
	if (timestampAnnotation){
		SYSTEMTIME systime;
		::GetLocalTime(&systime);
		timestamp.text.Format("%s %02d:%02d:%02d:%03d", "Recording", systime.wHour, systime.wMinute, systime.wSecond, systime.wMilliseconds);
		//InsertText(hMemDC, 0, 0, msg);
		InsertText(hMemDC, rect, timestamp);
		//InsertText(hMemDC, 0, 0, "Recorded by You!!!");
	}
	if (captionAnnotation){
		InsertText(hMemDC, rect, caption);
	}
	if (watermarkAnnotation){
		InsertImage(hMemDC, rect, watermark);
	}

	//Get Cursor Pos
	POINT xPoint;
	GetCursorPos( &xPoint);
	HCURSOR hcur= FetchCursorHandle();
	xPoint.x-=left;
	xPoint.y-=top;

	//Draw the HighLight
	if (g_highlightcursor==1) {
		POINT highlightPoint;
		highlightPoint.x = xPoint.x -64;
		highlightPoint.y = xPoint.y -64;
		InsertHighLight( hMemDC, highlightPoint.x, highlightPoint.y);
	}

	//Draw the Cursor
	if (g_recordcursor==1) {
		ICONINFO iconinfo;
		BOOL ret;
		ret = GetIconInfo( hcur, &iconinfo);
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

		::DrawIcon( hMemDC, xPoint.x, xPoint.y, hcur);
	}

	SelectObject(hMemDC, oldbm);
	LPBITMAPINFOHEADER pBM_HEADER = (LPBITMAPINFOHEADER)GlobalLock(Bitmap2Dib(hbm, bits));
	if (pBM_HEADER == NULL) {
		//MessageBox(NULL,"Error reading a frame!","Error",MB_OK | MB_ICONEXCLAMATION);
		MessageOut(NULL,IDS_STRING_ERRFRAME,IDS_STRING_NOTE,MB_OK | MB_ICONEXCLAMATION);

		exit(1);	// todo: WHAT??? Exit???
	}

	DeleteObject(hbm);
	DeleteDC(hMemDC);

	//if flashing rect
	if (flashingRect && !tempDisableRect) {
		pFlashingWnd->DrawFlashingRect(FALSE, (autopan) ? 1 : 0);
	}

	::ReleaseDC(NULL,hScreenDC);

	return pBM_HEADER;
}

void CRecorderView::FreeFrame(LPBITMAPINFOHEADER alpbi)
{
	if (!alpbi)
		return;

	GlobalFreePtr(alpbi);
	alpbi = 0;
}

UINT CRecorderView::RecordAVIThread(LPVOID pParam)
{
	TRACE("CRecorderView::RecordAVIThread\n");
	//Test the validity of writing to the file
	//Make sure the file to be created is currently not used by another application
	CString csTempFolder(GetTempPath(tempPath_Access, specifieddir));
	tempfilepath.Format("%s\\temp.avi", (LPCSTR)csTempFolder);

	srand( (unsigned)time( NULL));
	bool fileverified = false;
	while (!fileverified)
	{
		OFSTRUCT ofstruct;
		HFILE hFile = OpenFile(tempfilepath, &ofstruct, OF_SHARE_EXCLUSIVE | OF_WRITE | OF_CREATE);
		fileverified = (hFile != HFILE_ERROR);
		if (fileverified) {
			CloseHandle((HANDLE)hFile);
			DeleteFile(tempfilepath);
		} else {
			tempfilepath.Format("%s\\temp%d.avi", (LPCSTR)csTempFolder, rand());
		}
	}

	int top = rcUse.top;
	int left = rcUse.left;
	int width = rcUse.right - rcUse.left+1;
	int height = rcUse.bottom - rcUse.top + 1;
	int fps = frames_per_second;

	RecordVideo(top, left, width, height, fps, tempfilepath);

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

	actualwidth = width;
	actualheight = height;

	////////////////////////////////////////////////
	// CAPTURE FIRST FRAME
	////////////////////////////////////////////////
	LPBITMAPINFOHEADER alpbi = captureScreenFrame(left, top, width, height, 1);

	////////////////////////////////////////////////
	// TEST VALIDITY OF COMPRESSOR
	//////////////////////////////////////////////////

	if (selected_compressor > 0) {
		HIC hic = ICOpen(compressor_info[selected_compressor].fccType, compressor_info[selected_compressor].fccHandler, ICMODE_QUERY);
		if (hic) {
			int newleft;
			int newtop;
			int newwidth;
			int newheight;
			int align = 1;
			while (ICERR_OK!=ICCompressQuery(hic, alpbi, NULL)) {
				//Try adjusting width/height a little bit
				align = align * 2 ;
				if (align>8)
					break;

				newleft=left;
				newtop=top;
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
				alpbi=captureScreenFrame(newleft, newtop, newwidth, newheight, 1);
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

				actualwidth = newwidth;
				actualheight = newheight;
			} else {
				compfccHandler = mmioFOURCC('M', 'S', 'V', 'C');
				strCodec = CString("MS Video 1");
			}

			ICClose(hic);
		} else {
			compfccHandler = mmioFOURCC('M', 'S', 'V', 'C');
			strCodec = CString("MS Video 1");
			//MessageBox(NULL, "hic default", "note", MB_OK);
		}
	} //selected_compressor

	//Special Cases
	//DIVX
	//if (compfccHandler == mmioFOURCC('D', 'I', 'V', 'X')) { //Still Can't Handle DIVX
	//	compfccHandler = mmioFOURCC('M', 'S', 'V', 'C');
	//	strCodec = CString("MS Video 1");
	//}

	//IV50
	if (compfccHandler == mmioFOURCC('I', 'V', '5', '0')) { //Still Can't Handle Indeo 5.04
		compfccHandler = mmioFOURCC('M', 'S', 'V', 'C');
		strCodec = CString("MS Video 1");
	}

	////////////////////////////////////////////////
	// Set Up Flashing Rect
	////////////////////////////////////////////////
	if (flashingRect) {
		if (autopan) {
			pFlashingWnd->SetUpRegion(left, top, width, height, 1);
		} else {
			pFlashingWnd->SetUpRegion(left, top, width, height, 0);
		}
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

	//strhdr.fccHandler             = compfccHandler;
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
	aopts[0]->fccHandler		= compfccHandler;
	aopts[0]->dwKeyFrameEvery	= keyFramesEvery;		// keyframe rate
	aopts[0]->dwQuality			= compquality;        // compress quality 0-10, 000
	aopts[0]->dwBytesPerSecond	= 0;		// bytes per second
	aopts[0]->dwFlags			= AVICOMPRESSF_VALID | AVICOMPRESSF_KEYFRAMES;    // flags
	aopts[0]->lpFormat			= 0x0;                         // save format
	aopts[0]->cbFormat			= 0;
	aopts[0]->dwInterleaveEvery = 0;			// for non-video streams only

	//ver 2.26
	if (RecordingMode == ModeFlash) {
		//Internally adjust codec to MSVC 100 Quality
		aopts[0]->fccHandler = mmioFOURCC('M', 'S', 'V', 'C');	 //msvc
		strCodec = CString("MS Video 1");
		aopts[0]->dwQuality = 10000;
	} else {
		//Ver 1.2
		//
		if ((compfccHandler == CompressorStateIsFor) && (compfccHandler != 0)) {
			//make a copy of the pVideoCompressParams just in case after compression, this variable become messed up
			if (MakeCompressParamsCopy(CompressorStateSize, pVideoCompressParams)) {
				aopts[0]->lpParms = pParamsUse;
				aopts[0]->cbParms = CompressorStateSize;
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

	if (autopan) {
		panrect_current.left = left;
		panrect_current.top = top;
		panrect_current.right = left + width - 1;
		panrect_current.bottom = top + height - 1;
	}

	//////////////////////////////////////////////
	// Recording Audio
	//////////////////////////////////////////////
	if ((recordaudio == 2) || (useMCI)) {
		mciRecordOpen();
		mciSetWaveFormat();
		mciRecordStart();

		//if (shiftType == 1)
		//{
		//	mci::mciRecordPause(tempaudiopath);
		//	unshifted = 1;
		//}
	} else if (recordaudio) {
		InitAudioRecording();
		StartAudioRecording(&m_Format);
	}

	if (shiftType == 2) {
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

	long divx = 0L;
	long oldsec = 0L;
	while (recordstate) {  //repeatedly loop
		if (initcapture == 0) {
			timeexpended = timeGetTime() - initialtime;
		} else {
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
			} else if (xPoint.x  > extright ) { //need to pan right
				panrect_dest.left = xPoint.x - width/2;
				panrect_dest.right = panrect_dest.left +  width - 1;
				if (panrect_dest.right >= maxxScreen) {
					panrect_dest.right = maxxScreen - 1;
					panrect_dest.left  = panrect_dest.right - width + 1;
				}
			} else {
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
			} else if (xPoint.y  > extbottom ) { //need to pan down
				panrect_dest.top = xPoint.y - height/2;
				panrect_dest.bottom = panrect_dest.top +  height - 1;
				if (panrect_dest.bottom >= maxyScreen) {
					panrect_dest.bottom = maxyScreen - 1;
					panrect_dest.top  = panrect_dest.bottom - height + 1;
				}
			} else {
				panrect_dest.top = panrect_current.top;
				panrect_dest.bottom  = panrect_current.bottom;
			}

			//Determine Pan Values
			int xdiff, ydiff;
			xdiff = panrect_dest.left - panrect_current.left;
			ydiff = panrect_dest.top - panrect_current.top;

			if (abs(xdiff) < maxpan) {
				panrect_current.left += xdiff;
			} else {
				if (xdiff<0) {
					panrect_current.left -= maxpan;
				} else {
					panrect_current.left += maxpan;
				}
			}

			if (abs(ydiff) < maxpan) {
				panrect_current.top += ydiff;
			} else {
				if (ydiff<0) {
					panrect_current.top -= maxpan;
				} else {
					panrect_current.top += maxpan;
				}
			}

			panrect_current.right = panrect_current.left + width - 1;
			panrect_current.bottom =  panrect_current.top + height - 1;

			alpbi=captureScreenFrame(panrect_current.left, panrect_current.top, width, height, 0);
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

		if (initcapture == 0) {
			if (timelapse>1000) {
				frametime++;
			} else {
				frametime = (DWORD) (((double) timeexpended /1000.0 ) * (double) (1000.0/timelapse));
			}
		} else {
			initcapture = 0;
		}

		fTimeLength = ((float) timeexpended) /((float) 1000.0);

		if (recordpreset) {
			if (int(fTimeLength) >= presettime) {
				//recordstate = 0;
				::PostMessage(hWndGlobal, WM_USER_RECORDINTERRUPTED, 0, 0);
			}

			//CString msgStr;
			//msgStr.Format("%.2f %d", fTimeLength, presettime);
			//MessageBox(NULL, msgStr, "N", MB_OK);

			//or should we post messages
		}

		//if ((shiftType == 1) && (unshifted))
		//{
		//	cc++;
		//	unsigned long thistime = timeGetTime();
		//	int diffInTime = thistime - initialtime;
		//	if (diffInTime >= timeshift)
		//	{
		//		ErrMsg("cc %d diffInTime %d", cc-1, diffInTime);
		//		if ((recordaudio == 2) || (useMCI))
		//		{
		//			mci::mciRecordResume(tempaudiopath);
		//			unshifted = 0;
		//		}
		//	}
		//}

		if ((frametime == 0) || (frametime>oldframetime)) {
			//ver 1.8
			//if (shiftType == 1) {
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

			//if frametime repeats (frametime == oldframetime) ...the avistreamwrite will cause an error
			hr = AVIStreamWrite(psCompressed, frametime, 1, (LPBYTE) alpbi + alpbi->biSize + alpbi->biClrUsed * sizeof(RGBQUAD), alpbi->biSizeImage, 0, NULL, NULL);
			//}

			if (hr != AVIERR_OK) {
				break;
			}

			nActualFrame ++ ;
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
			alpbi=NULL;

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

			if ((recordaudio == 2) || (useMCI)) {
				if (alreadyMCIPause == 0) {
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
			if ((recordaudio == 2) || (useMCI)) {
				if (alreadyMCIPause == 1) {
					mciRecordResume(tempaudiopath);
					alreadyMCIPause = 0;
				}
			}

			timeendpause = timeGetTime();
			timedurationpause =  timeendpause - timestartpause;

			//CString msgstr;
			//msgstr.Format("timestartpause %ld\ntimeendpause %ld\ntimedurationpause %ld", timeendpause, timeendpause, timedurationpause);
			//MessageBox(NULL, msgstr, "Note", MB_OK);

			initialtime = initialtime + timedurationpause;
		} else {
			//introduce time lapse
			//maximum lapse when recordstate changes will be less than 100 milliseconds
			int no_iteration = timelapse/50;
			int remainlapse = timelapse - no_iteration * 50;
			for (int j = 0; j < no_iteration; j++) {
				::Sleep(50); //Sleep for 50 milliseconds many times
				if (recordstate == 0) {
					break;
				}
			}
			if (recordstate == 1) {
				::Sleep(remainlapse);
			}
		}
	} //for loop

error:

	// Now close the file

	if (flashingRect) {
		pFlashingWnd->ShowWindow(SW_HIDE);
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

	AVISaveOptionsFree(1, (LPAVICOMPRESSOPTIONS FAR *) &aopts);

	//////////////////////////////////////////////
	// Recording Audio
	//////////////////////////////////////////////
	if ((recordaudio == 2) || (useMCI)) {
		GetTempWavePath();
		mciRecordStop(tempaudiopath);
		mciRecordClose();
		//restoreWave();
	} else if (recordaudio) {
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

		if (compfccHandler != mmioFOURCC('M', 'S', 'V', 'C')) {
			//if (IDYES == MessageBox(NULL, "Error recording AVI file using current compressor. Use default compressor ? ", "Note", MB_YESNO | MB_ICONEXCLAMATION)) {
			if (IDYES == MessageOut(NULL, IDS_STRING_ERRAVIDEFAULT, IDS_STRING_NOTE, MB_YESNO | MB_ICONQUESTION	)) {
				compfccHandler = mmioFOURCC('M', 'S', 'V', 'C');
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

	TRACE("Success end\n");
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

