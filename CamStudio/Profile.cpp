/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "Profile.h"
#include "CStudioLib.h"

int flashingRect = 1;
int launchPlayer = 3;
int minimizeOnStart = 0;
int MouseCaptureMode = 0;
int capturewidth = 320;
int captureheight = 240;
int timelapse = 5;
int frames_per_second = 200;
int keyFramesEvery = 200;
int compquality = 7000;
DWORD compfccHandler = 0;
DWORD CompressorStateIsFor = 0;
DWORD CompressorStateSize = 0;
int g_recordcursor = 1;
int g_customsel = 0;
int g_cursortype = 0;
int g_highlightcursor = 0;
int g_highlightsize = 64;
int g_highlightshape = 0;
int g_highlightclick = 0;
COLORREF g_highlightcolor = RGB(255,255,125);
COLORREF g_highlightclickcolorleft = RGB(255,0,0);
COLORREF g_highlightclickcolorright = RGB(0,0,255);
int autopan = 0;
int maxpan = 20;
UINT AudioDeviceID = WAVE_MAPPER;
DWORD cbwfx;
int recordaudio = 0;
DWORD waveinselected = WAVE_FORMAT_2S16;
int audio_bits_per_sample = 16;
int audio_num_channels = 2;
int audio_samples_per_seconds = 22050;
BOOL bAudioCompression = TRUE;
BOOL interleaveFrames = TRUE;
int interleaveFactor = 100;
int interleaveUnit = MILLISECONDS;
int viewtype = 0;
int g_autoadjust = 1;
int g_valueadjust = 1;
int savelen = 0;	// dir len?
int cursorlen = 0;	// dir len?
int threadPriority = THREAD_PRIORITY_NORMAL;
int captureleft = 100;
int capturetop = 100;
int fixedcapture = 0;
int tempPath_Access = USE_WINDOWS_TEMP_DIR;
int captureTrans = 1;
int specifiedDirLength=0;
int NumberOfMixerDevices=0;
int SelectedMixer=0;
int feedback_line = -1;
int feedback_lineInfo = -1;
int performAutoSearch = 1;


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
CProfile::CProfile(CString strFileName)
: m_strFileName(strFileName)
{
}

CProfile::~CProfile()
{
}

template <> CString CProfile::Read(const CString strSection, const CString strKey, CString& Value, const CString DefValue)
{
	TCHAR szValue[BUFSIZ];
	DWORD dwLen = ::GetPrivateProfileString(strSection, strKey, DefValue, szValue, BUFSIZ, m_strFileName);
	return Value = (dwLen) ? szValue : DefValue;
}
template <> int CProfile::Read(const CString strSection, const CString strKey, int& Value, const int DefValue)
{
	return Value = ::GetPrivateProfileInt(strSection, strKey, DefValue, m_strFileName); 
}
template <> long CProfile::Read(const CString strSection, const CString strKey, long& Value, const long DefValue)
{
	CString strValue("");
	CString strResult = Read(strSection, strKey, strValue, CString("X"));
	if (strResult == _T("X")) {
		return DefValue;
	}
	return Value = ::atol(strResult);
}
template <> float CProfile::Read(const CString strSection, const CString strKey, float & Value, const float DefValue)
{
	CString strValue("");
	CString strResult = Read(strSection, strKey, strValue, CString("X"));
	if (strResult == _T("X")) {
		return DefValue;
	}
	return Value = ::_tstof(strResult);
}
template <> bool CProfile::Read(const CString strSection, const CString strKey, bool & Value, const bool DefValue)
{
	int iValue = 0;
	int iResult = Read(strSection, strKey, iValue, DefValue ? 1 : 0);
	return Value = iResult ? true : false;
}
template <> COLORREF CProfile::Read(const CString strSection, const CString strKey, COLORREF & Value, const COLORREF DefValue)
{
	int iValue = 0;
	CString strColorKey;
	strColorKey.Format("%sR", strKey);
	int iRed = Read(strSection, strColorKey, iValue, DefValue ? 1 : 0);
	strColorKey.Format("%sG", strKey);
	int iGreen = Read(strSection, strColorKey, iValue, DefValue ? 1 : 0);
	strColorKey.Format("%sB", strKey);
	int iBlue = Read(strSection, strColorKey, iValue, DefValue ? 1 : 0);
	return Value = RGB(iRed, iGreen, iBlue);
}

