/////////////////////////////////////////////////////////////////////////////
// Profile.h
// include file for windows profile classes
/////////////////////////////////////////////////////////////////////////////
#pragma once

#include <vector>

// enumerated legacy settings
// used to identify setting string and values
enum eLegacySetting
{
	FLASHINGRECT, LAUNCHPLAYER, MINIMIZEONSTART, MOUSECAPTUREMODE, CAPTUREWIDTH
	, CAPTUREHEIGHT, TIMELAPSE, FRAMESPERSECOND, KEYFRAMESEVERY, COMPQUALITY
	, COMPFCCHANDLER, COMPRESSORSTATEISFOR, COMPRESSORSTATESIZE, RECORDCURSOR, CUSTOMSEL
	, CURSORTYPE, HIGHLIGHTCURSOR, HIGHLIGHTSIZE, HIGHLIGHTSHAPE, HIGHLIGHTCLICK
	, HIGHLIGHTCOLOR, HIGHLIGHTCLICKCOLORLEFT, HIGHLIGHTCLICKCOLORRIGHT, AUTOPAN, MAXPAN
	, AUDIODEVICEID, CBWFX, RECORDAUDIO, WAVEINSELECTED, AUDIOBITSPERSAMPLE
	, AUDIONUCHANNELS, AUDIOSAMPLESPERSECONDS, AUDIOCOMPRESSION, INTERLEAVEFRAMES
	, INTERLEAVEFACTOR, KEYRECORDSTART, KEYRECORDEND, KEYRECORDCANCEL, VIEWTYPE, AUTOADJUST
	, VALUEADJUST, SAVEDIR, CURSORDIR, THREADPRIORITY, CAPTURELEFT, CAPTURETOP, FIXEDCAPTURE
	, INTERLEAVEUNIT, TEMPPATHACCESS, CAPTURETRANS, SPECIFIEDDIR, NUMDEV, SELECTEDDEV
	, FEEDBACKLINE, FEEDBACKLINEINFO, PERFORMAUTOSEARCH, SUPPORTMOUSEDRAG, KEYRECORDSTARTCTRL
	, KEYRECORDENDCTRL, KEYRECORDCANCELCTRL, KEYRECORDSTARTALT, KEYRECORDENDALT
	, KEYRECORDCANCELALT, KEYRECORDSTARTSHIFT, KEYRECORDENDSHIFT, KEYRECORDCANCELSHIFT
	, KEYNEXT, KEYPREV, KEYSHOWLAYOUT, KEYNEXTCTRL, KEYPREVCTRL, KEYSHOWLAYOUTCTRL, KEYNEXTALT
	, KEYPREVALT, KEYSHOWLAYOUTALT, KEYNEXTSHIFT, KEYPREVSHIFT, KEYSHOWLAYOUTSHIFT, SHAPENAMEINT
	, SHAPENAMELEN, LAYOUTNAMEINT, LAYOUTNAMELEN, USEMCI, SHIFTTYPE, TIMESHIFT, FRAMESHIFT
	, LAUNCHPROPPROMPT, LAUNCHHTMLPLAYER, DELETEAVIAFTERUSE, RECORDINGMODE, AUTONAMING
	, RESTRICTVIDEOCODECS, PRESETTIME, RECORDPRESET, LANGUAGE, TIMESTAMPANNOTATION
	, TIMESTAMPBACKCOLOR, TIMESTAMPSELECTED, TIMESTAMPPOSITION, TIMESTAMPTEXTCOLOR
	, TIMESTAMPTEXTFONT, TIMESTAMPTEXTWEIGHT, TIMESTAMPTEXTHEIGHT, TIMESTAMPTEXTWIDTH
	, CAPTIONANNOTATION, CAPTIONBACKCOLOR, CAPTIONSELECTED, CAPTIONPOSITION
	, CAPTIONTEXTCOLOR, CAPTIONTEXTFONT, CAPTIONTEXTWEIGHT, CAPTIONTEXTHEIGHT
	, CAPTIONTEXTWIDTH, WATERMARKANNOTATION, WATERMARKPOSITION
	, MAX_LEGACY_SETTING
};

/////////////////////////////////////////////////////////////////////////////
// class CKeyGroup
// template class to hold section type names and values
// i.e. all integers in a section
// Add - add a setting name, key, and value 
// Read - red the section group values
// Write - write the section values
// Individual reads and writes are done with private sepcialization functions
// TODO: add group delete
/////////////////////////////////////////////////////////////////////////////
template <typename K, typename V>
class CKeyGroup
{
public:

	CKeyGroup()
	{
	}
	virtual ~CKeyGroup() {}

