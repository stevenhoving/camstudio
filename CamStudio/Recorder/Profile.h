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

#include "TextAttributes.h"
#include "ImageAttributes.h"
#include "CStudioLib.h"

#include <vector>

enum eLegacySettings
{
	// Application
	LANGUAGE
	, LAUNCHPROPPROMPT
	, LAUNCHHTMLPLAYER
	, DELETEAVIAFTERUSE
	, VIEWTYPE

	, SHIFTTYPE
	, TIMESHIFT
	, FRAMESHIFT
	, RESTRICTVIDEOCODECS
	, PRESETTIME
	, RECORDPRESET

	, NUMDEV
	, SELECTEDDEV
	, FEEDBACK_LINE
	, FEEDBACK_LINE_INFO
	, PERFORMAUTOSEARCH
	, INTERLEAVEUNIT

	// region
	, FIXEDCAPTURE
	, SUPPORTMOUSEDRAG
	, MOUSECAPTUREMODE
	, CAPTURETOP
	, CAPTURELEFT
	, CAPTUREWIDTH
	, CAPTUREHEIGHT

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

	// program options
	, AUTONAMING
	, CAPTURETRANS
	, FLASHINGRECT
	, MINIMIZEONSTART
	, RECORDINGMODE
	, LAUNCHPLAYER
	, SPECIFIEDDIR
	, TEMPPATH_ACCESS
	, THREADPRIORITY
	, AUTOPAN
	, MAXPAN

	// Video options
	, AUTOADJUST
	, VALUEADJUST
	, TIMELAPSE
	, FRAMES_PER_SECOND
	, KEYFRAMEINTERVAL
	, COMPQUALITY
	, COMPFCCHANDLER
	, COMPRESSORSTATEISFOR
	, COMPRESSORSTATESIZE
	, CBWFX

	// Shapes
	, SHAPENAMEINT
	, SHAPENAMELEN
	// layout
	, LAYOUTNAMEINT
	, LAYOUTNAMELEN
	
	// cursor settings
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
	, USEMCI
	, WAVEINSELECTED
	, AUDIO_BITS_PER_SAMPLE
	, AUDIO_NUM_CHANNELS
	, AUDIO_SAMPLES_PER_SECONDS
	, BAUDIOCOMPRESSION
	, INTERLEAVEFRAMES
	, INTERLEAVEFACTOR

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
	, KEYFRAMEINTERVAL
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
	bool Read(const int /*iID*/, T& /*Value*/)	{return false;}
	
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
	bool Write(const int /*iID*/, T& /*Value*/)	{return false;}

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
	bool Read();	// read the entire file
	bool Write();	// write the entire file

	// read a key value by ID + type
	template <typename T>
	bool Read(const int iID, T& Value)
	{
		// search all sections
		typedef std::vector <CProfileSection>::iterator sect_iter;
		bool bResult = false;
		for (sect_iter iter = m_vAllSections.begin(); !bResult && (iter != m_vAllSections.end()); ++iter)
		{
			bResult = iter->Read(iID, Value);
		}
		return bResult;
	}
	// write a key value by ID + type
	template <typename T>
	bool Write(const int iID, T& Value)
	{
		// search all sections
		typedef std::vector <CProfileSection>::iterator sect_iter;
		bool bResult = false;
		for (sect_iter iter = m_vAllSections.begin(); !bResult && (iter != m_vAllSections.end()); ++iter)
		{
			bResult = iter->Write(iID, Value);
		}
		return bResult;
	}

protected:
	CString m_strFileName;					// name of the *.ini file
	CProfileSection m_SectionLegacy;		// legacy (v2.50) section values
	CProfileSection m_SectionApp;			// application wide settings
	CProfileSection m_SectionVideo;			// Video options
	CProfileSection m_SectionAudio;			// Audio options
	CProfileSection m_SectionProgram;		// Program Options options
	CProfileSection m_SectionCursor;		// Cursor highlight settings
	CProfileSection m_SectionHotkeys;		// Cursor highlight settings
	CProfileSection m_SectionRegion;		// Region settings
	CProfileSection m_SectionTimeStamp;		// timestamp settings
	CProfileSection m_SectionCaption;		// caption annotation settings
	CProfileSection m_SectionWatermark;		// watermark annotation settings
	// TODO: use a vector for sections
	std::vector <CProfileSection> m_vAllSections;

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

#define NUMSTREAMS		2

// video options
// POD to hold them
struct sVideoOpts
{
	sVideoOpts()
		: m_bAutoAdjust(true)
		, m_iValueAdjust(1)
		, m_iTimeLapse(5)
		, m_iFramesPerSecond(200)
		, m_iKeyFramesEvery(200)
		, m_iCompQuality(7000)
		, m_iSelectedCompressor(0)
		, m_dwCompfccHandler(0UL)
		, m_dwCompressorStateIsFor(0UL)
		, m_dwCompressorStateSize(0UL)
	{
	}
	sVideoOpts(const sVideoOpts& rhs)
	{
		*this = rhs;
	}

