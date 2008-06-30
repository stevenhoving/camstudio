// SoundFile.h: interface for the CSoundFile class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SOUNDFILE_H__5536C963_E9A9_44AE_844A_DAE974E3C457__INCLUDED_)
#define AFX_SOUNDFILE_H__5536C963_E9A9_44AE_844A_DAE974E3C457__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "soundbase.h"

typedef enum{
	FILE_ERROR = 0,
	READ = 1,
	WRITE = 2
}EREADWRITE;

class CSoundFile : public CSoundBase
{
public:
	bool IsOK();
	void Close();
	EREADWRITE GetMode();
	bool Read(CBuffer* buffer);
	CBuffer* Read();
	bool Write(CBuffer* buffer);
	CSoundFile(CString FileName, WAVEFORMATEX* format = NULL);
	virtual ~CSoundFile();


protected:
	bool OpenWaveFile();
	bool CreateWaveFile();
	
private:
	HMMIO		m_hFile;
	CString		m_FileName;
	EREADWRITE	m_Mode;
	MMCKINFO	m_MMCKInfoData;
	MMCKINFO	m_MMCKInfoParent;
	MMCKINFO	m_MMCKInfoChild;
};

#endif // !defined(AFX_SOUNDFILE_H__5536C963_E9A9_44AE_844A_DAE974E3C457__INCLUDED_)
