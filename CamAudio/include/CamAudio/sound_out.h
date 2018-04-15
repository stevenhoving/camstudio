#pragma once

#include "sound_base.h"

class CBuffer;

// CWinThread must be before CSoundBase -> and it's very difficult to debug!
// Trust me!   Thomas.Holme@openmpeg4.org
class CSoundOut : public CWinThread, public CSoundBase
{
    // DECLARE_DYNCREATE(CSoundOut)

public:
    CSoundOut();
    virtual ~CSoundOut();

    // pointer to function
    void (*GetDataToSoundOut)(CBuffer *buffer, void *Owner);
    void *m_pOwner;

    virtual void Stop();
    virtual bool Start(WAVEFORMATEX *format = nullptr);
    static void waveOutErrorMsg(MMRESULT result, LPCTSTR addstr);

    afx_msg void OnMM_WOM_DONE(WPARAM parm1, LPARAM parm2);
    DECLARE_MESSAGE_MAP()

protected:
    BOOL InitInstance();
    int AddOutputBufferToQueue(CBuffer *buffer);

protected:
    bool m_bPlaying;
    HWAVEOUT m_hPlay;
    DWORD m_ThreadID;
    int m_QueuedBuffers;
};
