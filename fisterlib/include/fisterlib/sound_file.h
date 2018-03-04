#pragma once

#include "sound_base.h"
#include <string>

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
    CSoundFile(const std::string &FileName, WAVEFORMATEX *format = nullptr);
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
    HMMIO       m_hFile;
    std::string m_FileName;
    EREADWRITE  m_Mode;
    MMCKINFO    m_MMCKInfoData;
    MMCKINFO    m_MMCKInfoParent;
    MMCKINFO    m_MMCKInfoChild;
};