	// add item to section group
	void Add(const CString strName, K key, V value)	// add a key and default value
	{
		m_vLookup.push_back(std::pair <CString, K>(strName, key));
		m_vKeylist.push_back(element(key, value));
	}
	// read all items from profile
	bool Read(const CString strFileName, const CString strSection)
	{
		bool bResult = (0 < m_vKeylist.size());
		if (!bResult)
			return bResult;

		for (std::vector <element>::iterator iter = m_vKeylist.begin(); iter != m_vKeylist.end(); ++iter)
		{
			iter->second = Read(strFileName, strSection, *iter);
		}
		return bResult;
	}
	// write all items to profile
	bool Write(const CString strFileName, const CString strSection)
	{
		bool bResult = (0 < m_vKeylist.size());
		if (!bResult)
			return bResult;

		for (std::vector <element>::iterator iter = m_vKeylist.begin(); bResult && (iter != m_vKeylist.end()); ++iter)
		{
			bResult = bResult && Write(strFileName, strSection, *iter);
		}
		return bResult;
	}

private:
	typedef std::pair<K, V> element;
	std::vector <std::pair<K, V>> m_vKeylist;		// list key-value pairs
	std::vector<std::pair <CString, K>> m_vLookup;	// list of name-key pairs

	// brute force lookup of key by name
	typename std::vector <std::pair <CString, K>>::iterator find(const K key)
	{
		for (std::vector <std::pair <CString, K>>::iterator iter = m_vLookup.begin(); (iter != m_vLookup.end()); ++iter)
		{
			if (key == iter->second)
			{
				return iter;
			}
		}
		return m_vLookup.end();
	}

	// specialization read for key-string pair
	CString Read(const CString strFileName, const CString strSection, std::pair<K, CString> entry)
	{
		// lookup the string-name pair
		std::vector <std::pair <CString, K>>::iterator iter = find(entry.first);
		if (iter == m_vLookup.end())
		{
			// TODO: report error
			entry.second;
		}
		TCHAR szValue[BUFSIZ];
		DWORD dwLen = ::GetPrivateProfileString(strSection, iter->first, entry.second, szValue, BUFSIZ, strFileName);
		entry.second = (dwLen) ? szValue : entry.second;
		TRACE("CKeyGroup:Read: %s = %s\n", iter->first, entry.second);
		return entry.second;
	}
	// specialization read for key-int pair
	int Read(const CString strFileName, const CString strSection, std::pair<K, int> entry)
	{
		// lookup the string-name pair
		std::vector <std::pair <CString, K>>::iterator iter = find(entry.first);
		if (iter == m_vLookup.end())
		{
			// TODO: report error
			entry.second;
		}
		entry.second = ::GetPrivateProfileInt(strSection, iter->first, entry.second, strFileName);
		TRACE("CKeyGroup:Read: %s = %d\n", iter->first, entry.second);
		return entry.second;
	}
	// specialization read for key-bool pair
	bool Read(const CString strFileName, const CString strSection, std::pair<K, bool> entry)
	{		
		entry.second = (1 == Read(strFileName, strSection, std::pair<K, int>(entry.first, entry.second)))
			? true : false;
		//TRACE("CKeyGroup:Read: %s = %s\n", entry.first, entry.second ? "true" : "false");
		return entry.second;
	}
	// specialization read for key-long pair
	// read value as string
	long Read(const CString strFileName, const CString strSection, std::pair<K, long> entry)
	{
		CString strValue(_T("X"));
		strValue = Read(strFileName, strSection, std::pair<K, CString>(entry.first, strValue));
		entry.second = (strValue == _T("X"))
			? entry.second
			: ::atol(strValue);
		//TRACE("CKeyGroup:Read: %s = %ld\n", entry.first, entry.second);
		return entry.second;
	}
	// specialization read for key-double pair
	double Read(const CString strFileName, const CString strSection, std::pair<K, double> entry)
	{
		CString strValue(_T("X"));
		strValue = Read(strFileName, strSection, std::pair<K, CString>(entry.first, strValue));
		entry.second = (strValue == _T("X"))
			? entry.second
			: ::_tstof(strValue);
		//TRACE("CKeyGroup:Read: %s = %f\n", entry.first, entry.second);
		return entry.second;
	}
	// specialization read for key-COLORREF pair
	// note the item is in 3 entries
	COLORREF Read(const CString strFileName, const CString strSection, std::pair<K, COLORREF> entry)
	{
		// lookup the string-name pair
		std::vector <std::pair <CString, K>>::iterator iter = find(entry.first);
		if (iter == m_vLookup.end())
		{
			// TODO: report error
			entry.second;
		}
		CString strColorKey;
		strColorKey.Format("%sR", iter->first);
		int iRed = ::GetPrivateProfileInt(strSection, strColorKey, GetRValue(entry.second), strFileName);
		strColorKey.Format("%sG", iter->first);
		int iGreen = ::GetPrivateProfileInt(strSection, strColorKey, GetGValue(entry.second), strFileName);
		strColorKey.Format("%sB", iter->first);
		int iBlue = ::GetPrivateProfileInt(strSection, strColorKey, GetBValue(entry.second), strFileName);
		entry.second = RGB(iRed, iGreen, iBlue);
		//TRACE("CKeyGroup:Read: %s = COLOR : %ld\n", entry.first, entry.second);
		return entry.second;
	}
	