	sVideoOpts& operator=(const sVideoOpts& rhs)
	{
		if (this == &rhs)
			return *this;

		m_bAutoAdjust				= rhs.m_bAutoAdjust;
		m_iValueAdjust				= rhs.m_iValueAdjust;
		m_iTimeLapse				= rhs.m_iTimeLapse;
		m_iFramesPerSecond			= rhs.m_iFramesPerSecond;
		m_iKeyFramesEvery			= rhs.m_iKeyFramesEvery;
		m_iCompQuality				= rhs.m_iCompQuality;
		m_iSelectedCompressor		= rhs.m_iSelectedCompressor;
		m_dwCompfccHandler			= rhs.m_dwCompfccHandler;
		m_dwCompressorStateIsFor	= rhs.m_dwCompressorStateIsFor;
		m_dwCompressorStateSize		= rhs.m_dwCompressorStateSize;

		return *this;
	}

	bool Read(CProfile& cProfile)
	{
		VERIFY(cProfile.Read(AUTOADJUST, m_bAutoAdjust));
		VERIFY(cProfile.Read(VALUEADJUST, m_iValueAdjust));
		VERIFY(cProfile.Read(TIMELAPSE, m_iTimeLapse));
		VERIFY(cProfile.Read(FRAMES_PER_SECOND, m_iFramesPerSecond));
		VERIFY(cProfile.Read(KEYFRAMEINTERVAL, m_iKeyFramesEvery));
		VERIFY(cProfile.Read(COMPQUALITY, m_iCompQuality));
		VERIFY(cProfile.Read(COMPFCCHANDLER, m_dwCompfccHandler));
		VERIFY(cProfile.Read(COMPRESSORSTATEISFOR, m_dwCompressorStateIsFor));
		VERIFY(cProfile.Read(COMPRESSORSTATESIZE, m_dwCompressorStateSize));		
		// m_iSelectedCompressor
		return true;
	}
	bool Write(CProfile& cProfile)
	{
		VERIFY(cProfile.Write(AUTOADJUST, m_bAutoAdjust));
		VERIFY(cProfile.Write(VALUEADJUST, m_iValueAdjust));
		VERIFY(cProfile.Write(TIMELAPSE, m_iTimeLapse));
		VERIFY(cProfile.Write(FRAMES_PER_SECOND, m_iFramesPerSecond));
		VERIFY(cProfile.Write(KEYFRAMEINTERVAL, m_iKeyFramesEvery));
		VERIFY(cProfile.Write(COMPQUALITY, m_iCompQuality));
		VERIFY(cProfile.Write(COMPFCCHANDLER, m_dwCompfccHandler));
		VERIFY(cProfile.Write(COMPRESSORSTATEISFOR, m_dwCompressorStateIsFor));
		VERIFY(cProfile.Write(COMPRESSORSTATESIZE, m_dwCompressorStateSize));		
		// m_iSelectedCompressor
		return true;
	}

