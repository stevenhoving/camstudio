#include "Camstudio4XNote.h"

#pragma once

class CXnoteStopwatchFormat
{
public:
    static void FormatXnoteSampleString(char *cBuf128, long lDelayTimeInMilliSec, bool bDisplayCameraDelay, bool bDisplayCameraDelay2 );

    static void FormatXnoteDelayedTimeString(
        char *cBuf128,
        DWORD dwStartXnoteTickCounter,
        DWORD dwCurrTickCount,
        long lDelayTimeInMillisSec,
        bool bDisplayCameraDelay,
        bool bDisplayCameraDelay2
        );

    static void FormatXnoteInfoSourceSensor(
        char *cBuf128,
        int  iSourceInfo,
        int  iSensorInfo);

    static void FormatXnoteExtendedInfoSourceSensor(
        char *cBuf128,
        int  iSourceInfo,
        int  iSensorInfo);
};
