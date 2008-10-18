/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "Profile.h"
#include "TextAttributes.h"	// for position
#include "ImageAttributes.h"
#include "CStudioLib.h"

bool bFlashingRect = true;
bool bMinimizeOnStart = false;
bool bRecordCursor = true;
bool bHighlightCursor = false;
bool bHighlightClick = false;
bool bAutoPan = false;
bool bAutoAdjust = true;
bool bFixedCapture = false;
bool bCaptureTrans = true;
bool bPerformAutoSearch = true;
bool bSupportMouseDrag = true;
bool bUseMCI = false;
bool bLaunchPropPrompt = false;
bool bLaunchHTMLPlayer = true;
bool bDeleteAVIAfterUse = true;
bool bAutoNaming = false;
bool bRestrictVideoCodecs = false;
bool bRecordPreset = false;
bool bTimestampAnnotation = false;
bool bCaptionAnnotation = false;
bool bWatermarkAnnotation = false;
bool bAudioCompression = true;
bool bInterleaveFrames = true;

int iLaunchPlayer = 3;
int iMouseCaptureMode = 0;
int iCaptureWidth = 320;
int iCaptureHeight = 240;
int iTimeLapse = 5;
int iFramesPerSecond = 200;
int iKeyFramesEvery = 200;
int iCompQuality = 7000;
DWORD dwCompfccHandler = 0;
DWORD dwCompressorStateIsFor = 0;
DWORD dwCompressorStateSize = 0;
int iCustomSel = 0;
int iCursorType = 0;
int iHighlightSize = 64;
int iHighlightShape = 0;
COLORREF clrHighlightColor = RGB(255,255,125);
COLORREF clrHighlightClickColorLeft = RGB(255,0,0);
COLORREF clrHighlightClickColorRight = RGB(0,0,255);
int iMaxPan = 20;
UINT uAudioDeviceID = WAVE_MAPPER;
DWORD dwCbwFX;
int iRecordAudio = 0;
DWORD dwWaveinSelected = WAVE_FORMAT_2S16;
int iAudioBitsPerSample = 16;
int iAudioNumChannels = 2;
int iAudioSamplesPerSeconds = 22050;
int iInterleaveFactor = 100;
int iInterleaveUnit = MILLISECONDS;
int iViewType = 0;
int iValueAdjust = 1;
int iSaveLen = 0;	// dir len?
int iCursorLen = 0;	// dir len?
int iThreadPriority = THREAD_PRIORITY_NORMAL;
int iCaptureLeft = 100;
int iCaptureTop = 100;
int iTempPathAccess = USE_WINDOWS_TEMP_DIR;
int iSpecifiedDirLength=0;
int iNumberOfMixerDevices=0;
int iSelectedMixer=0;
int iFeedbackLine = -1;
int iFeedbackLineInfo = -1;
int iShapeNameInt = 1;
int iLayoutNameInt = 1;
int iShiftType = 0; // 0 : no shift, 1 : delayAudio, 2: delayVideo
int iTimeShift = 100;
int iFrameShift = 0;
int iRecordingMode = ModeAVI;
int iPresetTime = 60;
int iLanguageID;

TextAttributes taCaption = {TOP_LEFT, "ScreenCam", RGB(0, 0, 0), RGB(0xff, 0xff, 0xff), 0, 0};
TextAttributes taTimestamp = {TOP_LEFT, "", RGB(0, 0, 0), RGB(0xff, 0xff, 0xff), 0, 0};
ImageAttributes iaWatermark = {TOP_LEFT, ""};