	bool m_bAutoAdjust;
	int m_iValueAdjust;
	int m_iTimeLapse;
	int m_iFramesPerSecond;
	int m_iKeyFramesEvery;
	int m_iCompQuality;
	int m_iSelectedCompressor;
	DWORD m_dwCompfccHandler;
	DWORD m_dwCompressorStateIsFor;
	DWORD m_dwCompressorStateSize;
};
extern sVideoOpts cVideoOpts;

enum eVideoFormat
{
	ModeAVI
	, ModeFlash
};

struct sProgramOpts
{
	sProgramOpts()
		: m_bAutoNaming(false)
		, m_bCaptureTrans(true)
		, m_bFlashingRect(true)
		, m_bMinimizeOnStart(false)
		, m_bSaveSettings(true)
		, m_bAutoPan(false)
		, m_iMaxPan(20)
		, m_iRecordingMode(ModeAVI)
		, m_iLaunchPlayer(3)
		, m_iSpecifiedDirLength(0)
		, m_iTempPathAccess(USE_WINDOWS_TEMP_DIR)
		, m_iThreadPriority(THREAD_PRIORITY_NORMAL)
	{
	}
	sProgramOpts(const sProgramOpts& rhs)
	{
		*this = rhs;
	}
	sProgramOpts& operator=(const sProgramOpts& rhs)
	{
		if (this == &rhs)
			return *this;

		m_bAutoNaming			= rhs.m_bAutoNaming;
		m_bCaptureTrans			= rhs.m_bCaptureTrans;
		m_bFlashingRect			= rhs.m_bFlashingRect;
		m_bMinimizeOnStart		= rhs.m_bMinimizeOnStart;
		m_bSaveSettings			= rhs.m_bSaveSettings;
		m_bAutoPan				= rhs.m_bAutoPan;
		m_iMaxPan				= rhs.m_iMaxPan;
		m_iRecordingMode		= rhs.m_iRecordingMode;
		m_iLaunchPlayer			= rhs.m_bSaveSettings;
		m_iSpecifiedDirLength	= rhs.m_iSpecifiedDirLength;
		m_iTempPathAccess		= rhs.m_iTempPathAccess;
		m_iThreadPriority		= rhs.m_iThreadPriority;
		return *this;
	}
	bool Read(CProfile& cProfile)
	{
		VERIFY(cProfile.Read(AUTONAMING, m_bAutoNaming));
		VERIFY(cProfile.Read(CAPTURETRANS, m_bCaptureTrans));
		VERIFY(cProfile.Read(FLASHINGRECT, m_bFlashingRect));
		VERIFY(cProfile.Read(MINIMIZEONSTART, m_bMinimizeOnStart));
		VERIFY(cProfile.Read(RECORDINGMODE, m_iRecordingMode));
		VERIFY(cProfile.Read(LAUNCHPLAYER, m_iLaunchPlayer));
		VERIFY(cProfile.Read(SPECIFIEDDIR, m_iLaunchPlayer));
		VERIFY(cProfile.Read(TEMPPATH_ACCESS, m_iTempPathAccess));
		VERIFY(cProfile.Read(THREADPRIORITY, m_iThreadPriority));
		VERIFY(cProfile.Read(AUTOPAN, m_bAutoPan));
		VERIFY(cProfile.Read(MAXPAN, m_iMaxPan));
		return true;
	}
	bool Write(CProfile& cProfile)
	{
		VERIFY(cProfile.Write(AUTONAMING, m_bAutoNaming));
		VERIFY(cProfile.Write(CAPTURETRANS, m_bCaptureTrans));
		VERIFY(cProfile.Write(FLASHINGRECT, m_bFlashingRect));
		VERIFY(cProfile.Write(MINIMIZEONSTART, m_bMinimizeOnStart));
		VERIFY(cProfile.Write(RECORDINGMODE, m_iRecordingMode));
		VERIFY(cProfile.Write(LAUNCHPLAYER, m_iLaunchPlayer));
		VERIFY(cProfile.Write(SPECIFIEDDIR, m_iLaunchPlayer));
		VERIFY(cProfile.Write(TEMPPATH_ACCESS, m_iTempPathAccess));
		VERIFY(cProfile.Write(THREADPRIORITY, m_iThreadPriority));
		VERIFY(cProfile.Write(AUTOPAN, m_bAutoPan));
		VERIFY(cProfile.Write(MAXPAN, m_iMaxPan));
		return true;
	}

