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
	CCamStudioCommandLineInfo(void);
	virtual ~CCamStudioCommandLineInfo(void);

	void ParseParam(const TCHAR* pszParam, BOOL bFlag, BOOL bLast);

	bool isHelp() const	{return m_bHelp;}
	int CodecID() const	{return m_iCodecID;}
	int Seconds() const	{return m_iSeconds;}
	CString OutFile() const	{return m_csOutfile;}

private:
	bool m_bHelp;
	int m_iCodecID;
	int m_iSeconds;
	CString m_csOutfile;
};
