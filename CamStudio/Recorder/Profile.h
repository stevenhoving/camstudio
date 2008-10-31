// Profile.h
// include file for windows profile (*ini file) classes
//
// NOTA BENE: To a degree, modifications to a profile are messy. The profile
// classes help standardize the process of adding new values amd simplify
// accessing values. The classes support a number of standard types (int,
// bool, long, double, CString), composite types (LOGFONT, TextAttributes,
// ImageAttributes), and alias types (LANGID, COLORREF). 
//
// The CProfile class defines the interface to a *.ini file. Entries in the
// files are composed of string-value pairs (ValuePair) which are associated
// with a key ID (IDValuePair). The IDValuePair items are collected by ID
// in a CGroupType. The CGroupType are then collected into a CSectionGroup 
// for each type the section supports. CProfileSection then collectes all the
// CSectionGroup for that section and finally, the CProfileSections are
// collected in the CProfile class.
//
// Adding a profile entry for an existing type
// 1.	Define a new eProfileID value for the entry and add it to the end of
//		enumerated values for the eProfileID, leaving MAX_PROFILE_ID as the
//		last value. The value must be unique or the compiler will complain. 
// 2.	Initialize the new entry in CProfile::InitSections by calling Add
//		with the eProfileID, the entry string, and the default value. You
//		can add the entry to a specific section or default to the applciation
//		section. The default value type will define the group within the
//		section the entry will go in.  
//
// Adding a new type
//
/////////////////////////////////////////////////////////////////////////////
#ifndef PROFILE_H	// because pragma once is compiler specific
#define PROFILE_H
#pragma once

#include <vector>

#include "TextAttributes.h"
#include "ImageAttributes.h"
#include "CamCursor.h"

enum eLegacySettings
{
	// Application
	LANGUAGE
	, FLASHINGRECT
	, MINIMIZEONSTART
	, LAUNCHPLAYER
	, LAUNCHPROPPROMPT
	, LAUNCHHTMLPLAYER
	, DELETEAVIAFTERUSE
	, TEMPPATH_ACCESS
	, THREADPRIORITY
	, VIEWTYPE

	, FIXEDCAPTURE
	, CAPTURETOP
	, CAPTURELEFT
	, CAPTUREWIDTH
	, CAPTUREHEIGHT
	, CAPTURETRANS

	, TIMELAPSE
	, FRAMES_PER_SECOND
	, KEYFRAMESEVERY
	, COMPQUALITY
	, COMPFCCHANDLER
	, COMPRESSORSTATEISFOR
	, COMPRESSORSTATESIZE
	
	, USEMCI
	, SHIFTTYPE
	, TIMESHIFT
	, FRAMESHIFT
	, RECORDINGMODE
	, AUTONAMING
	, RESTRICTVIDEOCODECS
	, PRESETTIME
	, RECORDPRESET

	// cursor settings
	, MOUSECAPTUREMODE
	, RECORDCURSOR
	, SAVEDIR
	, CURSORDIR
	, CURSORTYPE
	, CUSTOMSEL
	, HIGHLIGHTCURSOR
	, HIGHLIGHTSIZE
	, HIGHLIGHTSHAPE
	, HIGHLIGHTCLICK
	, HIGHLIGHTCOLORR
	, HIGHLIGHTCOLORG
	, HIGHLIGHTCOLORB
	, HIGHLIGHTCLICKCOLORLEFTR
	, HIGHLIGHTCLICKCOLORLEFTG
	, HIGHLIGHTCLICKCOLORLEFTB
	, HIGHLIGHTCLICKCOLORRIGHTR
	, HIGHLIGHTCLICKCOLORRIGHTG
	, HIGHLIGHTCLICKCOLORRIGHTB
	
	// audio
	, RECORDAUDIO
	, AUDIODEVICEID
	, WAVEINSELECTED
	, AUDIO_BITS_PER_SAMPLE
	, AUDIO_NUM_CHANNELS
	, AUDIO_SAMPLES_PER_SECONDS
	, BAUDIOCOMPRESSION
	, INTERLEAVEFRAMES
	, INTERLEAVEFACTOR

	, AUTOPAN
	, MAXPAN
	, CBWFX