	bool m_bAutoNaming;
	bool m_bCaptureTrans;
	bool m_bFlashingRect;
	bool m_bMinimizeOnStart;
	bool m_bSaveSettings;
	bool m_bAutoPan;
	int m_iMaxPan;
	int m_iRecordingMode;
	int m_iLaunchPlayer;
	int m_iSpecifiedDirLength;
	int m_iTempPathAccess;
	int m_iThreadPriority;
};
extern sProgramOpts cProgramOpts;

enum eCaptureType
{
	CAPTURE_FIXED
	, CAPTURE_VARIABLE
	, CAPTURE_FULLSCREEN
	, CAPTURE_WINDOW
};

// miscellaneous options
struct sRegionOpts
{
	sRegionOpts()
		: m_bFixedCapture(false)
		, m_bSupportMouseDrag(false)
		, m_iMouseCaptureMode(CAPTURE_FIXED)
		, m_iCaptureLeft(100)
		, m_iCaptureTop(100)
		, m_iCaptureWidth(320)
		, m_iCaptureHeight(240)
	{
	}
	sRegionOpts(const sRegionOpts& rhs)
	{
		*this = rhs;
	}
	sRegionOpts& operator=(const sRegionOpts& rhs)
	{
		if (this == &rhs)
			return *this;

		m_bFixedCapture		= rhs.m_bFixedCapture;
		m_bSupportMouseDrag	= rhs.m_bSupportMouseDrag;
		m_iMouseCaptureMode	= rhs.m_iMouseCaptureMode;
		m_iCaptureLeft		= rhs.m_iCaptureLeft;
		m_iCaptureTop		= rhs.m_iCaptureTop;
		m_iCaptureWidth		= rhs.m_iCaptureWidth;
		m_iCaptureHeight	= rhs.m_iCaptureHeight;
		return *this;
	}
	bool Read(CProfile& cProfile)
	{
		VERIFY(cProfile.Read(FIXEDCAPTURE, m_bFixedCapture));
		VERIFY(cProfile.Read(SUPPORTMOUSEDRAG, m_bSupportMouseDrag));
		VERIFY(cProfile.Read(MOUSECAPTUREMODE, m_iMouseCaptureMode));		
		VERIFY(cProfile.Read(CAPTURETOP, m_iCaptureLeft));
		VERIFY(cProfile.Read(CAPTURELEFT, m_iCaptureTop));
		VERIFY(cProfile.Read(CAPTUREWIDTH, m_iCaptureWidth));
		VERIFY(cProfile.Read(CAPTUREHEIGHT, m_iCaptureHeight));

		return true;
	}
	bool Write(CProfile& cProfile)
	{
		VERIFY(cProfile.Write(FIXEDCAPTURE, m_bFixedCapture));
		VERIFY(cProfile.Write(SUPPORTMOUSEDRAG, m_bSupportMouseDrag));
		VERIFY(cProfile.Write(MOUSECAPTUREMODE, m_iMouseCaptureMode));		
		VERIFY(cProfile.Write(CAPTURETOP, m_iCaptureLeft));
		VERIFY(cProfile.Write(CAPTURELEFT, m_iCaptureTop));
		VERIFY(cProfile.Write(CAPTUREWIDTH, m_iCaptureWidth));
		VERIFY(cProfile.Write(CAPTUREHEIGHT, m_iCaptureHeight));
		return true;
	}

	bool isCaptureMode(const eCaptureType eType) const {return eType == m_iMouseCaptureMode;}

