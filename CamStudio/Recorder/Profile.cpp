/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "Recorder.h"
#include "Profile.h"
#include "TextAttributes.h"	// for position
#include "ImageAttributes.h"
#include "CStudioLib.h"

bool bAutoAdjust = true;
bool bAutoPan = false;
bool bAutoNaming = false;
bool bCaptureTrans = true;
bool bCaptionAnnotation = false;
bool bDeleteAVIAfterUse = true;
bool bFixedCapture = false;
bool bFlashingRect = true;
bool bLaunchPropPrompt = false;
bool bLaunchHTMLPlayer = true;
bool bMinimizeOnStart = false;
bool bPerformAutoSearch = true;
bool bRestrictVideoCodecs = false;
bool bRecordPreset = false;
bool bSupportMouseDrag = true;
bool bTimestampAnnotation = false;
bool bUseMCI = false;
bool bWatermarkAnnotation = false;

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
int iMaxPan = 20;
int iRecordAudio = 0;
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

TextAttributes taCaption(TOP_LEFT, "ScreenCam", RGB(0, 0, 0), RGB(0xff, 0xff, 0xff));
TextAttributes taTimestamp(TOP_LEFT, "", RGB(0, 0, 0), RGB(0xff, 0xff, 0xff));
ImageAttributes iaWatermark(TOP_LEFT, "");

