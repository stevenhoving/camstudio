// CamSys.cpp	- CamStudio Library file functions
/////////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "CamSys.h"

int GetOperatingSystem()
{
	OSVERSIONINFO osv;
	::ZeroMemory(&osv, sizeof (OSVERSIONINFO));
	osv.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	return (::GetVersionEx(&osv)) ? osv.dwMajorVersion : 0;
}

// message processing loop to allow thread to yield to other threads
BOOL WinYield()
{
	//Process 3 messages, then return false
	MSG msg;
	for (int i = 0; i < 3; i++) {
		if (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}
	}

	return FALSE;
}

LONG GetRegKey (HKEY key, LPCTSTR subkey, LPTSTR retdata)
{
	HKEY hkey;
	LONG retval = ::RegOpenKeyEx (key, subkey, 0, KEY_QUERY_VALUE, &hkey);
	if (retval == ERROR_SUCCESS)
	{
		long datasize = MAX_PATH;
		TCHAR data[MAX_PATH];
		::RegQueryValue (hkey, NULL, data, &datasize);
		// _tcscpy (retdata, data);  // Cause C4996 warning, marked as deprecation candidate
		strcpy_s(retdata, sizeof(retdata), data ); // Safe replacement
		::RegCloseKey (hkey);
	}
	return retval;
}
