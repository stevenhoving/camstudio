#include "stdafx.h"
#include "Recorder.h"
#include "MouseCaptureWnd.h"
#include "MainFrm.h" // for maxxScreen, maxyScreen
#include "RecorderView.h"
#include "HotKey.h"
#include <CamLib/CStudioLib.h>

HWND hMouseCaptureWnd;
// MouseCaptureWndProc referenced functions

BOOL CALLBACK DrawSelectMultiMonitorCallback(HMONITOR /*hMonitor*/, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData)
{
    DrawSelect(hdcMonitor, (dwData != (DWORD)0), lprcMonitor);

    return TRUE; // continue enumerating screens
}

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

    // Debug code to verify drawing to multiple monitors correctly
    //    CString strTrace;
    //    strTrace.Format("DrawSelect : (%d) - %X\n", fDraw, hdc);
    //    TRACE(strTrace);

    // If a rectangular clip region has been selected, draw it
    HBRUSH newbrush = (HBRUSH)CreateHatchBrush(HS_BDIAGONAL, RGB(0, 0, 100));
    HBRUSH oldbrush = (HBRUSH)SelectObject(hdc, newbrush);

    // PatBlt SRCINVERT regardless fDraw is TRUE or FALSE
    const int DINV = 3;
    PatBlt(hdc, rectDraw.left, rectDraw.top, rectDraw.Width(), DINV, PATINVERT);
    PatBlt(hdc, rectDraw.left, rectDraw.bottom - DINV, DINV, -(rectDraw.Height() - 2 * DINV), PATINVERT);
    PatBlt(hdc, rectDraw.right - DINV, rectDraw.top + DINV, DINV, rectDraw.Height() - 2 * DINV, PATINVERT);
    PatBlt(hdc, rectDraw.right, rectDraw.bottom - DINV, -rectDraw.Width(), DINV, PATINVERT);

    SelectObject(hdc, oldbrush);
    DeleteObject(newbrush);

    HDC hdcBits = CreateCompatibleDC(hdc);
    HFONT newfont = (HFONT)GetStockObject(ANSI_VAR_FONT);
    HFONT oldfont = (HFONT)SelectObject(hdc, newfont);

    CString strSize;
    strSize.Format(_T("Left : %d Top : %d Width : %d Height : %d"), rectDraw.left, rectDraw.top, rectDraw.Width(),
                   rectDraw.Height());
    SIZE sExtent;
    DWORD dw = GetTextExtentPoint(hdc, (LPCTSTR)strSize, strSize.GetLength(), &sExtent);
    VERIFY(0 != dw);

    int dx = sExtent.cx;
    int dy = sExtent.cy;
    int x = rectDraw.left + 10;
    int y = (rectDraw.top < (dy + DINV + 2)) ? rectDraw.bottom + DINV + 2 : rectDraw.top - dy - DINV - 2;

    if (fDraw)
    {
        // Save Original Picture
        SaveBitmapCopy(hSavedBitmap, hdc, hdcBits, x - 4, y - 4, dx + 8, dy + 8);

        // Text
        COLORREF oldtextcolor = SetTextColor(hdc, RGB(0, 0, 0));
        COLORREF oldbkcolor = SetBkColor(hdc, RGB(255, 255, 255));
        int oldbkmode = SetBkMode(hdc, TRANSPARENT);
        RoundRect(hdc, x - 4, y - 4, x + dx + 4, y + dy + 4, 10, 10);
        SetBkMode(hdc, OPAQUE);

        ExtTextOut(hdc, x, y, 0, nullptr, (LPCTSTR)strSize, strSize.GetLength(), nullptr);

        // Icon
        /* TODO: This creates a flicker when being draw to both screens
        if ((35 < (rectDraw.Width() - 10)) && ((dy + 40) < (rectDraw.Height() - 10)))
        {
            HBITMAP hbv = LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BITMAP1));
            HBITMAP old_bitmap = (HBITMAP) SelectObject(hdcBits, hbv);
            BitBlt(hdc, rectDraw.left + 10, rectDraw.bottom - 42, 30, 32, hdcBits, 0, 0, SRCINVERT);
            SelectObject(hdcBits, old_bitmap);
            DeleteObject(hbv);
        }*/

        SetBkColor(hdc, oldbkcolor);
        SetTextColor(hdc, oldtextcolor);
        SetBkMode(hdc, oldbkmode);
        SelectObject(hdc, oldfont);
    }
    else
    {
        RestoreBitmapCopy(hSavedBitmap, hdc, hdcBits, x - 4, y - 4, dx + 8, dy + 8);
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
            // TRACE("MouseCaptureWndProc : skip message\n");
            break;
        case WM_MOUSEMOVE:
        {
            // TRACE(_T("MouseCaptureWndProc : WM_MOUSEMOVE\n"));
            if (cRegionOpts.isCaptureMode(CAPTURE_FIXED))
            {
                // Fixed Region
                POINT pt;
                GetCursorPos(&pt);

                // Update rect with new mouse info
                g_rcClip.left = pt.x + g_rcOffset.left;
                g_rcClip.top = pt.y + g_rcOffset.top;
                g_rcClip.right = pt.x + g_rcOffset.right;
                g_rcClip.bottom = pt.y + g_rcOffset.bottom;

                if (g_rcClip.left < 0)
                {
                    g_rcClip.left = 0;
                    g_rcClip.right = g_rc.Width();
                }
                if (g_rcClip.top < 0)
                {
                    g_rcClip.top = 0;
                    g_rcClip.bottom = g_rc.Height();
                }
                if (g_rcClip.right > maxxScreen - 1)
                {
                    g_rcClip.right = maxxScreen - 1;
                    g_rcClip.left = maxxScreen - 1 - g_rc.Width();
                }
                if (g_rcClip.bottom > maxyScreen - 1)
                {
                    g_rcClip.bottom = maxyScreen - 1;
                    g_rcClip.top = maxyScreen - 1 - g_rc.Height();
                }

                if (!isRectEqual(g_old_rcClip, g_rcClip))
                {
                    // Multimonitor code -- will need to combine both EnumDisplays ... see comment above
                    // DrawSelectMultiMonitorCallback
                    HDC hCurMonitorDC = GetDC(nullptr);
                    EnumDisplayMonitors(hCurMonitorDC, &g_old_rcClip, DrawSelectMultiMonitorCallback,
                                        0); // 0 indicates erase old rubber-band
                    EnumDisplayMonitors(hCurMonitorDC, &g_rcClip, DrawSelectMultiMonitorCallback,
                                        1);         // 1 indicates draw new rubber-band
                    ReleaseDC(hWnd, hCurMonitorDC); /*
                      //single monitor code
                      HDC hScreenDC = GetDC(hWnd);
                      DrawSelect(hScreenDC, FALSE, &g_old_rcClip); // erase old rubber-band
                      DrawSelect(hScreenDC, TRUE, &g_rcClip); // new rubber-band
                      ReleaseDC(hWnd,hScreenDC);*/
                }                                   // if old

                g_old_rcClip = g_rcClip;
            }
            else if (cRegionOpts.isCaptureMode(CAPTURE_VARIABLE))
            { // Variable Region
                if (g_bCapturing)
                {
                    POINT pt;
                    GetCursorPos(&pt);

                    HDC hScreenDC = GetDC(hWnd);
                    HDC hCurMonitorDC = GetDC(nullptr);

                    // DrawSelect(hScreenDC, FALSE, &g_rcClip); // erase old rubber-band
                    EnumDisplayMonitors(hCurMonitorDC, &g_rcClip, DrawSelectMultiMonitorCallback,
                                        0); // erase old rubber-band

                    g_rcClip.left = g_ptOrigin.x;
                    g_rcClip.top = g_ptOrigin.y;
                    g_rcClip.right = pt.x;
                    g_rcClip.bottom = pt.y;

                    NormalizeRect(&g_rcClip);
                    // DrawSelect(hScreenDC, TRUE, &g_rcClip); // new rubber-band
                    EnumDisplayMonitors(hCurMonitorDC, &g_rcClip, DrawSelectMultiMonitorCallback,
                                        1); // draw new rubber-band

                    ReleaseDC(hWnd, hScreenDC);
                    // ReleaseDC(hWnd,hCurMonitorDC);
                }
            }
        }
        break;

        case WM_LBUTTONUP:
        {
            // TRACE(_T("MouseCaptureWndProc : WM_LBUTTONUP\n"));

            if (cRegionOpts.isCaptureMode(CAPTURE_FIXED))
            {
                // erase final
                HDC hScreenDC = GetDC(hWnd);
                DrawSelect(hScreenDC, FALSE, &g_old_rcClip);
                g_old_rcClip = g_rcClip;
                ReleaseDC(hWnd, hScreenDC);
            }
            else if (cRegionOpts.isCaptureMode(CAPTURE_VARIABLE))
            {
                NormalizeRect(&g_rcClip);
                g_old_rcClip = g_rcClip;
                g_bCapturing = FALSE;
            }

            ShowWindow(hWnd, SW_HIDE);

            if (!IsRectEmpty(&g_old_rcClip))
            {
                NormalizeRect(&g_old_rcClip);
                CopyRect(&g_rcUse, &g_old_rcClip);
                if (g_iDefineMode == 0)
                {
                    TRACE(_T("MouseCaptureWndProc: CRecorderView::WM_USER_RECORDSTART\n"));
                    ::PostMessage(g_hWndGlobal, CRecorderView::WM_USER_RECORDSTART, 0, (LPARAM)0);
                }
                else
                {
                    TRACE(_T("MouseCaptureWndProc: WM_APP_REGIONUPDATE\n"));
                    ::PostMessage(g_hFixedRegionWnd, WM_APP_REGIONUPDATE, 0, (LPARAM)0);
                }
            }
        }
        break;

        case WM_LBUTTONDOWN:
        {
            // TRACE(_T("MouseCaptureWndProc : WM_LBUTTONDOWN\n"));
            // User pressed left button, initialize selection
            // Set origin to current mouse position (in window coords)

            if (cRegionOpts.isCaptureMode(CAPTURE_VARIABLE))
            {
                CPoint pt;
                GetCursorPos(&pt);
                g_ptOrigin = pt;
                g_rcClip.left = g_rcClip.right = pt.x;
                g_rcClip.top = g_rcClip.bottom = pt.y;
                g_rcClip.NormalizeRect(); // Make sure it is a normal rect
                CWindowDC cScreenDC(CWnd::FromHandle(hWnd));
                DrawSelect(cScreenDC, TRUE, &g_rcClip); // Draw the rubber-band box
                // HDC hScreenDC = GetDC(hWnd);
                // DrawSelect(hScreenDC, TRUE, &g_rcClip); // Draw the rubber-band box
                // ReleaseDC(hWnd,hScreenDC);

                g_bCapturing = TRUE;
            }
        }
        break;

        case WM_RBUTTONDOWN:
        {
            // TRACE(_T("MouseCaptureWndProc : WM_RBUTTONDOWN\n"));
            if (cRegionOpts.isCaptureMode(CAPTURE_FIXED))
            {
                // Cancel the operation
                // erase final
                HDC hScreenDC = GetDC(hWnd);
                DrawSelect(hScreenDC, FALSE, &g_old_rcClip);
                ReleaseDC(hWnd, hScreenDC);

                // Cancel the operation
                ShowWindow(hWnd, SW_HIDE);

                // ver 1.2
                bAllowNewRecordStartKey = TRUE;
            }
        }
        break;

        case WM_KEYDOWN:
        {
            // TRACE(_T("MouseCaptureWndProc : WM_KEYDOWN\n"));
            int nVirtKey = (int)wParam; // virtual-key code
            // int lKeyData = lParam; // key data

            // ver 1.2
            if (nVirtKey == (int)cHotKeyOpts.m_RecordCancel.m_vKey)
            { // Cancel the operation
                // if (nVirtKey==VK_ESCAPE) {
                if (cRegionOpts.isCaptureMode(CAPTURE_FIXED))
                {
                    // erase final
                    HDC hScreenDC = GetDC(hWnd);
                    DrawSelect(hScreenDC, FALSE, &g_old_rcClip);
                    ReleaseDC(hWnd, hScreenDC);
                }
                else if (cRegionOpts.isCaptureMode(CAPTURE_VARIABLE))
                {
                    NormalizeRect(&g_rcClip);
                    g_old_rcClip = g_rcClip;
                    if (g_bCapturing)
                    {
                        g_bCapturing = FALSE;
                    }
                }

                ShowWindow(hWnd, SW_HIDE);

                // ver 1.2
                bAllowNewRecordStartKey = TRUE;
            } // VK_ESCAPE (uKeyRecordCancel)
        }
        break;
    }

    return DefWindowProc(hWnd, wMessage, wParam, lParam);
}

