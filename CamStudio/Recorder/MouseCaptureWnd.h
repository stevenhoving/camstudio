// MouseCaptureWnd.h
// TODO: Put into a CWnd class
/////////////////////////////////////////////////////////////////////////////
#ifndef MOUSECAPTUREWND_H
#define MOUSECAPTUREWND_H

#pragma once

// MouseCaptureWndProc referenced variables
extern RECT rc;
extern RECT rcClip;
extern RECT old_rcClip;
extern RECT rcOffset;
extern RECT rcUse;

extern int iDefineMode;
extern POINT ptOrigin;
extern BOOL bCapturing;
extern BOOL bAllowNewRecordStartKey;

extern HBITMAP hSavedBitmap;

extern HWND hWndGlobal;
extern HWND hMouseCaptureWnd;
extern HWND hFixedRegionWnd;

extern long WINAPI MouseCaptureWndProc(HWND hWnd, UINT wMessage, WPARAM wParam, LPARAM lParam);
extern int CreateShiftWindow();
extern int DestroyShiftWindow();

void DrawSelect(HDC hdc, BOOL fDraw, LPRECT lprClip);

#endif	// MOUSECAPTUREWND_H
