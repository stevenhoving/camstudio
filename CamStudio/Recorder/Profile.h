// Profile.h
// include file for windows profile (*ini file) classes
/////////////////////////////////////////////////////////////////////////////
#ifndef PROFILE_H	// because pragma once is compiler specific
#define PROFILE_H
#pragma once

#include <vector>

#include "TextAttributes.h"
#include "ImageAttributes.h"

enum eLegacySettings
{
	FLASHINGRECT, LAUNCHPLAYER, MINIMIZEONSTART, MOUSECAPTUREMODE, CAPTUREWIDTH,
	CAPTUREHEIGHT, TIMELAPSE, FRAMES_PER_SECOND, KEYFRAMESEVERY, COMPQUALITY,
	COMPFCCHANDLER, COMPRESSORSTATEISFOR, COMPRESSORSTATESIZE, G_RECORDCURSOR,
	G_CUSTOMSEL, G_CURSORTYPE, G_HIGHLIGHTCURSOR, G_HIGHLIGHTSIZE, G_HIGHLIGHTSHAPE,
	G_HIGHLIGHTCLICK, G_HIGHLIGHTCOLORR, G_HIGHLIGHTCOLORG, G_HIGHLIGHTCOLORB,
	G_HIGHLIGHTCLICKCOLORLEFTR, G_HIGHLIGHTCLICKCOLORLEFTG, G_HIGHLIGHTCLICKCOLORLEFTB,
	G_HIGHLIGHTCLICKCOLORRIGHTR, G_HIGHLIGHTCLICKCOLORRIGHTG, G_HIGHLIGHTCLICKCOLORRIGHTB,
	AUTOPAN, MAXPAN, AUDIODEVICEID, CBWFX, RECORDAUDIO, WAVEINSELECTED, AUDIO_BITS_PER_SAMPLE,
	AUDIO_NUM_CHANNELS, AUDIO_SAMPLES_PER_SECONDS, BAUDIOCOMPRESSION, INTERLEAVEFRAMES,
	INTERLEAVEFACTOR, KEYRECORDSTART, KEYRECORDEND, KEYRECORDCANCEL, VIEWTYPE, G_AUTOADJUST,
	G_VALUEADJUST, SAVEDIR, CURSORDIR, THREADPRIORITY, CAPTURELEFT, CAPTURETOP, FIXEDCAPTURE,
	INTERLEAVEUNIT, TEMPPATH_ACCESS, CAPTURETRANS, SPECIFIEDDIR, NUMDEV, SELECTEDDEV,
	FEEDBACK_LINE, FEEDBACK_LINE_INFO, PERFORMAUTOSEARCH, SUPPORTMOUSEDRAG, KEYRECORDSTARTCTRL,
	KEYRECORDENDCTRL, KEYRECORDCANCELCTRL, KEYRECORDSTARTALT, KEYRECORDENDALT, KEYRECORDCANCELALT,
	KEYRECORDSTARTSHIFT, KEYRECORDENDSHIFT, KEYRECORDCANCELSHIFT, KEYNEXT, KEYPREV, KEYSHOWLAYOUT,
	KEYNEXTCTRL, KEYPREVCTRL, KEYSHOWLAYOUTCTRL, KEYNEXTALT, KEYPREVALT, KEYSHOWLAYOUTALT,
	KEYNEXTSHIFT, KEYPREVSHIFT, KEYSHOWLAYOUTSHIFT, SHAPENAMEINT, SHAPENAMELEN, LAYOUTNAMEINT,
	G_LAYOUTNAMELEN, USEMCI, SHIFTTYPE, TIMESHIFT, FRAMESHIFT, LAUNCHPROPPROMPT, LAUNCHHTMLPLAYER,
	DELETEAVIAFTERUSE, RECORDINGMODE, AUTONAMING, RESTRICTVIDEOCODECS, PRESETTIME, RECORDPRESET,
	LANGUAGE, TIMESTAMPANNOTATION, TIMESTAMPBACKCOLOR, TIMESTAMPSELECTED, TIMESTAMPPOSITION,
	TIMESTAMPTEXTCOLOR, TIMESTAMPTEXTFONT, TIMESTAMPTEXTWEIGHT, TIMESTAMPTEXTHEIGHT,
	TIMESTAMPTEXTWIDTH, CAPTIONANNOTATION, CAPTIONBACKCOLOR, CAPTIONSELECTED, CAPTIONPOSITION,
	CAPTIONTEXTCOLOR, CAPTIONTEXTFONT, CAPTIONTEXTWEIGHT, CAPTIONTEXTHEIGHT, CAPTIONTEXTWIDTH,
	WATERMARKANNOTATION,
	MAX_LEGACT_SETTING
};

