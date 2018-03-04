#pragma once

#include <mmsystem.h>    // for WAVEFORMATEX

class CSoundBase
{
public:
    CSoundBase();
    virtual ~CSoundBase();

public:
    const WAVEFORMATEX& Format() const
    {
        return m_Format;
    }

    WAVEFORMATEX& Format(const WAVEFORMATEX& rhs)
    {
        return m_Format = rhs;
    }

    int NumberOfChannels() const
    {
        return m_Format.nChannels;
    }

    int NumberOfChannels(int nchan)
    {
        m_Format.nChannels = WORD(nchan);    // Cast, eliminate L4 warning
        Update();
        return m_Format.nChannels;
    }
    int SamplesPerSecond() const
    {
        return m_Format.nSamplesPerSec;
    }

    int SamplesPerSecond(int sps)
    {
        m_Format.nSamplesPerSec = (DWORD)sps;
        // What is this?
        Update();
        return m_Format.nSamplesPerSec;
    }

    int BitsPerSample() const
    {
        return m_Format.wBitsPerSample;
    }

    int BitsPerSample(int bps)
    {
        m_Format.wBitsPerSample = (WORD)bps;
        Update();
        return m_Format.wBitsPerSample;
    }

protected:
    WAVEFORMATEX m_Format;

private:
    void Update()
    {
        m_Format.nAvgBytesPerSec = m_Format.nSamplesPerSec * (m_Format.wBitsPerSample/8);
        m_Format.nBlockAlign = m_Format.nChannels * (m_Format.wBitsPerSample/8);
    }
};
