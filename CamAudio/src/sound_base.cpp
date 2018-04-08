#include "stdafx.h"
#include "CamAudio/sound_base.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

CSoundBase::CSoundBase()
{
    m_BufferSize = 1000; // samples per callback
    m_Format.wFormatTag = WAVE_FORMAT_PCM;
    m_Format.cbSize = 0;
    m_Format.wBitsPerSample = 1024; // Buffer Size 1024 bits

    BitsPerSample(16);
    SamplesPerSecond(22050);
    NumberOfChannels(1);
}

WAVEFORMATEX& CSoundBase::Format(const WAVEFORMATEX& rhs)
{
    return m_Format = rhs;
}

const WAVEFORMATEX& CSoundBase::Format() const
{
    return m_Format;
}

int CSoundBase::NumberOfChannels(int nchan)
{
    m_Format.nChannels = WORD(nchan);    // Cast, eliminate L4 warning
    Update();
    return m_Format.nChannels;
}

int CSoundBase::NumberOfChannels() const
{
    return m_Format.nChannels;
}

int CSoundBase::SamplesPerSecond(int sps)
{
    m_Format.nSamplesPerSec = (DWORD)sps;
    // What is this?
    Update();
    return m_Format.nSamplesPerSec;
}

int CSoundBase::SamplesPerSecond() const
{
    return m_Format.nSamplesPerSec;
}

int CSoundBase::BitsPerSample(int bps)
{
    m_Format.wBitsPerSample = (WORD)bps;
    Update();
    return m_Format.wBitsPerSample;
}

int CSoundBase::BitsPerSample() const
{
    return m_Format.wBitsPerSample;
}

void CSoundBase::SetBufferSize(int NumberOfSamples)
{
    m_BufferSize = NumberOfSamples;
}

int CSoundBase::GetBufferSize()
{
    return m_BufferSize;
}

void CSoundBase::Update()
{
    m_Format.nAvgBytesPerSec = m_Format.nSamplesPerSec * (m_Format.wBitsPerSample / 8);
    m_Format.nBlockAlign = m_Format.nChannels * (m_Format.wBitsPerSample / 8);
}