	// hotkeys
	, KEYRECORDSTART
	, KEYRECORDSTARTCTRL
	, KEYRECORDSTARTALT
	, KEYRECORDSTARTSHIFT
	, KEYRECORDEND
	, KEYRECORDENDCTRL
	, KEYRECORDENDALT
	, KEYRECORDENDSHIFT
	, KEYRECORDCANCEL
	, KEYRECORDCANCELCTRL
	, KEYRECORDCANCELALT
	, KEYRECORDCANCELSHIFT
	, KEYNEXT
	, KEYNEXTCTRL
	, KEYNEXTALT
	, KEYNEXTSHIFT
	, KEYPREV
	, KEYPREVCTRL
	, KEYPREVALT
	, KEYPREVSHIFT
	, KEYSHOWLAYOUT
	, KEYSHOWLAYOUTCTRL
	, KEYSHOWLAYOUTALT
	, KEYSHOWLAYOUTSHIFT

	, AUTOADJUST
	, VALUEADJUST
	, INTERLEAVEUNIT
	, SPECIFIEDDIR
	, NUMDEV
	, SELECTEDDEV
	, FEEDBACK_LINE
	, FEEDBACK_LINE_INFO
	, PERFORMAUTOSEARCH
	, SUPPORTMOUSEDRAG

	// Shapes
	, SHAPENAMEINT
	, SHAPENAMELEN
	// layout
	, LAYOUTNAMEINT
	, LAYOUTNAMELEN
	
	// Timestamp
	, TIMESTAMPANNOTATION
	, TIMESTAMPTEXTATTRIBUTES
	, TIMESTAMPBACKCOLOR
	, TIMESTAMPSELECTED
	, TIMESTAMPPOSITION
	, TIMESTAMPTEXTCOLOR
	, TIMESTAMPTEXTFONT
	, TIMESTAMPTEXTWEIGHT
	, TIMESTAMPTEXTHEIGHT
	, TIMESTAMPTEXTWIDTH
	
	// Caption
	, CAPTIONANNOTATION
	, CAPTIONTEXTATTRIBUTES
	, CAPTIONBACKCOLOR
	, CAPTIONSELECTED
	, CAPTIONPOSITION
	, CAPTIONTEXTCOLOR
	, CAPTIONTEXTFONT
	, CAPTIONTEXTWEIGHT
	, CAPTIONTEXTHEIGHT
	, CAPTIONTEXTWIDTH
	
	// Watermark
	, WATERMARKANNOTATION
	, WATERMARKIMAGEATTRIBUTES
	
	, MAX_LEGACT_ID
};
#if 0
enum eProfileID
{
	FLASHINGRECT
	, LAUNCHPLAYER
	, MINIMIZEONSTART
	, MOUSECAPTUREMODE
	, CAPTUREWIDTH
	, CAPTUREHEIGHT
	, TIMELAPSE
	, FRAMES_PER_SECOND
	, KEYFRAMESEVERY
	, COMPQUALITY
	, COMPFCCHANDLER
	, COMPRESSORSTATEISFOR
	, COMPRESSORSTATESIZE
	, G_RECORDCURSOR
	, G_CUSTOMSEL
	, G_CURSORTYPE
	, G_HIGHLIGHTCURSOR
	, G_HIGHLIGHTSIZE
	, G_HIGHLIGHTSHAPE
	, G_HIGHLIGHTCLICK
	, G_HIGHLIGHTCOLORR
	, G_HIGHLIGHTCOLORG
	, G_HIGHLIGHTCOLORB
	, G_HIGHLIGHTCLICKCOLORLEFTR
	, G_HIGHLIGHTCLICKCOLORLEFTG
	, G_HIGHLIGHTCLICKCOLORLEFTB
	, G_HIGHLIGHTCLICKCOLORRIGHTR
	, G_HIGHLIGHTCLICKCOLORRIGHTG
	, G_HIGHLIGHTCLICKCOLORRIGHTB
	, AUTOPAN
	, MAXPAN
	, AUDIODEVICEID
	, CBWFX
	, RECORDAUDIO
	, WAVEINSELECTED
	, AUDIO_BITS_PER_SAMPLE
	, AUDIO_NUM_CHANNELS
	, AUDIO_SAMPLES_PER_SECONDS
	, BAUDIOCOMPRESSION
	, INTERLEAVEFRAMES
	, INTERLEAVEFACTOR
	, KEYRECORDSTART
	, KEYRECORDEND
	, KEYRECORDCANCEL
	, VIEWTYPE
	, G_AUTOADJUST
	, G_VALUEADJUST
	, SAVEDIR
	, CURSORDIR
	, THREADPRIORITY
	, CAPTURELEFT
	, CAPTURETOP
	, FIXEDCAPTURE
	, INTERLEAVEUNIT
	, TEMPPATH_ACCESS
	, CAPTURETRANS
	, SPECIFIEDDIR
	, NUMDEV
	, SELECTEDDEV
	, FEEDBACK_LINE
	, FEEDBACK_LINE_INFO
	, PERFORMAUTOSEARCH
	, SUPPORTMOUSEDRAG
	, KEYRECORDSTARTCTRL
	, KEYRECORDENDCTRL
	, KEYRECORDCANCELCTRL
	, KEYRECORDSTARTALT
	, KEYRECORDENDALT
	, KEYRECORDCANCELALT
	, KEYRECORDSTARTSHIFT
	, KEYRECORDENDSHIFT
	, KEYRECORDCANCELSHIFT
	, KEYNEXT
	, KEYPREV
	, KEYSHOWLAYOUT
	, KEYNEXTCTRL
	, KEYPREVCTRL
	, KEYSHOWLAYOUTCTRL
	, KEYNEXTALT
	, KEYPREVALT
	, KEYSHOWLAYOUTALT
	, KEYNEXTSHIFT
	, KEYPREVSHIFT
	, KEYSHOWLAYOUTSHIFT
	, SHAPENAMEINT
	, SHAPENAMELEN
	, LAYOUTNAMEINT
	, G_LAYOUTNAMELEN
	, USEMCI
	, SHIFTTYPE
	, TIMESHIFT
	, FRAMESHIFT
	, LAUNCHPROPPROMPT
	, LAUNCHHTMLPLAYER
	, DELETEAVIAFTERUSE
	, RECORDINGMODE
	, AUTONAMING
	, RESTRICTVIDEOCODECS
	, PRESETTIME
	, RECORDPRESET
	, LANGUAGE
	
