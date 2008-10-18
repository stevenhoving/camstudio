// MouseCaptureWnd.cpp : implementation file
//
/////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "Recorder.h"
#include "MouseCaptureWnd.h"
#include "RecorderView.h"
#include "CStudioLib.h"

// MouseCaptureWndProc referenced functions

/////////////////////////////////////////////////////////////////////////////
// DrawSelect
// Draws the selected clip rectangle with its dimensions on the DC
/////////////////////////////////////////////////////////////////////////////
void DrawSelect(HDC hdc, BOOL fDraw, LPRECT lprClip)
{
	if (!lprClip || IsRectEmpty(lprClip))
	{
		return;
	}
	CRect rectDraw(lprClip);

	// If a rectangular clip region has been selected, draw it
	HBRUSH newbrush = (HBRUSH) CreateHatchBrush(HS_BDIAGONAL, RGB(0,0,100));
	HBRUSH oldbrush = (HBRUSH) SelectObject(hdc, newbrush);

	//PatBlt SRCINVERT regardless fDraw is TRUE or FALSE
	const int DINV = 3;
	PatBlt(hdc, rectDraw.left, rectDraw.top, rectDraw.Width(), DINV, PATINVERT);
	PatBlt(hdc, rectDraw.left, rectDraw.bottom - DINV, DINV, -(rectDraw.Height() - 2 * DINV), PATINVERT);
	PatBlt(hdc, rectDraw.right - DINV, rectDraw.top + DINV, DINV, rectDraw.Height() - 2 * DINV, PATINVERT);
	PatBlt(hdc, rectDraw.right, rectDraw.bottom - DINV, -rectDraw.Width(), DINV, PATINVERT);

	SelectObject(hdc,oldbrush);
	DeleteObject(newbrush);

	HDC hdcBits = CreateCompatibleDC(hdc);
	HFONT newfont = (HFONT) GetStockObject(ANSI_VAR_FONT);
	HFONT oldfont = (HFONT) SelectObject(hdc, newfont);

	CString strSize;
	strSize.Format("Left : %d Top : %d Width : %d Height : %d", rectDraw.left, rectDraw.top, rectDraw.Width() + 1, rectDraw.Height() + 1);
	SIZE sExtent;
	DWORD dw = GetTextExtentPoint(hdc, (LPCSTR)strSize, strSize.GetLength(), &sExtent);

	int dx = sExtent.cx;
	int dy = sExtent.cy;
	int x = rectDraw.left + 10;
	int y = (rectDraw.top < (dy + DINV + 2))
		? rectDraw.bottom + DINV + 2
		: rectDraw.top - dy - DINV - 2;

	if (fDraw) {
		//Save Original Picture
		SaveBitmapCopy(hSavedBitmap, hdc, hdcBits, x - 4, y - 4, dx + 8, dy + 8);

		//Text
		COLORREF oldtextcolor = SetTextColor(hdc,RGB(0,0,0));
		COLORREF oldbkcolor = SetBkColor(hdc,RGB(255,255,255));
		SetBkMode(hdc,TRANSPARENT);
		RoundRect(hdc, x - 4, y - 4, x + dx + 4, y + dy + 4, 10, 10);
		SetBkMode(hdc,OPAQUE);

		ExtTextOut(hdc, x, y, 0, NULL, (LPCSTR)strSize, strSize.GetLength(), NULL);
		SetBkColor(hdc,oldbkcolor);
		SetTextColor(hdc,oldtextcolor);
		SelectObject(hdc, oldfont);
	} else {
		RestoreBitmapCopy(hSavedBitmap, hdc, hdcBits, x - 4, y - 4, dx + 8, dy + 8);
	}

	//Icon
	if ((35 < (rectDraw.Width() - 10)) && ((dy + 40) < (rectDraw.Height() - 10)))
	{
		HBITMAP hbv = LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BITMAP1));
		HBITMAP old_bitmap = (HBITMAP) SelectObject(hdcBits, hbv);
		BitBlt(hdc, rectDraw.left + 10, rectDraw.bottom - 42, 30, 32, hdcBits, 0, 0, SRCINVERT);
		SelectObject(hdcBits, old_bitmap);
		DeleteObject(hbv);
	}

	DeleteDC(hdcBits);
}