// CGroupItem
// template class for section items
/////////////////////////////////////////////////////////////////////////////
template <typename V>
class CGroupItem
{
	CGroupItem(); // not implemented
public:
	CGroupItem(int iID, CString strName, V Value)
	{
		m_vIDValue.push_back(IDValuePair(iID, ValuePair(strName, Value)));
	}
	virtual ~CGroupItem() {}

	bool Read(const CString strFile, const CString strSection)
	{
		// iterate over the group items
		for (IDVALUE_ITER iter = m_vIDValue.begin(); iter != m_vIDValue.end(); ++iter)
		{
			iter->second.second = Read(strFile, strSection, iter->second.first, iter->second.second);
		}
		return true;
	}
	bool Write(const CString strFile, const CString strSection)
	{
		bool bResult = true;
		for (IDVALUE_ITER iter = m_vIDValue.begin(); iter != m_vIDValue.end(); ++iter)
		{
			bResult = bResult && Write(strFile, strSection, iter->second.first, iter->second.second);
		}
		return bResult;
	}

protected:
	typename typedef std::pair <CString, V> ValuePair;
	typename typedef std::pair <int, ValuePair> IDValuePair;
	typename typedef std::vector <IDValuePair>::iterator IDVALUE_ITER;
	
	std::vector <IDValuePair> m_vIDValue;

	CString Read(CString strFilename, CString strSection, CString strKeyName, CString DefValue)
	{
		TRACE("Read File(CString): %s,\nSection: %s\nKey: %s\nValue: %s\n", strFilename, strSection, strKeyName, DefValue);
		const int BUFSIZE = 260;
		TCHAR szBuf[BUFSIZE];
		DWORD dwLen = ::GetPrivateProfileString(strSection, strKeyName, DefValue, szBuf, BUFSIZE, strFilename);
		return (dwLen) ? szBuf : "";
	}
	int Read(CString strFilename, CString strSection, CString strKeyName, int DefValue)
	{
		TRACE("Read File(int): %s\nSection: %s\nKey: %s\nValue: %d\n", strFilename, strSection, strKeyName, DefValue);
		return ::GetPrivateProfileInt(strSection, strKeyName, DefValue, strFilename);
	}
	bool Read(CString strFilename, CString strSection, CString strKeyName, bool DefValue)
	{
		TRACE("Read File(bool): %s\nSection: %s\nKey: %s\nValue: %d\n", strFilename, strSection, strKeyName, DefValue);
		return ::GetPrivateProfileInt(strSection, strKeyName, DefValue, strFilename)
			? true : false;
	}
	long Read(CString strFilename, CString strSection, CString strKeyName, long DefValue)
	{
		TRACE("Read File(long): %s,\nSection: %s\nKey: %s\nValue: %ld\n", strFilename, strSection, strKeyName, DefValue);
		const int BUFSIZE = 128;
		TCHAR szBuf[BUFSIZE];
		DWORD dwLen = ::GetPrivateProfileString(strSection, strKeyName, _T(""), szBuf, BUFSIZE, strFilename);
		return (dwLen) ? ::_tstol(szBuf) : DefValue;
	}

	bool Write(CString strFilename, CString strSection, CString strKeyName, CString Value)
	{
		TRACE("Write File(CString): %s\nSection: %s\nKey: %s\nValue: %s\n", strFilename, strSection, strKeyName, Value);
		return ::WritePrivateProfileString(strSection, strKeyName, Value, strFilename)
			? true : false;
	}
	bool Write(CString strFilename, CString strSection, CString strKeyName, int Value)
	{
		TRACE("Write File(int): %s\nSection: %s\nKey: %s\nValue: %d\n", strFilename, strSection, strKeyName, Value);
		CString strValue;
		strValue.Format("%d", Value);
		return ::WritePrivateProfileString(strSection, strKeyName, strValue, strFilename)
			? true : false;
	}
	bool Write(CString strFilename, CString strSection, CString strKeyName, bool Value)
	{
		TRACE("Write File(bool): %s\nSection: %s\nKey: %s\nValue: %d\n", strFilename, strSection, strKeyName, Value);
		CString strValue;
		strValue.Format("%d", Value ? 1 : 0);
		return ::WritePrivateProfileString(strSection, strKeyName, strValue, strFilename)
			? true : false;
	}
	bool Write(CString strFilename, CString strSection, CString strKeyName, long Value)
	{
		TRACE("Write File(bool): %s\nSection: %s\nKey: %s\nValue: %d\n", strFilename, strSection, strKeyName, Value);
		CString strValue;
		strValue.Format("%ld", Value);
		return ::WritePrivateProfileString(strSection, strKeyName, strValue, strFilename)
			? true : false;
	}
};

