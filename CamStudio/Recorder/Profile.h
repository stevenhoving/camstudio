// Profile.h
// include file for windows profile (*ini file) classes
/*****
NEWS NEWS NEWS 10/17/2010
Config* cfg is defined in StdAfx.h . While it is disputable about it's location,
meanwhile all new neccesary settings can be accesed directly
cfg->getRoot()["MySection"]["MySubsection"]["MyValue"]
see http://www.hyperrealm.com/libconfig/libconfig_manual.html
CProfile is obsolete
****/

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

#include "CStudioLib.h"
#include "addons\Camstudio4XNote.h"
#include "addons\TextAttributes.h"
#include "addons\ImageAttributes.h"

#include <vector>

void ReadFont(Setting& s, LOGFONT& f);
void ReadIA(Setting& s, ImageAttributes& iaResult);
void ReadTA(Setting& s, TextAttributes& taResult);

void WriteFont(Setting& s, LOGFONT& f);
void WriteIA(Setting& s, ImageAttributes& iaResult);
void WriteTA(Setting& s, TextAttributes& taResult);

template <class T>
void UpdateSetting(Setting& s, const char* name, T& value, Setting::Type t) {
	if (s.exists(name))
		s[name] = value;
	else
		s.add(name, t) = value;
}

#define LEGACY_PROFILE_DISABLE

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
		, m_cStartRecordingString("")
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
		, m_cStartRecordingString("")
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
		m_cStartRecordingString		= rhs.m_cStartRecordingString;

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
		// TODO, Possible memory leak, where is the delete operation of the new below done?
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

	bool Read(Setting& cProfile)
	{
		cProfile.lookupValue("restrictVideoCodecs", m_bRestrictVideoCodecs);
		cProfile.lookupValue("AutoAdjust", m_bAutoAdjust);
		cProfile.lookupValue("ValueAdjust", m_iValueAdjust);
		cProfile.lookupValue("TimeLapse", m_iTimeLapse);
		cProfile.lookupValue("fps", m_iFramesPerSecond);
		cProfile.lookupValue("KeyFramesEvery", m_iKeyFramesEvery);
		cProfile.lookupValue("CompQuality", m_iCompQuality);
		cProfile.lookupValue("shiftType", m_iShiftType);
		cProfile.lookupValue("timeshift", m_iTimeShift);
		cProfile.lookupValue("CompFCCHandler", (unsigned&)m_dwCompfccHandler);
		cProfile.lookupValue("CompressorStateIsFor", (unsigned&)m_dwCompressorStateIsFor);
		DWORD dwSize = 0UL;
		cProfile.lookupValue("CompressorStateSize", (unsigned&)dwSize);
		State(dwSize);
		CString  m_cStartRecordingString = "";
		// m_iSelectedCompressor
		return true;
	}
	bool Write(Setting& cProfile)
	{
		UpdateSetting(cProfile,"restrictVideoCodecs", m_bRestrictVideoCodecs, Setting::TypeBoolean);
		UpdateSetting(cProfile,"AutoAdjust", m_bAutoAdjust, Setting::TypeBoolean);
		UpdateSetting(cProfile,"ValueAdjust", m_iValueAdjust, Setting::TypeInt);
		UpdateSetting(cProfile,"TimeLapse", m_iTimeLapse, Setting::TypeInt);
		UpdateSetting(cProfile,"fps", m_iFramesPerSecond, Setting::TypeInt);
		UpdateSetting(cProfile,"KeyFramesEvery", m_iKeyFramesEvery, Setting::TypeInt);
		UpdateSetting(cProfile,"CompQuality", m_iCompQuality, Setting::TypeInt);
		UpdateSetting(cProfile,"shiftType", m_iShiftType, Setting::TypeInt);
		UpdateSetting(cProfile,"timeshift", m_iTimeShift, Setting::TypeInt);
		UpdateSetting(cProfile,"CompFCCHandler", (long&)m_dwCompfccHandler, Setting::TypeInt);
		UpdateSetting(cProfile,"CompressorStateIsFor", (long&)m_dwCompressorStateIsFor, Setting::TypeInt);
		UpdateSetting(cProfile,"CompressorStateSize", (long&)m_dwCompressorStateSize, Setting::TypeInt);
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
	CString m_cStartRecordingString;
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
		: m_bFixed(false)
		, m_bMouseDrag(false)
		, m_iCaptureMode(CAPTURE_FIXED)
		, m_iLeft(100)
		, m_iTop(100)
		, m_iWidth(320)
		, m_iHeight(240)
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

		m_bFixed		= rhs.m_bFixed;
		m_bMouseDrag	= rhs.m_bMouseDrag;
		m_iCaptureMode	= rhs.m_iCaptureMode;
		m_iLeft		= rhs.m_iLeft;
		m_iTop		= rhs.m_iTop;
		m_iWidth		= rhs.m_iWidth;
		m_iHeight	= rhs.m_iHeight;
		return *this;
	}
	bool Read(Setting& cProfile)
	{
		cProfile.lookupValue("FixedCapture", m_bFixed);
		cProfile.lookupValue("SupportMouseDrag", m_bMouseDrag);
		cProfile.lookupValue("MouseCaptureMode", m_iCaptureMode);		
		cProfile.lookupValue("Left", m_iLeft);
		cProfile.lookupValue("Top", m_iTop);
		cProfile.lookupValue("Width", m_iWidth);
		cProfile.lookupValue("Height", m_iHeight);

		return true;
	}
	bool Write(Setting& cProfile)
	{
		UpdateSetting(cProfile,"FixedCapture", m_bFixed, Setting::TypeBoolean);
		UpdateSetting(cProfile,"SupportMouseDrag", m_bMouseDrag, Setting::TypeBoolean);
		UpdateSetting(cProfile,"MouseCaptureMode", m_iCaptureMode, Setting::TypeInt);		
		UpdateSetting(cProfile,"Left", m_iLeft, Setting::TypeInt);
		UpdateSetting(cProfile,"Top", m_iTop, Setting::TypeInt);
		UpdateSetting(cProfile,"Width", m_iWidth, Setting::TypeInt);
		UpdateSetting(cProfile,"Height", m_iHeight, Setting::TypeInt);
		return true;
	}

	bool isCaptureMode(const eCaptureType eType) const {return eType == m_iCaptureMode;}

	bool m_bFixed;
	bool m_bMouseDrag;
	int m_iCaptureMode;
	// TODO: should be a CRect
	int m_iLeft;			// TODO: (if not CRect)
	int m_iTop;				// TODO: (if not CRect)
	int m_iWidth;			// TODO: (if not CRect)
	int m_iHeight;			// TODO: (if not CRect)
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
	bool Read(Setting& cProfile)
	{ // from Caption group
		cProfile.lookupValue("Annotation", m_bAnnotation);
		if (cProfile.exists("TextAttributes"))
			ReadTA(cProfile["TextAttributes"],m_taCaption);
		return true;
	}
	bool Write(Setting& cProfile)
	{
		UpdateSetting(cProfile,"Annotation", m_bAnnotation,Setting::TypeBoolean);
		Setting* s;
		if (cProfile.exists("TextAttributes"))
			s = &(cProfile["TextAttributes"]);
		else
			s = &(cProfile.add("TextAttributes", Setting::TypeGroup));
		WriteTA(*s, m_taCaption);
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
	bool Read(Setting& cProfile)
	{// TimeStamp
		cProfile.lookupValue("Annotation", m_bAnnotation);
		if(cProfile.exists("TextAttributes"))
			ReadTA(cProfile["TextAttributes"],m_taTimestamp);
		return true;
	}
	bool Write(Setting& cProfile)
	{
		UpdateSetting(cProfile,"Annotation", m_bAnnotation, Setting::TypeBoolean);
		Setting* s;
		if (cProfile.exists("TextAttributes"))
			s = &(cProfile["TextAttributes"]);
		else
			s = &(cProfile.add("TextAttributes", Setting::TypeGroup));
		WriteTA(*s, m_taTimestamp);
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
		: m_bAnnotation(true)							// True, because I did not managed to read info from config	
		, m_bXnoteRemoteControlMode(true)				// Default On: We want that externals as Xnote and Video motion alerts are recognoized by Camstudio.
#else
		: m_bAnnotation(false)							// False, default as it should be with stand Camstudio
		, m_bXnoteRemoteControlMode(false)				// Default Off: Minimize the Xnote effects in standard Camstudio
#endif
		, m_taXNote(BOTTOM_LEFT)
		, m_bXnoteDisplayCameraDelayMode(true)			// Default On: Show used delay in capture
		, m_bXnoteDisplayCameraDelayDirection(true)			// Default On: Show used delay in capture
		, m_ulXnoteCameraDelayInMilliSec(175UL)			// Average delay, default 175 ms
		, m_cXnoteDisplayFormatString("(0000)..00:00:00.000")	// Default (Delay) hh:mm:ss.ttt, Not really a format. As long as the timer is not running this will be showed
		, m_bXnoteRecordDurationLimitMode(true)			// Default On: Show used delay in capture
		, m_ulXnoteRecordDurationLimitInMilliSec(1750UL)		// Average recording duration, default 1750 ms. Required otherwise recording lenght is zero
		, m_ulStartXnoteTickCounter(0)					// A non persistent member
		, m_ulSnapXnoteTickCounter(0)					// A non persistent member
		, m_cSnapXnoteTimesString("")					// A non persistent member
	{
	}
	sXNoteOpts(const sXNoteOpts& rhs)
// copy ala sVideoOpts
#ifdef CAMSTUDIO4XNOTE
		: m_bAnnotation(true)							// True, because I did not managed to read info from config	
		, m_bXnoteRemoteControlMode(true)			// Default On: We want that externals as Xnote and Video motion alerts are recognoized by Camstudio.
#else
		: m_bAnnotation(false)							// False, default as it should be with stand Camstudio
		, m_bXnoteRemoteControlMode(false)		// Default Off: Minimize the Xnote effects in standard Camstudio
#endif
		, m_taXNote(BOTTOM_LEFT)
		, m_bXnoteDisplayCameraDelayMode(true)			// Default On: Show used delay in capture
		, m_bXnoteDisplayCameraDelayDirection(true)			// Default On: Show used delay in capture
		, m_ulXnoteCameraDelayInMilliSec(175UL)			// Average delay, default 175 ms
		, m_cXnoteDisplayFormatString("(0000)..00:00:00.000")	// Default hh:mm:ss.ttt, Not really a format. As long as the timer is not running this will be showed
		, m_bXnoteRecordDurationLimitMode(true)			// Default On: Show used delay in capture
		, m_ulXnoteRecordDurationLimitInMilliSec(1750UL)	// Average recordin duration, default 1750 ms
		, m_ulStartXnoteTickCounter(0UL)				// A non persistent member
		, m_ulSnapXnoteTickCounter(0UL)					// A non persistent member
		, m_cSnapXnoteTimesString("")					// A non persistent member
	{
		*this = rhs;
	}
	sXNoteOpts& operator=(const sXNoteOpts& rhs)
	{
		if (this == &rhs)
			return *this;

		m_bAnnotation = rhs.m_bAnnotation;
		m_taXNote = rhs.m_taXNote;

		m_bXnoteRemoteControlMode= rhs.m_bXnoteRemoteControlMode;
		m_bXnoteDisplayCameraDelayMode = rhs.m_bXnoteDisplayCameraDelayMode;
		m_bXnoteDisplayCameraDelayDirection = rhs.m_bXnoteDisplayCameraDelayDirection;
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
	bool Read(Setting& cProfile)
	{ // XNote
		//TRACE("## bool Read(Setting& cProfile\n");																				
		cProfile.lookupValue("Annotation", m_bAnnotation);
		if (cProfile.exists("TextAttributes"))
			ReadTA(cProfile["TextAttributes"], m_taXNote);

		if (cProfile.exists("font"))
			ReadFont(cProfile["font"], m_taXNote.logfont);
		
		cProfile.lookupValue("RemoteControl", m_bXnoteRemoteControlMode);
		cProfile.lookupValue("DisplayCameraDelay", m_bXnoteDisplayCameraDelayMode);
		cProfile.lookupValue("DisplayCameraDelayForwards", m_bXnoteDisplayCameraDelayDirection);
		cProfile.lookupValue("CameraDelayInMilliSec", (unsigned&) m_ulXnoteCameraDelayInMilliSec);
		std::string text;
		cProfile.lookupValue("DisplayFormatString", text);
		m_cXnoteDisplayFormatString = text.c_str();
		
		cProfile.lookupValue("RecordDurationLimitMode", m_bXnoteRecordDurationLimitMode);
		cProfile.lookupValue("RecordDurationLimitInMilliSec", (unsigned&) m_ulXnoteRecordDurationLimitInMilliSec);
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
		//TRACE("## ----------------------------------------------------------------------------\n");			
		//TRACE("## m_bAnnotation : [%d]\n", m_bAnnotation   );
		//TRACE("## m_taXNote.text : [%s]\n", m_taXNote.text.GetString()   );
		//TRACE("## m_bXnoteRemoteControlMode: [%d]\n", m_bXnoteRemoteControlMode  );
		//TRACE("## m_bXnoteDisplayCameraDelayMode : [%d]\n", m_bXnoteDisplayCameraDelayMode   );
		//TRACE("## m_ulXnoteCameraDelayInMilliSec : [%lu]\n", m_ulXnoteCameraDelayInMilliSec   );
		//TRACE("## m_cXnoteDisplayFormatString : [%s]\n", m_cXnoteDisplayFormatString   );
		//TRACE("## m_bXnoteRecordDurationLimitMode : [%d]\n", m_bXnoteRecordDurationLimitMode   );
		//TRACE("## m_ulXnoteRecordDurationLimitInMilliSec : [%lu]\n", m_ulXnoteRecordDurationLimitInMilliSec   );
		//TRACE("## ----------------------------------------------------------------------------\n");			

		return true;
	}
	bool Write(Setting& cProfile)
	{
		//TRACE("## bool Write(Setting& cProfile\n");			
		//TRACE("## ----------------------------------------------------------------------------\n");			
		//TRACE("## m_bAnnotation : [%d]\n", m_bAnnotation   );
		//TRACE("## m_taXNote.text : [%s]\n", m_taXNote.text.GetString()   );
		//TRACE("## m_bXnoteRemoteControlMode: [%d]\n", m_bXnoteRemoteControlMode  );
		//TRACE("## m_bXnoteDisplayCameraDelayMode : [%d]\n", m_bXnoteDisplayCameraDelayMode   );
		//TRACE("## m_ulXnoteCameraDelayInMilliSec : [%lu]\n", m_ulXnoteCameraDelayInMilliSec   );
		//TRACE("## m_cXnoteDisplayFormatString : [%s]\n", m_cXnoteDisplayFormatString   );
		//TRACE("## m_bXnoteRecordDurationLimitMode : [%d]\n", m_bXnoteRecordDurationLimitMode   );
		//TRACE("## m_ulXnoteRecordDurationLimitInMilliSec : [%lu]\n", m_ulXnoteRecordDurationLimitInMilliSec   );
		//TRACE("## m_ulXnoteRecordDurationLimitInMilliSec : [%lu]\n", m_ulXnoteRecordDurationLimitInMilliSec   );
		//TRACE("## ----------------------------------------------------------------------------\n");			

		// Assign Xnote format string to .text because we need this to get a formatted print on the capture without pressing OK first in AnnotationEffects dialog.
		m_taXNote.text = m_cXnoteDisplayFormatString;

		// Apply default colors if back and foreground are the same
		if ( m_taXNote.backgroundColor == m_taXNote.textColor )
		{
			m_taXNote.backgroundColor = RGB(255,255,255);
			m_taXNote.textColor = RGB(0,0,0);
			m_taXNote.logfont.lfHeight = 12;
		}

		UpdateSetting(cProfile,"Annotation", m_bAnnotation, Setting::TypeBoolean);
		Setting* s;
		if (cProfile.exists("TextAttributes"))
			s = &(cProfile["TextAttributes"]);
		else
			s = &(cProfile.add("TextAttributes", Setting::TypeGroup));
		WriteTA(*s, m_taXNote);

		UpdateSetting(cProfile,"RemoteControl", m_bXnoteRemoteControlMode, Setting::TypeBoolean);
		UpdateSetting(cProfile,"DisplayCameraDelay", m_bXnoteDisplayCameraDelayMode, Setting::TypeBoolean);
		UpdateSetting(cProfile,"DisplayCameraDelayForwards", m_bXnoteDisplayCameraDelayDirection, Setting::TypeBoolean);
		UpdateSetting(cProfile,"CameraDelayInMilliSec", (long&)m_ulXnoteCameraDelayInMilliSec, Setting::TypeInt);
		std::string text(m_cXnoteDisplayFormatString);
		UpdateSetting(cProfile,"DisplayFormatString", text, Setting::TypeString);

		UpdateSetting(cProfile,"RecordDurationLimitMode", m_bXnoteRecordDurationLimitMode, Setting::TypeBoolean);
		UpdateSetting(cProfile,"RecordDurationLimitInMilliSec", (long&) m_ulXnoteRecordDurationLimitInMilliSec, Setting::TypeInt);

		// m_ulStartXnoteTickCounter is a non persistent member
		return true;
	}

	bool	m_bAnnotation;
	TextAttributes m_taXNote;

	bool	m_bXnoteRemoteControlMode;
	CButton m_CheckBoxXnoteRemoteControlMode;

	bool	m_bXnoteDisplayCameraDelayMode;
	bool	m_bXnoteDisplayCameraDelayDirection;
	ULONG	m_ulXnoteCameraDelayInMilliSec;
	CButton m_CheckBoxXnoteDisplayCameraDelayMode;
	CButton m_CheckBoxXnoteDisplayCameraDelayDirection;

	bool	m_bXnoteRecordDurationLimitMode;
	ULONG	m_ulXnoteRecordDurationLimitInMilliSec;
	CButton m_CheckBoxXnoteRecordDurationLimitMode;

	CString	m_cXnoteDisplayFormatString;
	ULONG	m_ulStartXnoteTickCounter;
	ULONG	m_ulSnapXnoteTickCounter;
	CString	m_cSnapXnoteTimesString;
	CString m_cXnoteStartEntendedInfo;

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
	bool Read(Setting& cProfile)
	{
		cProfile.lookupValue("Annotation", m_bAnnotation);
		if (cProfile.exists("ImageAttributes"))
			ReadIA(cProfile["ImageAttributes"], m_iaWatermark);
		return true;
	}
	bool Write(Setting& cProfile)
	{
		UpdateSetting(cProfile,"Annotation", m_bAnnotation, Setting::TypeBoolean);
		Setting* s;
		if (cProfile.exists("ImageAttributes"))
			s = &(cProfile["ImageAttributes"]);
		else
			s = &(cProfile.add("ImageAttributes", Setting::TypeGroup));
		WriteIA(*s, m_iaWatermark);
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

	bool Read(Setting& cProfile)
	{
		cProfile.lookupValue("AudioDeviceID", m_uDeviceID);
		cProfile.lookupValue("AudioCompression", m_bCompression);
		cProfile.lookupValue("useMCI", m_bUseMCI);
		cProfile.lookupValue("performAutoSearch", m_bPerformAutoSearch);		
		cProfile.lookupValue("RecordAudio", m_iRecordAudio);		
		cProfile.lookupValue("cbwfx", (unsigned&)m_dwCbwFX);
		cProfile.lookupValue("waveinselected", (unsigned&)m_dwWaveinSelected);
		cProfile.lookupValue("audio_bits_per_sample", m_iBitsPerSample);
		cProfile.lookupValue("audio_num_channels", m_iNumChannels);
		cProfile.lookupValue("audio_samples_per_seconds", m_iSamplesPerSeconds);
		cProfile.lookupValue("NumDev", m_iMixerDevices);		
		cProfile.lookupValue("SelectedDev", m_iSelectedMixer);
		cProfile.lookupValue("CompressionFormatTag", m_wFormatTag);
		AudioFormat().wFormatTag = static_cast<WORD>(m_wFormatTag);
		cProfile.lookupValue("feedback_line", m_iFeedbackLine);
		cProfile.lookupValue("feedback_line_info", m_iFeedbackLineInfo);		
		cProfile.lookupValue("InterleaveFrames", m_bInterleaveFrames);
		cProfile.lookupValue("InterleaveFactor", m_iInterleaveFactor);
		cProfile.lookupValue("InterleaveUnit", m_iInterleavePeriod);
		return true;
	}
	bool Write(Setting& cProfile)
	{
		UpdateSetting(cProfile,"AudioDeviceID", (long&)m_uDeviceID,Setting::TypeInt);
		UpdateSetting(cProfile,"AudioCompression", m_bCompression,Setting::TypeBoolean);
		UpdateSetting(cProfile,"useMCI", m_bUseMCI,Setting::TypeBoolean);
		UpdateSetting(cProfile,"performAutoSearch", m_bPerformAutoSearch,Setting::TypeBoolean);
		UpdateSetting(cProfile,"RecordAudio", m_iRecordAudio,Setting::TypeInt);	
		UpdateSetting(cProfile,"cbwfx", (long&)m_dwCbwFX,Setting::TypeInt);
		UpdateSetting(cProfile,"waveinselected", (long&)m_dwWaveinSelected,Setting::TypeInt);
		UpdateSetting(cProfile,"audio_bits_per_sample", m_iBitsPerSample,Setting::TypeInt);
		UpdateSetting(cProfile,"audio_num_channels", m_iNumChannels,Setting::TypeInt);
		UpdateSetting(cProfile,"audio_samples_per_seconds", m_iSamplesPerSeconds,Setting::TypeInt);
		UpdateSetting(cProfile,"NumDev", m_iMixerDevices,Setting::TypeInt);		
		UpdateSetting(cProfile,"SelectedDev", m_iSelectedMixer,Setting::TypeInt);
		m_wFormatTag = AudioFormat().wFormatTag;
		UpdateSetting(cProfile,"CompressionFormatTag", m_wFormatTag,Setting::TypeInt);
		UpdateSetting(cProfile,"feedback_line", m_iFeedbackLine,Setting::TypeInt);
		UpdateSetting(cProfile,"feedback_line_info", m_iFeedbackLineInfo,Setting::TypeInt);		
		UpdateSetting(cProfile,"InterleaveFrames", m_bInterleaveFrames,Setting::TypeBoolean);
		UpdateSetting(cProfile,"InterleaveFactor", m_iInterleaveFactor,Setting::TypeInt);
		UpdateSetting(cProfile,"InterleaveUnit", m_iInterleavePeriod,Setting::TypeInt);
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
	bool Read(Setting& cProfile)
	{
		cProfile.lookupValue("launchPropPrompt", m_bLaunchPropPrompt);
		cProfile.lookupValue("launchHTMLPlayer", m_bLaunchHTMLPlayer);
		cProfile.lookupValue("deleteAVIAfterUse", m_bDeleteAVIAfterUse);
		return true;
	}
	bool Write(Setting& cProfile)
	{
		UpdateSetting(cProfile,"launchPropPrompt", m_bLaunchPropPrompt,Setting::TypeBoolean);
		UpdateSetting(cProfile,"launchHTMLPlayer", m_bLaunchHTMLPlayer,Setting::TypeBoolean);
		UpdateSetting(cProfile,"deleteAVIAfterUse", m_bDeleteAVIAfterUse,Setting::TypeBoolean);
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
	bool Read(Setting& cProfile)
	{
		cProfile.lookupValue("AutoNaming", m_bAutoNaming);
		cProfile.lookupValue("CaptureTrans", m_bCaptureTrans);
		cProfile.lookupValue("FlashingRect", m_bFlashingRect);
		cProfile.lookupValue("MinimizeOnStart", m_bMinimizeOnStart);
		cProfile.lookupValue("RecordPreset", m_bRecordPreset);
		cProfile.lookupValue("PresetTime", m_iPresetTime);
		cProfile.lookupValue("RecordingMode", m_iRecordingMode);
		cProfile.lookupValue("LaunchPlayer", m_iLaunchPlayer);
		std::string text;
		if (cProfile.lookupValue("SaveDir", text))
			m_strSpecifiedDir = text.c_str();
		cProfile.lookupValue("TempPathAccess", m_iTempPathAccess);
		cProfile.lookupValue("ThreadPriority", m_iThreadPriority);
		cProfile.lookupValue("AutoPan", m_bAutoPan);
		cProfile.lookupValue("MaxPan", m_iMaxPan);
		cProfile.lookupValue("ViewType", m_iViewType);		
		// TODO: libconfig supports arrays, so the following should be removed in the future
		cProfile.lookupValue("ShapeNameInt", m_iShapeNameInt);		
		cProfile.lookupValue("LayoutNameInt", m_iLayoutNameInt);
		// TODO: check these two
		cProfile.lookupValue("LayoutNameLen", m_iSaveLen);		
		cProfile.lookupValue("ShapeNameLen", m_iCursorLen);
		return true;
	}
	bool Write(Setting& cProfile)
	{
		UpdateSetting(cProfile,"AutoNaming", m_bAutoNaming, Setting::TypeBoolean);
		UpdateSetting(cProfile,"CaptureTrans", m_bCaptureTrans, Setting::TypeBoolean);
		UpdateSetting(cProfile,"FlashingRect", m_bFlashingRect, Setting::TypeBoolean);
		UpdateSetting(cProfile,"MinimizeOnStart", m_bMinimizeOnStart, Setting::TypeBoolean);
		UpdateSetting(cProfile,"RecordPreset", m_bRecordPreset, Setting::TypeBoolean);
		UpdateSetting(cProfile,"PresetTime", m_iPresetTime, Setting::TypeInt);
		UpdateSetting(cProfile,"RecordingMode", m_iRecordingMode, Setting::TypeInt);
		UpdateSetting(cProfile,"LaunchPlayer", m_iLaunchPlayer, Setting::TypeInt);
		std::string text(m_strSpecifiedDir);
		UpdateSetting(cProfile,"SaveDir", text, Setting::TypeString);
		UpdateSetting(cProfile,"TempPathAccess", m_iTempPathAccess, Setting::TypeInt);
		UpdateSetting(cProfile,"ThreadPriority", m_iThreadPriority, Setting::TypeInt);
		UpdateSetting(cProfile,"AutoPan", m_bAutoPan, Setting::TypeBoolean);
		UpdateSetting(cProfile,"MaxPan", m_iMaxPan, Setting::TypeInt);
		UpdateSetting(cProfile,"ViewType", m_iViewType, Setting::TypeInt);		
		UpdateSetting(cProfile,"ShapeNameInt", m_iShapeNameInt, Setting::TypeInt);		
		UpdateSetting(cProfile,"LayoutNameInt", m_iLayoutNameInt, Setting::TypeInt);
		// TODO: check these two
		UpdateSetting(cProfile,"LayoutNameLen", m_iSaveLen, Setting::TypeInt);		
		UpdateSetting(cProfile,"ShapeNameLen", m_iCursorLen, Setting::TypeInt);
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