/////////////////////////////////////////////////////////////////////////////
// write templates
/////////////////////////////////////////////////////////////////////////////
template <> bool CProfile::Write(const CString strSection, const CString strKey, const CString& Value)
{
	bool bResult = ((Value.IsEmpty())
		? ::WritePrivateProfileString(strSection, strKey, 0, m_strFileName)
		: ::WritePrivateProfileString(strSection, strKey, Value, m_strFileName))
		? true : false;
	// TODO: error handler
	return bResult;
}
template <> bool CProfile::Write(const CString strSection, const CString strKey, const int& Value)
{
	CString strValue;
	strValue.Format("%d", Value);
	bool bResult = Write(strSection, strKey, strValue);
	return bResult;
}
template <> bool CProfile::Write(const CString strSection, const CString strKey, const long& Value)
{
	CString strValue;
	strValue.Format("%ld", Value);
	bool bResult = Write(strSection, strKey, strValue);
	return bResult;
}
template <> bool CProfile::Write(const CString strSection, const CString strKey, const float& Value)
{
	CString strValue;
	strValue.Format("%f", Value);
	bool bResult = Write(strSection, strKey, strValue);
	return bResult;
}
template <> bool CProfile::Write(const CString strSection, const CString strKey, const bool& Value)
{
	CString strValue;
	strValue.Format("%d", Value);
	bool bResult = Write(strSection, strKey, strValue);
	return bResult;
}
template <> bool CProfile::Write(const CString strSection, const CString strKey, const COLORREF& Value)
{
	CString strValue;
	strValue.Format("%d", GetRValue(Value));
	CString strColorKey;
	strColorKey.Format("%sR", strKey);
	bool bResult = Write(strSection, strColorKey, strValue);
	strValue.Format("%d", GetGValue(Value));
	strColorKey.Format("%sG", strKey);
	bResult = bResult && Write(strSection, strColorKey, strValue);
	strValue.Format("%d", GetBValue(Value));
	strColorKey.Format("%sB", strKey);
	bResult = bResult && Write(strSection, strColorKey, strValue);
	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
CCamStudioSettings::CCamStudioSettings(CString strFileName)
:	CProfile(strFileName)
, m_bflashingRect(true)
{
	CString strSection;
	strSection.Format(" CamStudio Settings ver%.2f -- Please do not edit ", 2.5);

	// build integer value section
	CProfileSection sectionInt(strSection);
	sectionInt.Add(CProfileKey(_T("flashingRect")));
	sectionInt.Add(CProfileKey(_T("launchPlayer")));
	sectionInt.Add(CProfileKey(_T("minimizeOnStart")));
	sectionInt.Add(CProfileKey(_T("MouseCaptureMode")));
	sectionInt.Add(CProfileKey(_T("capturewidth")));
	sectionInt.Add(CProfileKey(_T("captureheight")));
	sectionInt.Add(CProfileKey(_T("timelapse")));
	sectionInt.Add(CProfileKey(_T("frames_per_second")));
	sectionInt.Add(CProfileKey(_T("keyFramesEvery")));
	sectionInt.Add(CProfileKey(_T("compquality")));
	sectionInt.Add(CProfileKey(_T("compfccHandler")));
	sectionInt.Add(CProfileKey(_T("CompressorStateIsFor")));
	sectionInt.Add(CProfileKey(_T("CompressorStateSize")));
	sectionInt.Add(CProfileKey(_T("g_recordcursor")));
	sectionInt.Add(CProfileKey(_T("g_customsel")));
	sectionInt.Add(CProfileKey(_T("g_cursortype")));
	sectionInt.Add(CProfileKey(_T("g_highlightcursor")));
	sectionInt.Add(CProfileKey(_T("g_highlightsize")));
	sectionInt.Add(CProfileKey(_T("g_highlightshape")));
	sectionInt.Add(CProfileKey(_T("g_highlightclick")));
	sectionInt.Add(CProfileKey(_T("g_highlightcolor")));
	sectionInt.Add(CProfileKey(_T("g_highlightclickcolorleft")));
	sectionInt.Add(CProfileKey(_T("g_highlightclickcolorright")));
	sectionInt.Add(CProfileKey(_T("autopan")));
	sectionInt.Add(CProfileKey(_T("maxpan")));
	sectionInt.Add(CProfileKey(_T("AudioDeviceID")));
	sectionInt.Add(CProfileKey(_T("cbwfx")));
	sectionInt.Add(CProfileKey(_T("recordaudio")));
	sectionInt.Add(CProfileKey(_T("waveinselected")));
	sectionInt.Add(CProfileKey(_T("audio_bits_per_sample")));
	sectionInt.Add(CProfileKey(_T("audio_num_channels")));
	sectionInt.Add(CProfileKey(_T("audio_samples_per_seconds")));
	sectionInt.Add(CProfileKey(_T("bAudioCompression")));
	sectionInt.Add(CProfileKey(_T("interleaveFrames")));
	sectionInt.Add(CProfileKey(_T("interleaveFactor")));
	sectionInt.Add(CProfileKey(_T("keyRecordStart")));
	sectionInt.Add(CProfileKey(_T("keyRecordEnd")));
	sectionInt.Add(CProfileKey(_T("keyRecordCancel")));
	sectionInt.Add(CProfileKey(_T("viewtype")));
	sectionInt.Add(CProfileKey(_T("g_autoadjust")));
	sectionInt.Add(CProfileKey(_T("g_valueadjust")));
	sectionInt.Add(CProfileKey(_T("savedir")));
	sectionInt.Add(CProfileKey(_T("cursordir")));
	sectionInt.Add(CProfileKey(_T("threadPriority")));
	sectionInt.Add(CProfileKey(_T("captureleft")));
	sectionInt.Add(CProfileKey(_T("capturetop")));
	sectionInt.Add(CProfileKey(_T("fixedcapture")));
	sectionInt.Add(CProfileKey(_T("interleaveUnit")));
	sectionInt.Add(CProfileKey(_T("tempPath_Access")));
	sectionInt.Add(CProfileKey(_T("captureTrans")));
	sectionInt.Add(CProfileKey(_T("specifieddir")));
	sectionInt.Add(CProfileKey(_T("NumDev")));
	sectionInt.Add(CProfileKey(_T("SelectedDev")));
	sectionInt.Add(CProfileKey(_T("feedback_line")));
	sectionInt.Add(CProfileKey(_T("feedback_line_info")));
	sectionInt.Add(CProfileKey(_T("performAutoSearch")));
	sectionInt.Add(CProfileKey(_T("supportMouseDrag")));
	sectionInt.Add(CProfileKey(_T("keyRecordStartCtrl")));
	sectionInt.Add(CProfileKey(_T("keyRecordEndCtrl")));
	sectionInt.Add(CProfileKey(_T("keyRecordCancelCtrl")));
	sectionInt.Add(CProfileKey(_T("keyRecordStartAlt")));
	sectionInt.Add(CProfileKey(_T("keyRecordEndAlt")));
	sectionInt.Add(CProfileKey(_T("keyRecordCancelAlt")));
	sectionInt.Add(CProfileKey(_T("keyRecordStartShift")));
	sectionInt.Add(CProfileKey(_T("keyRecordEndShift")));
	sectionInt.Add(CProfileKey(_T("keyRecordCancelShift")));
	sectionInt.Add(CProfileKey(_T("keyNext")));
	sectionInt.Add(CProfileKey(_T("keyPrev")));
	sectionInt.Add(CProfileKey(_T("keyShowLayout")));
	sectionInt.Add(CProfileKey(_T("keyNextCtrl")));
	sectionInt.Add(CProfileKey(_T("keyPrevCtrl")));
	sectionInt.Add(CProfileKey(_T("keyShowLayoutCtrl")));
	sectionInt.Add(CProfileKey(_T("keyNextAlt")));
	sectionInt.Add(CProfileKey(_T("keyPrevAlt")));
	sectionInt.Add(CProfileKey(_T("keyShowLayoutAlt")));
	sectionInt.Add(CProfileKey(_T("keyNextShift")));
	sectionInt.Add(CProfileKey(_T("keyPrevShift")));
	sectionInt.Add(CProfileKey(_T("keyShowLayoutShift")));
	sectionInt.Add(CProfileKey(_T("shapeNameInt")));
	sectionInt.Add(CProfileKey(_T("shapeNameLen")));
	sectionInt.Add(CProfileKey(_T("layoutNameInt")));
	sectionInt.Add(CProfileKey(_T("g_layoutNameLen")));
	sectionInt.Add(CProfileKey(_T("useMCI")));
	sectionInt.Add(CProfileKey(_T("shiftType")));
	sectionInt.Add(CProfileKey(_T("timeshift")));
	sectionInt.Add(CProfileKey(_T("frameshift")));
	sectionInt.Add(CProfileKey(_T("launchPropPrompt")));
	sectionInt.Add(CProfileKey(_T("launchHTMLPlayer")));
	sectionInt.Add(CProfileKey(_T("deleteAVIAfterUse")));
	sectionInt.Add(CProfileKey(_T("RecordingMode")));
	sectionInt.Add(CProfileKey(_T("autonaming")));
	sectionInt.Add(CProfileKey(_T("restrictVideoCodecs")));
	sectionInt.Add(CProfileKey(_T("presettime")));
	sectionInt.Add(CProfileKey(_T("recordpreset")));
	sectionInt.Add(CProfileKey(_T("language")));
	sectionInt.Add(CProfileKey(_T("timestampAnnotation")));
	sectionInt.Add(CProfileKey(_T("timestampBackColor")));
	sectionInt.Add(CProfileKey(_T("timestampSelected")));
	sectionInt.Add(CProfileKey(_T("timestampPosition")));
	sectionInt.Add(CProfileKey(_T("timestampTextColor")));
	sectionInt.Add(CProfileKey(_T("timestampTextWeight")));
	sectionInt.Add(CProfileKey(_T("timestampTextHeight")));
	sectionInt.Add(CProfileKey(_T("timestampTextWidth")));
	sectionInt.Add(CProfileKey(_T("captionAnnotation")));
	sectionInt.Add(CProfileKey(_T("captionBackColor")));
	sectionInt.Add(CProfileKey(_T("captionSelected")));
	sectionInt.Add(CProfileKey(_T("captionPosition")));
	sectionInt.Add(CProfileKey(_T("captionTextColor")));
	sectionInt.Add(CProfileKey(_T("captionTextWeight")));
	sectionInt.Add(CProfileKey(_T("captionTextHeight")));
	sectionInt.Add(CProfileKey(_T("captionTextWidth")));
	sectionInt.Add(CProfileKey(_T("watermarkAnnotation")));
	sectionInt.Add(CProfileKey(_T("watermarkPosition")));

	Add(sectionInt);

	// build string value section
	CProfileSection sectionString(strSection);
	sectionString.Add(CProfileKey(_T("captionTextFont")));
	sectionString.Add(CProfileKey(_T("timestampTextFont")));
	sectionString.Add(CProfileKey(_T("captionTextFont")));
	
	Add(sectionString);
}

CCamStudioSettings::~CCamStudioSettings()
{
}

/////////////////////////////////////////////////////////////////////////////
// TODO: Read and write every value as a string and convert the result 
/////////////////////////////////////////////////////////////////////////////
bool CCamStudioSettings::LoadSettings()
{
	CString strSection;
	strSection.Format(" CamStudio Settings ver%.2f -- Please do not edit ", 2.5);

	Read(strSection, _T("flashingRect"), m_bflashingRect, true);
	Read(strSection, _T("launchPlayer"), m_ilaunchPlayer, 0);
	Read(strSection, _T("minimizeOnStart"), m_iminimizeOnStart, 0);

	Read(strSection, _T("MouseCaptureMode"), m_iMouseCaptureMode, 0);
	Read(strSection, _T("capturewidth"), m_icapturewidth, 0);
	Read(strSection, _T("captureheight"), m_icaptureheight, 0);

	Read(strSection, _T("timelapse"), m_itimelapse, 0);
	Read(strSection, _T("frames_per_second"), m_iframes_per_second, 0);
	Read(strSection, _T("keyFramesEvery"), m_ikeyFramesEvery, 0);
	Read(strSection, _T("compquality"), m_icompquality, 0);
	Read(strSection, _T("compfccHandler"), m_dwcompfccHandler, 0UL);

	Read(strSection, _T("CompressorStateIsFor"), m_dwCompressorStateIsFor, 0UL);
	Read(strSection, _T("CompressorStateSize"), m_dwCompressorStateSize, 0UL);

	Read(strSection, _T("g_recordcursor"), m_irecordcursor, 0);
	Read(strSection, _T("g_customsel"), m_icustomsel, 0);
	Read(strSection, _T("g_cursortype"), m_icursortype, 0);
	Read(strSection, _T("g_highlightcursor"), m_ihighlightcursor, 0);
	Read(strSection, _T("g_highlightsize"), m_ihighlightsize, 0);
	Read(strSection, _T("g_highlightshape"), m_ihighlightshape, 0);
	Read(strSection, _T("g_highlightclick"), m_ihighlightclick, 0);

	Read(strSection, _T("g_highlightcolor"), m_clrhighlightcolor, RGB(0,0,0));
	Read(strSection, _T("g_highlightclickcolorleft"), m_clrhighlightclickcolorleft, RGB(0,0,0));
	Read(strSection, _T("g_highlightclickcolorright"), m_clrhighlightclickcolorright, RGB(0,0,0));

	Read(strSection, _T("autopan"), m_iautopan, 0);
	Read(strSection, _T("maxpan"), m_imaxpan, 0);

	Read(strSection, _T("AudioDeviceID"), m_uAudioDeviceID, 0U);
	
	Read(strSection, _T("cbwfx"), m_icbwfx, 0);
	Read(strSection, _T("recordaudio"), m_irecordaudio, 0);

	Read(strSection, _T("waveinselected"), m_iwaveinselected, 0);
	Read(strSection, _T("audio_bits_per_sample"), m_iaudio_bits_per_sample, 0);
	Read(strSection, _T("audio_num_channels"), m_iaudio_num_channels, 0);
	Read(strSection, _T("audio_samples_per_seconds"), m_iaudio_samples_per_seconds, 0);
	Read(strSection, _T("bAudioCompression"), m_ibAudioCompression, 0);

	Read(strSection, _T("interleaveFrames"), m_iinterleaveFrames, 0);
	Read(strSection, _T("interleaveFactor"), m_iinterleaveFactor, 0);

	Read(strSection, _T("keyRecordStart"), m_ikeyRecordStart, 0);
	Read(strSection, _T("keyRecordEnd"), m_ikeyRecordEnd, 0);
	Read(strSection, _T("keyRecordCancel"), m_ikeyRecordCancel, 0);

	Read(strSection, _T("viewtype"), m_iviewtype, 0);

	Read(strSection, _T("g_autoadjust"), m_iautoadjust, 0);
	Read(strSection, _T("g_valueadjust"), m_ivalueadjust, 0);
	
	Read(strSection, _T("savedir"), m_isavelen, 0);
	Read(strSection, _T("cursordir"), m_icursorlen, 0);

	Read(strSection, _T("threadPriority"), m_ithreadPriority, 0);

	Read(strSection, _T("captureleft"), m_icaptureleft, 0);
	Read(strSection, _T("capturetop"), m_icapturetop, 0);
	Read(strSection, _T("fixedcapture"), m_ifixedcapture, 0);
	Read(strSection, _T("interleaveUnit"), m_iinterleaveUnit, 0);
	
	Read(strSection, _T("tempPath_Access"), m_itempPath_Access, 0);
	Read(strSection, _T("captureTrans"), m_icaptureTrans, 0);
	Read(strSection, _T("specifieddir"), m_ispecifiedDirLength, 0);
	Read(strSection, _T("NumDev"), m_iNumberOfMixerDevices, 0);
	Read(strSection, _T("SelectedDev"), m_iSelectedMixer, 0);
	Read(strSection, _T("feedback_line"), m_ifeedback_line, 0);
	Read(strSection, _T("feedback_line_info"), m_ifeedback_lineInfo, 0);
	Read(strSection, _T("performAutoSearch"), m_iperformAutoSearch, 0);
	
	Read(strSection, _T("supportMouseDrag"), m_isupportMouseDrag, 0);

	Read(strSection, _T("keyRecordStartCtrl"), m_ikeyRecordStartCtrl, 0);
	Read(strSection, _T("keyRecordEndCtrl"), m_ikeyRecordEndCtrl, 0);
	Read(strSection, _T("keyRecordCancelCtrl"), m_ikeyRecordCancelCtrl, 0);
	Read(strSection, _T("keyRecordStartAlt"), m_ikeyRecordStartAlt, 0);
	Read(strSection, _T("keyRecordEndAlt"), m_ikeyRecordEndAlt, 0);
	Read(strSection, _T("keyRecordCancelAlt"), m_ikeyRecordCancelAlt, 0);
	Read(strSection, _T("keyRecordStartShift"), m_ikeyRecordStartShift, 0);
	Read(strSection, _T("keyRecordEndShift"), m_ikeyRecordEndShift, 0);
	Read(strSection, _T("keyRecordCancelShift"), m_ikeyRecordCancelShift, 0);
	Read(strSection, _T("keyNext"), m_ikeyNext, 0);
	Read(strSection, _T("keyPrev"), m_ikeyPrev, 0);
	Read(strSection, _T("keyShowLayout"), m_ikeyShowLayout, 0);
	Read(strSection, _T("keyNextCtrl"), m_ikeyNextCtrl, 0);
	Read(strSection, _T("keyPrevCtrl"), m_ikeyPrevCtrl, 0);
	Read(strSection, _T("keyShowLayoutCtrl"), m_ikeyShowLayoutCtrl, 0);
	Read(strSection, _T("keyNextAlt"), m_ikeyNextAlt, 0);
	Read(strSection, _T("keyPrevAlt"), m_ikeyPrevAlt, 0);
	Read(strSection, _T("keyShowLayoutAlt"), m_ikeyShowLayoutAlt, 0);
	
	Read(strSection, _T("keyNextShift"), m_ikeyNextShift, 0);
	Read(strSection, _T("keyPrevShift"), m_ikeyPrevShift, 0);
	Read(strSection, _T("keyShowLayoutShift"), m_ikeyShowLayoutShift, 0);
	
	Read(strSection, _T("shapeNameInt"), m_ishapeNameInt, 0);
	Read(strSection, _T("shapeNameLen"), m_ishapeNameLen, 0);
	
	Read(strSection, _T("layoutNameInt"), m_ilayoutNameInt, 0);
	Read(strSection, _T("g_layoutNameLen"), m_ilayoutNameLen, 0);
	
	Read(strSection, _T("useMCI"), m_iuseMCI, 0);
	Read(strSection, _T("shiftType"), m_ishiftType, 0);
	Read(strSection, _T("timeshift"), m_itimeshift, 0);
	Read(strSection, _T("frameshift"), m_iframeshift, 0);
	
	Read(strSection, _T("launchPropPrompt"), m_ilaunchPropPrompt, 0);
	Read(strSection, _T("launchHTMLPlayer"), m_ilaunchHTMLPlayer, 0);
	Read(strSection, _T("deleteAVIAfterUse"), m_ideleteAVIAfterUse, 0);
	Read(strSection, _T("RecordingMode"), m_iRecordingMode, 0);
	Read(strSection, _T("autonaming"), m_iautonaming, 0);
	Read(strSection, _T("restrictVideoCodecs"), m_irestrictVideoCodecs, 0);
	
	Read(strSection, _T("presettime"), m_ipresettime, 0);
	Read(strSection, _T("recordpreset"), m_irecordpreset, 0);
	
	Read(strSection, _T("language"), m_ilanguageID, 0);
	
	//TextAttributes timestamp
	Read(strSection, _T("timestampAnnotation"), m_itimestampAnnotation, 0);
	Read(strSection, _T("timestampBackColor"), m_itimestamp_backgroundColor, 0);
	Read(strSection, _T("timestampSelected"), m_itimestamp_isFontSelected, 0);
	Read(strSection, _T("timestampPosition"), m_itimestamp_position, 0);
	Read(strSection, _T("timestampTextColor"), m_itimestamp_textColor, 0);
	Read(strSection, _T("timestampTextWeight"), m_itimestamp_logfont_lfWeight, 0);
	Read(strSection, _T("timestampTextHeight"), m_itimestamp_logfont_lfHeight, 0);
	Read(strSection, _T("timestampTextWidth"), m_itimestamp_logfont_lfWidth, 0);

	Read(strSection, _T("captionAnnotation"), m_icaptionAnnotation, 0);
	Read(strSection, _T("captionBackColor"), m_icaption_backgroundColor, 0);
	Read(strSection, _T("captionSelected"), m_icaption_isFontSelected, 0);
	Read(strSection, _T("captionPosition"), m_icaption_position, 0);
	Read(strSection, _T("captionTextColor"), m_icaption_textColor, 0);
	Read(strSection, _T("captionTextWeight"), m_icaption_logfont_lfWeight, 0);
	Read(strSection, _T("captionTextHeight"), m_icaption_logfont_lfHeight, 0);
	Read(strSection, _T("captionTextWidth"), m_icaption_logfont_lfWidth, 0);
	Read(strSection, _T("watermarkAnnotation"), m_iwatermarkAnnotation, 0);
	Read(strSection, _T("watermarkPosition"), m_iwatermark_position, 0);

	Read(strSection, _T("captionTextFont"), m_strcaption_logfont_lfFaceName, CString(_T("")));
	Read(strSection, _T("timestampTextFont"), m_strtimestamp_logfont_lfFaceName, CString(_T("")));
	Read(strSection, _T("captionTextFont"), m_strcaption_text, CString(_T("")));

	return false;
}

/////////////////////////////////////////////////////////////////////////////
// TODO: Read and write every value as a string and convert the result 
/////////////////////////////////////////////////////////////////////////////
bool CCamStudioSettings::WriteSettings()
{
	CString strSection;
	strSection.Format(" CamStudio Settings ver%.2f -- Please do not edit ", 2.5);

	Write(strSection, _T("flashingRect"), m_bflashingRect);
	Write(strSection, _T("launchPlayer"), m_ilaunchPlayer);
	Write(strSection, _T("minimizeOnStart"), m_iminimizeOnStart);

	Write(strSection, _T("MouseCaptureMode"), m_iMouseCaptureMode);
	Write(strSection, _T("capturewidth"), m_icapturewidth);
	Write(strSection, _T("captureheight"), m_icaptureheight);

	Write(strSection, _T("timelapse"), m_itimelapse);
	Write(strSection, _T("frames_per_second"), m_iframes_per_second);
	Write(strSection, _T("keyFramesEvery"), m_ikeyFramesEvery);
	Write(strSection, _T("compquality"), m_icompquality);
	Write(strSection, _T("compfccHandler"), m_dwcompfccHandler);

	Write(strSection, _T("CompressorStateIsFor"), m_dwCompressorStateIsFor);
	Write(strSection, _T("CompressorStateSize"), m_dwCompressorStateSize);

	Write(strSection, _T("g_recordcursor"), m_irecordcursor);
	Write(strSection, _T("g_customsel"), m_icustomsel);
	Write(strSection, _T("g_cursortype"), m_icursortype);
	Write(strSection, _T("g_highlightcursor"), m_ihighlightcursor);
	Write(strSection, _T("g_highlightsize"), m_ihighlightsize);
	Write(strSection, _T("g_highlightshape"), m_ihighlightshape);
	Write(strSection, _T("g_highlightclick"), m_ihighlightclick);

	Write(strSection, _T("g_highlightcolor"), m_clrhighlightcolor);
	Write(strSection, _T("g_highlightclickcolorleft"), m_clrhighlightclickcolorleft);
	Write(strSection, _T("g_highlightclickcolorright"), m_clrhighlightclickcolorright);

	Write(strSection, _T("autopan"), m_iautopan);
	Write(strSection, _T("maxpan"), m_imaxpan);

	Write(strSection, _T("AudioDeviceID"), m_uAudioDeviceID);
	
	Write(strSection, _T("cbwfx"), m_icbwfx);
	Write(strSection, _T("recordaudio"), m_irecordaudio);

	Write(strSection, _T("waveinselected"), m_iwaveinselected);
	Write(strSection, _T("audio_bits_per_sample"), m_iaudio_bits_per_sample);
	Write(strSection, _T("audio_num_channels"), m_iaudio_num_channels);
	Write(strSection, _T("audio_samples_per_seconds"), m_iaudio_samples_per_seconds);
	Write(strSection, _T("bAudioCompression"), m_ibAudioCompression);

	Write(strSection, _T("interleaveFrames"), m_iinterleaveFrames);
	Write(strSection, _T("interleaveFactor"), m_iinterleaveFactor);

	Write(strSection, _T("keyRecordStart"), m_ikeyRecordStart);
	Write(strSection, _T("keyRecordEnd"), m_ikeyRecordEnd);
	Write(strSection, _T("keyRecordCancel"), m_ikeyRecordCancel);

	Write(strSection, _T("viewtype"), m_iviewtype);

	Write(strSection, _T("g_autoadjust"), m_iautoadjust);
	Write(strSection, _T("g_valueadjust"), m_ivalueadjust);
	
	Write(strSection, _T("savedir"), m_isavelen);
	Write(strSection, _T("cursordir"), m_icursorlen);

	Write(strSection, _T("threadPriority"), m_ithreadPriority);

	Write(strSection, _T("captureleft"), m_icaptureleft);
	Write(strSection, _T("capturetop"), m_icapturetop);
	Write(strSection, _T("fixedcapture"), m_ifixedcapture);
	Write(strSection, _T("interleaveUnit"), m_iinterleaveUnit);
	
	Write(strSection, _T("tempPath_Access"), m_itempPath_Access);
	Write(strSection, _T("captureTrans"), m_icaptureTrans);
	Write(strSection, _T("specifieddir"), m_ispecifiedDirLength);
	Write(strSection, _T("NumDev"), m_iNumberOfMixerDevices);
	Write(strSection, _T("SelectedDev"), m_iSelectedMixer);
	Write(strSection, _T("feedback_line"), m_ifeedback_line);
	Write(strSection, _T("feedback_line_info"), m_ifeedback_lineInfo);
	Write(strSection, _T("performAutoSearch"), m_iperformAutoSearch);
	
	Write(strSection, _T("supportMouseDrag"), m_isupportMouseDrag);

	Write(strSection, _T("keyRecordStartCtrl"), m_ikeyRecordStartCtrl);
	Write(strSection, _T("keyRecordEndCtrl"), m_ikeyRecordEndCtrl);
	Write(strSection, _T("keyRecordCancelCtrl"), m_ikeyRecordCancelCtrl);
	Write(strSection, _T("keyRecordStartAlt"), m_ikeyRecordStartAlt);
	Write(strSection, _T("keyRecordEndAlt"), m_ikeyRecordEndAlt);
	Write(strSection, _T("keyRecordCancelAlt"), m_ikeyRecordCancelAlt);
	Write(strSection, _T("keyRecordStartShift"), m_ikeyRecordStartShift);
	Write(strSection, _T("keyRecordEndShift"), m_ikeyRecordEndShift);
	Write(strSection, _T("keyRecordCancelShift"), m_ikeyRecordCancelShift);
	Write(strSection, _T("keyNext"), m_ikeyNext);
	Write(strSection, _T("keyPrev"), m_ikeyPrev);
	Write(strSection, _T("keyShowLayout"), m_ikeyShowLayout);
	Write(strSection, _T("keyNextCtrl"), m_ikeyNextCtrl);
	Write(strSection, _T("keyPrevCtrl"), m_ikeyPrevCtrl);
	Write(strSection, _T("keyShowLayoutCtrl"), m_ikeyShowLayoutCtrl);
	Write(strSection, _T("keyNextAlt"), m_ikeyNextAlt);
	Write(strSection, _T("keyPrevAlt"), m_ikeyPrevAlt);
	Write(strSection, _T("keyShowLayoutAlt"), m_ikeyShowLayoutAlt);
	
	Write(strSection, _T("keyNextShift"), m_ikeyNextShift);
	Write(strSection, _T("keyPrevShift"), m_ikeyPrevShift);
	Write(strSection, _T("keyShowLayoutShift"), m_ikeyShowLayoutShift);
	
	Write(strSection, _T("shapeNameInt"), m_ishapeNameInt);
	Write(strSection, _T("shapeNameLen"), m_ishapeNameLen);
	
	Write(strSection, _T("layoutNameInt"), m_ilayoutNameInt);
	Write(strSection, _T("g_layoutNameLen"), m_ilayoutNameLen);
	
	Write(strSection, _T("useMCI"), m_iuseMCI);
	Write(strSection, _T("shiftType"), m_ishiftType);
	Write(strSection, _T("timeshift"), m_itimeshift);
	Write(strSection, _T("frameshift"), m_iframeshift);
	
	Write(strSection, _T("launchPropPrompt"), m_ilaunchPropPrompt);
	Write(strSection, _T("launchHTMLPlayer"), m_ilaunchHTMLPlayer);
	Write(strSection, _T("deleteAVIAfterUse"), m_ideleteAVIAfterUse);
	Write(strSection, _T("RecordingMode"), m_iRecordingMode);
	Write(strSection, _T("autonaming"), m_iautonaming);
	Write(strSection, _T("restrictVideoCodecs"), m_irestrictVideoCodecs);
	
	Write(strSection, _T("presettime"), m_ipresettime);
	Write(strSection, _T("recordpreset"), m_irecordpreset);
	
	Write(strSection, _T("language"), m_ilanguageID);
	
	//TextAttributes timestamp
	Write(strSection, _T("timestampAnnotation"), m_itimestampAnnotation);
	Write(strSection, _T("timestampBackColor"), m_itimestamp_backgroundColor);
	Write(strSection, _T("timestampSelected"), m_itimestamp_isFontSelected);
	Write(strSection, _T("timestampPosition"), m_itimestamp_position);
	Write(strSection, _T("timestampTextColor"), m_itimestamp_textColor);
	Write(strSection, _T("timestampTextWeight"), m_itimestamp_logfont_lfWeight);
	Write(strSection, _T("timestampTextHeight"), m_itimestamp_logfont_lfHeight);
	Write(strSection, _T("timestampTextWidth"), m_itimestamp_logfont_lfWidth);

	Write(strSection, _T("captionAnnotation"), m_icaptionAnnotation);
	Write(strSection, _T("captionBackColor"), m_icaption_backgroundColor);
	Write(strSection, _T("captionSelected"), m_icaption_isFontSelected);
	Write(strSection, _T("captionPosition"), m_icaption_position);
	Write(strSection, _T("captionTextColor"), m_icaption_textColor);
	Write(strSection, _T("captionTextWeight"), m_icaption_logfont_lfWeight);
	Write(strSection, _T("captionTextHeight"), m_icaption_logfont_lfHeight);
	Write(strSection, _T("captionTextWidth"), m_icaption_logfont_lfWidth);
	Write(strSection, _T("watermarkAnnotation"), m_iwatermarkAnnotation);
	Write(strSection, _T("watermarkPosition"), m_iwatermark_position);

	Write(strSection, _T("captionTextFont"), m_strcaption_logfont_lfFaceName);
	Write(strSection, _T("timestampTextFont"), m_strtimestamp_logfont_lfFaceName);
	Write(strSection, _T("captionTextFont"), m_strcaption_text);
	return false;
}
