#include "stdafx.h"
#include "Recorder.h"
#include "MouseCaptureWnd.h"
#include "MainFrm.h" // for g_maxx_screen, g_maxy_screen
#include "RecorderView.h"
#include "HotKey.h"
#include "settings_model.h"
#include <CamLib/CStudioLib.h>
#include <CamLib/rect.h>
#include <fmt/printf.h>

#include <CamCapture/cam_capture.h>
#include <CamCapture/cam_gdiplus.h>

#include <fmt/ostream.h>

std::ostream &operator<<(std::ostream &os, const CRect &rect)
{
    return os << fmt::format("{} {} {} {}", rect.left, rect.top, rect.right, rect.bottom);
}

HWND g_hMouseCaptureWnd;

LRESULT WINAPI MouseCaptureWndProc(HWND hWnd, UINT wMessage, WPARAM wParam, LPARAM lParam)
{
    LONG *mouse_capture_ptr = (LONG *)::GetWindowLongPtr(hWnd, GWLP_USERDATA);
    if (mouse_capture_ptr == nullptr)
        return DefWindowProc(hWnd, wMessage, wParam, lParam);

    auto mouse_capture = reinterpret_cast<mouse_capture_wnd *>(mouse_capture_ptr);
    return mouse_capture->WndProc(hWnd, wMessage, wParam, lParam);
}

const wchar_t *CaptureClass = _T("RegionCaptureWindow");

static bool RegionCaptureWindow_reg = false;
void mouse_capture_wnd::RegisterWIndowClass(HINSTANCE instance)
{
    if (RegionCaptureWindow_reg)
        return;
    WNDCLASS wc = {};
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = MouseCaptureWndProc;
    wc.hInstance = instance;
    wc.hIcon = LoadIcon(instance, _T("WINCAP"));
    wc.hCursor = LoadIcon(instance, MAKEINTRESOURCE(IDI_ICONCROSSHAIR));
    wc.lpszClassName = CaptureClass;

    if (!RegisterClass(&wc))
    {
        fmt::print("register region capture window failed\n");
    }
    RegionCaptureWindow_reg = true;
}

HWND mouse_capture_wnd::CreateRegionWindow(HINSTANCE instance, HWND parent)
{
    // WS_EX_LAYERED
    HWND hwnd = CreateWindowEx(WS_EX_TOPMOST, CaptureClass, NULL,
        WS_POPUP, // WS_OVERLAPPEDWINDOW,
        g_minx_screen,
        g_miny_screen,
        g_maxx_screen,
        g_maxy_screen,
        parent, NULL, instance, NULL);

    ::SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)this);

    return hwnd;
}

void mouse_capture_wnd::show()
{
    auto cap_rect = rect<int>(max_screen_rect_.left, max_screen_rect_.top, max_screen_rect_.right,
        max_screen_rect_.bottom);

    if (!background_capture_)
        background_capture_ = std::make_unique<cam_capture_source>(nullptr, cap_rect);

    background_capture_->capture_frame(cap_rect);
    // hack
    background_bitmap_.reset();
    background_frame_ = const_cast<cam_frame *>(background_capture_->get_frame());
    background_bitmap_ = std::make_unique<Gdiplus::Bitmap>(background_frame_->width,
        background_frame_->height, background_frame_->stride, PixelFormat32bppPARGB, (BYTE*)background_frame_->bitmap_data);

    ::ShowWindow(hwnd_, SW_SHOW);
    ::UpdateWindow(hwnd_);
}

