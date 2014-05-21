// MouseCaptureWnd.h
// TODO: Put into a CWnd class
/////////////////////////////////////////////////////////////////////////////
#ifndef MOUSECAPTUREWND_H
#define MOUSECAPTUREWND_H

#pragma once

// MouseCaptureWndProc referenced variables

extern int iDefineMode;
extern BOOL bCapturing;
extern BOOL bAllowNewRecordStartKey;

extern HBITMAP hSavedBitmap;

extern HWND hWndGlobal;
extern HWND hMouseCaptureWnd;
extern HWND hFixedRegionWnd;

long WINAPI MouseCaptureWndProc(HWND hWnd, UINT wMessage, WPARAM wParam, LPARAM lParam);
bool CreateShiftWindow();
int DestroyShiftWindow();
void MoveShiftWindow();
void DrawSelect(HDC hdc, BOOL fDraw, LPRECT lprClip);
void ClearScreen();
#endif	// MOUSECAPTUREWND_H