	bool m_bFixedCapture;		// TODO: should be m_bFixed
	bool m_bSupportMouseDrag;	// TODO: should be m_bMouseDrag
	int m_iMouseCaptureMode;	// TODO: Mouse??? Just CaptureMode.
	// TODO: should be a CRect
	int m_iCaptureLeft;			// TODO: should be m_iLeft (if not CRect)
	int m_iCaptureTop;			// TODO: should be m_iTop (if not CRect)
	int m_iCaptureWidth;		// TODO: should be m_iWidth (if not CRect)
	int m_iCaptureHeight;		// TODO: should be m_iHeight (if not CRect)
};
extern sRegionOpts cRegionOpts;

// Audio format values
// POD to hold them
const int MILLISECONDS	= 0;
const int FRAMES		= 1;
enum eAudioInput
{
	NONE
	, MICROPHONE
	, SPEAKERS
};

struct sAudioFormat
{
public:
	sAudioFormat()
		: m_uDeviceID(WAVE_MAPPER)
		, m_bCompression(true)
		, m_bInterleaveFrames(true)
		, m_bUseMCI(false)
		, m_iRecordAudio(NONE)
		, m_iNumChannels(2)
		, m_iBitsPerSample(16)
		, m_iSamplesPerSeconds(22050)
		, m_iInterleaveFactor(100)
		, m_iInterleavePeriod(MILLISECONDS)
		, m_dwCbwFX(0)
		, m_dwWaveinSelected(WAVE_FORMAT_2S16)
	{
	}
	sAudioFormat(const sAudioFormat& rhs)
	{
		*this = rhs;
	}
	sAudioFormat& operator=(const sAudioFormat& rhs)
	{
		if (this == &rhs)
			return *this;

		m_uDeviceID				= rhs.m_uDeviceID;
		m_bCompression			= rhs.m_bCompression;
		m_bInterleaveFrames		= rhs.m_bInterleaveFrames;
		m_bUseMCI				= rhs.m_bUseMCI;
		m_iRecordAudio			= rhs.m_iRecordAudio;
		m_iNumChannels			= rhs.m_iNumChannels;
		m_iBitsPerSample		= rhs.m_iBitsPerSample;
		m_iSamplesPerSeconds	= rhs.m_iSamplesPerSeconds;
		m_iInterleaveFactor		= rhs.m_iInterleaveFactor;
		m_iInterleavePeriod		= rhs.m_iInterleavePeriod;
		m_dwCbwFX				= rhs.m_dwCbwFX;
		m_dwWaveinSelected		= rhs.m_dwWaveinSelected;
		return *this;
	}
	bool Read(CProfile& cProfile)
	{
		VERIFY(cProfile.Read(AUDIODEVICEID, m_uDeviceID));
		VERIFY(cProfile.Read(BAUDIOCOMPRESSION, m_bCompression));
		VERIFY(cProfile.Read(USEMCI, m_bUseMCI));
		VERIFY(cProfile.Read(RECORDAUDIO, m_iRecordAudio));		
		VERIFY(cProfile.Read(CBWFX, m_dwCbwFX));
		VERIFY(cProfile.Read(WAVEINSELECTED, m_dwWaveinSelected));
		VERIFY(cProfile.Read(AUDIO_BITS_PER_SAMPLE, m_iBitsPerSample));
		VERIFY(cProfile.Read(AUDIO_NUM_CHANNELS, m_iNumChannels));
		VERIFY(cProfile.Read(AUDIO_SAMPLES_PER_SECONDS, m_iSamplesPerSeconds));
		VERIFY(cProfile.Read(INTERLEAVEFRAMES, m_bInterleaveFrames));
		VERIFY(cProfile.Read(INTERLEAVEFACTOR, m_iInterleaveFactor));
		VERIFY(cProfile.Read(INTERLEAVEUNIT, m_iInterleavePeriod));
		return true;
	}
	bool Write(CProfile& cProfile)
	{
		VERIFY(cProfile.Write(AUDIODEVICEID, m_uDeviceID));
		VERIFY(cProfile.Write(BAUDIOCOMPRESSION, m_bCompression));
		VERIFY(cProfile.Write(USEMCI, m_bUseMCI));		
		VERIFY(cProfile.Write(RECORDAUDIO, m_iRecordAudio));		
		VERIFY(cProfile.Write(CBWFX, m_dwCbwFX));
		VERIFY(cProfile.Write(WAVEINSELECTED, m_dwWaveinSelected));
		VERIFY(cProfile.Write(AUDIO_BITS_PER_SAMPLE, m_iBitsPerSample));
		VERIFY(cProfile.Write(AUDIO_NUM_CHANNELS, m_iNumChannels));
		VERIFY(cProfile.Write(AUDIO_SAMPLES_PER_SECONDS, m_iSamplesPerSeconds));
		VERIFY(cProfile.Write(INTERLEAVEFRAMES, m_bInterleaveFrames));
		VERIFY(cProfile.Write(INTERLEAVEFACTOR, m_iInterleaveFactor));
		VERIFY(cProfile.Write(INTERLEAVEUNIT, m_iInterleavePeriod));
		return true;
	}
	bool isInput(eAudioInput eInput) const {return eInput == m_iRecordAudio;}

	bool m_bCompression;
	bool m_bInterleaveFrames;
	bool m_bUseMCI;
	int m_iRecordAudio;
	int m_iNumChannels;
	int m_iBitsPerSample;
	int m_iSamplesPerSeconds;
	int m_iInterleaveFactor;
	int m_iInterleavePeriod;
	UINT m_uDeviceID;
	DWORD m_dwCbwFX;
	DWORD m_dwWaveinSelected;
};
extern sAudioFormat cAudioFormat;

// record from speaker
extern int iNumberOfMixerDevices;
extern int iSelectedMixer;
extern int iFeedbackLine;
extern int iFeedbackLineInfo;

extern bool bPerformAutoSearch;
extern bool bLaunchPropPrompt;
extern bool bLaunchHTMLPlayer;
extern bool bDeleteAVIAfterUse;
extern bool bRestrictVideoCodecs;
extern bool bRecordPreset;

extern int iViewType;
extern int iSaveLen;
extern int iCursorLen;
extern int iShapeNameInt;
extern int iShapeNameLen;	// string length
extern int iLayoutNameInt;
extern int iLayoutNameLen;	// string length
extern int iShiftType;
extern int iTimeShift;
extern int iFrameShift;
extern int iPresetTime;

extern bool bCaptionAnnotation;
extern TextAttributes taCaption;

extern bool bTimestampAnnotation;
extern TextAttributes taTimestamp;

extern bool bWatermarkAnnotation;
extern ImageAttributes iaWatermark;

#endif	// PROFILE_H