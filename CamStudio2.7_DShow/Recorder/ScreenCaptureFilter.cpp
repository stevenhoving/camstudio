#include "StdAfx.h"
#include "ScreenCaptureFilter.h"


CScreenCaptureFilter::CScreenCaptureFilter(CString strName)
{
	LoadFilter(CLSID_VideoInputDeviceCategory, strName);
}


CScreenCaptureFilter::~CScreenCaptureFilter(void)
{
}
