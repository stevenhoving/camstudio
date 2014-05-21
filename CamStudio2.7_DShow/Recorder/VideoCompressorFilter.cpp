#include "StdAfx.h"
#include "VideoCompressorFilter.h"


CVideoCompressorFilter::CVideoCompressorFilter(CString strName)
{
	LoadFilter(CLSID_VideoCompressorCategory, strName);
}


CVideoCompressorFilter::~CVideoCompressorFilter(void)
{
}
