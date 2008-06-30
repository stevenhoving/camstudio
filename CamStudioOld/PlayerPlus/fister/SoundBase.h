// SoundBase.h: interface for the CSoundBase class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SOUNDBASE_H__041D24CC_3A2F_4E33_AAB9_6C98297DAC59__INCLUDED_)
#define AFX_SOUNDBASE_H__041D24CC_3A2F_4E33_AAB9_6C98297DAC59__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

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
	int			 m_BufferSize;	// number of samples

private:
	void Update();
};

#endif // !defined(AFX_SOUNDBASE_H__041D24CC_3A2F_4E33_AAB9_6C98297DAC59__INCLUDED_)
