#include "stdafx.h"
#include "CamLib/CamSys.h"

int GetOperatingSystem()
{
    OSVERSIONINFO osv;
    ::ZeroMemory(&osv, sizeof(OSVERSIONINFO));
    osv.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
    return (::GetVersionEx(&osv)) ? osv.dwMajorVersion : 0;
}

// message processing loop to allow thread to yield to other threads
BOOL WinYield()
{
    // Process 3 messages, then return false
    MSG msg;
    for (int i = 0; i < 3; i++)
    {
        if (::PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
        }
    }

    return false;
}

LONG GetRegKey(HKEY key, LPCTSTR subkey, LPTSTR retdata)
{
    HKEY hkey;
    LONG retval = ::RegOpenKeyEx(key, subkey, 0, KEY_QUERY_VALUE, &hkey);
    if (retval == ERROR_SUCCESS)
    {
        long datasize = MAX_PATH;
        TCHAR data[MAX_PATH];
        ::RegQueryValue(hkey, nullptr, data, &datasize);
        // _tcscpy (retdata, data);  // Cause C4996 warning, marked as deprecation candidate
        _tcscpy_s(retdata, sizeof(retdata), data); // Safe replacement
        ::RegCloseKey(hkey);
    }
    return retval;
}
