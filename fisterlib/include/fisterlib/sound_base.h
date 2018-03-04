#pragma once

#include <mmsystem.h>    // for WAVEFORMATEX

class CSoundBase
{
public:
    CSoundBase();
    virtual ~CSoundBase() = default;

public:
    const WAVEFORMATEX& Format() const;
    WAVEFORMATEX& Format(const WAVEFORMATEX& rhs);
    int NumberOfChannels() const;
    int NumberOfChannels(int nchan);
    int SamplesPerSecond() const;
    int SamplesPerSecond(int sps);
    int BitsPerSample() const;
    int BitsPerSample(int bps);
    void SetBufferSize(int NumberOfSamples);
    int GetBufferSize();
protected:
    WAVEFORMATEX m_Format;
    int m_BufferSize; // number of samples
private:
    void Update();
};
