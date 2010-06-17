#include "StdAfx.h"
#include "CamError.h"
#include <strsafe.h>		// for StringCchPrintf

/////////////////////////////////////////////////////////////////////////////
// OnError
// Windows Error handler
// Gets the last windows error and then resets the error; gets the string
// associated with the error and displays a messagebox of the error
/////////////////////////////////////////////////////////////////////////////
void OnError(LPCSTR lpszFunction)
{
	// Retrieve the system error message for the last-error code
	DWORD dwError = ::GetLastError();
	if (ERROR_SUCCESS == dwError) {
		return;
	}
	TRACE(_T("OnError: %s: %u\n"), lpszFunction, dwError);
	::SetLastError(ERROR_SUCCESS);	// reset the error

	LPVOID lpMsgBuf = 0;
	DWORD dwLen = ::FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER
		| FORMAT_MESSAGE_FROM_SYSTEM
		| FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		dwError,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR) &lpMsgBuf,
		0, NULL );

	if (0 == dwLen) {
		TRACE(_T("OnError: FormatMessage error: %ud\n"), ::GetLastError());
		::SetLastError(ERROR_SUCCESS);	// reset the error
		return;
	}
	// Display the error message and exit the process
	LPVOID lpDisplayBuf = (LPVOID)::LocalAlloc(LMEM_ZEROINIT, (lstrlen((LPCTSTR)lpMsgBuf) + lstrlen((LPCTSTR)lpszFunction) + 40) * sizeof(TCHAR));
	if (!lpDisplayBuf) {
		TRACE(_T("OnError: LocalAlloc error: %ud\n"), ::GetLastError());
		::SetLastError(ERROR_SUCCESS);	// reset the error
		::LocalFree(lpMsgBuf);
		return;
	}
	HRESULT hr = StringCchPrintf((LPTSTR)lpDisplayBuf, ::LocalSize(lpDisplayBuf) / sizeof(TCHAR), TEXT("%s failed with error %d: %s"), lpszFunction, dwError, lpMsgBuf);
	if (SUCCEEDED(hr)) {
		//::MessageBox(0, (LPCTSTR)lpDisplayBuf, TEXT("Error"), MB_OK);
		TRACE(_T("OnError: : %s\n"), lpDisplayBuf);
	} else {
		TRACE(_T("OnError: StringCchPrintf error: %ud\n"), ::GetLastError());
		::SetLastError(ERROR_SUCCESS);	// reset the error
	}

	::LocalFree(lpMsgBuf);
	::LocalFree(lpDisplayBuf);
}

void ErrMsg(char frmt[], ...)
{
	DWORD written;
	char buf[5000];
	va_list val;

	va_start(val, frmt);
	// wvsprintf(buf, frmt, val);   // C4995 warning, function marked as deprecated once
	_vstprintf_s(buf, frmt, val);	// Save replacement

	const COORD _80x50 = {80,50};
	static BOOL startup = (AllocConsole(), SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), _80x50));
	WriteConsole(GetStdHandle(STD_OUTPUT_HANDLE), buf, lstrlen(buf), &written, 0);
}

int MessageOut(HWND hWnd,long strMsg, long strTitle, UINT mbstatus)
{
	CString tstr("");
	CString mstr("");
	VERIFY(tstr.LoadString(strTitle));
	VERIFY(mstr.LoadString(strMsg));

	return ::MessageBox(hWnd,mstr,tstr,mbstatus);
}

int MessageOut(HWND hWnd, long strMsg, long strTitle, UINT mbstatus, long val)
{
	CString tstr("");
	CString mstr("");
	CString fstr("");
	VERIFY(tstr.LoadString(strTitle));
	VERIFY(mstr.LoadString(strMsg));
	fstr.Format(mstr,val);

	return ::MessageBox(hWnd,fstr,tstr,mbstatus);
}

int MessageOut(HWND hWnd, long strMsg, long strTitle, UINT mbstatus, long val1, long val2)
{
	CString tstr("");
	CString mstr("");
	CString fstr("");
	VERIFY(tstr.LoadString(strTitle));
	VERIFY(mstr.LoadString(strMsg));
	fstr.Format(mstr,val1,val2);

	return ::MessageBox(hWnd,fstr,tstr,mbstatus);
}

