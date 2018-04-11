#pragma once

// general error handler
void OnError(const TCHAR *lpszFunction);
extern void ErrorMsg(const char *frmt, ...);
extern int MessageOut(HWND hWnd, long strMsg, long strTitle, UINT mbstatus);
extern int MessageOut(HWND hWnd, long strMsg, long strTitle, UINT mbstatus, long val);
extern int MessageOut(HWND hWnd, long strMsg, long strTitle, UINT mbstatus, long val1, long val2);
