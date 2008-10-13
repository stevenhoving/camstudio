/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
#pragma once

#include <vector>

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
class CProfileKey
{
	CProfileKey();
public:
	CProfileKey(CString strKeyName)
		: m_strKeyName(strKeyName)
	{
	}
	virtual ~CProfileKey() {}

	CString Key() const {return m_strKeyName;}
private:
	CString m_strKeyName;
};

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
class CProfileSection
{
	CProfileSection();
public:
	CProfileSection(CString strSectionName)
		: m_strSectionName(strSectionName)
	{
	}
	virtual ~CProfileSection() {}
	void Add(const CProfileKey& key)
	{
		m_vSection.push_back(key);
	}

private:
	CString m_strSectionName;
	std::vector <CProfileKey> m_vSection;
};

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
class CProfile
{
	CProfile();
public:
	CProfile(CString strFileName);
	virtual ~CProfile();

	template <typename T> T Read(const CString strSection, const CString strKey, T& Value, T DefValue)
	{
		return DefValue;
	}
	template <typename T> bool Write(const CString strSection, const CString strKey, const T& Value)
	{
		return false;
	}
	void Add(const CProfileSection& section)
	{
		m_vProfile.push_back(section);
	}

private:
	CString m_strFileName;
	std::vector <CProfileSection> m_vProfile;
};

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
class CCamStudioSettings : public CProfile
{
public:
	CCamStudioSettings(CString strFileName = ::AfxGetApp()->m_pszProfileName);
	virtual ~CCamStudioSettings();
public:
	virtual bool LoadSettings();
	virtual bool WriteSettings();

private:
	bool m_bflashingRect;
	int m_ilaunchPlayer;
	int m_iminimizeOnStart;
	int m_iMouseCaptureMode;
	int m_icapturewidth;
	int m_icaptureheight;
	int m_itimelapse;
	int m_iframes_per_second;
	int m_ikeyFramesEvery;
	int m_icompquality;
	DWORD m_dwcompfccHandler;
	DWORD m_dwCompressorStateIsFor;
	DWORD m_dwCompressorStateSize;
	int m_irecordcursor;
	int m_icustomsel;
	int m_icursortype;
	int m_ihighlightcursor;
	int m_ihighlightsize;
	int m_ihighlightshape;
	int m_ihighlightclick;
	COLORREF m_clrhighlightcolor;
	COLORREF m_clrhighlightclickcolorleft;
	COLORREF m_clrhighlightclickcolorright;
	int m_iautopan;
	int m_imaxpan;
	UINT m_uAudioDeviceID;
	int m_icbwfx;
	int m_irecordaudio;
	int m_iwaveinselected;
	int m_iaudio_bits_per_sample;
	int m_iaudio_num_channels;
	int m_iaudio_samples_per_seconds;
	int m_ibAudioCompression;
	int m_iinterleaveFrames;
	int m_iinterleaveFactor;
	int m_ikeyRecordStart;
	int m_ikeyRecordEnd;
	int m_ikeyRecordCancel;
	int m_iviewtype;
	int m_iautoadjust;
	int m_ivalueadjust;
	int m_isavelen;
	int m_icursorlen;
	int m_ithreadPriority;
	int m_icaptureleft;
	int m_icapturetop;
	int m_ifixedcapture;
	int m_iinterleaveUnit;
	int m_itempPath_Access;
	int m_icaptureTrans;
	int m_ispecifiedDirLength;
	int m_iNumberOfMixerDevices;
	int m_iSelectedMixer;
	int m_ifeedback_line;
	int m_ifeedback_lineInfo;
	int m_iperformAutoSearch;
	int m_isupportMouseDrag;
	int m_ikeyRecordStartCtrl;
	int m_ikeyRecordEndCtrl;
	int m_ikeyRecordCancelCtrl;
	int m_ikeyRecordStartAlt;
	int m_ikeyRecordEndAlt;
	int m_ikeyRecordCancelAlt;
	int m_ikeyRecordStartShift;
	int m_ikeyRecordEndShift;
	int m_ikeyRecordCancelShift;
	int m_ikeyNext;
	int m_ikeyPrev;
	int m_ikeyShowLayout;
	int m_ikeyNextCtrl;
	int m_ikeyPrevCtrl;
	int m_ikeyShowLayoutCtrl;
	int m_ikeyNextAlt;
	int m_ikeyPrevAlt;
	int m_ikeyShowLayoutAlt;
	int m_ikeyNextShift;
	int m_ikeyPrevShift;
	int m_ikeyShowLayoutShift;
	int m_ishapeNameInt;
	int m_ishapeNameLen;
	int m_ilayoutNameInt;
	int m_ilayoutNameLen;
	int m_iuseMCI;
	int m_ishiftType;
	int m_itimeshift;
	int m_iframeshift;
	int m_ilaunchPropPrompt;
	int m_ilaunchHTMLPlayer;
	int m_ideleteAVIAfterUse;
	int m_iRecordingMode;
	int m_iautonaming;
	int m_irestrictVideoCodecs;
	int m_ipresettime;
	int m_irecordpreset;
	int m_ilanguageID;
	int m_itimestampAnnotation;
	//TextAttributes timestamp
	int m_itimestamp_backgroundColor;
	int m_itimestamp_isFontSelected;
	int m_itimestamp_position;
	int m_itimestamp_textColor;
	int m_itimestamp_logfont_lfWeight;
	int m_itimestamp_logfont_lfHeight;
	int m_itimestamp_logfont_lfWidth;
	int m_icaptionAnnotation;
	int m_icaption_backgroundColor;
	int m_icaption_isFontSelected;
	int m_icaption_position;
	int m_icaption_textColor;
	int m_icaption_logfont_lfWeight;
	int m_icaption_logfont_lfHeight;
	int m_icaption_logfont_lfWidth;
	int m_iwatermarkAnnotation;
	int m_iwatermark_position;
	CString m_strcaption_logfont_lfFaceName;
	CString m_strtimestamp_logfont_lfFaceName;
	CString m_strcaption_text;
};

