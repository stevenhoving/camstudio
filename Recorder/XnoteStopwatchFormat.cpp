// XnoteStopwatchFormat.cpp : implementation file
/////////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "XnoteStopwatchFormat.h"

/*
CXnoteStopwatchFormat::CXnoteStopwatchFormat(void)
{
}


CXnoteStopwatchFormat::~CXnoteStopwatchFormat(void)
{
}
*/

void CXnoteStopwatchFormat::FormatXnoteSampleString(char *cBuf128, long lDelayTimeInMilliSec, bool bDisplayCameraDelay, bool bDisplayCameraDelay2 )
{
	// Format (delay) hh:mm:ss.ttt	
	FormatXnoteDelayedTimeString( cBuf128, 0, 0 ,lDelayTimeInMilliSec, bDisplayCameraDelay, bDisplayCameraDelay2 );
}


// Formatting relative time since start to hh:mm:ss.ttt with or without delay information
void CXnoteStopwatchFormat::FormatXnoteDelayedTimeString(
		char *cBuf128, 
		DWORD dwStartXnoteTickCounter, 
		DWORD dwCurrTickCount, 
		long lDelayTimeInMillisSec, 
		bool bDisplayCameraDelay,
		bool bDisplayCameraDelay2)
{
	// Formatting hh:mm:ss.ttt with or without delay information
	//TRACE("## FormatXnoteDelayedTimeString  lDelayTimeInMillisSec :[%d] bShowDelayTimeWithMarker[%d]\n", lDelayTimeInMillisSec, bShowDelayTimeWithMarker );

//HIER_BEN_IK

	// JaHo: It appears that time event information from Xnote is delayed and Xnote time we have to write in the frame should be added instead of subtracted.
	// And to allow that we can define delaytime in two directions it must become a long instead of an ulong in all code.
	// (This is wrong direction) DWORD dwSubstractCorrTime = dwStartXnoteTickCounter + lDelayTimeInMillisSec;
	
	ULONG ulPassedTime = 0;
	ULONG ulTickTimeWholeSeconds = 0;
	ULONG ulTickTimeRest = 0;

	ULONG ulTickTimeDays = 0;
	ULONG ulTickTimeHour = 0;
	ULONG ulTickMinutes = 0;
	ULONG ulTickTimeSeconds = 0;
	//ULONG ulTickTimeHunderds = 0;
	ULONG ulTickTimeThousands = 0;

	DWORD dwSubstractCorrTime = 0;

	// Define Delay derection, forward or backwards
	if ( bDisplayCameraDelay2 ) {
		dwSubstractCorrTime = dwStartXnoteTickCounter + lDelayTimeInMillisSec;
	} else {
		dwSubstractCorrTime = dwStartXnoteTickCounter - lDelayTimeInMillisSec;
	}

	if ( dwSubstractCorrTime <= dwCurrTickCount ) 
	{
		ulPassedTime = ULONG(dwCurrTickCount - dwSubstractCorrTime);
			
		ulTickTimeWholeSeconds = ulPassedTime/1000 ;
		ulTickTimeThousands = ulPassedTime-ulTickTimeWholeSeconds*1000;
		//ulTickTimeHunderds = ulTickTimeThousands/10;   // Round to floor not to ceiling because we do not add an extra second here 
		
		ulTickTimeRest = ulTickTimeWholeSeconds;

		ulTickTimeDays = (ulTickTimeRest/86400);
		ulTickTimeRest -= (ulTickTimeDays*86400);

		ulTickTimeHour = (ulTickTimeRest/3600);
		ulTickTimeRest -= (ulTickTimeHour*3600);

		ulTickMinutes = (ulTickTimeRest/60);
		ulTickTimeRest -= (ulTickMinutes*60);

		ulTickTimeSeconds = ulTickTimeRest;
	}
		
	// Format (delay) hh:mm:ss.ttt	
	if ( bDisplayCameraDelay ){
		(void) sprintf( cBuf128, "(%c%04lu)  %02lu:%02lu:%02lu.%03lu",   
			bDisplayCameraDelay2 ? '-' : '+' ,
			lDelayTimeInMillisSec, 
			ulTickTimeHour, ulTickMinutes, ulTickTimeSeconds, ulTickTimeThousands);
	} else {
		(void) sprintf( cBuf128, "%02lu:%02lu:%02lu.%03lu", 
			ulTickTimeHour, ulTickMinutes, ulTickTimeSeconds, ulTickTimeThousands);
	}
	////TRACE("## FormatXnoteDelayedTimeString  Formatted relative time:[%s]\n", cBuf128);
}

// Attach info about source and sensor after the buf string
void CXnoteStopwatchFormat::FormatXnoteInfoSourceSensor(
		char *cBuf128, 
		int  iSourceInfo,
		int  iSensorInfo)
{
	char cSourceTag = '.';
	char cSensorTag = '.';

	switch ( iSourceInfo )
	{
		case XNOTE_SOURCE_UNDEFINED:
			cSourceTag = '-';
			break;
		case XNOTE_SOURCE_XNOTESTOPWATCH:
			cSourceTag = 'x';
			break;
		case XNOTE_SOURCE_MOTIONDETECTOR:			
			cSourceTag = 'v';
			break;
	}

	switch ( iSensorInfo )
	{
		case XNOTE_TRIGGER_UNDEFINED	:
		case XNOTE_TRIGGER_STOPWATCH_UNDEFINED	:
			cSensorTag = '-';
			break;
		case XNOTE_TRIGGER_STOPWATCH_MANUAL	:
			cSensorTag = 'm';
			break;
		case XNOTE_TRIGGER_STOPWATCH_DEVICE	:
		case XNOTE_TRIGGER_MOTIONDETECTOR	:
			cSensorTag = 'a';
			break;
	}

	(void) sprintf( cBuf128, "%s%c%c", cBuf128, cSourceTag, cSensorTag);
}

// Attach info about source and sensor after the buf string
void CXnoteStopwatchFormat::FormatXnoteExtendedInfoSourceSensor(
		char *cBuf128, 
		int  iSourceInfo,
		int  iSensorInfo)
{
	CString cSourceTag = _T("");
	CString cSensorTag = _T("");

	switch ( iSourceInfo )
	{
						   //123456789012345
		case XNOTE_SOURCE_UNDEFINED:
			cSourceTag = _T("UndefinedSource");
			break;
		case XNOTE_SOURCE_XNOTESTOPWATCH:
			cSourceTag = _T("XNote-Stopwatch");
			break;
		case XNOTE_SOURCE_MOTIONDETECTOR:			
			cSourceTag = _T("Motion-Detector");
			break;
		default:
			cSourceTag = _T("---------------");
			break;
	}

	switch ( iSensorInfo )
	{
						   //1234567
		case XNOTE_TRIGGER_UNDEFINED	:
		case XNOTE_TRIGGER_STOPWATCH_UNDEFINED	:
			cSensorTag = _T("Undef.");
			break;
		case XNOTE_TRIGGER_STOPWATCH_MANUAL	:
			cSensorTag = _T("Manual");
			break;
		case XNOTE_TRIGGER_STOPWATCH_DEVICE	:
		case XNOTE_TRIGGER_MOTIONDETECTOR	:
			cSensorTag = _T("Sensor");
			break;
		default:
			cSourceTag = _T("------");
			break;
	}

	(void) sprintf( cBuf128, "%s%s %s", cBuf128, cSourceTag, cSensorTag);
}

