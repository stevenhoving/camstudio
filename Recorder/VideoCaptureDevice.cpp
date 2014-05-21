#include "StdAfx.h"
#include "VideoCaptureDevice.h"


CVideoCaptureDevice::CVideoCaptureDevice(CString strName)
{
	LoadFilter(CLSID_VideoInputDeviceCategory, strName);
}


CVideoCaptureDevice::~CVideoCaptureDevice(void)
{
}
