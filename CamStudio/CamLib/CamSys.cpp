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

