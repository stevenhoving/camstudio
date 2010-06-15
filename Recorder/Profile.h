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
#include "Camstudio4XNote.h"

#include <vector>

// define LEGACY_PROFILE_DISABLE to disable CRecordView::LoadSettings()
// and CRecorderView::SaveSettings() from changing CamStudio.ini file
#define LEGACY_PROFILE_DISABLE

enum eLegacySettings
{
	// Application
	LANGUAGE
	// legacy, obsolete
	, FRAMESHIFT

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
	, PRESETTIME
	, RECORDPRESET
	, VIEWTYPE
	// Shapes
	, SHAPENAMEINT
	, SHAPENAMELEN
	// layout
	, LAYOUTNAMEINT
	, LAYOUTNAMELEN

	// Producer options
	, LAUNCHPROPPROMPT
	, LAUNCHHTMLPLAYER
	, DELETEAVIAFTERUSE

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

	// Video options
	, RESTRICTVIDEOCODECS
	, AUTOADJUST
	, VALUEADJUST
	, TIMELAPSE
	, FRAMES_PER_SECOND
	, KEYFRAMEINTERVAL
	, COMPQUALITY
	, SHIFTTYPE
	, TIMESHIFT
	, COMPFCCHANDLER
	, COMPRESSORSTATEISFOR
	, COMPRESSORSTATESIZE
	, CBWFX

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
	, HIGHLIGHTCOLOR
	, HIGHLIGHTCOLORR
	, HIGHLIGHTCOLORG
	, HIGHLIGHTCOLORB
	, HIGHLIGHTCLICKCOLORLEFT
	, HIGHLIGHTCLICKCOLORLEFTR
	, HIGHLIGHTCLICKCOLORLEFTG
	, HIGHLIGHTCLICKCOLORLEFTB
	, HIGHLIGHTCLICKCOLORRIGHT
	, HIGHLIGHTCLICKCOLORRIGHTR
	, HIGHLIGHTCLICKCOLORRIGHTG
	, HIGHLIGHTCLICKCOLORRIGHTB
	
	// audio
	, RECORDAUDIO
	, AUDIODEVICEID
	, USEMCI
	, PERFORMAUTOSEARCH
	, WAVEINSELECTED
	, AUDIO_BITS_PER_SAMPLE
	, AUDIO_NUM_CHANNELS
	, AUDIO_SAMPLES_PER_SECONDS
	, BAUDIOCOMPRESSION
	, INTERLEAVEFRAMES
	, INTERLEAVEFACTOR
	, INTERLEAVEUNIT
	, NUMDEV
	, SELECTEDDEV
	, COMPRESSFORMATTAG
	, FEEDBACK_LINE
	, FEEDBACK_LINE_INFO

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
	
