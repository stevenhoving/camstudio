// CamWindow.cpp	- CamStudio Library window functions
/////////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "CamWindow.h"

// TODO: should be in mainframe class
#pragma message("SetTitleBar depreciated; use MainFrame class")
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

/////////////////////////////////////////////////////////////////////////////
// FrameWindow()
// Highlight the window frame
/////////////////////////////////////////////////////////////////////////////
#pragma message("FrameWindow obsolete")
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

	HDC hdc = ::GetWindowDC(hWnd);
	::GetWindowRect(hWnd, &rectWin);

	RECT rectFrame = rectWin;
	::OffsetRect(&rectFrame, -rectFrame.left, -rectFrame.top);

	if (!IsRectEmpty(&rectFrame)) {
		HBRUSH newbrush = (HBRUSH) ::CreateHatchBrush(HS_BDIAGONAL, RGB(0, 0, 100));
		HBRUSH oldbrush = (HBRUSH) ::SelectObject(hdc, newbrush);

		const int DINV = 3;
		::PatBlt(hdc, rcClip.left, rcClip.top, rcClip.right - rcClip.left, DINV, PATINVERT);
		::PatBlt(hdc, rcClip.left, rcClip.bottom - DINV, DINV, -(rcClip.bottom - rcClip.top -2 * DINV), PATINVERT);
		::PatBlt(hdc, rcClip.right - DINV, rcClip.top + DINV, DINV, rcClip.bottom - rcClip.top - 2 * DINV, PATINVERT);
		::PatBlt(hdc, rcClip.right, rcClip.bottom - DINV, -(rcClip.right - rcClip.left), DINV, PATINVERT);

		::SelectObject(hdc,oldbrush);
		::DeleteObject(newbrush);
	}

	::ReleaseDC(hWnd, hdc);

	return rectWin;
}

