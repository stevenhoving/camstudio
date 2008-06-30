// SoundBase.cpp: implementation of the CSoundBase class.
//
//////////////////////////////////////////////////////////////////////

#include "../stdafx.h"
#include "SoundBase.h"

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
	m_Format.wFormatTag	= WAVE_FORMAT_PCM;
	m_Format.cbSize		= 0;
	m_BufferSize		= 1000;  // samples per callback
	SetBitsPerSample(16);
	SetSamplesPerSecond(22050);
	SetNumberOfChannels(1);
}

CSoundBase::~CSoundBase()
{

}

void CSoundBase::SetBitsPerSample(int bps)
{
	m_Format.wBitsPerSample = bps;
	Update();
}

int CSoundBase::GetBitsPerSample()
{
	return m_Format.wBitsPerSample;	
}

void CSoundBase::SetSamplesPerSecond(int sps)
{
	m_Format.nSamplesPerSec = sps;
	Update();
}

int CSoundBase::GetSamplesPerSecond()
{
	return m_Format.nSamplesPerSec;
}

void CSoundBase::SetNumberOfChannels(int nchan)
{
	m_Format.nChannels = nchan;
	Update();
}

int CSoundBase::GetNumberOfChannels()
{
	return m_Format.nChannels;
}

void CSoundBase::Update()
{
	m_Format.nAvgBytesPerSec	= m_Format.nSamplesPerSec*(m_Format.wBitsPerSample/8);
	m_Format.nBlockAlign		= m_Format.nChannels     *(m_Format.wBitsPerSample/8);
}

void CSoundBase::SetBufferSize(int NumberOfSamples)
{
	m_BufferSize = NumberOfSamples;
}

int CSoundBase::GetBufferSize()
{
	return m_BufferSize;
}

WAVEFORMATEX* CSoundBase::GetFormat()
{
	return &m_Format;
}

