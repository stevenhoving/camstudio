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
	const WAVEFORMATEX& Format() const					{return m_Format;}
	WAVEFORMATEX& Format(const WAVEFORMATEX& rhs) 		{return m_Format = rhs;}
	int BufferSize() const								{return m_BufferSize;}
	int BufferSize(int NumberOfSamples)					{return m_BufferSize = NumberOfSamples;}
	int NumberOfChannels() const						{return m_Format.nChannels;}
	int NumberOfChannels(int nchan)						{m_Format.nChannels = nchan; Update(); return m_Format.nChannels;}
	int SamplesPerSecond() const						{return m_Format.nSamplesPerSec;}
	int SamplesPerSecond(int sps)						{m_Format.nSamplesPerSec = sps; Update(); return m_Format.nSamplesPerSec;}
	int BitsPerSample() const							{return m_Format.wBitsPerSample;}
	int BitsPerSample(int bps)							{m_Format.wBitsPerSample = bps; Update(); return m_Format.wBitsPerSample;}
	CSoundBase();
	virtual ~CSoundBase();

protected:
	WAVEFORMATEX m_Format;
	int			 m_BufferSize;	// number of samples

private:
	void Update()
	{
		m_Format.nAvgBytesPerSec = m_Format.nSamplesPerSec * (m_Format.wBitsPerSample/8);
		m_Format.nBlockAlign = m_Format.nChannels * (m_Format.wBitsPerSample/8);
	}
};

#endif // !defined(AFX_SOUNDBASE_H__041D24CC_3A2F_4E33_AAB9_6C98297DAC59__INCLUDED_)
