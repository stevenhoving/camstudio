#include "stdafx.h"
#include <CamLib/CamError.h>
#include <strsafe.h> // for StringCchPrintf

/////////////////////////////////////////////////////////////////////////////
// OnError
// Windows Error handler
// Gets the last windows error and then resets the error; gets the string
// associated with the error and displays a messagebox of the error
/////////////////////////////////////////////////////////////////////////////

void OnError(const TCHAR *lpszFunction)
{
    WCHAR   wszMsgBuff[512];
    DWORD   dwChars;

    // Retrieve the system error message for the last-error code
    DWORD dwError = ::GetLastError();
    if (ERROR_SUCCESS == dwError)
    {
        return;
    }
    TRACE(_T("OnError: %ws: %u\n"), lpszFunction, dwError);
    ::SetLastError(ERROR_SUCCESS); // reset the error

    dwChars = FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        dwError,
        0,
        wszMsgBuff,
        512,
        NULL);

    if (0 == dwChars)
    {
        // The error code did not exist in the system errors.
        // Try Ntdsbmsg.dll for the error code.

        HINSTANCE hInst;

        // Load the library.
        hInst = ::LoadLibrary(L"Ntdsbmsg.dll");
        if (NULL == hInst)
        {
            printf("cannot load Ntdsbmsg.dll\n");
            exit(1);  // Could 'return' instead of 'exit'.
        }

        // Try getting message text from ntdsbmsg.
        dwChars = ::FormatMessage(FORMAT_MESSAGE_FROM_HMODULE |
            FORMAT_MESSAGE_IGNORE_INSERTS,
            hInst,
            dwError,
            0,
            wszMsgBuff,
            512,
            NULL);

        // Free the library.
        ::FreeLibrary(hInst);
    }

    TRACE(_T("OnError value: %d Message: %ws\n"),
        dwError,
        dwChars ? wszMsgBuff : L"Error message not found.");
}

void ErrorMsg(const TCHAR *frmt, ...)
{
    DWORD written = 0;
    TCHAR buf[5000];
    va_list val;

    va_start(val, frmt);
    _vstprintf_s(buf, frmt, val); // Save replacement
    va_end(val);

    const COORD _80x50 = {80, 50};
    static BOOL startup = (AllocConsole(), SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), _80x50));
    WriteConsole(GetStdHandle(STD_OUTPUT_HANDLE), buf, (DWORD)lstrlen(buf), &written, nullptr);
}

int MessageOut(HWND hWnd, long strMsg, long strTitle, UINT mbstatus)
{
    CString tstr("");
    CString mstr("");
    VERIFY(tstr.LoadString((UINT)strTitle));
    VERIFY(mstr.LoadString((UINT)strMsg));

    return ::MessageBox(hWnd, mstr, tstr, mbstatus);
}

int MessageOut(HWND hWnd, long strMsg, long strTitle, UINT mbstatus, long val)
{
    CString tstr("");
    CString mstr("");
    CString fstr("");
    VERIFY(tstr.LoadString((UINT)strTitle));
    VERIFY(mstr.LoadString((UINT)strMsg));
    fstr.Format(mstr, val);

    return ::MessageBox(hWnd, fstr, tstr, mbstatus);
}

int MessageOut(HWND hWnd, long strMsg, long strTitle, UINT mbstatus, long val1, long val2)
{
    CString tstr("");
    CString mstr("");
    CString fstr("");
    VERIFY(tstr.LoadString((UINT)strTitle));
    VERIFY(mstr.LoadString((UINT)strMsg));
    fstr.Format(mstr, val1, val2);

    return ::MessageBox(hWnd, fstr, tstr, mbstatus);
}