/////////////////////////////////////////////////////////////////////////////
//
// MouseCaptureWndProc()
//
/////////////////////////////////////////////////////////////////////////////
long WINAPI MouseCaptureWndProc(HWND hWnd, UINT wMessage, WPARAM wParam, LPARAM lParam)
{
	switch (wMessage)
	{
	default:
		//TRACE("MouseCaptureWndProc : skip message\n");
		break;
	case WM_MOUSEMOVE:
		{
			TRACE("MouseCaptureWndProc : WM_MOUSEMOVE\n");
			if (iMouseCaptureMode == 0) {
				//Fixed Region
				POINT pt;
				GetCursorPos(&pt);

				rcClip.left = pt.x + rcOffset.left; // Update rect with new mouse info
				rcClip.top = pt.y + rcOffset.top;
				rcClip.right = pt.x + rcOffset.right;
				rcClip.bottom = pt.y + rcOffset.bottom;

				if (rcClip.left < 0) {
					rcClip.left = 0;
					rcClip.right = (rc.right - rc.left);
				}
				if (rcClip.top < 0) {
					rcClip.top = 0;
					rcClip.bottom = (rc.bottom - rc.top);
				}
				if (rcClip.right > maxxScreen - 1) {
					rcClip.right = maxxScreen - 1;
					rcClip.left = maxxScreen - 1 - (rc.right - rc.left);
				}
				if (rcClip.bottom > maxyScreen - 1) {
					rcClip.bottom = maxyScreen - 1;
					rcClip.top = maxyScreen - 1 - (rc.bottom - rc.top);
				}

				if (!isRectEqual(old_rcClip,rcClip)) {
					HDC hScreenDC = GetDC(hWnd);
					DrawSelect(hScreenDC, FALSE, &old_rcClip); // erase old rubber-band
					DrawSelect(hScreenDC, TRUE, &rcClip); // new rubber-band
					ReleaseDC(hWnd,hScreenDC);
				} // if old

				old_rcClip = rcClip;
			} else if (iMouseCaptureMode == 1) { //Variable Region
				if (bCapturing) {
					POINT pt;
					GetCursorPos(&pt);

					HDC hScreenDC = GetDC(hWnd);

					DrawSelect(hScreenDC, FALSE, &rcClip); // erase old rubber-band

					rcClip.left = ptOrigin.x;
					rcClip.top = ptOrigin.y;
					rcClip.right = pt.x;
					rcClip.bottom = pt.y;

					NormalizeRect(&rcClip);
					DrawSelect(hScreenDC, TRUE, &rcClip); // new rubber-band
					//TextOut(hScreenDC,pt.x,pt.y,"Lolo",4);

					ReleaseDC(hWnd,hScreenDC);
				}
			}
		}
		break;

	case WM_LBUTTONUP:
		{
			TRACE("MouseCaptureWndProc : WM_LBUTTONUP\n");

			if (iMouseCaptureMode == 0) {
				//erase final
				HDC hScreenDC = GetDC(hWnd);
				DrawSelect(hScreenDC, FALSE, &old_rcClip);
				old_rcClip = rcClip;
				ReleaseDC(hWnd,hScreenDC);
			} else if (iMouseCaptureMode == 1) {
				NormalizeRect(&rcClip);
				old_rcClip = rcClip;
				bCapturing = FALSE;
			}

			ShowWindow(hWnd, SW_HIDE);

			if (!IsRectEmpty(&old_rcClip))
			{
				NormalizeRect(&old_rcClip);
				CopyRect(&rcUse, &old_rcClip);
				if (iDefineMode == 0)
				{
					TRACE("MouseCaptureWndProc: CRecorderView::WM_USER_RECORDSTART\n");
					::PostMessage (hWndGlobal, CRecorderView::WM_USER_RECORDSTART, 0, (LPARAM) 0);
				}
				else
				{
					TRACE("MouseCaptureWndProc: WM_USER_REGIONUPDATE\n");
					::PostMessage (hFixedRegionWnd, WM_USER_REGIONUPDATE, 0, (LPARAM) 0);
				}
			}
		}
		break;

	case WM_LBUTTONDOWN:
		{
			TRACE("MouseCaptureWndProc : WM_LBUTTONDOWN\n");
			// User pressed left button, initialize selection
			// Set origin to current mouse position (in window coords)

			if (iMouseCaptureMode == 1) {
				POINT pt;
				GetCursorPos(&pt);
				ptOrigin = pt;
				rcClip.left = rcClip.right = pt.x;
				rcClip.top = rcClip.bottom = pt.y;
				NormalizeRect(&rcClip); // Make sure it is a normal rect
				HDC hScreenDC = GetDC(hWnd);
				DrawSelect(hScreenDC, TRUE, &rcClip); // Draw the rubber-band box
				ReleaseDC(hWnd,hScreenDC);

				bCapturing = TRUE;
			}
		}
		break;

	case WM_RBUTTONDOWN:
		{
			TRACE("MouseCaptureWndProc : WM_RBUTTONDOWN\n");
			if (iMouseCaptureMode == 0) {
				//Cancel the operation
				//erase final
				HDC hScreenDC = GetDC(hWnd);
				DrawSelect(hScreenDC, FALSE, &old_rcClip);
				ReleaseDC(hWnd,hScreenDC);

				//Cancel the operation
				ShowWindow(hWnd,SW_HIDE);

				//ver 1.2
				AllowNewRecordStartKey = TRUE;
			}
		}
		break;

	case WM_KEYDOWN:
		{
			TRACE("MouseCaptureWndProc : WM_KEYDOWN\n");
			int nVirtKey = (int) wParam; // virtual-key code
			int lKeyData = lParam; // key data

			//ver 1.2
			if (nVirtKey == (int) uKeyRecordCancel) { //Cancel the operation
				//if (nVirtKey==VK_ESCAPE) {
				if (iMouseCaptureMode == 0) {
					//erase final
					HDC hScreenDC = GetDC(hWnd);
					DrawSelect(hScreenDC, FALSE, &old_rcClip);
					ReleaseDC(hWnd,hScreenDC);
				} else if (iMouseCaptureMode == 1) {
					NormalizeRect(&rcClip);
					old_rcClip = rcClip;
					if (bCapturing) {
						bCapturing = FALSE;
					}
				}

				ShowWindow(hWnd,SW_HIDE);

				//ver 1.2
				AllowNewRecordStartKey = TRUE;
			} //VK_ESCAPE (uKeyRecordCancel)
		}
		break;
	}

	return DefWindowProc(hWnd, wMessage, wParam, lParam);
}