	// specialization write for key-string pair
	bool Write(const CString strFileName, const CString strSection, std::pair<K, CString> entry)
	{
		std::vector <std::pair <CString, K>>::iterator iter = find(entry.first);
		if (iter == m_vLookup.end())
		{
			// TODO: report error
			return false;
		}
		return (entry.second.IsEmpty())
			? ::WritePrivateProfileString(strSection, iter->first, 0, strFileName)
				? true : false
			: ::WritePrivateProfileString(strSection, iter->first, entry.second, strFileName)
				? true : false;
	}
	// specialization write for key-int pair
	bool Write(const CString strFileName, const CString strSection, std::pair<K, int> entry)
	{
		CString strValue;
		strValue.Format("%d", entry.second);
		return Write(strFileName, strSection, std::pair<K, CString>(entry.first, strValue));
	}
	// specialization write for key-bool pair
	bool Write(const CString strFileName, const CString strSection, std::pair<K, bool> entry)
	{
		CString strValue;
		strValue.Format("%d", entry.second);
		return Write(strFileName, strSection, std::pair<K, CString>(entry.first, strValue));
	}
	// specialization write for key-long pair
	bool Write(const CString strFileName, const CString strSection, std::pair<K, long> entry)
	{
		CString strValue;
		strValue.Format("%ld", entry.second);
		return Write(strFileName, strSection, std::pair<K, CString>(entry.first, strValue));
	}
	// specialization write for key-double pair
	bool Write(const CString strFileName, const CString strSection, std::pair<K, double> entry)
	{
		CString strValue;
		strValue.Format("%f", entry.second);
		return Write(strFileName, strSection, std::pair<K, CString>(entry.first, strValue));
	}
	// specialization write for key-COLORREF pair
	// note the item is in 3 entries
	bool Write(const CString strFileName, const CString strSection, std::pair<K, COLORREF> entry)
	{
		// lookup the string-name pair
		std::vector <std::pair <CString, K>>::iterator iter = find(entry.first);
		if (iter == m_vLookup.end())
		{
			// TODO: report error
			return false;
		}
		CString strValue;
		strValue.Format("%d", GetRValue(entry.second));
		CString strColorKey;
		strColorKey.Format("%sR", iter->first);
		bool bResult = ::WritePrivateProfileString(strSection, strColorKey, strValue, strFileName);

		strValue.Format("%d", GetGValue(entry.second));
		strColorKey.Format("%sG", iter->first);
		bResult = bResult && ::WritePrivateProfileString(strSection, strColorKey, strValue, strFileName);
		strValue.Format("%d", GetBValue(entry.second));
		strColorKey.Format("%sB", iter->first);
		bResult = bResult && ::WritePrivateProfileString(strSection, strColorKey, strValue, strFileName);
		return bResult;
	}
};

/////////////////////////////////////////////////////////////////////////////
// class CProfileSection
// wrapper for group + section name 
/////////////////////////////////////////////////////////////////////////////
template <typename K, typename V>
class CProfileSection
{
	CProfileSection();
public:
	CProfileSection(CString strSectionName)
		: m_strSectionName(strSectionName)
	{
	}
	virtual ~CProfileSection() {}

	void Add(const CString strName, K key, V value)
	{
		m_KeyGroup.Add(strName, key, value);
	}
	bool Read(const CString strFileName)
	{
		return m_KeyGroup.Read(strFileName, m_strSectionName);
	}
	bool Write(const CString strFileName)
	{
		return m_KeyGroup.Write(strFileName, m_strSectionName);
	}

protected:
	CString m_strSectionName;
	CKeyGroup <K, V> m_KeyGroup;
};

/////////////////////////////////////////////////////////////////////////////
// class CProfile
// base profile class = holds filename
/////////////////////////////////////////////////////////////////////////////
class CProfile
{
	CProfile();
public:
	CProfile(CString strFileName);
	virtual ~CProfile();

protected:
	CString m_strFileName;
};