	// XNote
	, XNOTEANNOTATION
	, XNOTECAMERADELAYINMILLISEC
	, XNOTEDISPLAYCAMERADELAY
	, XNOTERECORDDURATIONLIMITINMILLISEC
	, XNOTERECORDDURATIONLIMITMODE
	, XNOTEDISPLAYFORMATSTRING
	, XNOTETEXTATTRIBUTES
	, XNOTEBACKCOLOR
	, XNOTESELECTED
	, XNOTEPOSITION
	, XNOTETEXTCOLOR
	, XNOTETEXTFONT
	, XNOTETEXTWEIGHT
	, XNOTETEXTHEIGHT
	, XNOTETEXTWIDTH

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
		for (GRPITEM_ITER iter = m_vValue.begin(); !bFound && (iter != m_vValue.end()); ++iter)
		{
			bFound = iter->Read(iID, Value);
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
		for (GRPITEM_ITER iter = m_vValue.begin(); !bFound && (iter != m_vValue.end()); ++iter)
		{
			bFound = iter->Write(iID, Value);
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

	bool isSection(const CString & strName)
	{
		return strName == m_strSectionName;
	}

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
		// TODO: order of evaluation is undefined for logical OR
		// but it should not matter.
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
		// TODO: order of evaluation is undefined for logical OR
		// but it should not matter.
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
	CProfileSection m_SectionApp;			// Application wide settings
	CProfileSection m_SectionProgram;		// Program/recording options
	CProfileSection m_SectionVideo;			// Video options
	CProfileSection m_SectionAudio;			// Audio options
	CProfileSection m_SectionCursor;		// Cursor highlight settings
	CProfileSection m_SectionHotkeys;		// Cursor highlight settings
	CProfileSection m_SectionRegion;		// Region settings
	CProfileSection m_SectionTimeStamp;		// timestamp settings
	CProfileSection m_SectionXNote;			// XNote timestamp settings
	CProfileSection m_SectionCaption;		// caption annotation settings
	CProfileSection m_SectionWatermark;		// watermark annotation settings
	CProfileSection m_SectionLegacy;		// legacy (v2.50) section values

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
	template <typename T>
	bool Convert(CProfileSection& section, const int iID, const T& /*Value*/)
	{
		T OldValue;
		bool bResult = section.Read(iID, OldValue);
		bResult = bResult && Write(iID, OldValue);
		return bResult;
	}
	bool Convert();

	void InitLegacySection();
	void InitSections();
};

// legacy settings values.

// video options
enum eSynchType
{
	NOSYNCH
	, VIDEOFIRST
	, AUDIOFIRST
};

// POD to hold them
struct sVideoOpts
{
	sVideoOpts()
		: m_bRestrictVideoCodecs(false)
		, m_bAutoAdjust(true)
		, m_iValueAdjust(1)
		, m_iTimeLapse(5)
		, m_iFramesPerSecond(200)
		, m_iKeyFramesEvery(200)
		, m_iCompQuality(7000)
		, m_iSelectedCompressor(0)
		, m_iShiftType(NOSYNCH)
		, m_iTimeShift(100)
		, m_dwCompfccHandler(0UL)
		, m_dwCompressorStateIsFor(0UL)
		, m_dwCompressorStateSize(0UL)
		, m_pState(0)
	{
	}
	sVideoOpts(const sVideoOpts& rhs)
		: m_bRestrictVideoCodecs(false)
		, m_bAutoAdjust(true)
		, m_iValueAdjust(1)
		, m_iTimeLapse(5)
		, m_iFramesPerSecond(200)
		, m_iKeyFramesEvery(200)
		, m_iCompQuality(7000)
		, m_iSelectedCompressor(0)
		, m_iShiftType(NOSYNCH)
		, m_iTimeShift(100)
		, m_dwCompfccHandler(0UL)
		, m_dwCompressorStateIsFor(0UL)
		, m_dwCompressorStateSize(0UL)
		, m_pState(0)
	{
		*this = rhs;
	}
	virtual ~sVideoOpts()
	{
		if (m_pState)
			delete [] m_pState;
	}

	sVideoOpts& operator=(const sVideoOpts& rhs)
	{
		if (this == &rhs)
			return *this;

		m_bRestrictVideoCodecs		= rhs.m_bRestrictVideoCodecs;
		m_bAutoAdjust				= rhs.m_bAutoAdjust;
		m_iValueAdjust				= rhs.m_iValueAdjust;
		m_iTimeLapse				= rhs.m_iTimeLapse;
		m_iFramesPerSecond			= rhs.m_iFramesPerSecond;
		m_iKeyFramesEvery			= rhs.m_iKeyFramesEvery;
		m_iCompQuality				= rhs.m_iCompQuality;
		m_iSelectedCompressor		= rhs.m_iSelectedCompressor;
		m_iShiftType				= rhs.m_iShiftType;
		m_iTimeShift				= rhs.m_iTimeShift;
		m_dwCompfccHandler			= rhs.m_dwCompfccHandler;
		m_dwCompressorStateIsFor	= rhs.m_dwCompressorStateIsFor;

		State(rhs.m_pState, rhs.m_dwCompressorStateSize);

		return *this;
	}

	DWORD StateSize() const	{return m_dwCompressorStateSize;}
	// n.b. Keep LPCVOID definition; good for memory corruption check
	//LPCVOID State() const	{return m_pState;}
	LPVOID State() const	{return m_pState;}
	LPVOID State(DWORD dwStateSize)
	{
		if (m_pState) {
			delete [] m_pState;
			m_pState = 0;
			m_dwCompressorStateSize = 0L;
		}

		ASSERT(0L == m_dwCompressorStateSize);
		if (dwStateSize < 1L) {
			return m_pState;
		}
		m_dwCompressorStateSize = dwStateSize;
		m_pState = new char[m_dwCompressorStateSize];
		return m_pState;
	}
	LPVOID State(LPCVOID pState, DWORD dwStateSize)
	{
		State(pState ? dwStateSize : 0L);
		if (!pState || (dwStateSize < 1L)) {
			return m_pState;
		}
		
		::_memccpy(m_pState, pState, 1, m_dwCompressorStateSize);

		return m_pState;
	}
	// CAVEAT!!!  CAVEAT!!!  CAVEAT!!!  CAVEAT!!!
	// Caller of CopyState() must delete the allocated memory!
	//LPVOID CopyState()
	//{
	//	if (!m_pState)
	//		return m_pState;
	//	ASSERT(0L < m_dwCompressorStateSize);
	//	LPVOID lpOldState = m_pState;
	//	DWORD dwOldSize = m_dwCompressorStateSize;
	//	m_pState = 0;
	//	m_dwCompressorStateSize = 0L;
	//	VERIFY(0 != State(lpOldState, dwOldSize));
	//	return lpOldState;
	//}

	bool Read(CProfile& cProfile)
	{
		VERIFY(cProfile.Read(RESTRICTVIDEOCODECS, m_bRestrictVideoCodecs));		
		VERIFY(cProfile.Read(AUTOADJUST, m_bAutoAdjust));
		VERIFY(cProfile.Read(VALUEADJUST, m_iValueAdjust));
		VERIFY(cProfile.Read(TIMELAPSE, m_iTimeLapse));
		VERIFY(cProfile.Read(FRAMES_PER_SECOND, m_iFramesPerSecond));
		VERIFY(cProfile.Read(KEYFRAMEINTERVAL, m_iKeyFramesEvery));
		VERIFY(cProfile.Read(COMPQUALITY, m_iCompQuality));
		VERIFY(cProfile.Read(SHIFTTYPE, m_iShiftType));
		VERIFY(cProfile.Read(TIMESHIFT, m_iTimeShift));
		VERIFY(cProfile.Read(COMPFCCHANDLER, m_dwCompfccHandler));
		VERIFY(cProfile.Read(COMPRESSORSTATEISFOR, m_dwCompressorStateIsFor));
		DWORD dwSize = 0UL;
		VERIFY(cProfile.Read(COMPRESSORSTATESIZE, dwSize));
		State(dwSize);
		// m_iSelectedCompressor
		return true;
	}
	bool Write(CProfile& cProfile)
	{
		VERIFY(cProfile.Write(RESTRICTVIDEOCODECS, m_bRestrictVideoCodecs));		
		VERIFY(cProfile.Write(AUTOADJUST, m_bAutoAdjust));
		VERIFY(cProfile.Write(VALUEADJUST, m_iValueAdjust));
		VERIFY(cProfile.Write(TIMELAPSE, m_iTimeLapse));
		VERIFY(cProfile.Write(FRAMES_PER_SECOND, m_iFramesPerSecond));
		VERIFY(cProfile.Write(KEYFRAMEINTERVAL, m_iKeyFramesEvery));
		VERIFY(cProfile.Write(COMPQUALITY, m_iCompQuality));
		VERIFY(cProfile.Write(SHIFTTYPE, m_iShiftType));
		VERIFY(cProfile.Write(TIMESHIFT, m_iTimeShift));
		VERIFY(cProfile.Write(COMPFCCHANDLER, m_dwCompfccHandler));
		VERIFY(cProfile.Write(COMPRESSORSTATEISFOR, m_dwCompressorStateIsFor));
		VERIFY(cProfile.Write(COMPRESSORSTATESIZE, m_dwCompressorStateSize));		
		// m_iSelectedCompressor
		return true;
	}

	bool m_bRestrictVideoCodecs;
	bool m_bAutoAdjust;
	int m_iValueAdjust;
	int m_iTimeLapse;
	int m_iFramesPerSecond;
	int m_iKeyFramesEvery;
	int m_iCompQuality;
	int m_iSelectedCompressor;
	int m_iShiftType;	// NOSYNCH, VIDEOFIRST, AUDIOFIRST
	int m_iTimeShift;
	FOURCC m_dwCompfccHandler;
	FOURCC m_dwCompressorStateIsFor;
protected:
	LPVOID m_pState;
	DWORD m_dwCompressorStateSize;
};
extern sVideoOpts cVideoOpts;

enum eCaptureType
{
	CAPTURE_FIXED
	, CAPTURE_VARIABLE
	, CAPTURE_FULLSCREEN
	, CAPTURE_WINDOW
	, CAPTURE_ALLSCREENS
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

struct sCaptionOpts
{
	sCaptionOpts()
		: m_bAnnotation(false)
		, m_taCaption(TOP_LEFT)
	{
	}
	sCaptionOpts(const sCaptionOpts& rhs)
	{
		*this = rhs;
	}
	sCaptionOpts& operator=(const sCaptionOpts& rhs)
	{
		if (this == &rhs)
			return *this;

		m_bAnnotation	= rhs.m_bAnnotation;
		m_taCaption		= rhs.m_taCaption;
		return *this;
	}
	bool Read(CProfile& cProfile)
	{
		VERIFY(cProfile.Read(CAPTIONANNOTATION, m_bAnnotation));
		VERIFY(cProfile.Read(CAPTIONTEXTATTRIBUTES, m_taCaption));
		return true;
	}
	bool Write(CProfile& cProfile)
	{
		VERIFY(cProfile.Write(CAPTIONANNOTATION, m_bAnnotation));
		VERIFY(cProfile.Write(CAPTIONTEXTATTRIBUTES, m_taCaption));
		return true;
	}

	bool m_bAnnotation;
	TextAttributes m_taCaption;
};
extern sCaptionOpts cCaptionOpts;

struct sTimestampOpts
{
	sTimestampOpts()
		: m_bAnnotation(false)
		, m_taTimestamp(TOP_LEFT)
	{
	}
	sTimestampOpts(const sTimestampOpts& rhs)
	{
		*this = rhs;
	}
	sTimestampOpts& operator=(const sTimestampOpts& rhs)
	{
		if (this == &rhs)
			return *this;

		m_bAnnotation	= rhs.m_bAnnotation;
		m_taTimestamp	= rhs.m_taTimestamp;
		return *this;
	}
	bool Read(CProfile& cProfile)
	{
		VERIFY(cProfile.Read(TIMESTAMPANNOTATION, m_bAnnotation));
		VERIFY(cProfile.Read(TIMESTAMPTEXTATTRIBUTES, m_taTimestamp));
		return true;
	}
	bool Write(CProfile& cProfile)
	{
		VERIFY(cProfile.Write(TIMESTAMPANNOTATION, m_bAnnotation));
		VERIFY(cProfile.Write(TIMESTAMPTEXTATTRIBUTES, m_taTimestamp));
		return true;
	}

	bool m_bAnnotation;
	TextAttributes m_taTimestamp;
};
extern sTimestampOpts cTimestampOpts;

//  == Xnote Stopwatch  - Begin  ======================================================================
struct sXNoteOpts
{
	// settings here are the ones that are initially used for dialog screen..!
	sXNoteOpts()
#ifdef CAMSTUDIO4XNOTE
		: m_bAnnotation(true)						// True, because I did not managed to read info from config	
#else
		: m_bAnnotation(false)						// False, default as it should be with stand Camstudio
#endif
		, m_taXNote(BOTTOM_LEFT)
		, m_bXnoteDisplayCameraDelay(true)			// Default On: Show used delay in capture
		, m_ulXnoteCameraDelayInMilliSec(175)		// Average delay, default 175 ms
		, m_cXnoteDisplayFormatString("(0000)  00:00:00.000")	// Default hh:mm:ss.ttt, Not really a format. As long as the timer is not running this will be showed
		, m_bXnoteRecordDurationLimitMode(true)			// Default On: Show used delay in capture
		, m_ulXnoteRecordDurationLimitInMilliSec(1750)		// Average recordin duration, default 1750 ms
		, m_ulStartXnoteTickCounter(0)				// A non persistent member
		, m_ulSnapXnoteTickCounter(0)				// A non persistent member
		, m_cSnapXnoteTimesString("")				// A non persistent member
	{
	}
	sXNoteOpts(const sXNoteOpts& rhs)
	{
		*this = rhs;
	}
	sXNoteOpts& operator=(const sXNoteOpts& rhs)
	{
		if (this == &rhs)
			return *this;

		m_bAnnotation = rhs.m_bAnnotation;
		m_taXNote = rhs.m_taXNote;

		m_bXnoteDisplayCameraDelay = rhs.m_bXnoteDisplayCameraDelay;
		m_ulXnoteCameraDelayInMilliSec = rhs.m_ulXnoteCameraDelayInMilliSec;
		m_cXnoteDisplayFormatString = rhs.m_cXnoteDisplayFormatString;

		m_bXnoteRecordDurationLimitMode = rhs.m_bXnoteRecordDurationLimitMode;
		m_ulXnoteRecordDurationLimitInMilliSec = rhs.m_ulXnoteRecordDurationLimitInMilliSec;

		// Assign Xnote format string to 'Text atribute'.text (Required to get a time displayed on the capture without pressing OK first in AnnotationEffects dialog.)
		m_taXNote.text = m_cXnoteDisplayFormatString;

		// Apply default colors if back and foreground are the same
		if ( m_taXNote.backgroundColor == m_taXNote.textColor )
		{
			m_taXNote.backgroundColor = RGB(255,255,255);
			m_taXNote.textColor = RGB(0,0,0);
			m_taXNote.logfont.lfHeight = 12;
		}

		return *this;
	}
	bool Read(CProfile& cProfile)
	{
		//TRACE("## bool Read(CProfile& cProfile\n");																				
		VERIFY(cProfile.Read(XNOTEANNOTATION, m_bAnnotation));
		VERIFY(cProfile.Read(XNOTETEXTATTRIBUTES, m_taXNote));
		
		VERIFY(cProfile.Read(XNOTEDISPLAYCAMERADELAY, m_bXnoteDisplayCameraDelay));
		VERIFY(cProfile.Read(XNOTECAMERADELAYINMILLISEC, m_ulXnoteCameraDelayInMilliSec));
		VERIFY(cProfile.Read(XNOTEDISPLAYFORMATSTRING, m_cXnoteDisplayFormatString));
		
		VERIFY(cProfile.Read(XNOTERECORDDURATIONLIMITMODE, m_bXnoteRecordDurationLimitMode));
		VERIFY(cProfile.Read(XNOTERECORDDURATIONLIMITINMILLISEC, m_ulXnoteRecordDurationLimitInMilliSec));
		// m_ulStartXnoteTickCounter is a non persistent member

		// Assign Xnote format string to 'Text atribute'.text (Required to get a time displayed on the capture without pressing OK first in AnnotationEffects dialog.)
		m_taXNote.text = m_cXnoteDisplayFormatString;

		// Apply default colors if back and foreground are the same
		if ( m_taXNote.backgroundColor == m_taXNote.textColor )
		{
			m_taXNote.backgroundColor = RGB(255,255,255);
			m_taXNote.textColor = RGB(0,0,0);
			m_taXNote.logfont.lfHeight = 12;
		}
/*
		TRACE("## ----------------------------------------------------------------------------\n");			
		TRACE("## m_bAnnotation : [%d]\n", m_bAnnotation   );
		TRACE("## m_taXNote.text : [%s]\n", m_taXNote.text.GetString()   );
		TRACE("## m_bXnoteDisplayCameraDelay : [%d]\n", m_bXnoteDisplayCameraDelay   );
		TRACE("## m_ulXnoteCameraDelayInMilliSec : [%ul]\n", m_ulXnoteCameraDelayInMilliSec   );
		TRACE("## m_cXnoteDisplayFormatString : [%s]\n", m_cXnoteDisplayFormatString   );
		TRACE("## m_bXnoteRecordDurationLimitMode : [%d]\n", m_bXnoteRecordDurationLimitMode   );
		TRACE("## m_ulXnoteRecordDurationLimitInMilliSec : [%ul]\n", m_ulXnoteRecordDurationLimitInMilliSec   );
		TRACE("## ----------------------------------------------------------------------------\n");			
*/
		return true;
	}
	bool Write(CProfile& cProfile)
	{
/*
		TRACE("## bool Write(CProfile& cProfile\n");			
		TRACE("## ----------------------------------------------------------------------------\n");			
		TRACE("## m_bAnnotation : [%d]\n", m_bAnnotation   );
		TRACE("## m_taXNote.text : [%s]\n", m_taXNote.text.GetString()   );
		TRACE("## m_bXnoteDisplayCameraDelay : [%d]\n", m_bXnoteDisplayCameraDelay   );
		TRACE("## m_ulXnoteCameraDelayInMilliSec : [%ul]\n", m_ulXnoteCameraDelayInMilliSec   );
		TRACE("## m_cXnoteDisplayFormatString : [%s]\n", m_cXnoteDisplayFormatString   );
		TRACE("## m_bXnoteRecordDurationLimitMode : [%d]\n", m_bXnoteRecordDurationLimitMode   );
		TRACE("## m_ulXnoteRecordDurationLimitInMilliSec : [%ul]\n", m_ulXnoteRecordDurationLimitInMilliSec   );
		TRACE("## ----------------------------------------------------------------------------\n");			
*/

		// Assign Xnote format string to .text because we need this to get a formatted print on the capture without pressing OK first in AnnotationEffects dialog.
		m_taXNote.text = m_cXnoteDisplayFormatString;

		// Apply default colors if back and foreground are the same
		if ( m_taXNote.backgroundColor == m_taXNote.textColor )
		{
			m_taXNote.backgroundColor = RGB(255,255,255);
			m_taXNote.textColor = RGB(0,0,0);
			m_taXNote.logfont.lfHeight = 12;
		}

		VERIFY(cProfile.Write(XNOTEANNOTATION, m_bAnnotation));
		VERIFY(cProfile.Write(XNOTETEXTATTRIBUTES, m_taXNote));

		VERIFY(cProfile.Write(XNOTEDISPLAYCAMERADELAY, m_bXnoteDisplayCameraDelay));
		VERIFY(cProfile.Write(XNOTECAMERADELAYINMILLISEC, m_ulXnoteCameraDelayInMilliSec));
		VERIFY(cProfile.Write(XNOTEDISPLAYFORMATSTRING, m_cXnoteDisplayFormatString));

		VERIFY(cProfile.Write(XNOTERECORDDURATIONLIMITMODE, m_bXnoteRecordDurationLimitMode));
		VERIFY(cProfile.Write(XNOTERECORDDURATIONLIMITINMILLISEC, m_ulXnoteRecordDurationLimitInMilliSec));

		// m_ulStartXnoteTickCounter is a non persistent member
		return true;
	}

	bool	m_bAnnotation;
	TextAttributes m_taXNote;

	bool	m_bXnoteDisplayCameraDelay;
	ULONG	m_ulXnoteCameraDelayInMilliSec;
	CButton m_CheckBoxXnoteDisplayCameraDelay;

	bool	m_bXnoteRecordDurationLimitMode;
	ULONG	m_ulXnoteRecordDurationLimitInMilliSec;
	CButton m_CheckBoxXnoteRecordDurationLimitMode;

	CString	m_cXnoteDisplayFormatString;
	ULONG	m_ulStartXnoteTickCounter;
	ULONG	m_ulSnapXnoteTickCounter;
	CString	m_cSnapXnoteTimesString;

};
extern sXNoteOpts cXNoteOpts;

//  == Xnote Stopwatch - End  ======================================================================


struct sWatermarkOpts
{
	sWatermarkOpts()
		: m_bAnnotation(false)
		, m_iaWatermark(TOP_LEFT)
	{
	}
	sWatermarkOpts(const sWatermarkOpts& rhs)
	{
		*this = rhs;
	}
	sWatermarkOpts& operator=(const sWatermarkOpts& rhs)
	{
		if (this == &rhs)
			return *this;

		m_bAnnotation	= rhs.m_bAnnotation;
		m_iaWatermark	= rhs.m_iaWatermark;
		return *this;
	}
	bool Read(CProfile& cProfile)
	{
		VERIFY(cProfile.Read(WATERMARKANNOTATION, m_bAnnotation));
		VERIFY(cProfile.Read(WATERMARKIMAGEATTRIBUTES, m_iaWatermark));
		return true;
	}
	bool Write(CProfile& cProfile)
	{
		VERIFY(cProfile.Write(WATERMARKANNOTATION, m_bAnnotation));
		VERIFY(cProfile.Write(WATERMARKIMAGEATTRIBUTES, m_iaWatermark));
		return true;
	}

	bool m_bAnnotation;
	ImageAttributes m_iaWatermark;
};
extern sWatermarkOpts cWatermarkOpts;


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
		, m_bPerformAutoSearch(true)
		, m_iRecordAudio(NONE)
		, m_iNumChannels(2)
		, m_iBitsPerSample(16)
		, m_iSamplesPerSeconds(22050)
		, m_iInterleaveFactor(100)
		, m_iInterleavePeriod(MILLISECONDS)
		, m_iMixerDevices(0)
		, m_iSelectedMixer(0)
		, m_iFeedbackLine(0)
		, m_iFeedbackLineInfo(0)
		, m_dwCbwFX(0)
		, m_dwWaveinSelected(WAVE_FORMAT_2S16)
		, m_pwfx(0)
		, m_wFormatTag(0)
	{
	}
	sAudioFormat(const sAudioFormat& rhs)
		: m_uDeviceID(WAVE_MAPPER)
		, m_bCompression(true)
		, m_bInterleaveFrames(true)
		, m_bUseMCI(false)
		, m_bPerformAutoSearch(true)
		, m_iRecordAudio(NONE)
		, m_iNumChannels(2)
		, m_iBitsPerSample(16)
		, m_iSamplesPerSeconds(22050)
		, m_iInterleaveFactor(100)
		, m_iInterleavePeriod(MILLISECONDS)
		, m_iMixerDevices(0)
		, m_iSelectedMixer(0)
		, m_iFeedbackLine(0)
		, m_iFeedbackLineInfo(0)
		, m_dwCbwFX(0)
		, m_dwWaveinSelected(WAVE_FORMAT_2S16)
		, m_pwfx(0)
		, m_wFormatTag(0)
	{
		*this = rhs;
	}
	virtual ~sAudioFormat()
	{
		DeleteAudio();
	}

	sAudioFormat& operator=(const sAudioFormat& rhs);

	////////////////////////////
	// TODO: need pimple idom
	////////////////////////////

	bool Read(CProfile& cProfile)
	{
		VERIFY(cProfile.Read(AUDIODEVICEID, m_uDeviceID));
		VERIFY(cProfile.Read(BAUDIOCOMPRESSION, m_bCompression));
		VERIFY(cProfile.Read(USEMCI, m_bUseMCI));
		VERIFY(cProfile.Read(PERFORMAUTOSEARCH, m_bPerformAutoSearch));		
		VERIFY(cProfile.Read(RECORDAUDIO, m_iRecordAudio));		
		VERIFY(cProfile.Read(CBWFX, m_dwCbwFX));
		VERIFY(cProfile.Read(WAVEINSELECTED, m_dwWaveinSelected));
		VERIFY(cProfile.Read(AUDIO_BITS_PER_SAMPLE, m_iBitsPerSample));
		VERIFY(cProfile.Read(AUDIO_NUM_CHANNELS, m_iNumChannels));
		VERIFY(cProfile.Read(AUDIO_SAMPLES_PER_SECONDS, m_iSamplesPerSeconds));
		VERIFY(cProfile.Read(NUMDEV, m_iMixerDevices));		
		VERIFY(cProfile.Read(SELECTEDDEV, m_iSelectedMixer));
		VERIFY(cProfile.Read(COMPRESSFORMATTAG, m_wFormatTag));
		AudioFormat().wFormatTag = static_cast<WORD>(m_wFormatTag);
		VERIFY(cProfile.Read(FEEDBACK_LINE, m_iFeedbackLine));
		VERIFY(cProfile.Read(FEEDBACK_LINE_INFO, m_iFeedbackLineInfo));		
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
		VERIFY(cProfile.Write(PERFORMAUTOSEARCH, m_bPerformAutoSearch));
		VERIFY(cProfile.Write(RECORDAUDIO, m_iRecordAudio));		
		VERIFY(cProfile.Write(CBWFX, m_dwCbwFX));
		VERIFY(cProfile.Write(WAVEINSELECTED, m_dwWaveinSelected));
		VERIFY(cProfile.Write(AUDIO_BITS_PER_SAMPLE, m_iBitsPerSample));
		VERIFY(cProfile.Write(AUDIO_NUM_CHANNELS, m_iNumChannels));
		VERIFY(cProfile.Write(AUDIO_SAMPLES_PER_SECONDS, m_iSamplesPerSeconds));
		VERIFY(cProfile.Write(NUMDEV, m_iMixerDevices));		
		VERIFY(cProfile.Write(SELECTEDDEV, m_iSelectedMixer));
		m_wFormatTag = AudioFormat().wFormatTag;
		VERIFY(cProfile.Write(COMPRESSFORMATTAG, m_wFormatTag));
		VERIFY(cProfile.Write(FEEDBACK_LINE, m_iFeedbackLine));
		VERIFY(cProfile.Write(FEEDBACK_LINE_INFO, m_iFeedbackLineInfo));		
		VERIFY(cProfile.Write(INTERLEAVEFRAMES, m_bInterleaveFrames));
		VERIFY(cProfile.Write(INTERLEAVEFACTOR, m_iInterleaveFactor));
		VERIFY(cProfile.Write(INTERLEAVEUNIT, m_iInterleavePeriod));
		return true;
	}
	bool isInput(eAudioInput eInput) const	{return eInput == m_iRecordAudio;}
	bool isAudioFormat() const				{return (m_pwfx) ? true : false;}
	WAVEFORMATEX& AudioFormat()
	{
		if (!m_pwfx) {
			VERIFY(NewAudio());
		}
		return *m_pwfx;
	}

	bool DeleteAudio();
	bool WriteAudio(const LPWAVEFORMATEX pwfx);
	void BuildRecordingFormat();

	bool m_bCompression;
	bool m_bInterleaveFrames;
	bool m_bUseMCI;
	bool m_bPerformAutoSearch;	// TODO: not used
	int m_iRecordAudio;
	int m_iNumChannels;
	int m_iBitsPerSample;
	int m_iSamplesPerSeconds;
	int m_iInterleaveFactor;
	int m_iInterleavePeriod;
	int m_iMixerDevices;
	int m_iSelectedMixer;
	int m_iFeedbackLine;
	int m_iFeedbackLineInfo;
	UINT m_uDeviceID;
	int m_wFormatTag;
	DWORD m_dwCbwFX;	// TODO; can be 
	DWORD m_dwWaveinSelected;
private:
	bool NewAudio();
	bool CopyAudio(LPWAVEFORMATEX pwfx, DWORD dwCbwFX);
	LPWAVEFORMATEX m_pwfx;
};
extern sAudioFormat cAudioFormat;

struct sProducerOpts
{
	sProducerOpts()
		: m_bLaunchPropPrompt(false)
		, m_bLaunchHTMLPlayer(true)
		, m_bDeleteAVIAfterUse(true)
	{
	}
	sProducerOpts(const sProducerOpts& rhs)
	{
		*this = rhs;
	}
	sProducerOpts& operator=(const sProducerOpts& rhs)
	{
		if (this == &rhs)
			return *this;

		m_bLaunchPropPrompt		= rhs.m_bLaunchPropPrompt;
		m_bLaunchHTMLPlayer		= rhs.m_bLaunchHTMLPlayer;
		m_bDeleteAVIAfterUse	= rhs.m_bDeleteAVIAfterUse;

		return *this;
	}
	bool Read(CProfile& cProfile)
	{
		VERIFY(cProfile.Read(LAUNCHPROPPROMPT, m_bLaunchPropPrompt));
		VERIFY(cProfile.Read(LAUNCHHTMLPLAYER, m_bLaunchHTMLPlayer));
		VERIFY(cProfile.Read(DELETEAVIAFTERUSE, m_bDeleteAVIAfterUse));
		return true;
	}
	bool Write(CProfile& cProfile)
	{
		VERIFY(cProfile.Write(LAUNCHPROPPROMPT, m_bLaunchPropPrompt));
		VERIFY(cProfile.Write(LAUNCHHTMLPLAYER, m_bLaunchHTMLPlayer));
		VERIFY(cProfile.Write(DELETEAVIAFTERUSE, m_bDeleteAVIAfterUse));
		return true;
	}

	bool m_bLaunchPropPrompt;
	bool m_bLaunchHTMLPlayer;
	bool m_bDeleteAVIAfterUse;
};
extern sProducerOpts cProducerOpts;

enum eVideoFormat
{
	ModeAVI
	, ModeFlash
};
enum eAVIPlay
{
	NO_PLAYER
	, CAM1_PLAYER
	, DEFAULT_PLAYER
	, CAM2_PLAYER
};

enum eViewType
{
	VIEW_NORMAL
	, VIEW_COMPACT
	, VIEW_BUTTON
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
		, m_bRecordPreset(false)
		, m_iPresetTime(60)
		, m_iMaxPan(20)
		, m_iRecordingMode(ModeAVI)
		, m_iLaunchPlayer(CAM2_PLAYER)
		, m_iSpecifiedDirLength(0)
		, m_iTempPathAccess(USE_WINDOWS_TEMP_DIR)
		, m_iThreadPriority(THREAD_PRIORITY_NORMAL)
		, m_iViewType(VIEW_NORMAL)
		, m_iSaveLen(0)
		, m_iCursorLen(0)
		, m_iShapeNameInt(0)
		, m_iLayoutNameInt(0)
		, m_strSpecifiedDir("")
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
		m_bRecordPreset			= rhs.m_bRecordPreset;
		m_iPresetTime			= rhs.m_iPresetTime;
		m_iMaxPan				= rhs.m_iMaxPan;
		m_iRecordingMode		= rhs.m_iRecordingMode;
		m_iLaunchPlayer			= rhs.m_bSaveSettings;
		m_iTempPathAccess		= rhs.m_iTempPathAccess;
		m_iThreadPriority		= rhs.m_iThreadPriority;
		m_iViewType				= rhs.m_iViewType;
		m_iSaveLen				= rhs.m_iSaveLen;
		m_iCursorLen			= rhs.m_iCursorLen;
		m_iShapeNameInt			= rhs.m_iShapeNameInt;
		m_iLayoutNameInt		= rhs.m_iLayoutNameInt;
		m_iSpecifiedDirLength	= rhs.m_iSpecifiedDirLength;
		m_strSpecifiedDir		= rhs.m_strSpecifiedDir;
		if (m_strSpecifiedDir.GetLength() != m_iSpecifiedDirLength) {
			m_iSpecifiedDirLength = m_strSpecifiedDir.GetLength();
		}
		return *this;
	}
	bool Read(CProfile& cProfile)
	{
		VERIFY(cProfile.Read(AUTONAMING, m_bAutoNaming));
		VERIFY(cProfile.Read(CAPTURETRANS, m_bCaptureTrans));
		VERIFY(cProfile.Read(FLASHINGRECT, m_bFlashingRect));
		VERIFY(cProfile.Read(MINIMIZEONSTART, m_bMinimizeOnStart));
		VERIFY(cProfile.Read(RECORDPRESET, m_bRecordPreset));
		VERIFY(cProfile.Read(PRESETTIME, m_iPresetTime));
		VERIFY(cProfile.Read(RECORDINGMODE, m_iRecordingMode));
		VERIFY(cProfile.Read(LAUNCHPLAYER, m_iLaunchPlayer));
		VERIFY(cProfile.Read(SPECIFIEDDIR, m_strSpecifiedDir));
		VERIFY(cProfile.Read(TEMPPATH_ACCESS, m_iTempPathAccess));
		VERIFY(cProfile.Read(THREADPRIORITY, m_iThreadPriority));
		VERIFY(cProfile.Read(AUTOPAN, m_bAutoPan));
		VERIFY(cProfile.Read(MAXPAN, m_iMaxPan));
		VERIFY(cProfile.Read(VIEWTYPE, m_iViewType));		
		VERIFY(cProfile.Read(SHAPENAMEINT, m_iShapeNameInt));		
		VERIFY(cProfile.Read(LAYOUTNAMEINT, m_iLayoutNameInt));
		// TODO: check these two
		VERIFY(cProfile.Read(LAYOUTNAMELEN, m_iSaveLen));		
		VERIFY(cProfile.Read(SHAPENAMELEN, m_iCursorLen));
		return true;
	}
	bool Write(CProfile& cProfile)
	{
		VERIFY(cProfile.Write(AUTONAMING, m_bAutoNaming));
		VERIFY(cProfile.Write(CAPTURETRANS, m_bCaptureTrans));
		VERIFY(cProfile.Write(FLASHINGRECT, m_bFlashingRect));
		VERIFY(cProfile.Write(MINIMIZEONSTART, m_bMinimizeOnStart));
		VERIFY(cProfile.Write(RECORDPRESET, m_bRecordPreset));
		VERIFY(cProfile.Write(PRESETTIME, m_iPresetTime));
		VERIFY(cProfile.Write(RECORDINGMODE, m_iRecordingMode));
		VERIFY(cProfile.Write(LAUNCHPLAYER, m_iLaunchPlayer));
		VERIFY(cProfile.Write(SPECIFIEDDIR, m_strSpecifiedDir));
		VERIFY(cProfile.Write(TEMPPATH_ACCESS, m_iTempPathAccess));
		VERIFY(cProfile.Write(THREADPRIORITY, m_iThreadPriority));
		VERIFY(cProfile.Write(AUTOPAN, m_bAutoPan));
		VERIFY(cProfile.Write(MAXPAN, m_iMaxPan));
		VERIFY(cProfile.Write(VIEWTYPE, m_iViewType));		
		VERIFY(cProfile.Write(SHAPENAMEINT, m_iShapeNameInt));		
		VERIFY(cProfile.Write(LAYOUTNAMEINT, m_iLayoutNameInt));
		// TODO: check these two
		VERIFY(cProfile.Write(LAYOUTNAMELEN, m_iSaveLen));		
		VERIFY(cProfile.Write(SHAPENAMELEN, m_iCursorLen));
		return true;
	}

	bool m_bAutoNaming;
	bool m_bCaptureTrans;
	bool m_bFlashingRect;
	bool m_bMinimizeOnStart;
	bool m_bSaveSettings;
	bool m_bAutoPan;
	bool m_bRecordPreset;
	int m_iPresetTime;
	int m_iMaxPan;
	int m_iRecordingMode;
	int m_iLaunchPlayer;
	int m_iTempPathAccess;
	int m_iThreadPriority;
	int m_iViewType;	
	int m_iSaveLen;
	int m_iCursorLen;
	int m_iShapeNameInt;
	int m_iLayoutNameInt;
	CString m_strSpecifiedDir;
private:
	int m_iSpecifiedDirLength;
};
extern sProgramOpts cProgramOpts;

extern int iSaveLen;
extern int iCursorLen;
extern int iShapeNameInt;
extern int iLayoutNameInt;
extern int iFrameShift;

//extern int iShapeNameLen;	// string length
//extern int iLayoutNameLen;	// string length

#endif	// PROFILE_H