	, TIMESTAMPANNOTATION
	, TIMESTAMPTEXTATTRIBUTES
	, TIMESTAMPBACKCOLOR
	, TIMESTAMPSELECTED
	, TIMESTAMPPOSITION
	, TIMESTAMPTEXTCOLOR
	, TIMESTAMPTEXTFONT
	, TIMESTAMPTEXTWEIGHT
	, TIMESTAMPTEXTHEIGHT
	, TIMESTAMPTEXTWIDTH
	
	, CAPTIONANNOTATION
	, CAPTIONTEXTATTRIBUTES
	, CAPTIONBACKCOLOR
	, CAPTIONSELECTED
	, CAPTIONPOSITION
	, CAPTIONTEXTCOLOR
	, CAPTIONTEXTFONT
	, CAPTIONTEXTWEIGHT
	, CAPTIONTEXTHEIGHT
	, CAPTIONTEXTWIDTH
	
	, WATERMARKANNOTATION
	, WATERMARKIMAGEATTRIBUTES
	
	, MAX_PROFILE_ID
};
#endif

/////////////////////////////////////////////////////////////////////////////
//
namespace baseprofile {

	// declarations for type specific template fuctions for low level read
	// and write of profile values. see profile.cpp for implementation
	template <typename T>
	T ReadEntry(CString strFilename, CString strSection, CString strKeyName, const T& DefValue);
	template <typename T>
	bool WriteEntry(CString strFilename, CString strSection, CString strKeyName, const T& Value);

}	// namespace baseprofile


// CGroupType
// template class for group items.
// CGroupType connects the key ID with both a string and a value.
/////////////////////////////////////////////////////////////////////////////
template <typename T>
class CGroupType
{
	CGroupType(); // not implemented
public:
	CGroupType(int iID, CString strName, T Value)
	{
		m_vIDValue.push_back(IDValuePair(iID, ValuePair(strName, Value)));
	}
	virtual ~CGroupType() {}

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

	// read and copy an item by ID and type.
	bool Read(const int iID, T& Value)
	{
		bool bFound = false;
		for (IDVALUE_ITER iter = m_vIDValue.begin(); iter != m_vIDValue.end(); ++iter)
		{
			bFound = (iID == iter->first);		// matching ID
			if (bFound) {
				Value = iter->second.second;	// copy value
				break;
			}
		}
		return bFound;
	}

	bool Write(const int iID, T& Value)
	{
		bool bFound = false;
		for (IDVALUE_ITER iter = m_vIDValue.begin(); iter != m_vIDValue.end(); ++iter)
		{
			bFound = (iID == iter->first);		// matching ID
			if (bFound) {
				iter->second.second = Value;	// write value
				break;
			}
		}
		return bFound;
	}

protected:
	// connect key ID with both key string and value
	typename typedef std::pair <CString, T> ValuePair;
	typename typedef std::pair <int, ValuePair> IDValuePair;
	typename typedef std::vector <IDValuePair>::iterator IDVALUE_ITER;
	
