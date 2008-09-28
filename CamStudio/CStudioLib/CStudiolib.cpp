// CStudioLib.cpp	- implementation file for CamStudio Library 
/////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "CStudioLib.h"

BOOL isRectEqual(RECT a, RECT b)
{
	CRect rectA(a);
	return rectA == b;

	//if ((a.left == b.left)
	//	&& (a.right == b.right)
	//	&& (a.top == b.top)
	//	&& (a.bottom == b.bottom))
	//	return TRUE;
	//else
	//	return FALSE;
}

// The rectangle is normalized for fourth-quadrant positioning, which Windows typically uses for coordinates. 
// this is dubious
void WINAPI NormalizeRect(LPRECT prc)
{
	CRect rectSrc(*prc);
	rectSrc.NormalizeRect();
	::CopyRect(prc, rectSrc);

//#define SWAP(x,y) ((x) ^= (y) ^= (x) ^= (y))
//	if (prc->right < prc->left)
//		SWAP(prc->right, prc->left);
//	if (prc->bottom < prc->top)
//		SWAP(prc->bottom, prc->top);
//#undef SWAP
}

int GetOperatingSystem()
{
	OSVERSIONINFO osv;
	osv.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

	if (GetVersionEx(&osv)) {
		return osv.dwMajorVersion;
	}

	return 0;
}

// todo: should be in mainframe class
void SetTitleBar(CString title)
{
	CWinApp* app = AfxGetApp();
	if (app) {
		HWND mainwnd = app->m_pMainWnd->m_hWnd;
		if (mainwnd) {
			::SetWindowText(mainwnd, LPCTSTR(title));
		}
	}
}

