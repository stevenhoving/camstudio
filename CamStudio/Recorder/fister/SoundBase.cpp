// SoundBase.cpp: implementation of the CSoundBase class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SoundBase.h"

#pragma message("CamStudio\\fister\\SoundBase.cpp")

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSoundBase::CSoundBase()
{
	m_Format.wFormatTag = WAVE_FORMAT_PCM;
	m_Format.cbSize = 0;
	m_Format.wBitsPerSample = 1024; // Buffer Size 1024 bits

	BitsPerSample(16);
	SamplesPerSecond(22050);
	NumberOfChannels(1);
}

CSoundBase::~CSoundBase()
{
}