mouse_capture_wnd::mouse_capture_wnd(settings_model &settings, HWND parent)
    : settings_(settings)
{
    const auto capture_rect = settings_.get_capture_rect();
    capture_rect_ = {capture_rect.left(), capture_rect.top(), capture_rect.right(), capture_rect.bottom()};

    g_maxx_screen = ::GetSystemMetrics(SM_CXVIRTUALSCREEN);
    g_maxy_screen = ::GetSystemMetrics(SM_CYVIRTUALSCREEN);
    g_minx_screen = ::GetSystemMetrics(SM_XVIRTUALSCREEN);
    g_miny_screen = ::GetSystemMetrics(SM_YVIRTUALSCREEN);

    HINSTANCE instance = AfxGetInstanceHandle();
    RegisterWIndowClass(instance);

    hwnd_ = g_hMouseCaptureWnd = CreateRegionWindow(instance, parent);
}

mouse_capture_wnd::~mouse_capture_wnd()
{
    DestroyShiftWindow();
}

void mouse_capture_wnd::on_paint(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    RECT rc;
    HDC hdcMem;
    HBITMAP hbmMem, hbmOld;
    //HBRUSH hbrBkGnd;
    //HFONT hfntOld;

    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hWnd, &ps);

    //
    // Get the size of the client rectangle.
    //

    GetClientRect(hWnd, &rc);

    //
    // Create a compatible DC.
    //

    hdcMem = CreateCompatibleDC(ps.hdc);

    //
    // Create a bitmap big enough for our client rectangle.
    //

    hbmMem = CreateCompatibleBitmap(ps.hdc, rc.right - rc.left, rc.bottom - rc.top);

    //
    // Select the bitmap into the off-screen DC.
    //

    hbmOld = (HBITMAP)SelectObject(hdcMem, hbmMem);

    //
    // Erase the background.
    //

    //hbrBkGnd = CreateSolidBrush(GetSysColor(COLOR_WINDOW));
    //FillRect(hdcMem, &rc, hbrBkGnd);
    //DeleteObject(hbrBkGnd);

    //
    // Select the font.
    //

    //if (hfnt)
    //{
    //    hfntOld = SelectObject(hdcMem, hfnt);
    //}

    //
    // Render the image into the offscreen DC.
    //

    auto canvas = std::make_unique<Gdiplus::Graphics>(hdcMem);

    auto ret = canvas->DrawImage(background_bitmap_.get(), 0, 0, 0, 0, background_frame_->width,
                                 background_frame_->height, Gdiplus::Unit::UnitPixel);
    if (ret != Gdiplus::Status::Ok)
    {
        fmt::print("draw background image failed: {}\n", ret);
    }

    Gdiplus::Pen pen(Gdiplus::Color::Red);
    Gdiplus::Rect rect(capture_rect_drag_rect_.left, capture_rect_drag_rect_.top, capture_rect_drag_rect_.right,
                       capture_rect_drag_rect_.bottom);
    canvas->DrawRectangle(&pen, rect);

    //SetBkMode(hdcMem, TRANSPARENT);
    //SetTextColor(hdcMem, GetSysColor(COLOR_WINDOWTEXT));
    //auto caption = L"henk";
    //DrawText(hdcMem, (LPWSTR)caption, -1, &rc, DT_CENTER);

    //if (hfntOld)
    //{
    //    SelectObject(hdcMem, hfntOld);
    //}

    //
    // Blt the changes to the screen DC.
    //
    BitBlt(ps.hdc, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, hdcMem, 0, 0, SRCCOPY);

    //
    // Done with off-screen bitmap and DC.
    //

    SelectObject(hdcMem, hbmOld);
    DeleteObject(hbmMem);
    DeleteDC(hdcMem);
    EndPaint(hWnd, &ps);
}

