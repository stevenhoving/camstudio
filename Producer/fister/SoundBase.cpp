#include "../stdafx.h"
#include "SoundBase.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

WAVEFORMATEX& CSoundBase::Format(const WAVEFORMATEX& rhs)
{
    return m_Format = rhs;
}

const WAVEFORMATEX& CSoundBase::Format() const
{
    return m_Format;
}

int CSoundBase::BufferSize(int NumberOfSamples)
{
    return m_BufferSize = NumberOfSamples;
}

int CSoundBase::BufferSize() const
{
    return m_BufferSize;
}

int CSoundBase::NumberOfChannels(int nchan)
{
    m_Format.nChannels = nchan;
    Update();
    return m_Format.nChannels;
}

int CSoundBase::NumberOfChannels() const
{
    return m_Format.nChannels;
}

int CSoundBase::SamplesPerSecond(int sps)
{
    m_Format.nSamplesPerSec = sps;
    Update();
    return m_Format.nSamplesPerSec;
}

int CSoundBase::SamplesPerSecond() const
{
    return m_Format.nSamplesPerSec;
}

int CSoundBase::BitsPerSample(int bps)
{
    m_Format.wBitsPerSample = bps; Update(); return m_Format.wBitsPerSample;
}

int CSoundBase::BitsPerSample() const
{
    return m_Format.wBitsPerSample;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSoundBase::CSoundBase()
{
    m_Format.wFormatTag = WAVE_FORMAT_PCM;
    m_Format.cbSize = 0;
    m_BufferSize = 1000; // samples per callback
    BitsPerSample(16);
    SamplesPerSecond(22050);
    NumberOfChannels(1);
}

CSoundBase::~CSoundBase()
{
}

void CSoundBase::Update()
{
    m_Format.nAvgBytesPerSec = m_Format.nSamplesPerSec * (m_Format.wBitsPerSample / 8);
    m_Format.nBlockAlign = m_Format.nChannels * (m_Format.wBitsPerSample / 8);
}