// Code For Creating a Window for Specifying Region
// A borderless, invisible window used only for capturing mouse input for the whole screen
int CreateShiftWindow()
{
	HINSTANCE hInstance = AfxGetInstanceHandle();
	HICON hcur = ::LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICONCROSSHAIR));

	WNDCLASS wndclass;
	wndclass.style = 0;
	wndclass.lpfnWndProc = (WNDPROC)MouseCaptureWndProc;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = hInstance;
	wndclass.hIcon = LoadIcon(hInstance, "WINCAP");
	wndclass.lpszMenuName = NULL;
	wndclass.lpszClassName = (LPSTR) "ShiftRegionWindow";
	wndclass.hCursor = hcur;
	wndclass.hbrBackground = NULL;

	if (!RegisterClass(&wndclass))
		return 0;

	HDC hScreenDC = ::GetDC(NULL);
	maxxScreen = ::GetDeviceCaps(hScreenDC, HORZRES);
	maxyScreen = ::GetDeviceCaps(hScreenDC, VERTRES);
	::ReleaseDC(NULL,hScreenDC);

	hMouseCaptureWnd = ::CreateWindowEx(WS_EX_TOPMOST, "ShiftRegionWindow", "Title", WS_POPUP, 0, 0, maxxScreen, maxyScreen, NULL, NULL, hInstance, NULL);
	TRACE("CreateShiftWindow : %s\n", ::IsWindow(hMouseCaptureWnd) ? "SUCCEEDED" : "FAIL");

	return 0;
}

int DestroyShiftWindow()
{
	if (hMouseCaptureWnd)
		::DestroyWindow(hMouseCaptureWnd);
	return 0;
}