/////////////////////////////////////////////////////////////////////////////
// DrawSelect
// Draws the selected clip rectangle with its dimensions on the DC
/////////////////////////////////////////////////////////////////////////////
void DrawSelect(HWND hwnd, HDC hdc, BOOL fDraw, LPRECT lprClip)
{
    if (!lprClip || IsRectEmpty(lprClip))
    {
        return;
    }
    //CRect rectDraw(lprClip);

    //::InvalidateRect(hwnd, lprClip, false);

#if 0

    // Debug code to verify drawing to multiple monitors correctly
    //    CString strTrace;
    //    strTrace.Format("DrawSelect : (%d) - %X\n", fDraw, hdc);
    //    TRACE(strTrace);

    // If a rectangular clip region has been selected, draw it
    HBRUSH newbrush = (HBRUSH)CreateHatchBrush(HS_BDIAGONAL, RGB(0, 0, 100));
    HBRUSH oldbrush = (HBRUSH)SelectObject(hdc, newbrush);

    DrawRect(hdc, rectDraw);

    SelectObject(hdc, oldbrush);
    DeleteObject(newbrush);
#endif

#if 0
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

    //if (fDraw)
    if (0)
    {
        // Save Original Picture
        SaveBitmapCopy(g_hSavedBitmap, hdc, hdcBits, x - 4, y - 4, dx + 8, dy + 8);

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
        //RestoreBitmapCopy(g_hSavedBitmap, hdc, hdcBits, x - 4, y - 4, dx + 8, dy + 8);
    }

    DeleteDC(hdcBits);
#endif
}

void print_wm(UINT wMessage)
{
    switch(wMessage)
    {
    case WM_NOTIFY: fmt::print("WM_NOTIFY\n"); break;
    case WM_SHOWWINDOW: fmt::print("WM_SHOWWINDOW\n"); break;
    default: fmt::print("{}\n", wMessage); break;

    }
}

LRESULT WINAPI mouse_capture_wnd::WndProc(HWND hWnd, UINT wMessage, WPARAM wParam, LPARAM lParam)
{
    switch (wMessage)
    {
        case WM_MOUSEMOVE:
            on_mouse_move(hWnd);
        return 0;

        case WM_LBUTTONUP:
            on_lbutton_up(hWnd);
        return 0;

        case WM_LBUTTONDOWN:
            on_lbutton_down(hWnd);
        return 0;

        case WM_RBUTTONDOWN:
            on_rbutton_down(hWnd);
        return 0;

        case WM_KEYDOWN:
            on_key_down(hWnd, wParam, lParam);
        return 0;

        case WM_PAINT:
            on_paint(hWnd, wParam, lParam);
        return 0;

        case WM_ERASEBKGND:
        return 1;

        default:
            print_wm(wMessage);
            //fmt::print("mouse_capture_wnd::WndProc\n");
        break;
    }

    return DefWindowProc(hWnd, wMessage, wParam, lParam);
}

int mouse_capture_wnd::DestroyShiftWindow()
{
    if (g_hMouseCaptureWnd)
        ::DestroyWindow(g_hMouseCaptureWnd);
    return 0;
}

