#pragma once
#include <afxinet.h>
class CInternetConn
{
public:
	CInternetConn(void);
	~CInternetConn(void);
	BOOL ReadFile(CString strUrl , CString &strFileContent);
protected:
	CInternetSession* m_pSharedInternetSession;	
};