	std::vector <IDValuePair> m_vIDValue;

	// read the entry by type from the file
	T Read(CString strFilename, CString strSection, CString strKeyName, const T& DefValue)
	{
		return baseprofile::ReadEntry(strFilename, strSection, strKeyName, DefValue);
	}

	// write the entry by type to the file
	bool Write(CString strFilename, CString strSection, CString strKeyName, const T& Value)
	{
		return baseprofile::WriteEntry(strFilename, strSection, strKeyName, Value);
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

	// add value to group if not already there.
	bool Add(const int iID, const CString strName, const V Value)
	{
		V NewValue = Value;
		bool bResult = Read(iID, NewValue);
		if (!bResult) {
			m_vValue.push_back(CGroupType<V>(iID, strName, Value));
			bResult = true;
		}
		return bResult;
	}
	// write group values
	bool Read(const CString strFile, const CString strSection)
	{
		for (GRPITEM_ITER iter = m_vValue.begin(); iter != m_vValue.end(); ++iter)
		{
			iter->Read(strFile, strSection);
		}
		return true;
	}
	// write group values
	bool Write(const CString strFile, const CString strSection)
	{
		for (GRPITEM_ITER iter = m_vValue.begin(); iter != m_vValue.end(); ++iter)
		{
			iter->Write(strFile, strSection);
		}
		return true;
	}
	
	// default read item by ID always fails
	template <typename T>
	bool Read(const int iID, T& Value)	{return false;}
	
	// specialized read item by ID for group type searchs the group values
	template <>
	bool Read(const int iID, V& Value)
	{
		bool bFound = false;
		for (GRPITEM_ITER iter = m_vValue.begin(); iter != m_vValue.end(); ++iter)
		{
			bFound = iter->Read(iID, Value);
			if (bFound) {
				break;
			}
		}
		return bFound;
	}

	// default write item by ID always fails
	template <typename T>
	bool Write(const int iID, T& Value)	{return false;}

	// specialized find item by ID for group type searchs the group values
	template <>
	bool Write(const int iID, V& Value)
	{
		bool bFound = false;
		for (GRPITEM_ITER iter = m_vValue.begin(); iter != m_vValue.end(); ++iter)
		{
			bFound = iter->Write(iID, Value);
			if (bFound) {
				break;
			}
		}
		return bFound;
	}

protected:
	typename typedef std::vector <CGroupType<V>>::iterator GRPITEM_ITER;
	std::vector <CGroupType<V>> m_vValue;
};

// class CProfileSection
// CProfileSection encapsulates a profile section behavior.
// CProfileSection have a section name string and a set of entries grouped
// by type.
/////////////////////////////////////////////////////////////////////////////
class CProfileSection
{
	CProfileSection();	// not implemented
public:
	CProfileSection(const CString strSectionName);
	virtual ~CProfileSection();

	// add item to specific section group
	template <typename T>
	bool Add(const int iID, const CString strName, const T& Value);

	// read items from section groups
	bool Read(const CString strFile);
	// write items from section groups
	bool Write(const CString strFile);

	// delete the section
	bool Delete(const CString strFile)
	{
		::WritePrivateProfileString(m_strSectionName, 0, 0, strFile);
		return true;
	}

	// find item by id specialized for item type
	template <typename T>
	bool Read(const int iID, T& Value)
	{
		// search each group for ID
		bool bResult = m_grpLANGID.Read(iID, Value);
		bResult = bResult || m_grpStrings.Read(iID, Value);
		bResult = bResult || m_grpIntegers.Read(iID, Value);
		bResult = bResult || m_grpUINT.Read(iID, Value);
		bResult = bResult || m_grpBools.Read(iID, Value);
		bResult = bResult || m_grpLongs.Read(iID, Value);
		bResult = bResult || m_grpDWORD.Read(iID, Value);
		bResult = bResult || m_grpDoubles.Read(iID, Value);
		bResult = bResult || m_grpLogFont.Read(iID, Value);
		bResult = bResult || m_grpTextAttribs.Read(iID, Value);
		bResult = bResult || m_grpImageAttribs.Read(iID, Value);
		return bResult;
	}
	template <typename T>
	bool Write(const int iID, T& Value)
	{
		// search each group for ID
		bool bResult = m_grpLANGID.Write(iID, Value);
		bResult = bResult || m_grpStrings.Write(iID, Value);
		bResult = bResult || m_grpIntegers.Write(iID, Value);
		bResult = bResult || m_grpUINT.Write(iID, Value);
		bResult = bResult || m_grpBools.Write(iID, Value);
		bResult = bResult || m_grpLongs.Write(iID, Value);
		bResult = bResult || m_grpDWORD.Write(iID, Value);
		bResult = bResult || m_grpDoubles.Write(iID, Value);
		bResult = bResult || m_grpLogFont.Write(iID, Value);
		bResult = bResult || m_grpTextAttribs.Write(iID, Value);
		bResult = bResult || m_grpImageAttribs.Write(iID, Value);
		return bResult;
	}

protected:
	CString m_strSectionName;	// section name
	