void mouse_capture_wnd::on_mouse_move(HWND hWnd)
{
    max_screen_rect_ = {
        g_minx_screen,
        g_miny_screen,
        g_maxx_screen,
        g_maxx_screen,
    };

    const CPoint captur_rect_center(
        capture_rect_.Width() / 2,
        capture_rect_.Height() / 2);

    switch(settings_.get_capture_mode())
    {
        case capture_type::fixed: {
            
            // Fixed Region
            POINT pt;
            GetCursorPos(&pt);

            //CRect previouse_drag_rect = capture_rect_ + capture_rect_drag_end_ - captur_rect_center;
            //capture_rect_drag_rect_old_ = previouse_drag_rect;

            CRect new_drag_rect = capture_rect_ + pt - captur_rect_center;
            new_drag_rect = clamp_rect(new_drag_rect, max_screen_rect_);

            capture_rect_drag_rect_old_ = capture_rect_drag_rect_;
            //capture_rect_drag_end_ = new_drag_rect.CenterPoint();
            capture_rect_drag_rect_ = new_drag_rect;

            //fmt::print("mouse move fixed: {}\n", capture_rect_drag_rect_);

#if 0
            fmt::print("on_mouse_move - fixed\n"
                "  max: {} {} {} {}\n"
                "  old: {} {} {} {}\n"
                "  mouse: {}x{}\n",
                max_screen_rect_.left, max_screen_rect_.top, max_screen_rect_.right, max_screen_rect_.bottom,
                previouse_drag_rect.left, previouse_drag_rect.top, previouse_drag_rect.right,previouse_drag_rect.bottom,
                capture_rect_drag_end_.x, capture_rect_drag_end_.y);
#endif

            //g_maxx_screen g_maxy_screen g_minx_screen g_miny_screen

            // Update rect with new mouse info
            // g_rcClip.left = pt.x + g_rcOffset.left;
            // g_rcClip.top = pt.y + g_rcOffset.top;
            // g_rcClip.right = pt.x + g_rcOffset.right;
            // g_rcClip.bottom = pt.y + g_rcOffset.bottom;

            // if (g_rcClip.left < 0)
            //{
            //    g_rcClip.left = 0;
            //    g_rcClip.right = g_rc.Width();
            //}
            // if (g_rcClip.top < 0)
            //{
            //    g_rcClip.top = 0;
            //    g_rcClip.bottom = g_rc.Height();
            //}
            // if (g_rcClip.right > g_maxx_screen - 1)
            //{
            //    g_rcClip.right = g_maxx_screen - 1;
            //    g_rcClip.left = g_maxx_screen - 1 - g_rc.Width();
            //}
            // if (g_rcClip.bottom > g_maxy_screen - 1)
            //{
            //    g_rcClip.bottom = g_maxy_screen - 1;
            //    g_rcClip.top = g_maxy_screen - 1 - g_rc.Height();
            //}
            //auto test = merge_rect(capture_rect_drag_rect_, capture_rect_drag_rect_old_);
            //test.InflateRect(10, 10);
            //auto test = capture_rect_drag_rect_;
            //test.InflateRect(10, 10);

            auto test = max_screen_rect_;

            // force a pain message to be queued
            //fmt::print("invalidate - {} {} {} {}\n", test.left, test.top, test.right, test.bottom);
            ::InvalidateRect(hWnd, &test, FALSE);

            //SetRegionRect(g_hMouseCaptureWnd, capture_rect_drag_rect_);

            if (!isRectEqual(capture_rect_drag_rect_, capture_rect_drag_rect_old_))
            {
                // Multimonitor code -- will need to combine both EnumDisplays ... see comment above
                // DrawSelectMultiMonitorCallback

                //HDC hCurMonitorDC = GetDC(nullptr);

                


                // 0 indicates erase old rubber-band
                //EnumDisplayMonitors(hCurMonitorDC, &previouse_drag_rect, DrawSelectMultiMonitorCallback, 0);
                // 1 indicates draw new rubber-band
                //EnumDisplayMonitors(hCurMonitorDC, &new_drag_rect, DrawSelectMultiMonitorCallback, 1);
                //ReleaseDC(hWnd, hCurMonitorDC);
                /*
                    //single monitor code
                    HDC hScreenDC = GetDC(hWnd);
                    DrawSelect(hScreenDC, FALSE, &g_old_rcClip); // erase old rubber-band
                    DrawSelect(hScreenDC, TRUE, &g_rcClip); // new rubber-band
                    ReleaseDC(hWnd,hScreenDC);
                    */
            }

            //g_old_rcClip = g_rcClip;
        } break;

        case capture_type::variable: {
            fmt::print("mouse move variable\n");

            // Variable Region
            if (g_bCapturing)
            {
                fmt::print("on_mouse_move - variable\n");

                POINT pt;
                GetCursorPos(&pt);

                HDC hScreenDC = GetDC(hWnd);
                HDC hCurMonitorDC = GetDC(nullptr);

                //CRect drag_rect(capture_rect_drag_start_, capture_rect_drag_end_);

                 // erase old rubber-band
                //EnumDisplayMonitors(hCurMonitorDC, &drag_rect, DrawSelectMultiMonitorCallback, 0);

                //capture_rect_drag_end_ = pt;

                //drag_rect = CRect(capture_rect_drag_start_, capture_rect_drag_end_);

                NormalizeRect(&g_rcClip);
                //EnumDisplayMonitors(hCurMonitorDC, &drag_rect, DrawSelectMultiMonitorCallback,
                //                    1); // draw new rubber-band

                ReleaseDC(hWnd, hScreenDC);
            }
        } break;

        default:
        break;
    }
}

