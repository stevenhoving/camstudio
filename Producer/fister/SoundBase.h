// SoundBase.h: interface for the CSoundBase class.
#pragma once

#include <mmsystem.h>
#include "buffer.h"

class CSoundBase
{
public:
    const WAVEFORMATEX& Format() const;
    WAVEFORMATEX& Format(const WAVEFORMATEX& rhs);
    int BufferSize() const;
    int BufferSize(int NumberOfSamples);
    int NumberOfChannels() const;
    int NumberOfChannels(int nchan);
    int SamplesPerSecond() const;
    int SamplesPerSecond(int sps);
    int BitsPerSample() const;
    int BitsPerSample(int bps);
    CSoundBase();
    virtual ~CSoundBase();

protected:
    WAVEFORMATEX m_Format;
    int             m_BufferSize;    // number of samples

private:
    void Update();
};