// CSectionGroup
// template class for section items of same type
/////////////////////////////////////////////////////////////////////////////
template <typename V>
class CSectionGroup
{
public:
	CSectionGroup() {}
	virtual ~CSectionGroup() {}
	bool Add(const int iID, const CString strName, const V Value)
	{
		m_vValue.push_back(CGroupItem<V>(iID, strName, Value));
		return true;
	}
	bool Read(const CString strFile, const CString strSection)
	{
		for (GRPITEM_ITER iter = m_vValue.begin(); iter != m_vValue.end(); ++iter)
		{
			iter->Read(strFile, strSection);
		}
		return true;
	}
	bool Write(const CString strFile, const CString strSection)
	{
		for (GRPITEM_ITER iter = m_vValue.begin(); iter != m_vValue.end(); ++iter)
		{
			iter->Write(strFile, strSection);
		}
		return true;
	}
protected:
	typename typedef std::vector <CGroupItem<V>>::iterator GRPITEM_ITER;
	std::vector <CGroupItem<V>> m_vValue;
};

// class CProfileSection
// CProfileSection encapsulates a profile section behavior
/////////////////////////////////////////////////////////////////////////////
class CProfileSection
{
	CProfileSection();	// not implemented
public:
	CProfileSection(const CString strSectionName);
	virtual ~CProfileSection();

	// add item to specific section group

	bool Add(const int iID, const CString strName, const CString Value)
	{
		return m_grpStrings.Add(iID, strName, Value);
	}
	bool Add(const int iID, const CString strName, const int Value)
	{
		return m_grpIntegers.Add(iID, strName, Value);
	}
	bool Add(const int iID, const CString strName, const bool Value)
	{
		return m_grpBools.Add(iID, strName, Value);
	}
	bool Add(const int iID, const CString strName, const long Value)
	{
		return m_grpLongs.Add(iID, strName, Value);
	}

	// read items from section groups
	bool Read(const CString strFile)
	{
		bool bResult = m_grpStrings.Read(strFile, m_strSectionName);
		bResult = bResult && m_grpIntegers.Read(strFile, m_strSectionName);
		bResult = bResult && m_grpBools.Read(strFile, m_strSectionName);
		bResult = bResult && m_grpLongs.Read(strFile, m_strSectionName);
		return bResult;
	}
	// write items from section groups
	bool Write(const CString strFile)
	{
		bool bResult = m_grpStrings.Write(strFile, m_strSectionName);
		bResult = bResult && m_grpIntegers.Write(strFile, m_strSectionName);
		bResult = bResult && m_grpBools.Write(strFile, m_strSectionName);
		bResult = bResult && m_grpLongs.Write(strFile, m_strSectionName);
		return bResult;
	}
protected:
	CString m_strSectionName;
	
	// supported group types
	CSectionGroup<CString>			m_grpStrings;
	CSectionGroup<int>				m_grpIntegers;
	CSectionGroup<bool>				m_grpBools;
	CSectionGroup<long>				m_grpLongs;
	// TODO: activate these types
	//CSectionGroup<double>			m_grpDoubles;
	//CSectionGroup<COLORREF>			m_grpColorRefs;
	//CSectionGroup<TextAttributes>	m_taTextAttribs;
	//CSectionGroup<ImageAttributes>	m_iaImageAttribs;
};

// class CProfile
// The CProfile encapsulates the reading and writing of an application
// settings file (profile) on Windows 
/////////////////////////////////////////////////////////////////////////////
class CProfile
{
	CProfile();	// not implemented
public:
	CProfile(const CString strFileName);
	virtual ~CProfile();
protected:
	CString m_strFileName;
	CProfileSection m_Section;
public:
	// add item to section
	bool Add(const int iID, const CString strName, const CString Value);
	bool Add(const int iID, const CString strName, const int Value);
	bool Add(const int iID, const CString strName, const bool Value);
	bool Add(const int iID, const CString strName, const long Value);
	bool Read()		{return m_Section.Read(m_strFileName);}
	bool Write()	{return m_Section.Write(m_strFileName);}
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

extern TextAttributes taCaption;
extern TextAttributes taTimestamp;
extern ImageAttributes iaWatermark;

#endif	// PROFILE_H