void mouse_capture_wnd::on_lbutton_up(HWND hWnd)
{
    const auto capture_mode = settings_.get_capture_mode();
    if (capture_mode == capture_type::fixed)
    {
        // erase final
        HDC hScreenDC = GetDC(hWnd);
        //DrawSelect(hScreenDC, FALSE, &g_old_rcClip);
        g_old_rcClip = g_rcClip;
        ReleaseDC(hWnd, hScreenDC);
    }
    else if (capture_mode == capture_type::variable)
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
            fmt::print("MouseCaptureWndProc: CRecorderView::WM_USER_RECORDSTART\n");
            ::PostMessage(g_hWndGlobal, CRecorderView::WM_USER_RECORDSTART, 0, (LPARAM)0);
        }
        else
        {
            fmt::print("MouseCaptureWndProc: WM_APP_REGIONUPDATE\n");
            ::PostMessage(g_hFixedRegionWnd, WM_APP_REGIONUPDATE, 0, (LPARAM)0);
        }
    }
}

void mouse_capture_wnd::on_lbutton_down(HWND hWnd)
{
    const auto capture_mode = settings_.get_capture_mode();
    if (capture_mode != capture_type::variable)
        return;

    CPoint pt;
    GetCursorPos(&pt);
    g_ptOrigin = pt;
    capture_rect_drag_start_ = pt;

    //g_rcClip.left = g_rcClip.right = pt.x;
    //g_rcClip.top = g_rcClip.bottom = pt.y;
    CRect drag_rect(pt.x, pt.y, pt.x, pt.x);

    drag_rect.NormalizeRect();
    CWindowDC cScreenDC(CWnd::FromHandle(hWnd));
    //DrawSelect(cScreenDC, TRUE, &drag_rect);

    // HDC hScreenDC = GetDC(hWnd);
    // DrawSelect(hScreenDC, TRUE, &g_rcClip); // Draw the rubber-band box
    // ReleaseDC(hWnd,hScreenDC);

    g_bCapturing = TRUE;
}

void mouse_capture_wnd::on_rbutton_down(HWND hWnd)
{
    const auto capture_mode = settings_.get_capture_mode();
    if (capture_mode != capture_type::fixed)
        return;

    // Cancel the operation
    // erase final
    HDC hScreenDC = GetDC(hWnd);
    //DrawSelect(hScreenDC, FALSE, &g_old_rcClip);
    ReleaseDC(hWnd, hScreenDC);

    // Cancel the operation
    ShowWindow(hWnd, SW_HIDE);

    // ver 1.2
    g_bAllowNewRecordStartKey = TRUE;
}

void mouse_capture_wnd::on_key_down(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    int nVirtKey = (int)wParam; // virtual-key code
    // int lKeyData = lParam; // key data

    const auto capture_mode = settings_.get_capture_mode();
    if (capture_mode != capture_type::fixed)
        return;

    // ver 1.2
    if (nVirtKey == (int)cHotKeyOpts.m_RecordCancel.m_vKey)
    { // Cancel the operation
        if (capture_mode == capture_type::fixed)
        {
            // erase final
            HDC hScreenDC = GetDC(hWnd);
            //DrawSelect(hScreenDC, FALSE, &g_old_rcClip);
            ReleaseDC(hWnd, hScreenDC);
        }
        else if (capture_mode == capture_type::variable)
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
        g_bAllowNewRecordStartKey = TRUE;
    } // VK_ESCAPE (uKeyRecordCancel)
}
