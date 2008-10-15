/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "Profile.h"
#include "resource.h"       // main symbols
#include "EffectsOptions.h"
#include "EffectsOptions2.h"
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


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
CProfile::CProfile(CString strFileName)
: m_strFileName(strFileName)
{
}

CProfile::~CProfile()
{
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
char const * const CCamStudioSettings::LEGACY_SECTION = _T(" CamStudio Settings ver2.50 -- Please do not edit ");
CCamStudioSettings::CCamStudioSettings(CString strFileName)
:	CProfile(strFileName)
, m_StrSection(LEGACY_SECTION)
, m_IntSection(LEGACY_SECTION)
, m_BoolSection(LEGACY_SECTION)
, m_LongSection(LEGACY_SECTION)
, m_DblSection(LEGACY_SECTION)
, m_ColorSection(LEGACY_SECTION)
{
//Still moving legacy names around so thery are here for easy edit purposes.

//flashingRect=1 
//launchPlayer=3 
//minimizeOnStart=0 
//MouseCaptureMode= 0 
//capturewidth=320 
//captureheight=240 
//timelapse=50 
//frames_per_second= 20 
//keyFramesEvery= 30 
//compquality= 7000 
//compfccHandler= 1129730893 
//CompressorStateIsFor= 1129730893 
//CompressorStateSize= 4 
//g_recordcursor=1 
//g_customsel=0 
//g_cursortype=0 
//g_highlightcursor=0 
//g_highlightsize=64 
//g_highlightshape=0 
//g_highlightclick=0 
//g_highlightcolorR=255 
//g_highlightcolorG=255 
//g_highlightcolorB=125 
//g_highlightclickcolorleftR=255 
//g_highlightclickcolorleftG=0 
//g_highlightclickcolorleftB=0 
//g_highlightclickcolorrightR=0 
//g_highlightclickcolorrightG=0 
//g_highlightclickcolorrightB=255 
//autopan=0 
//maxpan= 20 
//AudioDeviceID= -1 
//cbwfx= 50 
//recordaudio= 0 
//waveinselected= 128 
//audio_bits_per_sample= 16 
//audio_num_channels= 2 
//audio_samples_per_seconds= 22050 
//bAudioCompression= 1 
//interleaveFrames= 1 
//interleaveFactor= 100 
//keyRecordStart= 119 
//keyRecordEnd= 120 
//keyRecordCancel= 121 
//viewtype= 0 
//g_autoadjust= 0 
//g_valueadjust= 1 
//savedir=58 
//cursordir=18 
//threadPriority=0 
//captureleft= 100 
//capturetop= 100 
//fixedcapture=0 
//interleaveUnit= 0 
//tempPath_Access=0 
//captureTrans=1 
//specifieddir=15 
//NumDev=0 
//SelectedDev=0 
//feedback_line=-1 
//feedback_line_info=-1 
//performAutoSearch=1 
//supportMouseDrag=1 
//keyRecordStartCtrl=0 
//keyRecordEndCtrl=0 
//keyRecordCancelCtrl=0 
//keyRecordStartAlt=0 
//keyRecordEndAlt=0 
//keyRecordCancelAlt=0 
//keyRecordStartShift=0 
//keyRecordEndShift=0 
//keyRecordCancelShift=0 
//keyNext=122 
//keyPrev=123 
//keyShowLayout=100000 
//keyNextCtrl=1 
//keyPrevCtrl=1 
//keyShowLayoutCtrl=0 
//keyNextAlt=0 
//keyPrevAlt=0 
//keyShowLayoutAlt=0 
//keyNextShift=0 
//keyPrevShift=0 
//keyShowLayoutShift=0 
//shapeNameInt=1 
//shapeNameLen=6 
//layoutNameInt=1 
//g_layoutNameLen=7 
//useMCI=0 
//shiftType=0 
//timeshift=100 
//frameshift=0 
//launchPropPrompt=0 
//launchHTMLPlayer=1 
//deleteAVIAfterUse=1 
//RecordingMode=0 
//autonaming=0 
//restrictVideoCodecs=0 
//presettime=60 
//recordpreset=0 
//language=9 
//timestampAnnotation=0 
//timestampBackColor=0 
//timestampSelected=0 
//timestampPosition=0 
//timestampTextColor=16777215 
//timestampTextFont=timestampTextWeight=0 
//timestampTextWeight=0 
//timestampTextHeight=0 
//timestampTextWidth=0 
//captionAnnotation=0 
//captionBackColor=0 
//captionSelected=0 
//captionPosition=0 
//captionTextColor=16777215 
//captionTextFont=captionTextWeight=0 
//captionTextWeight=0 
//captionTextHeight=0 
//captionTextWidth=0 
//watermarkAnnotation=0 
//watermarkAnnotation=0 

#if defined(VERSION) && (VERSION < 260)
	// TODO: Place this is a throw away section/profile for conversion
	// Old names
// booleans
	Add(_T("flashingRect"), FLASHINGRECT, true);
	Add(_T("minimizeOnStart"), MINIMIZEONSTART, false);
	Add(_T("g_recordcursor"), RECORDCURSOR, true);
	Add(_T("g_highlightcursor"), HIGHLIGHTCURSOR, false);
	Add(_T("g_highlightclick"), HIGHLIGHTCLICK, false);
	Add(_T("autopan"), AUTOPAN, false);
	Add(_T("bAudioCompression"), AUDIOCOMPRESSION, true);
	Add(_T("interleaveFrames"), INTERLEAVEFRAMES, true);
	Add(_T("g_autoadjust"), AUTOADJUST, true);
	Add(_T("fixedcapture"), FIXEDCAPTURE, false);
	Add(_T("captureTrans"), CAPTURETRANS, true);
	Add(_T("performAutoSearch"), PERFORMAUTOSEARCH, true);
	Add(_T("supportMouseDrag"), SUPPORTMOUSEDRAG, true);
// TODO
	Add(_T("bUseMCI"), USEMCI, false);
	Add(_T("bLaunchPropPrompt"), LAUNCHPROPPROMPT, false);
	Add(_T("bLaunchHTMLPlayer"), LAUNCHHTMLPLAYER, true);
	Add(_T("bDeleteAVIAfterUse"), DELETEAVIAFTERUSE, true);
	Add(_T("bAutoNaming"), AUTONAMING, false);
	Add(_T("bRestrictVideoCodecs"), RESTRICTVIDEOCODECS, false);
	Add(_T("bRecordPreset"), RECORDPRESET, false);
	Add(_T("bTimestampAnnotation"), TIMESTAMPANNOTATION, false);
	Add(_T("bCaptionAnnotation"), CAPTIONANNOTATION, false);
	Add(_T("bWatermarkAnnotation"), WATERMARKANNOTATION, false);

// integers
	Add(_T("launchPlayer"), LAUNCHPLAYER, 3);
	Add(_T("MouseCaptureMode"), MOUSECAPTUREMODE, 0);
	Add(_T("capturewidth"), CAPTUREWIDTH, 320);
	Add(_T("captureheight"), CAPTUREHEIGHT, 240);
	Add(_T("timelapse"), TIMELAPSE, 5);
	Add(_T("frames_per_second"), FRAMESPERSECOND, 200);
	Add(_T("keyFramesEvery"), KEYFRAMESEVERY, 200);
	Add(_T("compquality"), COMPQUALITY, 7000);
	Add(_T("compfccHandler"), COMPFCCHANDLER, 0);
	Add(_T("CompressorStateIsFor"), COMPRESSORSTATEISFOR, 0);
	Add(_T("CompressorStateSize"), COMPRESSORSTATEISFOR, 0);
	Add(_T("g_customsel"), COMPRESSORSTATEISFOR, 0);
	Add(_T("g_cursortype"), CURSORTYPE, 0);
	Add(_T("g_highlightsize"), HIGHLIGHTSIZE, 64);
	Add(_T("g_highlightshape"), HIGHLIGHTSHAPE, 0);
	Add(_T("g_highlightcolor"), HIGHLIGHTCOLOR, RGB(255,255,125));
	Add(_T("g_highlightclickcolorleft"), HIGHLIGHTCLICKCOLORLEFT, RGB(255,0,0));
	Add(_T("g_highlightclickcolorright"), HIGHLIGHTCLICKCOLORRIGHT, RGB(0,0,255));
	Add(_T("maxpan"), MAXPAN, 20);
	Add(_T("AudioDeviceID"), AUDIODEVICEID, (int)WAVE_MAPPER);
	Add(_T("cbwfx"), CBWFX, 0);
	Add(_T("recordaudio"), RECORDAUDIO, 0);
	Add(_T("waveinselected"), WAVEINSELECTED, WAVE_FORMAT_2S16);
	Add(_T("audio_bits_per_sample"), AUDIOBITSPERSAMPLE, 16);
	Add(_T("audio_num_channels"), AUDIONUCHANNELS, 2);
	Add(_T("audio_samples_per_seconds"), AUDIOSAMPLESPERSECONDS, 22050);
	Add(_T("interleaveFactor"), INTERLEAVEFACTOR, 100);
	Add(_T("keyRecordStart"), KEYRECORDSTART, MILLISECONDS);
	Add(_T("keyRecordEnd"), KEYRECORDEND, 0);
	Add(_T("keyRecordCancel"), KEYRECORDCANCEL, 1);
	Add(_T("viewtype"), VIEWTYPE, 0);
	Add(_T("g_valueadjust"), VALUEADJUST, 0);
	Add(_T("savedir"), SAVEDIR, 0);
	Add(_T("cursordir"), CURSORDIR, 0);
	Add(_T("threadPriority"), THREADPRIORITY, THREAD_PRIORITY_NORMAL);
	Add(_T("captureleft"), CAPTURELEFT, 100);
	Add(_T("capturetop"), CAPTURETOP, 100);
	Add(_T("interleaveUnit"), INTERLEAVEUNIT, 0);
	Add(_T("tempPath_Access"), TEMPPATHACCESS, USE_WINDOWS_TEMP_DIR);
	Add(_T("specifieddir"), SPECIFIEDDIR, 0);
	Add(_T("NumDev"), NUMDEV, 0);
	Add(_T("SelectedDev"), SELECTEDDEV, 0);
	Add(_T("feedback_line"), FEEDBACKLINE, -1);
	Add(_T("feedback_line_info"), FEEDBACKLINEINFO, -1);
	Add(_T("keyRecordStartCtrl"), KEYRECORDSTARTCTRL, 0);
// TODO
	Add(_T("keyRecordEndCtrl"), KEYRECORDENDCTRL, 0);
	Add(_T("keyRecordCancelCtrl"), KEYRECORDCANCELCTRL, 0);
	Add(_T("keyRecordStartAlt"), KEYRECORDSTARTALT, 0);
	Add(_T("keyRecordEndAlt"), KEYRECORDENDALT, 0);
	Add(_T("keyRecordCancelAlt"), KEYRECORDCANCELALT, 0);
	Add(_T("keyRecordStartShift"), KEYRECORDSTARTSHIFT, 0);
	Add(_T("keyRecordEndShift"), KEYRECORDENDSHIFT, 0);
	Add(_T("keyRecordCancelShift"), KEYRECORDCANCELSHIFT, 0);
	Add(_T("keyNext"), KEYNEXT, 0);
	Add(_T("keyPrev"), KEYPREV, 0);
	Add(_T("keyShowLayout"), KEYSHOWLAYOUT, 0);
	Add(_T("keyNextCtrl"), KEYNEXTCTRL, 0);
	Add(_T("keyPrevCtrl"), KEYPREVCTRL, 0);
	Add(_T("keyShowLayoutCtrl"), KEYSHOWLAYOUTCTRL, 0);
	Add(_T("keyNextAlt"), KEYNEXTALT, 0);
	Add(_T("keyPrevAlt"), KEYPREVALT, 0);
	Add(_T("keyShowLayoutAlt"), KEYSHOWLAYOUTALT, 0);
	Add(_T("keyNextShift"), KEYNEXTSHIFT, 0);
	Add(_T("keyPrevShift"), KEYPREVSHIFT, 0);
	Add(_T("keyShowLayoutShift"), KEYSHOWLAYOUTSHIFT, 0);
	Add(_T("iShapeNameInt"), SHAPENAMEINT, 1);
	Add(_T("shapeNameLen"), SHAPENAMELEN, 0);
	Add(_T("iLayoutNameInt"), LAYOUTNAMEINT, 1);
	Add(_T("g_layoutNameLen"), LAYOUTNAMELEN, 0);
	Add(_T("iShiftType"), SHIFTTYPE, 0);
	Add(_T("iTimeShift"), TIMESHIFT, 100);
	Add(_T("iFrameShift"), FRAMESHIFT, 0);
	Add(_T("iRecordingMode"), RECORDINGMODE, ModeAVI);
	Add(_T("iPresetTime"), PRESETTIME, 60);
	Add(_T("language"), LANGUAGE, 0);
	Add(_T("timestampBackColor"), TIMESTAMPBACKCOLOR, RGB(255, 255, 255));
	Add(_T("timestampSelected"), TIMESTAMPSELECTED, 0);
	Add(_T("timestampPosition"), TIMESTAMPPOSITION, TOP_LEFT);
	Add(_T("timestampTextColor"), TIMESTAMPTEXTCOLOR, RGB(0, 0, 0));
	Add(_T("timestampTextWeight"), TIMESTAMPTEXTWEIGHT, 0);
	Add(_T("timestampTextHeight"), TIMESTAMPTEXTHEIGHT, 0);
	Add(_T("timestampTextWidth"), TIMESTAMPTEXTWIDTH, 0);
	Add(_T("captionBackColor"), CAPTIONBACKCOLOR, RGB(255, 255, 255));
	Add(_T("captionSelected"), CAPTIONSELECTED, 0);
	Add(_T("captionPosition"), CAPTIONPOSITION, TOP_LEFT);
	Add(_T("captionTextColor"), CAPTIONTEXTCOLOR, RGB(0, 0, 0));
	Add(_T("captionTextWeight"), CAPTIONTEXTWEIGHT, 0);
	Add(_T("captionTextHeight"), CAPTIONTEXTHEIGHT, 0);
	Add(_T("captionTextWidth"), CAPTIONTEXTWIDTH, 0);
	Add(_T("watermarkPosition"), WATERMARKPOSITION, TOP_LEFT);

	Add(_T("captionTextFont"), CAPTIONTEXTFONT, CString(_T("")));
	Add(_T("timestampTextFont"), TIMESTAMPTEXTFONT, CString(_T("")));
#else
	Add(_T("bFlashingRect"), FLASHINGRECT, true);
	Add(_T("bMinimizeOnStart"), MINIMIZEONSTART, false);
	Add(_T("bRecordCursor"), RECORDCURSOR, true);
	Add(_T("bHighlightCursor"), HIGHLIGHTCURSOR, false);
	Add(_T("bHighlightClick"), HIGHLIGHTCLICK, false);
	Add(_T("bAutoAdjust"), AUTOADJUST, true);
	Add(_T("bFixedCapture"), FIXEDCAPTURE, false);
	Add(_T("bCaptureTrans"), CAPTURETRANS, true);
	Add(_T("bPerformAutoSearch"), PERFORMAUTOSEARCH, true);
	Add(_T("bSupportMouseDrag"), SUPPORTMOUSEDRAG, true);
	Add(_T("bUseMCI"), USEMCI, false);
	Add(_T("bLaunchPropPrompt"), LAUNCHPROPPROMPT, false);
	Add(_T("bLaunchHTMLPlayer"), LAUNCHHTMLPLAYER, true);
	Add(_T("bDeleteAVIAfterUse"), DELETEAVIAFTERUSE, true);
	Add(_T("bAutoNaming"), AUTONAMING, false);
	Add(_T("bRestrictVideoCodecs"), RESTRICTVIDEOCODECS, false);
	Add(_T("bRecordPreset"), RECORDPRESET, false);
	Add(_T("bTimestampAnnotation"), TIMESTAMPANNOTATION, false);
	Add(_T("bCaptionAnnotation"), CAPTIONANNOTATION, false);
	Add(_T("bWatermarkAnnotation"), WATERMARKANNOTATION, false);
	Add(_T("bAudioCompression"), AUDIOCOMPRESSION, true);
	Add(_T("bInterleaveFrames"), INTERLEAVEFRAMES, true);
	Add(_T("bAutoPan"), AUTOPAN, false);

	Add(_T("iLaunchPlayer"), LAUNCHPLAYER, 3);
	Add(_T("iMouseCaptureMode"), MOUSECAPTUREMODE, 0);
	Add(_T("iCaptureWidth"), CAPTUREWIDTH, 320);
	Add(_T("iCaptureHeight"), CAPTUREHEIGHT, 240);
	Add(_T("iTimeLapse"), TIMELAPSE, 5);
	Add(_T("iFramesPerSecond"), FRAMESPERSECOND, 200);
	Add(_T("iKeyFramesEvery"), KEYFRAMESEVERY, 200);
	Add(_T("iCompQuality"), COMPQUALITY, 7000);
	Add(_T("dwCompfccHandler"), COMPFCCHANDLER, 0);
	Add(_T("dwCompressorStateIsFor"), COMPRESSORSTATEISFOR, 0);
	Add(_T("dwCompressorStateSize"), COMPRESSORSTATEISFOR, 0);
	Add(_T("iCustomSel"), COMPRESSORSTATEISFOR, 0);
	Add(_T("iCursorType"), CURSORTYPE, 0);
	Add(_T("iHighlightSize"), HIGHLIGHTSIZE, 64);
	Add(_T("iHighlightShape"), HIGHLIGHTSHAPE, 0);
	Add(_T("clrHighlightColor"), HIGHLIGHTCOLOR, RGB(255,255,125));
	Add(_T("clrHighlightClickColorLeft"), HIGHLIGHTCLICKCOLORLEFT, RGB(255,0,0));
	Add(_T("clrHighlightClickColorRight"), HIGHLIGHTCLICKCOLORRIGHT, RGB(0,0,255));
	Add(_T("iMaxPan"), MAXPAN, 20);
	Add(_T("uAudioDeviceID"), AUDIODEVICEID, (int)WAVE_MAPPER);
	Add(_T("dwCbwFX"), CBWFX, 0);
	Add(_T("iRecordAudio"), RECORDAUDIO, 0);
	Add(_T("dwWaveinSelected"), WAVEINSELECTED, WAVE_FORMAT_2S16);
	Add(_T("iAudioBitsPerSample"), AUDIOBITSPERSAMPLE, 16);
	Add(_T("iAudioNumChannels"), AUDIONUCHANNELS, 2);
	Add(_T("iAudioSamplesPerSeconds"), AUDIOSAMPLESPERSECONDS, 22050);
	Add(_T("iInterleaveFactor"), INTERLEAVEFACTOR, 100);
	Add(_T("keyRecordStart"), KEYRECORDSTART, MILLISECONDS);
	Add(_T("keyRecordEnd"), KEYRECORDEND, 0);
	Add(_T("keyRecordCancel"), KEYRECORDCANCEL, 1);
	Add(_T("iViewType"), VIEWTYPE, 0);
	Add(_T("iValueAdjust"), VALUEADJUST, 0);
	Add(_T("savedir"), SAVEDIR, 0);
	Add(_T("cursordir"), CURSORDIR, 0);
	Add(_T("iThreadPriority"), THREADPRIORITY, THREAD_PRIORITY_NORMAL);
	Add(_T("iCaptureLeft"), CAPTURELEFT, 100);
	Add(_T("iCaptureTop"), CAPTURETOP, 100);
	Add(_T("iInterleaveUnit"), INTERLEAVEUNIT, 0);
	Add(_T("iTempPathAccess"), TEMPPATHACCESS, USE_WINDOWS_TEMP_DIR);
	Add(_T("specifieddir"), SPECIFIEDDIR, 0);
	Add(_T("NumDev"), NUMDEV, 0);
	Add(_T("SelectedDev"), SELECTEDDEV, 0);
	Add(_T("iFeedbackLine"), FEEDBACKLINE, -1);
	Add(_T("feedback_line_info"), FEEDBACKLINEINFO, -1);
	Add(_T("keyRecordStartCtrl"), KEYRECORDSTARTCTRL, 0);
	Add(_T("keyRecordEndCtrl"), KEYRECORDENDCTRL, 0);
	Add(_T("keyRecordCancelCtrl"), KEYRECORDCANCELCTRL, 0);
	Add(_T("keyRecordStartAlt"), KEYRECORDSTARTALT, 0);
	Add(_T("keyRecordEndAlt"), KEYRECORDENDALT, 0);
	Add(_T("keyRecordCancelAlt"), KEYRECORDCANCELALT, 0);
	Add(_T("keyRecordStartShift"), KEYRECORDSTARTSHIFT, 0);
	Add(_T("keyRecordEndShift"), KEYRECORDENDSHIFT, 0);
	Add(_T("keyRecordCancelShift"), KEYRECORDCANCELSHIFT, 0);
	Add(_T("keyNext"), KEYNEXT, 0);
	Add(_T("keyPrev"), KEYPREV, 0);
	Add(_T("keyShowLayout"), KEYSHOWLAYOUT, 0);
	Add(_T("keyNextCtrl"), KEYNEXTCTRL, 0);
	Add(_T("keyPrevCtrl"), KEYPREVCTRL, 0);
	Add(_T("keyShowLayoutCtrl"), KEYSHOWLAYOUTCTRL, 0);
	Add(_T("keyNextAlt"), KEYNEXTALT, 0);
	Add(_T("keyPrevAlt"), KEYPREVALT, 0);
	Add(_T("keyShowLayoutAlt"), KEYSHOWLAYOUTALT, 0);
	Add(_T("keyNextShift"), KEYNEXTSHIFT, 0);
	Add(_T("keyPrevShift"), KEYPREVSHIFT, 0);
	Add(_T("keyShowLayoutShift"), KEYSHOWLAYOUTSHIFT, 0);
	Add(_T("iShapeNameInt"), SHAPENAMEINT, 1);
	Add(_T("shapeNameLen"), SHAPENAMELEN, 0);
	Add(_T("iLayoutNameInt"), LAYOUTNAMEINT, 1);
	Add(_T("g_layoutNameLen"), LAYOUTNAMELEN, 0);
	Add(_T("iShiftType"), SHIFTTYPE, 0);
	Add(_T("iTimeShift"), TIMESHIFT, 100);
	Add(_T("iFrameShift"), FRAMESHIFT, 0);
	Add(_T("iRecordingMode"), RECORDINGMODE, ModeAVI);
	Add(_T("iPresetTime"), PRESETTIME, 60);
	Add(_T("language"), LANGUAGE, 0);
	Add(_T("timestampBackColor"), TIMESTAMPBACKCOLOR, RGB(255, 255, 255));
	Add(_T("timestampSelected"), TIMESTAMPSELECTED, 0);
	Add(_T("timestampPosition"), TIMESTAMPPOSITION, TOP_LEFT);
	Add(_T("timestampTextColor"), TIMESTAMPTEXTCOLOR, RGB(0, 0, 0));
	Add(_T("timestampTextWeight"), TIMESTAMPTEXTWEIGHT, 0);
	Add(_T("timestampTextHeight"), TIMESTAMPTEXTHEIGHT, 0);
	Add(_T("timestampTextWidth"), TIMESTAMPTEXTWIDTH, 0);
	Add(_T("captionBackColor"), CAPTIONBACKCOLOR, RGB(255, 255, 255));
	Add(_T("captionSelected"), CAPTIONSELECTED, 0);
	Add(_T("captionPosition"), CAPTIONPOSITION, TOP_LEFT);
	Add(_T("captionTextColor"), CAPTIONTEXTCOLOR, RGB(0, 0, 0));
	Add(_T("captionTextWeight"), CAPTIONTEXTWEIGHT, 0);
	Add(_T("captionTextHeight"), CAPTIONTEXTHEIGHT, 0);
	Add(_T("captionTextWidth"), CAPTIONTEXTWIDTH, 0);
	Add(_T("watermarkPosition"), WATERMARKPOSITION, TOP_LEFT);

	Add(_T("captionTextFont"), CAPTIONTEXTFONT, CString(_T("")));
	Add(_T("timestampTextFont"), TIMESTAMPTEXTFONT, CString(_T("")));
#endif
}

CCamStudioSettings::~CCamStudioSettings()
{
}

bool CCamStudioSettings::Add(const CString strName, const eLegacySetting eKey, const CString strDefault)
{
	m_StrSection.Add(strName, eKey, strDefault);
	return false;
}
bool CCamStudioSettings::Add(const CString strName, const eLegacySetting eKey, const int iDefault)
{
	m_IntSection.Add(strName, eKey, iDefault);
	return false;
}
bool CCamStudioSettings::Add(const CString strName, const eLegacySetting eKey, const bool bDefault)
{
	m_BoolSection.Add(strName, eKey, bDefault);
	return false;
}
bool CCamStudioSettings::Add(const CString strName, const eLegacySetting eKey, const long lDefault)
{
	m_LongSection.Add(strName, eKey, lDefault);
	return false;
}
bool CCamStudioSettings::Add(const CString strName, const eLegacySetting eKey, const double dDefault)
{
	m_DblSection.Add(strName, eKey, dDefault);
	return false;
}
bool CCamStudioSettings::Add(const CString strName, const eLegacySetting eKey, const COLORREF clrDefault)
{
	m_ColorSection.Add(strName, eKey, clrDefault);
	return false;
}

/////////////////////////////////////////////////////////////////////////////
bool CCamStudioSettings::LoadSettings()
{
#ifdef EXPERIMENTAL_CODE
	TRACE("CCamStudioSettings::LoadSettings\n");
	TRACE("CCamStudioSettings::LoadSettings: m_StrSection\n");
	m_StrSection.Read(m_strFileName);
	TRACE("CCamStudioSettings::LoadSettings: m_IntSection\n");
	m_IntSection.Read(m_strFileName);
	TRACE("CCamStudioSettings::LoadSettings: m_BoolSection\n");
	m_BoolSection.Read(m_strFileName);
	TRACE("CCamStudioSettings::LoadSettings: m_LongSection\n");
	m_LongSection.Read(m_strFileName);
	TRACE("CCamStudioSettings::LoadSettings: m_DblSection\n");
	m_DblSection.Read(m_strFileName);
	TRACE("CCamStudioSettings::LoadSettings: m_ColorSection\n");
	m_ColorSection.Read(m_strFileName);
#endif

	return false;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
bool CCamStudioSettings::WriteSettings()
{
#ifdef EXPERIMENTAL_CODE
	TRACE("CCamStudioSettings::WriteSettings\n");
	TRACE("CCamStudioSettings::WriteSettings: m_StrSection\n");
	m_StrSection.Write(m_strFileName);
	TRACE("CCamStudioSettings::WriteSettings: m_IntSection\n");
	m_IntSection.Write(m_strFileName);
	TRACE("CCamStudioSettings::WriteSettings: m_BoolSection\n");
	m_BoolSection.Write(m_strFileName);
	TRACE("CCamStudioSettings::WriteSettings: m_LongSection\n");
	m_LongSection.Write(m_strFileName);
	TRACE("CCamStudioSettings::WriteSettings: m_DblSection\n");
	m_DblSection.Write(m_strFileName);
	TRACE("CCamStudioSettings::WriteSettings: m_ColorSection\n");
	m_ColorSection.Write(m_strFileName);
#endif

	return false;
}
