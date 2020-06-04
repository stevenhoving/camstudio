#pragma once

// general error handler
void OnError(const TCHAR *lpszFunction);
int MessageOut(HWND hWnd, long strMsg, long strTitle, UINT mbstatus);
int MessageOut(HWND hWnd, long strMsg, long strTitle, UINT mbstatus, long val);
int MessageOut(HWND hWnd, long strMsg, long strTitle, UINT mbstatus, long val1, long val2);
