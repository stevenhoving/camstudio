// SoundBase.h: interface for the CSoundBase class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SOUNDBASE_H__041D24CC_3A2F_4E33_AAB9_6C98297DAC59__INCLUDED_)
#define AFX_SOUNDBASE_H__041D24CC_3A2F_4E33_AAB9_6C98297DAC59__INCLUDED_

#pragma message("CamStudio\\fister\\SoundBase.h")

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <mmsystem.h>	// for WAVEFORMATEX

class CSoundBase
{
public:
	CSoundBase();
	virtual ~CSoundBase();

public:
	// New
	const WAVEFORMATEX& Format() const					{return m_Format;}
	WAVEFORMATEX& Format(const WAVEFORMATEX& rhs) 		{return m_Format = rhs;}
	int NumberOfChannels() const						{return m_Format.nChannels;}
	int NumberOfChannels(int nchan)
	{
		m_Format.nChannels = WORD(nchan);	// Cast, eliminate L4 warning
		Update();
		return m_Format.nChannels;
	}
	int SamplesPerSecond() const						{return m_Format.nSamplesPerSec;}
	int SamplesPerSecond(int sps)
	{
		return m_Format.nSamplesPerSec = sps;
		// What is this?
		//Update();
		//return m_Format.nSamplesPerSec;
	}
	int BitsPerSample() const							{return m_Format.wBitsPerSample;}
	int BitsPerSample(int bps)
	{
		m_Format.wBitsPerSample = WORD(bps);
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

#endif // !defined(AFX_SOUNDBASE_H__041D24CC_3A2F_4E33_AAB9_6C98297DAC59__INCLUDED_)