/////////////////////////////////////////////////////////////////////////////
// class CCamStudioSettings
// Profile class
// One section (currently) with all types
// overloaded apis to add name-key-value tuples for all types.
/////////////////////////////////////////////////////////////////////////////
class CCamStudioSettings : public CProfile
{
public:
	CCamStudioSettings(CString strFileName);
	virtual ~CCamStudioSettings();
public:
	virtual bool LoadSettings();
	virtual bool WriteSettings();

	bool Add(const CString strName, const eLegacySetting eKey, const CString strDefault);
	bool Add(const CString strName, const eLegacySetting eKey, const int iDefault);
	bool Add(const CString strName, const eLegacySetting eKey, const bool bDefault);
	bool Add(const CString strName, const eLegacySetting eKey, const long lDefault);
	bool Add(const CString strName, const eLegacySetting eKey, const double dDefault);
	bool Add(const CString strName, const eLegacySetting eKey, const COLORREF clrDefault);
private:
	static char const * const LEGACY_SECTION;	// section name

	CProfileSection <eLegacySetting, CString>	m_StrSection;
	CProfileSection <eLegacySetting, int>		m_IntSection;
	CProfileSection <eLegacySetting, bool>		m_BoolSection;
	CProfileSection <eLegacySetting, long>		m_LongSection;
	CProfileSection <eLegacySetting, double>	m_DblSection;
	CProfileSection <eLegacySetting, COLORREF>	m_ColorSection;
};

// legacy settings values.

#define MILLISECONDS	0
#define FRAMES			1
#define NUMSTREAMS		2
#define ModeAVI			0
#define ModeFlash		1

extern bool bAudioCompression;
extern bool bInterleaveFrames;
extern bool bFlashingRect;
extern bool bMinimizeOnStart;
extern bool bRecordCursor;
extern bool bHighlightCursor;
extern bool bHighlightClick;
extern bool bAutoPan;
extern bool bAutoAdjust;
extern bool bFixedCapture;
extern bool bCaptureTrans;
extern bool bPerformAutoSearch;
extern bool bSupportMouseDrag;
extern bool bUseMCI;
extern bool bLaunchPropPrompt;
extern bool bLaunchHTMLPlayer;
extern bool bDeleteAVIAfterUse;
extern bool bAutoNaming;
extern bool bRestrictVideoCodecs;
extern bool bRecordPreset;
extern bool bTimestampAnnotation;
extern bool bCaptionAnnotation;
extern bool bWatermarkAnnotation;

extern int iLaunchPlayer;
extern int iMouseCaptureMode;
extern int iCaptureWidth;
extern int iCaptureHeight;
extern int iTimeLapse;
extern int iFramesPerSecond;
extern int iKeyFramesEvery;
extern int iCompQuality;
extern DWORD dwCompfccHandler;
extern DWORD dwCompressorStateIsFor;
extern DWORD dwCompressorStateSize;
extern int iCustomSel;
extern int iCursorType;
extern int iHighlightSize;
extern int iHighlightShape;
extern COLORREF clrHighlightColor;
extern COLORREF clrHighlightClickColorLeft;
extern COLORREF clrHighlightClickColorRight;
extern int iMaxPan;
extern UINT uAudioDeviceID;
extern DWORD dwCbwFX;
extern int iRecordAudio;
extern DWORD dwWaveinSelected;
extern int iAudioBitsPerSample;
extern int iAudioNumChannels;
extern int iAudioSamplesPerSeconds;
extern int iInterleaveFactor;
extern int iInterleaveUnit;
extern int iViewType;
extern int iValueAdjust;
extern int iSaveLen;
extern int iCursorLen;
extern int iThreadPriority;
extern int iCaptureLeft;
extern int iCaptureTop;
extern int iTempPathAccess;
extern int iSpecifiedDirLength;
extern int iNumberOfMixerDevices;
extern int iSelectedMixer;
extern int iFeedbackLine;
extern int iFeedbackLineInfo;
extern int iShapeNameInt;
extern int iShapeNameLen;	// string length
extern int iLayoutNameInt;
extern int iLayoutNameLen;	// string length
extern int iShiftType;
extern int iTimeShift;
extern int iFrameShift;
extern int iRecordingMode;
extern int iPresetTime;
extern int iLanguageID;

struct TextAttributes;	// forward declaration
struct ImageAttributes;	// forward declaration

extern TextAttributes taCaption;
extern TextAttributes taTimestamp;
extern ImageAttributes iaWatermark;