#define MILLISECONDS	0
#define FRAMES			1
#define NUMSTREAMS		2

extern int flashingRect;
extern int launchPlayer;
extern int minimizeOnStart;
extern int MouseCaptureMode;
extern int capturewidth;
extern int captureheight;
extern int timelapse;
extern int frames_per_second;
extern int keyFramesEvery;
extern int compquality;
extern DWORD compfccHandler;
extern DWORD CompressorStateIsFor;
extern DWORD CompressorStateSize;
extern int g_recordcursor;
extern int g_customsel;
extern int g_cursortype;
extern int g_highlightcursor;
extern int g_highlightsize;
extern int g_highlightshape;
extern int g_highlightclick;
extern COLORREF g_highlightcolor;
extern COLORREF g_highlightclickcolorleft;
extern COLORREF g_highlightclickcolorright;
extern int autopan;
extern int maxpan;
extern UINT AudioDeviceID;
extern DWORD cbwfx;
extern int recordaudio;
extern DWORD waveinselected;
extern int audio_bits_per_sample;
extern int audio_num_channels;
extern int audio_samples_per_seconds;
extern BOOL bAudioCompression;
extern BOOL interleaveFrames;
extern int interleaveFactor;
extern int interleaveUnit;
extern int viewtype;
extern int g_autoadjust;
extern int g_valueadjust;
extern int savelen;
extern int cursorlen;
extern int threadPriority;
extern int captureleft;
extern int capturetop;
extern int fixedcapture;
extern int tempPath_Access;
extern int captureTrans;
extern int specifiedDirLength;
extern int NumberOfMixerDevices;
extern int SelectedMixer;
extern int feedback_line;
extern int feedback_lineInfo;
extern int performAutoSearch;
