/**********************************************
 *  File: AudioCompressorFilter.cpp
 *  Desc: Represents an Audio Compressor filter
 *  Author: Alberto A. Heredia (bertoso)
 *
 **********************************************/
#include "StdAfx.h"
#include "AudioCompressorFilter.h"


CAudioCompressorFilter::CAudioCompressorFilter(CString strName)
{
	LoadFilter(CLSID_AudioCompressorCategory, strName);
}


CAudioCompressorFilter::~CAudioCompressorFilter(void)
{
}
