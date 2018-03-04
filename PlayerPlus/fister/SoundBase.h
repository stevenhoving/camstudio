#pragma once

#include <mmsystem.h>
#include "buffer.h"

class CSoundBase
{
public:
    WAVEFORMATEX* GetFormat();
    int GetBufferSize();
    void SetBufferSize(int NumberOfSamples);
    int GetNumberOfChannels();
    void SetNumberOfChannels(int nchan);
    int GetSamplesPerSecond();
    void SetSamplesPerSecond(int sps);
    int GetBitsPerSample();
    void SetBitsPerSample(int bps);
    CSoundBase();
    virtual ~CSoundBase();

protected:
    WAVEFORMATEX m_Format;
    int m_BufferSize;    // number of samples

private:
    void Update();
};
