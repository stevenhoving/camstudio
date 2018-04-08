/////////////////////////////////////////////////////////////////////////////
// todo: add dimiri's command line processing
//"Usage:"
//"-codec: which codec to use"
//"-outfile: .avi file to write to"
//"-seconds: how many seconds to record for ('0' means to record until a key is pressed)"
//"-help: this screen"
/////////////////////////////////////////////////////////////////////////////

#pragma once

class CCamStudioCommandLineInfo : public CCommandLineInfo
{
public:
    CCamStudioCommandLineInfo();
    ~CCamStudioCommandLineInfo() override;

    CCamStudioCommandLineInfo(const CCamStudioCommandLineInfo&) = delete;
    CCamStudioCommandLineInfo& operator=(const CCamStudioCommandLineInfo&) = delete;

    void ParseParam(const TCHAR *pszParam, BOOL bFlag, BOOL bLast) override;

    bool isHelp() const;
    int CodecID() const;
    int Seconds() const;
    CString OutFile() const;

private:
    CString m_csOutfile;
    int m_iCodecID;
    int m_iSeconds;
    bool m_bHelp;
};
