// CStudioLib.h	- include file for CamStudio Library
/////////////////////////////////////////////////////////////////////////////
#ifndef CSTUDIOLIB_H
#define CSTUDIOLIB_H

int GetOperatingSystem();

BOOL isRectEqual(RECT a, RECT b);
void WINAPI NormalizeRect(LPRECT prc);
void FixRectSizePos(LPRECT prc,int maxxScreen, int maxyScreen);

void SetTitleBar(CString title);
BOOL WinYield(void);
CString GetProgPath();
CString FindExtension(const CString& name);
// todo: matchSpecial unreferenced
int matchSpecial(int keyCtrl,int keyShift,int keyAlt,int ctrlDown,int shiftDown,int altDown);
RECT FrameWindow(HWND hWnd, int maxxScreen, int maxyScreen, RECT rcClip);
long rounddouble(double dbl);

void SaveBitmapCopy(HBITMAP& hBitmap, HDC hdc, HDC hdcbits, const RECT& rect);
void SaveBitmapCopy(HBITMAP& hBitmap, HDC hdc, HDC hdcbits, int x, int y, int sx, int sy);
void RestoreBitmapCopy(HBITMAP& hBitmap, HDC hdc, HDC hdcbits, const RECT& rect);
void RestoreBitmapCopy(HBITMAP& hBitmap, HDC hdc, HDC hdcbits, int x, int y, int sx, int sy);
HANDLE Bitmap2Dib(HBITMAP hbitmap, UINT bits);

const int USE_WINDOWS_TEMP_DIR		= 0;
const int USE_INSTALLED_DIR			= 1;
const int USE_USER_SPECIFIED_DIR	= 2;
CString GetTempPath(int tempPath_Access, CString specifieddir);

#endif	// CSTUDIOLIB_H