BOOL WinYield(void)
{
	//Process 3 messages, then return false
	MSG msg;
	for (int i=0;i<3; i++) {
		if (PeekMessage(&msg,NULL,0,0,PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return FALSE;
}

CString GetProgPath()
{
	// locals
	//CFile converter;
	//CString result;

	// get root
	TCHAR szTemp[300];
	GetModuleFileName(NULL, szTemp, 300);
	CString path = szTemp;
	if (0 < path.ReverseFind('\\')) {
		path = path.Left(path.ReverseFind('\\'));
	}
	return path;
}

CString FindExtension(const CString& name)
{
	int len = name.GetLength();
	for (int i = len-1; i >= 0; i--) {
		if (name[i] == '.') {
			return name.Mid(i+1);
		}
	}
	return "";
}
// todo: unreferences
int matchSpecial(int keyCtrl,int keyShift,int keyAlt,int ctrlDown,int shiftDown,int altDown)
{
	int ret = 1;

	if ((keyCtrl) && (!ctrlDown))
		ret = 0;

	if ((keyShift) && (!shiftDown))
		ret = 0;

	if ((keyAlt) && (!altDown))
		ret = 0;

	if ((!keyCtrl) && (ctrlDown))
		ret = 0;

	if ((!keyShift) && (shiftDown))
		ret = 0;

	if ((!keyAlt) && (altDown))
		ret = 0;

	//if ((keyCtrl) && (ctrlDown)) {
	//	if ((keyShift) && (shiftDown)) {
	//		if ((keyAlt) && (altDown)) {
	//		} else if ((!keyAlt) && (!altDown)) {
	//		} else if (keyAlt) {
	//			ret = 0;
	//		}
	//	} else if ((!keyShift) && (!shiftDown)) {
	//		if ((keyAlt) && (altDown)) {
	//		} else if ((!keyAlt) && (!altDown)) {
	//		} else if (keyAlt) {
	//			ret = 0;
	//		}
	//	} else if (keyShift) {
	//		ret = 0;
	//	}
	//} else if ((!keyCtrl) && (!ctrlDown)) {
	//	if ((keyShift) && (shiftDown)) {
	//		if ((keyAlt) && (altDown)) {
	//		} else if ((!keyAlt) && (!altDown)) {
	//		} else if (keyAlt) {
	//			ret = 0;
	//		}
	//	} else if ((!keyShift) && (!shiftDown)) {
	//		if ((keyAlt) && (altDown)) {
	//		} else if ((!keyAlt) && (!altDown)) {
	//		} else if (keyAlt) {
	//			ret = 0;
	//		}
	//	} else if (keyShift) {
	//		ret = 0;
	//	}
	//} else if (keyCtrl) {
	//	ret = 0;
	//}

	return ret;
}

void FixRectSizePos(LPRECT prc,int maxxScreen, int maxyScreen)
{
	NormalizeRect(prc);

	int width=((prc->right)-(prc->left))+1;
	int height=((prc->bottom)-(prc->top))+1;

	if (width>maxxScreen) {
		prc->left=0;
		prc->right=maxxScreen-1;
	}

	if (height>maxyScreen) {
		prc->top=0;
		prc->bottom=maxyScreen-1;
	}

	if (prc->left <0) {
		prc->left= 0;
		prc->right=width-1;
	}

	if (prc->top <0) {
		prc->top= 0;
		prc->bottom=height-1;
	}

	if (prc->right > maxxScreen-1) {
		prc->right = maxxScreen-1;
		prc->left= maxxScreen-width;
	}

	if (prc->bottom > maxyScreen-1) {
		prc->bottom = maxyScreen-1;
		prc->top= maxyScreen-height;
	}
}

/////////////////////////////////////////////////////////////////////////////
// Function: FrameWindow()
//
// Purpose: Highlight the window frame
/////////////////////////////////////////////////////////////////////////////
RECT FrameWindow(HWND hWnd, int maxxScreen, int maxyScreen, RECT rcClip)
{
	RECT rectWin;
	rectWin.left = 0;
	rectWin.top = 0;
	rectWin.right = maxxScreen - 1;
	rectWin.bottom = maxyScreen - 1;

	if (!IsWindow(hWnd)) {
		return rectWin;
	}

	HDC hdc = GetWindowDC(hWnd);
	GetWindowRect(hWnd, &rectWin);

	RECT rectFrame = rectWin;
	OffsetRect(&rectFrame, -rectFrame.left, -rectFrame.top);

	if (!IsRectEmpty(&rectFrame)) {
		HBRUSH newbrush = (HBRUSH) CreateHatchBrush(HS_BDIAGONAL, RGB(0, 0, 100));
		HBRUSH oldbrush = (HBRUSH) SelectObject(hdc, newbrush);

		const int DINV = 3;
		PatBlt(hdc, rcClip.left, rcClip.top, rcClip.right - rcClip.left, DINV, PATINVERT);
		PatBlt(hdc, rcClip.left, rcClip.bottom - DINV, DINV, -(rcClip.bottom - rcClip.top -2 * DINV), PATINVERT);
		PatBlt(hdc, rcClip.right - DINV, rcClip.top + DINV, DINV, rcClip.bottom - rcClip.top - 2 * DINV, PATINVERT);
		PatBlt(hdc, rcClip.right, rcClip.bottom - DINV, -(rcClip.right - rcClip.left), DINV, PATINVERT);

		SelectObject(hdc,oldbrush);
		DeleteObject(newbrush);
	}

	ReleaseDC(hWnd, hdc);

	return rectWin;
}

HANDLE Bitmap2Dib(HBITMAP hbitmap, UINT bits)
{
	BITMAP bitmap;
	GetObject(hbitmap, sizeof(BITMAP), &bitmap);

	// DWORD align the width of the DIB
	// Figure out the size of the colour table
	// Calculate the size of the DIB
	UINT wLineLen = (bitmap.bmWidth * bits + 31)/32 * 4;
	DWORD wColSize = sizeof(RGBQUAD) * ((bits <= 8) ? 1 << bits : 0);
	DWORD dwSize = sizeof(BITMAPINFOHEADER)
		+ wColSize
		+ (DWORD)(UINT)wLineLen * (DWORD)(UINT)bitmap.bmHeight;

	// Allocate room for a DIB and set the LPBI fields
	HANDLE hdib = GlobalAlloc(GHND, dwSize);
	if (!hdib)
		return hdib;

	LPBITMAPINFOHEADER lpbi = (LPBITMAPINFOHEADER)GlobalLock(hdib);
	lpbi->biSize = sizeof(BITMAPINFOHEADER);
	lpbi->biWidth = bitmap.bmWidth;
	lpbi->biHeight = bitmap.bmHeight;
	lpbi->biPlanes = 1;
	lpbi->biBitCount = (WORD) bits;
	lpbi->biCompression = BI_RGB;
	lpbi->biSizeImage = dwSize - sizeof(BITMAPINFOHEADER) - wColSize;
	lpbi->biXPelsPerMeter = 0;
	lpbi->biYPelsPerMeter = 0;
	lpbi->biClrUsed = (bits <= 8) ? 1<<bits : 0;
	lpbi->biClrImportant = 0;

	// Get the bits from the bitmap and stuff them after the LPBI
	LPBYTE lpBits = (LPBYTE)(lpbi + 1) + wColSize;

	HDC hdc = CreateCompatibleDC(NULL);
	GetDIBits(hdc, hbitmap, 0, bitmap.bmHeight, lpBits, (LPBITMAPINFO)lpbi, DIB_RGB_COLORS);
	lpbi->biClrUsed = (bits <= 8) ? 1<<bits : 0;

	DeleteDC(hdc);
	GlobalUnlock(hdib);

	return hdib;
}

//Round positive numbers
long rounddouble(double dbl)
{
	long num = (long) dbl;
	double diff = dbl - (double) num;
	if (diff>=0.5)
		num++;

	return num;
}


CString GetTempPath(int tempPath_Access, CString specifieddir)
{
	if (tempPath_Access == USE_WINDOWS_TEMP_DIR) {
		char dirx[_MAX_PATH];
		GetWindowsDirectory(dirx, _MAX_PATH);
		CString tempdir;
		tempdir.Format("%s\\temp", dirx);

		//Verify the chosen temp path is valid

		WIN32_FIND_DATA wfd;
		//memset(&wfd, 0, sizeof (wfd));
		::ZeroMemory(&wfd, sizeof (wfd));
		HANDLE hdir = FindFirstFile(LPCTSTR(tempdir), &wfd);
		if (!hdir) {
			return GetProgPath();
		}
		FindClose(hdir);

		//If valid directory, return Windows\temp as temp directory
		if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
			return tempdir;
		}

		//else return program path as temp directory
		tempPath_Access = USE_INSTALLED_DIR;
		return GetProgPath();
	} else if (tempPath_Access == USE_USER_SPECIFIED_DIR) {
		CString tempdir;
		tempdir = specifieddir;

		//Verify the chosen temp path is valid
		WIN32_FIND_DATA wfd;
		memset(&wfd, 0, sizeof (wfd));
		HANDLE hdir = FindFirstFile(LPCTSTR(tempdir), &wfd);
		if (!hdir) {
			return GetProgPath();
		}
		FindClose(hdir);

		//If valid directory, return Windows\temp as temp directory
		if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
			return tempdir;
		}

		//set temp path to installed directory
		tempPath_Access = USE_INSTALLED_DIR;
	}
	return GetProgPath();
}