	// COLORREF == DWORD
	// supported group types
	CSectionGroup<CString>			m_grpStrings;
	CSectionGroup<int>				m_grpIntegers;
	CSectionGroup<UINT>				m_grpUINT;
	CSectionGroup<bool>				m_grpBools;
	CSectionGroup<long>				m_grpLongs;
	CSectionGroup<DWORD>			m_grpDWORD;		// unsigned long
	CSectionGroup<double>			m_grpDoubles;
	CSectionGroup<LANGID>			m_grpLANGID;
	CSectionGroup<LOGFONT>			m_grpLogFont;
	CSectionGroup<TextAttributes>	m_grpTextAttribs;
	CSectionGroup<ImageAttributes>	m_grpImageAttribs;
};

// class CProfile
// The CProfile encapsulates the reading and writing of an application
// settings file (profile) on Windows. CProfile has a filename and a set
// of CProfileSection items it manages. The public interface allows reading
// and writing of the file as a whole and reading a entry by key ID.
/////////////////////////////////////////////////////////////////////////////
class CProfile
{
	CProfile();	// not implemented
public:
	CProfile(const CString strFileName);
	virtual ~CProfile();

public:
	// read the entire file
	bool Read()
	{
		bool bResult = m_SectionApp.Read(m_strFileName);
		bResult |= m_SectionHighLight.Read(m_strFileName);
		bResult |= m_SectionTimeStamp.Read(m_strFileName);
		bResult |= m_SectionCaption.Read(m_strFileName);
		bResult |= m_SectionWatermark.Read(m_strFileName);
		return bResult;
	}
	// write the entire file
	bool Write()
	{
		bool bResult = m_SectionApp.Write(m_strFileName);
		bResult |= m_SectionHighLight.Write(m_strFileName);
		bResult |= m_SectionTimeStamp.Write(m_strFileName);
		bResult |= m_SectionCaption.Write(m_strFileName);
		bResult |= m_SectionWatermark.Write(m_strFileName);
		return bResult;
	}

	// find/read a key value
	template <typename T>
	bool Read(const int iID, T& Value)
	{
		// search all sections
		bool bResult = m_SectionApp.Read(iID, Value);
		bResult = bResult || m_SectionHighLight.Read(iID, Value);
		bResult = bResult || m_SectionTimeStamp.Read(iID, Value);
		bResult = bResult || m_SectionCaption.Read(iID, Value);
		bResult = bResult || m_SectionWatermark.Read(iID, Value);
		return bResult;
	}
	template <typename T>
	bool Write(const int iID, T& Value)
	{
		// search all sections
		bool bResult = m_SectionApp.Write(iID, Value);
		bResult = bResult || m_SectionHighLight.Write(iID, Value);
		bResult = bResult || m_SectionTimeStamp.Write(iID, Value);
		bResult = bResult || m_SectionCaption.Write(iID, Value);
		bResult = bResult || m_SectionWatermark.Write(iID, Value);
		return bResult;
	}

protected:
	CString m_strFileName;					// name of the *.ini file
	CProfileSection m_SectionLegacy;		// legacy (v2.50) section values
	CProfileSection m_SectionApp;			// application wide settings
	CProfileSection m_SectionHighLight;		// Cursor highlight settings
	CProfileSection m_SectionTimeStamp;		// timestamp settings
	CProfileSection m_SectionCaption;		// caption annotation settings
	CProfileSection m_SectionWatermark;		// watermark annotation settings

	// add item to section
	template <typename T>
	bool Add(CProfileSection& section, const int iID, const CString strName, const T& Value)
	{
		bool bResult = section.Add(iID, strName, Value);
		return bResult;
	}
	// add item to app section (default)
	template <typename T>
	bool Add(const int iID, const CString strName, const T& Value)
	{
		return Add(m_SectionApp, iID, strName, Value);
	}
private:
	void InitLegacySection();
	void InitSections();
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

extern int iLanguageID;
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

extern TextAttributes taCaption;
extern TextAttributes taTimestamp;
extern ImageAttributes iaWatermark;

#endif	// PROFILE_H