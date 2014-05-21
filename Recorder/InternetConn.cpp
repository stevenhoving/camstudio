#include "StdAfx.h"
#include "InternetConn.h"

CInternetConn::CInternetConn(void)	
{
		m_pSharedInternetSession = 0;
		
		if (!m_pSharedInternetSession)
			m_pSharedInternetSession=new CInternetSession;
	}

CInternetConn::~CInternetConn(void)
{
	if (m_pSharedInternetSession)
	{
		m_pSharedInternetSession->Close();
		delete m_pSharedInternetSession;
		m_pSharedInternetSession = 0;
	}
}
BOOL CInternetConn::ReadFile(CString strUrl , CString &strFileContent)
{
	CStdioFile* pFile=0;
	strFileContent="";
	if(!m_pSharedInternetSession)
		return FALSE;
	try
	{
		m_pSharedInternetSession->SetOption(INTERNET_OPTION_CONNECT_TIMEOUT, 20000);
		pFile=m_pSharedInternetSession->OpenURL(strUrl,	1, INTERNET_FLAG_TRANSFER_ASCII|INTERNET_FLAG_DONT_CACHE);
		if (!pFile)
			return FALSE;
			
		char buff[1025];
		UINT len=0;
		while ((len=pFile->Read(buff, 1024)))
		{
			buff[len]=0;
			strFileContent+=buff;
		}
		pFile->Close();
		delete pFile;
	}
	catch (CInternetException* e)
	{			
		e->Delete();
		return FALSE;
	}
	return TRUE;
}
