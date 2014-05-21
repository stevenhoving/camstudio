/**********************************************
 *  File: AudioCaptureFilter.cpp
 *  Desc: Represents an Audio Capture device
 *  Author: Alberto A. Heredia (bertoso)
 *
 **********************************************/
#include "StdAfx.h"
#include "AudioCaptureFilter.h"


CAudioCaptureFilter::CAudioCaptureFilter(CString strName)
{
	LoadFilter(CLSID_AudioInputDeviceCategory, strName);
}

CAudioCaptureFilter::~CAudioCaptureFilter(void)
{
}