// Code For Creating a Window for Specifying Region
// A borderless, invisible window used only for capturing mouse input for the whole screen
bool CreateShiftWindow()
{
    HINSTANCE hInstance = AfxGetInstanceHandle();
    HICON hcur = ::LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICONCROSSHAIR));

    WNDCLASS wndclass;
    wndclass.style = 0;
    wndclass.lpfnWndProc = (WNDPROC)MouseCaptureWndProc;
    wndclass.cbClsExtra = 0;
    wndclass.cbWndExtra = 0;
    wndclass.hInstance = hInstance;
    wndclass.hIcon = LoadIcon(hInstance, _T("WINCAP"));
    wndclass.lpszMenuName = nullptr;
    wndclass.lpszClassName = _T("ShiftRegionWindow");
    wndclass.hCursor = hcur;
    wndclass.hbrBackground = nullptr;

    if (!RegisterClass(&wndclass))
        return false;

    // HDC hScreenDC = ::GetDC(nullptr);
    // maxxScreen = ::GetDeviceCaps(hScreenDC, HORZRES);
    maxxScreen = ::GetSystemMetrics(SM_CXVIRTUALSCREEN);
    // maxyScreen = ::GetDeviceCaps(hScreenDC, VERTRES);
    maxyScreen = ::GetSystemMetrics(SM_CYVIRTUALSCREEN);
    minxScreen = ::GetSystemMetrics(SM_XVIRTUALSCREEN);
    minyScreen = ::GetSystemMetrics(SM_YVIRTUALSCREEN);
    //::ReleaseDC(nullptr,hScreenDC);

    hMouseCaptureWnd = ::CreateWindowEx(WS_EX_TOPMOST, _T("ShiftRegionWindow"), _T("Title"), WS_POPUP, minxScreen,
                                        minyScreen, maxxScreen, maxyScreen, nullptr, nullptr, hInstance, nullptr);

    TRACE(_T("CreateShiftWindow : %s\n"), ::IsWindow(hMouseCaptureWnd) ? _T("SUCCEEDED") : _T("FAIL"));
    return ::IsWindow(hMouseCaptureWnd) ? true : false;
}

int DestroyShiftWindow()
{
    if (hMouseCaptureWnd)
        ::DestroyWindow(hMouseCaptureWnd);
    return 0;
}