/////////////////////////////////////////////////////////////////////////////
namespace baseprofile {

template <>
CString ReadEntry(CString strFilename, CString strSection, CString strKeyName, const CString& DefValue)
{
	TRACE("ReadEntry(CString): %s,\nSection: %s\nKey: %s\nValue: %s\n", strFilename, strSection, strKeyName, DefValue);
	const int BUFSIZE = 260;
	TCHAR szBuf[BUFSIZE];
	DWORD dwLen = ::GetPrivateProfileString(strSection, strKeyName, DefValue, szBuf, BUFSIZE, strFilename);
	return (dwLen) ? szBuf : "";
}
template <>
bool WriteEntry(CString strFilename, CString strSection, CString strKeyName, const CString& Value)
{
	TRACE("WriteEntry(CString): %s\nSection: %s\nKey: %s\nValue: %s\n", strFilename, strSection, strKeyName, Value);
	bool bDeleteSection = strKeyName.IsEmpty();
	bool bDeleteKey = Value.IsEmpty();
	bool bResult = (bDeleteSection || bDeleteKey);
	if (!bResult) {
		bResult = ::WritePrivateProfileString(strSection, strKeyName, Value, strFilename)
			? true : false;
	} else if (bDeleteSection) {
		bResult = ::WritePrivateProfileString(strSection, 0, 0, strFilename)
			? true : false;
	} else {
		bResult = ::WritePrivateProfileString(strSection, strKeyName, 0, strFilename)
			? true : false;
	}
	return bResult;
}

// int Read/Write
template <>
int ReadEntry(CString strFilename, CString strSection, CString strKeyName, const int& DefValue)
{
	TRACE("ReadEntry(int): %s\nSection: %s\nKey: %s\nValue: %d\n", strFilename, strSection, strKeyName, DefValue);
	return ::GetPrivateProfileInt(strSection, strKeyName, DefValue, strFilename);
}
template <>
bool WriteEntry(CString strFilename, CString strSection, CString strKeyName, const int& Value)
{
	TRACE("WriteEntry(int): %s\nSection: %s\nKey: %s\nValue: %d\n", strFilename, strSection, strKeyName, Value);
	CString strValue;
	strValue.Format("%d", Value);
	return WriteEntry(strFilename, strSection, strKeyName, strValue);
}

// bool Read/Write
template <>
bool ReadEntry(CString strFilename, CString strSection, CString strKeyName, const bool& DefValue)
{
	TRACE("ReadEntry(bool): %s\nSection: %s\nKey: %s\nValue: %d\n", strFilename, strSection, strKeyName, DefValue);
	return ::GetPrivateProfileInt(strSection, strKeyName, DefValue, strFilename)
		? true : false;
}
template <>
bool WriteEntry(CString strFilename, CString strSection, CString strKeyName, const bool& Value)
{
	TRACE("WriteEntry(bool): %s\nSection: %s\nKey: %s\nValue: %d\n", strFilename, strSection, strKeyName, Value);
	CString strValue;
	strValue.Format("%d", Value ? 1 : 0);
	return WriteEntry(strFilename, strSection, strKeyName, strValue);
}

// long Read/Write
template <>
long ReadEntry(CString strFilename, CString strSection, CString strKeyName, const long& DefValue)
{
	TRACE("ReadEntry(long): %s,\nSection: %s\nKey: %s\nValue: %ld\n", strFilename, strSection, strKeyName, DefValue);
	const int BUFSIZE = 128;
	TCHAR szBuf[BUFSIZE];
	DWORD dwLen = ::GetPrivateProfileString(strSection, strKeyName, _T(""), szBuf, BUFSIZE, strFilename);
	return (dwLen) ? ::_tstol(szBuf) : DefValue;
}
template <>
bool WriteEntry(CString strFilename, CString strSection, CString strKeyName, const long& Value)
{
	TRACE("WriteEntry(long): %s\nSection: %s\nKey: %s\nValue: %ld\n", strFilename, strSection, strKeyName, Value);
	CString strValue;
	strValue.Format("%ld", Value);
	return WriteEntry(strFilename, strSection, strKeyName, strValue);
}

// DWORD Read/Write
template <>
DWORD ReadEntry(CString strFilename, CString strSection, CString strKeyName, const DWORD& DefValue)
{
	TRACE("ReadEntry(DWORD): %s,\nSection: %s\nKey: %s\nValue: %ld\n", strFilename, strSection, strKeyName, DefValue);
	const int BUFSIZE = 128;
	TCHAR szBuf[BUFSIZE];
	DWORD dwLen = ::GetPrivateProfileString(strSection, strKeyName, _T(""), szBuf, BUFSIZE, strFilename);
	return (dwLen) ? ::_tstol(szBuf) : DefValue;
}
template <>
bool WriteEntry(CString strFilename, CString strSection, CString strKeyName, const DWORD& Value)
{
	TRACE("WriteEntry(DWORD): %s\nSection: %s\nKey: %s\nValue: %ld\n", strFilename, strSection, strKeyName, Value);
	CString strValue;
	strValue.Format("%ld", Value);
	return WriteEntry(strFilename, strSection, strKeyName, strValue);
}

// double Read/Write
template <>
double ReadEntry(CString strFilename, CString strSection, CString strKeyName, const double& DefValue)
{
	TRACE("ReadEntry(double): %s,\nSection: %s\nKey: %s\nValue: %ld\n", strFilename, strSection, strKeyName, DefValue);
	const int BUFSIZE = 128;
	TCHAR szBuf[BUFSIZE];
	DWORD dwLen = ::GetPrivateProfileString(strSection, strKeyName, _T(""), szBuf, BUFSIZE, strFilename);
	return (dwLen) ? ::_tstof(szBuf) : DefValue;
}
template <>
bool WriteEntry(CString strFilename, CString strSection, CString strKeyName, const double& Value)
{
	TRACE("WriteEntry(double): %s\nSection: %s\nKey: %s\nValue: %f\n", strFilename, strSection, strKeyName, Value);
	CString strValue;
	strValue.Format("%f", Value);
	return WriteEntry(strFilename, strSection, strKeyName, strValue);
}

// BYTE Read/Write
template <>
BYTE ReadEntry(CString strFilename, CString strSection, CString strKeyName, const BYTE& DefValue)
{
	TRACE("ReadEntry(BYTE): %s,\nSection: %s\nKey: %s\nValue: %u\n", strFilename, strSection, strKeyName, DefValue);
	BYTE iVal = ::GetPrivateProfileInt(strSection, strKeyName, DefValue, strFilename);
	return iVal;
}
template <>
bool WriteEntry(CString strFilename, CString strSection, CString strKeyName, const BYTE& Value)
{
	TRACE("WriteEntry(BYTE): %s\nSection: %s\nKey: %s\nValue: %u\n", strFilename, strSection, strKeyName, Value);
	CString strValue;
	strValue.Format("%u", Value);
	return WriteEntry(strFilename, strSection, strKeyName, strValue);
}

// UINT Read/Write
template <>
UINT ReadEntry(CString strFilename, CString strSection, CString strKeyName, const UINT& DefValue)
{
	TRACE("ReadEntry(UINT): %s,\nSection: %s\nKey: %s\nValue: %u\n", strFilename, strSection, strKeyName, DefValue);
	UINT iVal = ::GetPrivateProfileInt(strSection, strKeyName, DefValue, strFilename);
	return iVal;
}
template <>
bool WriteEntry(CString strFilename, CString strSection, CString strKeyName, const UINT& Value)
{
	TRACE("WriteEntry(BYTE): %s\nSection: %s\nKey: %s\nValue: %u\n", strFilename, strSection, strKeyName, Value);
	CString strValue;
	strValue.Format("%u", Value);
	return WriteEntry(strFilename, strSection, strKeyName, strValue);
}

// LANGID Read/Write
template <>
LANGID ReadEntry(CString strFilename, CString strSection, CString strKeyName, const LANGID& DefValue)
{
	TRACE("ReadEntry(LANGID): %s\nSection: %s\nKey: %s\nValue: %ld\n", strFilename, strSection, strKeyName, DefValue);
	int iDefValue = DefValue;	
	return LANGID(ReadEntry(strFilename, strSection, strKeyName, iDefValue));
}

template <>
bool WriteEntry(CString strFilename, CString strSection, CString strKeyName, const LANGID& Value)
{
	TRACE("WriteEntry(LANGID): %s\nSection: %s\nKey: %s\nValue: %ld\n", strFilename, strSection, strKeyName, Value);
	int iDefValue = Value;
	return WriteEntry(strFilename, strSection, strKeyName, iDefValue);
}

// ePosition Read/Write
template <>
ePosition ReadEntry(CString strFilename, CString strSection, CString strKeyName, const ePosition& DefValue)
{
	TRACE("ReadEntry(ePosition): %s\nSection: %s\nKey: %s\nValue: %ld\n", strFilename, strSection, strKeyName, DefValue);
	int iDefValue = DefValue;
	return ePosition(ReadEntry(strFilename, strSection, strKeyName, iDefValue));
}

template <>
bool WriteEntry(CString strFilename, CString strSection, CString strKeyName, const ePosition& Value)
{
	TRACE("WriteEntry(ePosition): %s\nSection: %s\nKey: %s\nValue: %ld\n", strFilename, strSection, strKeyName, Value);
	int iDefValue = Value;
	return WriteEntry(strFilename, strSection, strKeyName, iDefValue);
}

// LOGFONT Read/Write
template <>
LOGFONT ReadEntry(CString strFilename, CString strSection, CString strKeyName, const LOGFONT& DefValue)
{
	TRACE("ReadEntry(LOGFONT): %s,\nSection: %s\nKey: %s\nValue: \n", strFilename, strSection, strKeyName);

	LOGFONT lfResult = DefValue;
	CString strKeyNameEx;
	strKeyNameEx = strKeyName + "lfHeight";
	lfResult.lfHeight = ReadEntry(strFilename, strSection, strKeyNameEx, DefValue.lfHeight);
	strKeyNameEx = strKeyName + "lfWidth";
	lfResult.lfWidth = ReadEntry(strFilename, strSection, strKeyNameEx, DefValue.lfWidth);
	strKeyNameEx = strKeyName + "lfEscapement";
	lfResult.lfEscapement = ReadEntry(strFilename, strSection, strKeyNameEx, DefValue.lfEscapement);
	strKeyNameEx = strKeyName + "lfOrientation";
	lfResult.lfOrientation = ReadEntry(strFilename, strSection, strKeyNameEx, DefValue.lfOrientation);
	strKeyNameEx = strKeyName + "lfWeight";
	lfResult.lfWeight = ReadEntry(strFilename, strSection, strKeyNameEx, DefValue.lfWeight);
	strKeyNameEx = strKeyName + "lfItalic";
	lfResult.lfItalic = ReadEntry(strFilename, strSection, strKeyNameEx, DefValue.lfItalic);
	strKeyNameEx = strKeyName + "lfUnderline";
	lfResult.lfUnderline = ReadEntry(strFilename, strSection, strKeyNameEx, DefValue.lfUnderline);
	strKeyNameEx = strKeyName + "lfStrikeOut";
	lfResult.lfStrikeOut = ReadEntry(strFilename, strSection, strKeyNameEx, DefValue.lfStrikeOut);
	strKeyNameEx = strKeyName + "lfCharSet";
	lfResult.lfCharSet = ReadEntry(strFilename, strSection, strKeyNameEx, DefValue.lfCharSet);
	strKeyNameEx = strKeyName + "lfOutPrecision";
	lfResult.lfOutPrecision = ReadEntry(strFilename, strSection, strKeyNameEx, DefValue.lfOutPrecision);
	strKeyNameEx = strKeyName + "lfClipPrecision";
	lfResult.lfClipPrecision = ReadEntry(strFilename, strSection, strKeyNameEx, DefValue.lfClipPrecision);
	strKeyNameEx = strKeyName + "lfQuality";
	lfResult.lfQuality = ReadEntry(strFilename, strSection, strKeyNameEx, DefValue.lfQuality);
	strKeyNameEx = strKeyName + "lfPitchAndFamily";
	lfResult.lfPitchAndFamily = ReadEntry(strFilename, strSection, strKeyNameEx, DefValue.lfPitchAndFamily);
	strKeyNameEx = strKeyName + "lfFaceName";
	CString strFaceName(DefValue.lfFaceName);
	// TEST
	strFaceName = "Arial";
	strFaceName = ReadEntry(strFilename, strSection, strKeyNameEx, strFaceName);
	_tcscpy(lfResult.lfFaceName, strFaceName);
	return lfResult;
}
template <>
bool WriteEntry(CString strFilename, CString strSection, CString strKeyName, const LOGFONT& Value)
{
	TRACE("WriteEntry(LOGFONT): %s\nSection: %s\nKey: %s\nValue: %ld\n", strFilename, strSection, strKeyName, Value);
	CString strKeyNameEx;
	strKeyNameEx = strKeyName + "lfHeight";
	bool bResult = WriteEntry(strFilename, strSection, strKeyNameEx, Value.lfHeight);
	strKeyNameEx = strKeyName + "lfWidth";
	bResult &= WriteEntry(strFilename, strSection, strKeyNameEx, Value.lfWidth);
	strKeyNameEx = strKeyName + "lfEscapement";
	bResult &= WriteEntry(strFilename, strSection, strKeyNameEx, Value.lfEscapement);
	strKeyNameEx = strKeyName + "lfOrientation";
	bResult &= WriteEntry(strFilename, strSection, strKeyNameEx, Value.lfOrientation);
	strKeyNameEx = strKeyName + "lfWeight";
	bResult &= WriteEntry(strFilename, strSection, strKeyNameEx, Value.lfWeight);
	strKeyNameEx = strKeyName + "lfItalic";
	bResult &= WriteEntry(strFilename, strSection, strKeyNameEx, Value.lfItalic);
	strKeyNameEx = strKeyName + "lfUnderline";
	bResult &= WriteEntry(strFilename, strSection, strKeyNameEx, Value.lfUnderline);
	strKeyNameEx = strKeyName + "lfStrikeOut";
	bResult &= WriteEntry(strFilename, strSection, strKeyNameEx, Value.lfStrikeOut);
	strKeyNameEx = strKeyName + "lfCharSet";
	bResult &= WriteEntry(strFilename, strSection, strKeyNameEx, Value.lfCharSet);
	strKeyNameEx = strKeyName + "lfOutPrecision";
	bResult &= WriteEntry(strFilename, strSection, strKeyNameEx, Value.lfOutPrecision);
	strKeyNameEx = strKeyName + "lfClipPrecision";
	bResult &= WriteEntry(strFilename, strSection, strKeyNameEx, Value.lfClipPrecision);
	strKeyNameEx = strKeyName + "lfQuality";
	bResult &= WriteEntry(strFilename, strSection, strKeyNameEx, Value.lfQuality);
	strKeyNameEx = strKeyName + "lfPitchAndFamily";
	bResult &= WriteEntry(strFilename, strSection, strKeyNameEx, Value.lfPitchAndFamily);
	strKeyNameEx = strKeyName + "lfFaceName";
	bResult &= WriteEntry(strFilename, strSection, strKeyNameEx, CString(Value.lfFaceName));

	return bResult;
}

// TextAttributes Read/Write
template <>
TextAttributes ReadEntry(CString strFilename, CString strSection, CString strKeyName, const TextAttributes& DefValue)
{
	TRACE("ReadEntry(TextAttributes): %s,\nSection: %s\nKey: %s\nValue: %ld\n", strFilename, strSection, strKeyName, DefValue);
	TextAttributes taResult = DefValue;
	CString strKeyNameEx;
	strKeyNameEx = strKeyName + "position";
	taResult.position = ReadEntry(strFilename, strSection, strKeyNameEx, DefValue.position);
	strKeyNameEx = strKeyName + "text";
	taResult.text = ReadEntry(strFilename, strSection, strKeyNameEx, DefValue.text);
	strKeyNameEx = strKeyName + "backgroundColor";
	taResult.backgroundColor = ReadEntry(strFilename, strSection, strKeyNameEx, DefValue.backgroundColor);
	strKeyNameEx = strKeyName + "textColor";
	taResult.textColor = ReadEntry(strFilename, strSection, strKeyNameEx, DefValue.textColor);
	strKeyNameEx = strKeyName + "isFontSelected";
	taResult.isFontSelected = ReadEntry(strFilename, strSection, strKeyNameEx, DefValue.isFontSelected);
	strKeyNameEx = strKeyName + "logfont";
	taResult.logfont = ReadEntry(strFilename, strSection, strKeyNameEx, DefValue.logfont);

	return taResult;
}
template <>
bool WriteEntry(CString strFilename, CString strSection, CString strKeyName, const TextAttributes& Value)
{
	TRACE("WriteEntry(TextAttributes): %s\nSection: %s\nKey: %s\nValue: %ld\n", strFilename, strSection, strKeyName, Value);
	CString strKeyNameEx;
	strKeyNameEx = strKeyName + "position";
	bool bResult = WriteEntry(strFilename, strSection, strKeyNameEx, Value.position);
	strKeyNameEx = strKeyName + "text";
	bResult &= WriteEntry(strFilename, strSection, strKeyNameEx, Value.text);
	strKeyNameEx = strKeyName + "backgroundColor";
	bResult &= WriteEntry(strFilename, strSection, strKeyNameEx, Value.backgroundColor);
	strKeyNameEx = strKeyName + "textColor";
	bResult &= WriteEntry(strFilename, strSection, strKeyNameEx, Value.textColor);
	strKeyNameEx = strKeyName + "isFontSelected";
	bResult &= WriteEntry(strFilename, strSection, strKeyNameEx, Value.isFontSelected);
	strKeyNameEx = strKeyName + "logfont";
	bResult &= WriteEntry(strFilename, strSection, strKeyNameEx, Value.logfont);
	return bResult;
}

// ImageAttributes Read/Write
template <>
ImageAttributes ReadEntry(CString strFilename, CString strSection, CString strKeyName, const ImageAttributes& DefValue)
{
	TRACE("ReadEntry(ImageAttributes): %s,\nSection: %s\nKey: %s\nValue: %ld\n", strFilename, strSection, strKeyName, DefValue);
	ImageAttributes iaResult = DefValue;
	CString strKeyNameEx;
	strKeyNameEx = strKeyName + "position";
	iaResult.position = ReadEntry(strFilename, strSection, strKeyNameEx, DefValue.position);
	strKeyNameEx = strKeyName + "text";
	iaResult.text = ReadEntry(strFilename, strSection, strKeyNameEx, DefValue.text);
	return iaResult;
}
template <>
bool WriteEntry(CString strFilename, CString strSection, CString strKeyName, const ImageAttributes& Value)
{
	TRACE("WriteEntry(ImageAttributes): %s\nSection: %s\nKey: %s\nValue: %ld\n", strFilename, strSection, strKeyName, Value);
	CString strKeyNameEx;
	strKeyNameEx = strKeyName + "position";
	bool bResult = WriteEntry(strFilename, strSection, strKeyNameEx, Value.position);
	strKeyNameEx = strKeyName + "text";
	bResult &= WriteEntry(strFilename, strSection, strKeyNameEx, Value.text);
	return bResult;
}

}	// namespace baseprofile

