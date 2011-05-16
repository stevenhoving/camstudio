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
	static void CXnoteStopwatchFormat::FormatXnoteSampleString(char *cBuf128, long lDelayTimeInMilliSec, bool bDisplayCameraDelay, bool bDisplayCameraDelay2 );

	static void CXnoteStopwatchFormat::FormatXnoteDelayedTimeString(
		char *cBuf128, 
		DWORD dwStartXnoteTickCounter, 
		DWORD dwCurrTickCount, 
		long lDelayTimeInMillisSec, 
		bool bDisplayCameraDelay,
		bool bDisplayCameraDelay2
		);

	static void CXnoteStopwatchFormat::FormatXnoteInfoSourceSensor(
		char *cBuf128, 
		int  iSourceInfo,
		int  iSensorInfo);

	static void CXnoteStopwatchFormat::FormatXnoteExtendedInfoSourceSensor(
		char *cBuf128, 
		int  iSourceInfo,
		int  iSensorInfo);

};
#endif	// XNOTESTOPWATCHFORMAT_H
