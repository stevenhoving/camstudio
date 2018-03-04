#pragma once

#include "sound_base.h"

#include <MMSystem.h>

class CBuffer;

// CWinThread must be before CSoundBase -> and it's very difficult to debug!
// Trust me!   Thomas.Holme@openmpeg4.org
class CSoundIn : public CWinThread, public CSoundBase
{
    DECLARE_DYNCREATE(CSoundIn)

public:
    CSoundIn();
    virtual ~CSoundIn();

    // pointer to callback function
    void (*DataFromSoundIn)(CBuffer* buffer, void* Owner);
    void* m_pOwner;

    virtual void Stop();
    virtual bool Start(WAVEFORMATEX* format = nullptr);
    static void waveInErrorMsg(MMRESULT result, LPCTSTR addstr);

    afx_msg void OnMM_WIM_DATA(WPARAM parm1, LPARAM parm2);
    DECLARE_MESSAGE_MAP()

protected:
    BOOL InitInstance();
    int AddInputBufferToQueue();

protected:
    bool m_bRecording;
    HWAVEIN m_hRecord;
    int m_QueuedBuffers;
    DWORD m_ThreadID;
};
