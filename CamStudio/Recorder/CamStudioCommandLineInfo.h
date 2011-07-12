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
	bool isStealth() const	{return m_bStealth;}
 	bool isStart() const	{return m_bStart;}
 	bool isStop() const	{return m_bStop;}
 	bool isCodecs() const	{return m_bCodecs;}
	int CodecID() const	{return m_iCodecID;}
	int Seconds() const	{return m_iSeconds;}
	int X() const	{return m_iX;}
 	int Y() const	{return m_iY;}
 	int Width() const	{return m_iWidth;}
 	int Height() const	{return m_iHeight;}
	CString OutFile() const	{return m_csOutfile;}
	bool hasOutFile() const {return m_csOutfile.GetLength()>0;}
	CString CfgFile() const	{return m_csCfgfile;}
 	bool hasCfgFile() const {return m_csCfgfile.GetLength()>0;}
 	CString LoadCodec() const	{return m_csLoadcodec;}
 	bool hasLoadCodec() const {return m_csLoadcodec.GetLength()>0;}

private:
	bool m_bHelp;
	bool m_bStealth;
 	bool m_bStart;
 	bool m_bStop;
 	bool m_bCodecs;
	int m_iCodecID;
	int m_iSeconds;
	int m_iX;
 	int m_iY;
 	int m_iWidth;
 	int m_iHeight;
	CString m_csOutfile;
	CString m_csCfgfile;
 	CString m_csLoadcodec;
};
