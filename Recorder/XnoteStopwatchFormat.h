// CXnoteStopwatchFormat dialog
//
/////////////////////////////////////////////////////////////////////////////
#ifndef XNOTESTOPWATCHFORMAT_H	// belt and suspenders
#define XNOTESTOPWATCHFORMAT_H

#include "Camstudio4XNote.h"

#pragma once

class CXnoteStopwatchFormat
{
public:
	CXnoteStopwatchFormat(void);
	~CXnoteStopwatchFormat(void);

public:
	static void CXnoteStopwatchFormat::FormatXnoteSampleString(char *cBuf64, long lDelayTimeInMilliSec, bool bDisplayCameraDelay );

	static void CXnoteStopwatchFormat::FormatXnoteDelayedTimeString(
		char *cBuf64, 
		DWORD dwStartXnoteTickCounter, 
		DWORD dwCurrTickCount, 
		long lDelayTimeInMillisSec, 
		bool bDisplayCameraDelay );

};
#endif	// XNOTESTOPWATCHFORMAT_H
