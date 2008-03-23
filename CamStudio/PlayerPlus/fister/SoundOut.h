// SoundOut.h: interface for the CSoundOut class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SOUNDOUT_H__F926DE0E_4F1A_4A8C_B189_5E4D63446A95__INCLUDED_)
#define AFX_SOUNDOUT_H__F926DE0E_4F1A_4A8C_B189_5E4D63446A95__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "soundbase.h"

				// CWinThread must be before CSoundBase -> and it's very difficult to debug!
				// Trust me!   Thomas.Holme@openmpeg4.org
class CSoundOut : public CWinThread, public CSoundBase
{
//	DECLARE_DYNCREATE(CSoundOut)

public:
	CSoundOut();
	virtual ~CSoundOut();


	// pointer to function
	void (*GetDataToSoundOut)(CBuffer* buffer, void* Owner);
	void* m_pOwner;

	virtual void Stop();
	virtual bool Start(WAVEFORMATEX* format = NULL);
	static void waveOutErrorMsg(MMRESULT result, LPCTSTR addstr);

	afx_msg void OnMM_WOM_DONE(UINT parm1, LONG parm2);
	DECLARE_MESSAGE_MAP()

protected:
	bool m_bPlaying;
	HWAVEOUT m_hPlay;
	DWORD m_ThreadID;
	int m_QueuedBuffers;

	BOOL InitInstance();
	int AddOutputBufferToQueue(CBuffer* buffer);
};

#endif // !defined(AFX_SOUNDOUT_H__F926DE0E_4F1A_4A8C_B189_5E4D63446A95__INCLUDED_)
