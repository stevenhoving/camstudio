#pragma once

// general error handler
void OnError(const TCHAR *lpszFunction);
void ErrorMsg(const TCHAR *frmt, ...);
int MessageOut(HWND hWnd, long strMsg, long strTitle, UINT mbstatus);
int MessageOut(HWND hWnd, long strMsg, long strTitle, UINT mbstatus, long val);
int MessageOut(HWND hWnd, long strMsg, long strTitle, UINT mbstatus, long val1, long val2);