/////////////////////////////////////////////////////////////////////////////
const char * const LEGACY_SECTION = _T(" CamStudio Settings ver2.50 -- Please do not edit ");
const char * const APP_SECTION = _T("CamStudio");
const char * const CURSOR_SECTION = _T("Cursor");
const char * const TIMESTAMP_SECTION = _T("TimeStamp");
const char * const CAPTION_SECTION = _T("Caption");
const char * const WATERMARK_SECTION = _T("Watermark");

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
CProfile::CProfile(const CString strFileName)
: m_strFileName(strFileName)
, m_SectionLegacy(LEGACY_SECTION)
, m_SectionApp(APP_SECTION)
, m_SectionHighLight(CURSOR_SECTION)
, m_SectionTimeStamp(TIMESTAMP_SECTION)
, m_SectionCaption(CAPTION_SECTION)
, m_SectionWatermark(WATERMARK_SECTION)
{
	InitSections();
	InitLegacySection();
}

CProfile::~CProfile()
{
	Write();
	// delete the legacy section
	//m_SectionLegacy.Delete(m_strFileName);
}

void CProfile::InitSections()
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

	Add(AUTOPAN, "autopan", false);
	Add(MAXPAN, "maxpan", 0);
	Add(AUDIODEVICEID, "AudioDeviceID", 0U);
	Add(CBWFX, "cbwfx", 50UL);
	Add(RECORDAUDIO, "recordaudio", 0);
	Add(WAVEINSELECTED, "waveinselected", 128UL);
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
	Add(AUTOADJUST, "g_autoadjust", true);
	Add(VALUEADJUST, "g_valueadjust", 0);
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
	Add(LAYOUTNAMELEN, "g_layoutNameLen", 7);
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
	Add(LANGUAGE, "language", LANGID(STANDARD_LANGID));

	Add(m_SectionHighLight, RECORDCURSOR, "RecordCursor", true);
	Add(m_SectionHighLight, CURSORTYPE, "CursorType", 0);
	Add(m_SectionHighLight, CUSTOMSEL, "CustomSel", 0);
	Add(m_SectionHighLight, HIGHLIGHTCURSOR, "HighlightCursor", false);
	Add(m_SectionHighLight, HIGHLIGHTSIZE, "HighlightSize", 0);
	Add(m_SectionHighLight, HIGHLIGHTSHAPE, "HighlightShape", 0);
	Add(m_SectionHighLight, HIGHLIGHTCOLORR, "HighlightColor", RGB(255, 255, 125));
	Add(m_SectionHighLight, HIGHLIGHTCLICK, "HighlightClick", false);
	Add(m_SectionHighLight, HIGHLIGHTCLICKCOLORLEFTR, "ClickColorLeft", RGB(255, 0, 0));
	Add(m_SectionHighLight, HIGHLIGHTCLICKCOLORRIGHTR, "ClickColorRight", RGB(0, 0, 255));
	Add(m_SectionHighLight, SAVEDIR, "SaveDir", 25);		// TODO: should be string
	Add(m_SectionHighLight, CURSORDIR, "CursorDir", 18);	// TODO: should be string

	Add(m_SectionTimeStamp, TIMESTAMPANNOTATION, "timestampAnnotation", false);
	Add(m_SectionTimeStamp, TIMESTAMPTEXTATTRIBUTES, "TimestampTextAttributes", taTimestamp);

	Add(m_SectionCaption, CAPTIONANNOTATION, "captionAnnotation", false);
	Add(m_SectionCaption, CAPTIONTEXTATTRIBUTES, "CaptionTextAttributes", taCaption);

	Add(m_SectionWatermark, WATERMARKANNOTATION, "watermarkAnnotation", false);
	Add(m_SectionWatermark, WATERMARKIMAGEATTRIBUTES, "WatermarkImageAttributes", iaWatermark);
}

