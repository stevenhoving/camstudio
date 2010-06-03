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

void CXnoteStopwatchFormat::FormatXnoteSampleString(char *cBuf64, long lDelayTimeInMilliSec, bool bDisplayCameraDelay )
{
	// Format (delay) hh:mm:ss.ttt	
	FormatXnoteDelayedTimeString( cBuf64, 0, 0 ,lDelayTimeInMilliSec, bDisplayCameraDelay );
}


// Formatting relative time since start to hh:mm:ss.ttt with or without delay information
void CXnoteStopwatchFormat::FormatXnoteDelayedTimeString(
		char *cBuf64, 
		DWORD dwStartXnoteTickCounter, 
		DWORD dwCurrTickCount, 
		long lDelayTimeInMillisSec, 
		bool bDisplayCameraDelay )
{
	// Formatting hh:mm:ss.ttt with or without delay information
	//TRACE("## FormatXnoteDelayedTimeString  lDelayTimeInMillisSec :[%d] bShowDelayTimeWithMarker[%d]\n", lDelayTimeInMillisSec, bShowDelayTimeWithMarker );

	DWORD dwSubstractCorrTime = dwStartXnoteTickCounter + lDelayTimeInMillisSec;
	
	ULONG ulPassedTime = 0;
	ULONG ulTickTimeWholeSeconds = 0;
	ULONG ulTickTimeRest = 0;

	ULONG ulTickTimeDays = 0;
	ULONG ulTickTimeHour = 0;
	ULONG ulTickMinutes = 0;
	ULONG ulTickTimeSeconds = 0;
	//ULONG ulTickTimeHunderds = 0;
	ULONG ulTickTimeThousands = 0;

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
		(void) sprintf( cBuf64, "(%04lu)  %02lu:%02lu:%02lu.%03lu",   
			lDelayTimeInMillisSec, 
			ulTickTimeHour, ulTickMinutes, ulTickTimeSeconds, ulTickTimeThousands);
	} else {
		(void) sprintf( cBuf64, "%02lu:%02lu:%02lu.%03lu", 
			ulTickTimeHour, ulTickMinutes, ulTickTimeSeconds, ulTickTimeThousands);
	}
	////TRACE("## FormatXnoteDelayedTimeString  Formatted relative time:[%s]\n", cBuf64);
}