const char * const LEGACY_SECTION_NAME = _T(" CamStudio Settings ver2.50 -- Please do not edit ");

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
CProfile::CProfile(const CString strFileName)
: m_strFileName(strFileName)
, m_Section(LEGACY_SECTION_NAME)
{
	Add(FLASHINGRECT, "flashingRect", false);
	Add(LAUNCHPLAYER, "launchPlayer", 0);
	Add(MINIMIZEONSTART, "minimizeOnStart", false);
	Add(MOUSECAPTUREMODE, "MouseCaptureMode", 0);
	Add(CAPTUREWIDTH, "capturewidth", 0);
	Add(CAPTUREHEIGHT, "captureheight", 0);
	Add(TIMELAPSE, "timelapse", 0);
	Add(FRAMES_PER_SECOND, "frames_per_second", 0);
	Add(KEYFRAMESEVERY, "keyFramesEvery", 0);
	Add(COMPQUALITY, "compquality", 0);
	Add(COMPFCCHANDLER, "compfccHandler", 0);
	Add(COMPRESSORSTATEISFOR, "CompressorStateIsFor", 0);
	Add(COMPRESSORSTATESIZE, "CompressorStateSize", 0);
	Add(G_RECORDCURSOR, "g_recordcursor", true);
	Add(G_CUSTOMSEL, "g_customsel", 0);
	Add(G_CURSORTYPE, "g_cursortype", 0);
	Add(G_HIGHLIGHTCURSOR, "g_highlightcursor", false);
	Add(G_HIGHLIGHTSIZE, "g_highlightsize", 0);
	Add(G_HIGHLIGHTSHAPE, "g_highlightshape", 0);
	Add(G_HIGHLIGHTCLICK, "g_highlightclick", false);
	Add(G_HIGHLIGHTCOLORR, "g_highlightcolorR", 255);
	Add(G_HIGHLIGHTCOLORG, "g_highlightcolorG", 255);
	Add(G_HIGHLIGHTCOLORB, "g_highlightcolorB", 125);
	Add(G_HIGHLIGHTCLICKCOLORLEFTR, "g_highlightclickcolorleftR", 255);
	Add(G_HIGHLIGHTCLICKCOLORLEFTG, "g_highlightclickcolorleftG", 0);
	Add(G_HIGHLIGHTCLICKCOLORLEFTB, "g_highlightclickcolorleftB", 0);
	Add(G_HIGHLIGHTCLICKCOLORRIGHTR, "g_highlightclickcolorrightR", 0);
	Add(G_HIGHLIGHTCLICKCOLORRIGHTG, "g_highlightclickcolorrightG", 0);
	Add(G_HIGHLIGHTCLICKCOLORRIGHTB, "g_highlightclickcolorrightB", 255);
	Add(AUTOPAN, "autopan", false);
	Add(MAXPAN, "maxpan", 0);
	Add(AUDIODEVICEID, "AudioDeviceID", 0);
	Add(CBWFX, "cbwfx", 50);
	Add(RECORDAUDIO, "recordaudio", 0);
	Add(WAVEINSELECTED, "waveinselected", 128);
	Add(AUDIO_BITS_PER_SAMPLE, "audio_bits_per_sample", 16);
	Add(AUDIO_NUM_CHANNELS, "audio_num_channels", 2);
	Add(AUDIO_SAMPLES_PER_SECONDS, "audio_samples_per_seconds", 22050);
	Add(BAUDIOCOMPRESSION, "bAudioCompression", true);
	Add(INTERLEAVEFRAMES, "interleaveFrames", true);
	Add(INTERLEAVEFACTOR, "interleaveFactor", 100);
	Add(KEYRECORDSTART, "keyRecordStart", 0);
	Add(KEYRECORDEND, "keyRecordEnd", 100000);
	Add(KEYRECORDCANCEL, "uKeyRecordCancel", 100000);
	Add(VIEWTYPE, "viewtype", 0);
	Add(G_AUTOADJUST, "g_autoadjust", true);
	Add(G_VALUEADJUST, "g_valueadjust", 0);
	Add(SAVEDIR, "savedir", 25);
	Add(CURSORDIR, "cursordir", 18);
	Add(THREADPRIORITY, "threadPriority", 0);
	Add(CAPTURELEFT, "captureleft", 0);
	Add(CAPTURETOP, "capturetop", 0);
	Add(FIXEDCAPTURE, "fixedcapture", false);
	Add(INTERLEAVEUNIT, "interleaveUnit", 1);
	Add(TEMPPATH_ACCESS, "tempPath_Access", 0);
	Add(CAPTURETRANS, "captureTrans", true);
	Add(SPECIFIEDDIR, "specifieddir", 15);
	Add(NUMDEV, "NumDev", 0);
	Add(SELECTEDDEV, "SelectedDev", 0);
	Add(FEEDBACK_LINE, "feedback_line", 0);
	Add(FEEDBACK_LINE_INFO, "feedback_line_info", 0);
	Add(PERFORMAUTOSEARCH, "performAutoSearch", true);
	Add(SUPPORTMOUSEDRAG, "supportMouseDrag", true);
	Add(KEYRECORDSTARTCTRL, "keyRecordStartCtrl", 0);
	Add(KEYRECORDENDCTRL, "keyRecordEndCtrl", 0);
	Add(KEYRECORDCANCELCTRL, "keyRecordCancelCtrl", 0);
	Add(KEYRECORDSTARTALT, "keyRecordStartAlt", 0);
	Add(KEYRECORDENDALT, "keyRecordEndAlt", 0);
	Add(KEYRECORDCANCELALT, "keyRecordCancelAlt", 0);
	Add(KEYRECORDSTARTSHIFT, "keyRecordStartShift", 0);
	Add(KEYRECORDENDSHIFT, "keyRecordEndShift", 0);
	Add(KEYRECORDCANCELSHIFT, "keyRecordCancelShift", 0);
	Add(KEYNEXT, "keyNext", 100000);
	Add(KEYPREV, "keyPrev", 100000);
	Add(KEYSHOWLAYOUT, "keyShowLayout", 100000);
	Add(KEYNEXTCTRL, "keyNextCtrl", 0);
	Add(KEYPREVCTRL, "keyPrevCtrl", 0);
	Add(KEYSHOWLAYOUTCTRL, "keyShowLayoutCtrl", 0);
	Add(KEYNEXTALT, "keyNextAlt", 0);
	Add(KEYPREVALT, "keyPrevAlt", 0);
	Add(KEYSHOWLAYOUTALT, "keyShowLayoutAlt", 0);
	Add(KEYNEXTSHIFT, "keyNextShift", 0);
	Add(KEYPREVSHIFT, "keyPrevShift", 0);
	Add(KEYSHOWLAYOUTSHIFT, "keyShowLayoutShift", 0);
	Add(SHAPENAMEINT, "shapeNameInt", 0);
	Add(SHAPENAMELEN, "shapeNameLen", 6);
	Add(LAYOUTNAMEINT, "layoutNameInt", 0);
	Add(G_LAYOUTNAMELEN, "g_layoutNameLen", 7);
	Add(USEMCI, "useMCI", false);
	Add(SHIFTTYPE, "shiftType", 0);
	Add(TIMESHIFT, "timeshift", 0);
	Add(FRAMESHIFT, "frameshift", 0);
	Add(LAUNCHPROPPROMPT, "launchPropPrompt", false);
	Add(LAUNCHHTMLPLAYER, "launchHTMLPlayer", true);
	Add(DELETEAVIAFTERUSE, "deleteAVIAfterUse", true);
	Add(RECORDINGMODE, "RecordingMode", 0);
	Add(AUTONAMING, "autonaming", false);
	Add(RESTRICTVIDEOCODECS, "restrictVideoCodecs", false);
	Add(PRESETTIME, "presettime", 0);
	Add(RECORDPRESET, "recordpreset", false);
	Add(LANGUAGE, "language", 0);
	Add(TIMESTAMPANNOTATION, "timestampAnnotation", false);
	Add(TIMESTAMPBACKCOLOR, "timestampBackColor", 0);
	Add(TIMESTAMPSELECTED, "timestampSelected", 0);
	Add(TIMESTAMPPOSITION, "timestampPosition", 0);
	Add(TIMESTAMPTEXTCOLOR, "timestampTextColor", 16777215);
	Add(TIMESTAMPTEXTWEIGHT, "timestampTextWeight", 0);
	Add(TIMESTAMPTEXTHEIGHT, "timestampTextHeight", 0);
	Add(TIMESTAMPTEXTWIDTH, "timestampTextWidth", 0);
	Add(CAPTIONANNOTATION, "captionAnnotation", false);
	Add(CAPTIONBACKCOLOR, "captionBackColor", 0);
	Add(CAPTIONSELECTED, "captionSelected", 0);
	Add(CAPTIONPOSITION, "captionPosition", 0);
	Add(CAPTIONTEXTCOLOR, "captionTextColor", 16777215);
	Add(CAPTIONTEXTWEIGHT, "captionTextWeight", 0);
	Add(CAPTIONTEXTHEIGHT, "captionTextHeight", 0);
	Add(CAPTIONTEXTWIDTH, "captionTextWidth", 0);
	Add(WATERMARKANNOTATION, "watermarkAnnotation", false);

	Add(CAPTIONTEXTFONT, CString(_T("captionTextFont")), CString(_T("Arial")));
	Add(TIMESTAMPTEXTFONT, CString(_T("timestampTextFont")), CString(_T("Arial")));
}

CProfile::~CProfile()
{
}

bool CProfile::Add(const int iID, const CString strName, const int Value)
{
	m_Section.Add(iID, strName, Value);
	return false;
}

bool CProfile::Add(const int iID, const CString strName, const CString Value)
{
	m_Section.Add(iID, strName, Value);
	return false;
}

bool CProfile::Add(const int iID, const CString strName, const bool Value)
{
	m_Section.Add(iID, strName, Value);
	return false;
}

bool CProfile::Add(const int iID, const CString strName, const long Value)
{
	m_Section.Add(iID, strName, Value);
	return false;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
CProfileSection::CProfileSection(const CString strSectionName)
: m_strSectionName(strSectionName)
{
}

CProfileSection::~CProfileSection()
{
}