void CProfile::InitLegacySection()
{
	Add(m_SectionLegacy, FLASHINGRECT, "flashingRect", false);
	Add(m_SectionLegacy, LAUNCHPLAYER, "launchPlayer", 0);
	Add(m_SectionLegacy, MINIMIZEONSTART, "minimizeOnStart", false);
	Add(m_SectionLegacy, MOUSECAPTUREMODE, "MouseCaptureMode", 0);
	Add(m_SectionLegacy, CAPTUREWIDTH, "capturewidth", 0);
	Add(m_SectionLegacy, CAPTUREHEIGHT, "captureheight", 0);
	Add(m_SectionLegacy, TIMELAPSE, "timelapse", 0);
	Add(m_SectionLegacy, FRAMES_PER_SECOND, "frames_per_second", 0);
	Add(m_SectionLegacy, KEYFRAMESEVERY, "keyFramesEvery", 0);
	Add(m_SectionLegacy, COMPQUALITY, "compquality", 0);
	Add(m_SectionLegacy, COMPFCCHANDLER, "compfccHandler", 0);
	Add(m_SectionLegacy, COMPRESSORSTATEISFOR, "CompressorStateIsFor", 0);
	Add(m_SectionLegacy, COMPRESSORSTATESIZE, "CompressorStateSize", 0);
	Add(m_SectionLegacy, RECORDCURSOR, "g_recordcursor", true);
	Add(m_SectionLegacy, CUSTOMSEL, "g_customsel", 0);
	Add(m_SectionLegacy, CURSORTYPE, "g_cursortype", 0);

	Add(m_SectionLegacy, HIGHLIGHTCURSOR, "g_highlightcursor", false);
	Add(m_SectionLegacy, HIGHLIGHTSIZE, "g_highlightsize", 0);
	Add(m_SectionLegacy, HIGHLIGHTSHAPE, "g_highlightshape", 0);
	Add(m_SectionLegacy, HIGHLIGHTCLICK, "g_highlightclick", false);
	Add(m_SectionLegacy, HIGHLIGHTCOLORR, "g_highlightcolorR", 255);
	Add(m_SectionLegacy, HIGHLIGHTCOLORG, "g_highlightcolorG", 255);
	Add(m_SectionLegacy, HIGHLIGHTCOLORB, "g_highlightcolorB", 125);
	Add(m_SectionLegacy, HIGHLIGHTCLICKCOLORLEFTR, "g_highlightclickcolorleftR", 255);
	Add(m_SectionLegacy, HIGHLIGHTCLICKCOLORLEFTG, "g_highlightclickcolorleftG", 0);
	Add(m_SectionLegacy, HIGHLIGHTCLICKCOLORLEFTB, "g_highlightclickcolorleftB", 0);
	Add(m_SectionLegacy, HIGHLIGHTCLICKCOLORRIGHTR, "g_highlightclickcolorrightR", 0);
	Add(m_SectionLegacy, HIGHLIGHTCLICKCOLORRIGHTG, "g_highlightclickcolorrightG", 0);
	Add(m_SectionLegacy, HIGHLIGHTCLICKCOLORRIGHTB, "g_highlightclickcolorrightB", 255);
	Add(m_SectionLegacy, AUTOPAN, "autopan", false);
	Add(m_SectionLegacy, MAXPAN, "maxpan", 0);
	Add(m_SectionLegacy, AUDIODEVICEID, "AudioDeviceID", 0U);
	Add(m_SectionLegacy, CBWFX, "cbwfx", 50UL);
	Add(m_SectionLegacy, RECORDAUDIO, "recordaudio", 0);
	Add(m_SectionLegacy, WAVEINSELECTED, "waveinselected", 128UL);
	Add(m_SectionLegacy, AUDIO_BITS_PER_SAMPLE, "audio_bits_per_sample", 16);
	Add(m_SectionLegacy, AUDIO_NUM_CHANNELS, "audio_num_channels", 2);
	Add(m_SectionLegacy, AUDIO_SAMPLES_PER_SECONDS, "audio_samples_per_seconds", 22050);
	Add(m_SectionLegacy, BAUDIOCOMPRESSION, "bAudioCompression", true);
	Add(m_SectionLegacy, INTERLEAVEFRAMES, "interleaveFrames", true);
	Add(m_SectionLegacy, INTERLEAVEFACTOR, "interleaveFactor", 100);
	Add(m_SectionLegacy, KEYRECORDSTART, "keyRecordStart", 0);
	Add(m_SectionLegacy, KEYRECORDEND, "keyRecordEnd", 100000);
	Add(m_SectionLegacy, KEYRECORDCANCEL, "uKeyRecordCancel", 100000);
	Add(m_SectionLegacy, VIEWTYPE, "viewtype", 0);
	Add(m_SectionLegacy, AUTOADJUST, "g_autoadjust", true);
	Add(m_SectionLegacy, VALUEADJUST, "g_valueadjust", 0);
	Add(m_SectionLegacy, SAVEDIR, "savedir", 25);
	Add(m_SectionLegacy, CURSORDIR, "strCursorDir", 18);
	Add(m_SectionLegacy, THREADPRIORITY, "threadPriority", 0);
	Add(m_SectionLegacy, CAPTURELEFT, "captureleft", 0);
	Add(m_SectionLegacy, CAPTURETOP, "capturetop", 0);
	Add(m_SectionLegacy, FIXEDCAPTURE, "fixedcapture", false);
	Add(m_SectionLegacy, INTERLEAVEUNIT, "interleaveUnit", 1);
	Add(m_SectionLegacy, TEMPPATH_ACCESS, "tempPath_Access", 0);
	Add(m_SectionLegacy, CAPTURETRANS, "captureTrans", true);
	Add(m_SectionLegacy, SPECIFIEDDIR, "specifieddir", 15);
	Add(m_SectionLegacy, NUMDEV, "NumDev", 0);
	Add(m_SectionLegacy, SELECTEDDEV, "SelectedDev", 0);
	Add(m_SectionLegacy, FEEDBACK_LINE, "feedback_line", 0);
	Add(m_SectionLegacy, FEEDBACK_LINE_INFO, "feedback_line_info", 0);
	Add(m_SectionLegacy, PERFORMAUTOSEARCH, "performAutoSearch", true);
	Add(m_SectionLegacy, SUPPORTMOUSEDRAG, "supportMouseDrag", true);
	Add(m_SectionLegacy, KEYRECORDSTARTCTRL, "keyRecordStartCtrl", 0);
	Add(m_SectionLegacy, KEYRECORDENDCTRL, "keyRecordEndCtrl", 0);
	Add(m_SectionLegacy, KEYRECORDCANCELCTRL, "keyRecordCancelCtrl", 0);
	Add(m_SectionLegacy, KEYRECORDSTARTALT, "keyRecordStartAlt", 0);
	Add(m_SectionLegacy, KEYRECORDENDALT, "keyRecordEndAlt", 0);
	Add(m_SectionLegacy, KEYRECORDCANCELALT, "keyRecordCancelAlt", 0);
	Add(m_SectionLegacy, KEYRECORDSTARTSHIFT, "keyRecordStartShift", 0);
	Add(m_SectionLegacy, KEYRECORDENDSHIFT, "keyRecordEndShift", 0);
	Add(m_SectionLegacy, KEYRECORDCANCELSHIFT, "keyRecordCancelShift", 0);
	Add(m_SectionLegacy, KEYNEXT, "keyNext", 100000);
	Add(m_SectionLegacy, KEYPREV, "keyPrev", 100000);
	Add(m_SectionLegacy, KEYSHOWLAYOUT, "keyShowLayout", 100000);
	Add(m_SectionLegacy, KEYNEXTCTRL, "keyNextCtrl", 0);
	Add(m_SectionLegacy, KEYPREVCTRL, "keyPrevCtrl", 0);
	Add(m_SectionLegacy, KEYSHOWLAYOUTCTRL, "keyShowLayoutCtrl", 0);
	Add(m_SectionLegacy, KEYNEXTALT, "keyNextAlt", 0);
	Add(m_SectionLegacy, KEYPREVALT, "keyPrevAlt", 0);
	Add(m_SectionLegacy, KEYSHOWLAYOUTALT, "keyShowLayoutAlt", 0);
	Add(m_SectionLegacy, KEYNEXTSHIFT, "keyNextShift", 0);
	Add(m_SectionLegacy, KEYPREVSHIFT, "keyPrevShift", 0);
	Add(m_SectionLegacy, KEYSHOWLAYOUTSHIFT, "keyShowLayoutShift", 0);
	Add(m_SectionLegacy, SHAPENAMEINT, "shapeNameInt", 0);
	Add(m_SectionLegacy, SHAPENAMELEN, "shapeNameLen", 6);
	Add(m_SectionLegacy, LAYOUTNAMEINT, "layoutNameInt", 0);
	Add(m_SectionLegacy, LAYOUTNAMELEN, "g_layoutNameLen", 7);
	Add(m_SectionLegacy, USEMCI, "useMCI", false);
	Add(m_SectionLegacy, SHIFTTYPE, "shiftType", 0);
	Add(m_SectionLegacy, TIMESHIFT, "timeshift", 0);
	Add(m_SectionLegacy, FRAMESHIFT, "frameshift", 0);
	Add(m_SectionLegacy, LAUNCHPROPPROMPT, "launchPropPrompt", false);
	Add(m_SectionLegacy, LAUNCHHTMLPLAYER, "launchHTMLPlayer", true);
	Add(m_SectionLegacy, DELETEAVIAFTERUSE, "deleteAVIAfterUse", true);
	Add(m_SectionLegacy, RECORDINGMODE, "RecordingMode", 0);
	Add(m_SectionLegacy, AUTONAMING, "autonaming", false);
	Add(m_SectionLegacy, RESTRICTVIDEOCODECS, "restrictVideoCodecs", false);
	Add(m_SectionLegacy, PRESETTIME, "presettime", 0);
	Add(m_SectionLegacy, RECORDPRESET, "recordpreset", false);
	Add(m_SectionLegacy, LANGUAGE, "language", LANGID(STANDARD_LANGID));

	Add(m_SectionLegacy, TIMESTAMPANNOTATION, "timestampAnnotation", false);
	Add(m_SectionLegacy, TIMESTAMPBACKCOLOR, "timestampBackColor", 0);
	Add(m_SectionLegacy, TIMESTAMPSELECTED, "timestampSelected", 0);
	Add(m_SectionLegacy, TIMESTAMPPOSITION, "timestampPosition", 0);
	Add(m_SectionLegacy, TIMESTAMPTEXTCOLOR, "timestampTextColor", (COLORREF)(16777215));
	Add(m_SectionLegacy, TIMESTAMPTEXTWEIGHT, "timestampTextWeight", 0);
	Add(m_SectionLegacy, TIMESTAMPTEXTHEIGHT, "timestampTextHeight", 0);
	Add(m_SectionLegacy, TIMESTAMPTEXTWIDTH, "timestampTextWidth", 0);
	Add(m_SectionLegacy, TIMESTAMPTEXTFONT, CString(_T("timestampTextFont")), CString(_T("Arial")));
	Add(m_SectionLegacy, CAPTIONANNOTATION, "captionAnnotation", false);
	Add(m_SectionLegacy, CAPTIONBACKCOLOR, "captionBackColor", 0);
	Add(m_SectionLegacy, CAPTIONSELECTED, "captionSelected", 0);
	Add(m_SectionLegacy, CAPTIONPOSITION, "captionPosition", 0);
	Add(m_SectionLegacy, CAPTIONTEXTCOLOR, "captionTextColor", (COLORREF)(16777215));
	Add(m_SectionLegacy, CAPTIONTEXTWEIGHT, "captionTextWeight", 0);
	Add(m_SectionLegacy, CAPTIONTEXTHEIGHT, "captionTextHeight", 0);
	Add(m_SectionLegacy, CAPTIONTEXTWIDTH, "captionTextWidth", 0);
	Add(m_SectionLegacy, CAPTIONTEXTFONT, CString(_T("captionTextFont")), CString(_T("Arial")));
	Add(m_SectionLegacy, WATERMARKANNOTATION, "watermarkAnnotation", false);
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

// add item to specific section group
template <>
bool CProfileSection::Add<>(const int iID, const CString strName, const CString& Value)
{
	return m_grpStrings.Add(iID, strName, Value);
}
template <>
bool CProfileSection::Add<>(const int iID, const CString strName, const int& Value)
{
	return m_grpIntegers.Add(iID, strName, Value);
}
template <>
bool CProfileSection::Add<>(const int iID, const CString strName, const bool& Value)
{
	return m_grpBools.Add(iID, strName, Value);
}
template <>
bool CProfileSection::Add<>(const int iID, const CString strName, const long& Value)
{
	return m_grpLongs.Add(iID, strName, Value);
}
template <>
bool CProfileSection::Add<>(const int iID, const CString strName, const DWORD& Value)
{
	return m_grpDWORD.Add(iID, strName, Value);
}
template <>
bool CProfileSection::Add<>(const int iID, const CString strName, const double& Value)
{
	return m_grpDoubles.Add(iID, strName, Value);
}

template <>
bool CProfileSection::Add<>(const int iID, const CString strName, const LANGID& Value)
{
	return m_grpLANGID.Add(iID, strName, Value);
}

template <>
bool CProfileSection::Add<>(const int iID, const CString strName, const LOGFONT& Value)
{
	return m_grpLogFont.Add(iID, strName, Value);
}
template <>
bool CProfileSection::Add<>(const int iID, const CString strName, const TextAttributes& Value)
{
	return m_grpTextAttribs.Add(iID, strName, Value);
}
template <>
bool CProfileSection::Add<>(const int iID, const CString strName, const ImageAttributes& Value)
{
	return m_grpImageAttribs.Add(iID, strName, Value);
}

template <>
bool CProfileSection::Add<>(const int iID, const CString strName, const UINT& Value)
{
	return m_grpUINT.Add(iID, strName, Value);
}

// read items from section groups
bool CProfileSection::Read(const CString strFile)
{
	bool bResult = m_grpStrings.Read(strFile, m_strSectionName);
	bResult = bResult && m_grpIntegers.Read(strFile, m_strSectionName);
	bResult = bResult && m_grpBools.Read(strFile, m_strSectionName);
	bResult = bResult && m_grpLongs.Read(strFile, m_strSectionName);
	bResult = bResult && m_grpDWORD.Read(strFile, m_strSectionName);
	bResult = bResult && m_grpDoubles.Read(strFile, m_strSectionName);
	bResult = bResult && m_grpLANGID.Read(strFile, m_strSectionName);
	bResult = bResult && m_grpLogFont.Read(strFile, m_strSectionName);
	bResult = bResult && m_grpTextAttribs.Read(strFile, m_strSectionName);
	bResult = bResult && m_grpImageAttribs.Read(strFile, m_strSectionName);
	return bResult;
}
// write items from section groups
bool CProfileSection::Write(const CString strFile)
{
	bool bResult = m_grpStrings.Write(strFile, m_strSectionName);
	bResult = bResult && m_grpIntegers.Write(strFile, m_strSectionName);
	bResult = bResult && m_grpBools.Write(strFile, m_strSectionName);
	bResult = bResult && m_grpLongs.Write(strFile, m_strSectionName);
	bResult = bResult && m_grpDWORD.Write(strFile, m_strSectionName);	
	bResult = bResult && m_grpDoubles.Write(strFile, m_strSectionName);
	bResult = bResult && m_grpLANGID.Write(strFile, m_strSectionName);	
	bResult = bResult && m_grpLogFont.Write(strFile, m_strSectionName);
	bResult = bResult && m_grpTextAttribs.Write(strFile, m_strSectionName);		
	bResult = bResult && m_grpImageAttribs.Write(strFile, m_strSectionName);		
	return bResult;
}

// CGroupItem
/////////////////////////////////////////////////////////////////////////////

