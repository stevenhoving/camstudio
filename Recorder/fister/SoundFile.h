#pragma once


#include "soundbase.h"

enum EREADWRITE{
    FILE_ERROR = 0,
    READ = 1,
    WRITE = 2
};

// forward declaration
class CBuffer;

class CSoundFile : public CSoundBase
{
public:
    CSoundFile(CString FileName, WAVEFORMATEX* format = NULL);
    virtual ~CSoundFile();

    bool IsOK();
    void Close();
    EREADWRITE GetMode();
    bool Read(CBuffer* buffer);
    CBuffer* Read();
    bool Write(CBuffer* buffer);

protected:
    bool OpenWaveFile();
    bool CreateWaveFile();
    
private:
    HMMIO        m_hFile;
    CString        m_FileName;
    EREADWRITE    m_Mode;
    MMCKINFO    m_MMCKInfoData;
    MMCKINFO    m_MMCKInfoParent;
    MMCKINFO    m_MMCKInfoChild;
};


