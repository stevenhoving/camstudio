/**
 * Copyright(C) 2018  Steven Hoving
 *
 * This program is free software : you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#include "stdafx.h"
#include "mouse_capture_ui.h"
#include "settings_model.h"
#include "background_capture_source.h"

#include <CamLib/rect.h>

#include <CamCapture/cam_gdiplus.h>
#include <CamCapture/cam_rect.h>

#include <fmt/printf.h>
#include <fmt/ostream.h>

LRESULT WINAPI wnd_proc(HWND hWnd, UINT wMessage, WPARAM wParam, LPARAM lParam)
{
    const auto mouse_capture_ptr = reinterpret_cast<void *>(::GetWindowLongPtr(hWnd, GWLP_USERDATA));
    if (mouse_capture_ptr == nullptr)
        return DefWindowProc(hWnd, wMessage, wParam, lParam);

    auto mouse_capture = reinterpret_cast<mouse_capture_ui *>(mouse_capture_ptr);
    return mouse_capture->message_handler(hWnd, wMessage, wParam, lParam);
}

mouse_capture_ui::mouse_capture_ui(HINSTANCE instance, HWND parent, const std::function<void(const CRect &capture_rect)> &completed)
    : instance_(instance)
    , completed_(completed)
{
    const auto screen_x = ::GetSystemMetrics(SM_XVIRTUALSCREEN);
    const auto screen_y = ::GetSystemMetrics(SM_YVIRTUALSCREEN);
    const auto screen_width = ::GetSystemMetrics(SM_CXVIRTUALSCREEN);
    const auto screen_height = ::GetSystemMetrics(SM_CYVIRTUALSCREEN);

    max_screen_rect_ = CRect(
        screen_x,
        screen_y,
        screen_x + screen_width,
        screen_y + screen_height
    );

    register_window_class(instance);

    hwnd_ = create_capture_window(instance, parent, max_screen_rect_);
    background_capture_ = std::make_unique<background_capture_source>(nullptr);

    hatched_brush_ = (HBRUSH)::CreateHatchBrush(HS_BDIAGONAL, RGB(0, 0, 100));
}

mouse_capture_ui::~mouse_capture_ui()
{
    ::DeleteObject(hatched_brush_);
    ::DestroyWindow(hwnd_);
}

constexpr auto capture_class_name = _T("mouse_capture_window");

void mouse_capture_ui::register_window_class(HINSTANCE instance)
{
    static bool g_capture_window_registered = false;
    if (g_capture_window_registered)
        return;

    WNDCLASS wc = {};
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = wnd_proc;
    wc.hInstance = instance;
    // wc.hCursor = LoadIcon(instance, MAKEINTRESOURCE(IDI_ICONCROSSHAIR));
    wc.lpszClassName = capture_class_name;

    // \todo handle RegisterClass failure.
    RegisterClass(&wc);

    g_capture_window_registered = true;
}

HWND mouse_capture_ui::create_capture_window(HINSTANCE instance, HWND parent, CRect size)
{
    // \todo handle window creation failure.
    HWND hwnd = CreateWindowEx(WS_EX_TOPMOST, capture_class_name, NULL, WS_POPUP, size.left, size.top, size.Width(),
                               size.Height(), parent, NULL, instance, NULL);

    ::SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));

    return hwnd;
}

void mouse_capture_ui::show(const cam::rect<int> region, const capture_type type)
{
    // \todo make this load the correct icon based on the modify mode
    HICON icon = ::LoadIcon(instance_, MAKEINTRESOURCE(IDI_ICONCROSSHAIR));
    ::SetCursor(icon);

    // \todo also set max capture rect here.
    // \todo move this to separate function.
    // reset to initial ui state
    select_rect_ = false;
    capture_rect_drag_start_ = {0, 0};
    capture_rect_drag_end_ = {0, 0};
    capture_rect_.SetRect(region.left(), region.top(), region.right(), region.bottom());
    capture_rect_drag_rect_old_ = capture_rect_;
    capture_rect_drag_rect_ = capture_rect_;

    capture_mode_ = type;

    background_capture_->capture_frame();
    background_frame_ = background_capture_->get_frame();
    background_bitmap_ = std::unique_ptr<Gdiplus::Bitmap>(Gdiplus::Bitmap::FromHBITMAP(background_frame_, 0));

    ::ShowWindow(hwnd_, SW_SHOW);
    ::UpdateWindow(hwnd_);
}

void mouse_capture_ui::hide()
{
    ::ShowWindow(hwnd_, SW_HIDE);
}

void mouse_capture_ui::on_paint(HWND hWnd, WPARAM /*wParam*/, LPARAM /*lParam*/)
{
    PAINTSTRUCT ps;
    ::BeginPaint(hWnd, &ps);

    // Get the size of the client rectangle.
    RECT rc;
    ::GetClientRect(hWnd, &rc);

    // Create a compatible DC.
    auto hdc_mem = ::CreateCompatibleDC(ps.hdc);

    // Create a bitmap big enough for our client rectangle.
    auto hbmMem = ::CreateCompatibleBitmap(ps.hdc, rc.right - rc.left, rc.bottom - rc.top);

    // Select the bitmap into the off-screen DC.
    auto hbmOld = (HBITMAP)::SelectObject(hdc_mem, hbmMem);

    // Render the image into the offscreen DC.
    // \note using gdiplus to draw something is not fast...
    Gdiplus::Graphics canvas(hdc_mem);

    const Gdiplus::RectF paint_rect(
        static_cast<Gdiplus::REAL>(rc.left),
        static_cast<Gdiplus::REAL>(rc.top),
        static_cast<Gdiplus::REAL>(rc.right - rc.left),
        static_cast<Gdiplus::REAL>(rc.bottom - rc.top));
    const auto ret = canvas.DrawImage(background_bitmap_.get(), paint_rect);
    assert(ret == Gdiplus::Status::Ok);

    HBRUSH oldbrush = (HBRUSH)::SelectObject(hdc_mem, hatched_brush_);

    constexpr int region_selection_line_width = 3;
    ::PatBlt(hdc_mem, capture_rect_drag_rect_.left, capture_rect_drag_rect_.top, capture_rect_drag_rect_.Width(),
             region_selection_line_width, PATINVERT);
    ::PatBlt(hdc_mem, capture_rect_drag_rect_.left, capture_rect_drag_rect_.bottom - region_selection_line_width,
             region_selection_line_width, -(capture_rect_drag_rect_.Height() - 2 * region_selection_line_width),
             PATINVERT);
    ::PatBlt(hdc_mem, capture_rect_drag_rect_.right - region_selection_line_width,
             capture_rect_drag_rect_.top + region_selection_line_width, region_selection_line_width,
             capture_rect_drag_rect_.Height() - 2 * region_selection_line_width, PATINVERT);
    ::PatBlt(hdc_mem, capture_rect_drag_rect_.right, capture_rect_drag_rect_.bottom - region_selection_line_width,
             -capture_rect_drag_rect_.Width(), region_selection_line_width, PATINVERT);

    ::SelectObject(hdc_mem, oldbrush);

    // Blt the changes to the screen DC.
    ::BitBlt(ps.hdc, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, hdc_mem, 0, 0, SRCCOPY);

    // Done with off-screen bitmap and DC.
    ::SelectObject(hdc_mem, hbmOld);
    ::DeleteObject(hbmMem);
    ::DeleteDC(hdc_mem);
    ::EndPaint(hWnd, &ps);
}

void mouse_capture_ui::on_mouse_wheel(HWND hWnd, WPARAM wParam, LPARAM /*lParam*/)
{
    // in select mode the mouse wheel does not have any function
    if (modify_mode_ != modify_mode::move)
        return;

    const auto z_delta = GET_WHEEL_DELTA_WPARAM(wParam);

    capture_rect_.InflateRect(z_delta / 5, z_delta / 5);
    on_mouse_move(hWnd);
}

void mouse_capture_ui::set_modify_mode(modify_mode mode)
{
    modify_mode_ = mode;
}

void mouse_capture_ui::set_cancel_hotkey(int hotkey)
{
    cancel_vkey_ = hotkey;
}

LRESULT WINAPI mouse_capture_ui::message_handler(HWND hWnd, UINT wMessage, WPARAM wParam, LPARAM lParam)
{
    switch (wMessage)
    {
        case WM_MOUSEMOVE:
            on_mouse_move(hWnd);
            return 0;

        case WM_MOUSEWHEEL:
            on_mouse_wheel(hWnd, wParam, lParam);
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

        // we tell the windows that we have erased the background, because its part of our WM_PAINT
        // handling.
        case WM_ERASEBKGND:
            return 1;

        default:
            break;
    }

    return ::DefWindowProc(hWnd, wMessage, wParam, lParam);
}

void mouse_capture_ui::on_mouse_move(HWND hWnd)
{
    POINT pt;
    ::GetCursorPos(&pt);

    if (modify_mode_ == modify_mode::select)
    {
        // selecting a new rect is not yet started...
        if (!select_rect_)
            return;

        capture_rect_drag_end_ = pt;

        capture_rect_drag_rect_old_ = capture_rect_drag_rect_;
        capture_rect_drag_rect_ = CRect(capture_rect_drag_start_, capture_rect_drag_end_);
        capture_rect_drag_rect_ = align_rect(capture_rect_drag_rect_);

        auto invalidate = merge_rect(capture_rect_drag_rect_old_, capture_rect_drag_rect_);

        // make the invalidate rect 4px larger.
        // \todo inflation of the invalidate rect should be done according to the selection
        //       line width.
        invalidate.InflateRect(4, 4);

        // force a paint message to be queued
        ::InvalidateRect(hWnd, &invalidate, FALSE);
    }
    else
    {
        const CPoint capture_rect_center(capture_rect_.Width() / 2, capture_rect_.Height() / 2);

        CRect new_drag_rect = capture_rect_;
        new_drag_rect.MoveToXY(pt);
        new_drag_rect.OffsetRect(-capture_rect_center);

        new_drag_rect = clamp_rect(new_drag_rect, max_screen_rect_);

        capture_rect_drag_rect_old_ = capture_rect_drag_rect_;
        capture_rect_drag_rect_ = new_drag_rect;

        // make the invalidate rect 4px larger.
        // \todo inflation of the invalidate rect should be done according to the selection
        //       line width.
        auto invalidate = merge_rect(capture_rect_drag_rect_old_, capture_rect_drag_rect_);
        invalidate.InflateRect(4, 4);

        // force a paint message to be queued
        ::InvalidateRect(hWnd, &invalidate, FALSE);
    }
}

void mouse_capture_ui::on_lbutton_up(HWND /*hWnd*/)
{
    hide();

    // const auto capture_mode = settings_.get_capture_mode();
    if (capture_mode_ == capture_type::fixed)
    {
    }
    else if (capture_mode_ == capture_type::variable)
    {
        select_rect_ = false;
    }

    if (!capture_rect_drag_rect_.IsRectEmpty())
        capture_rect_drag_rect_.NormalizeRect();

    completed_(capture_rect_drag_rect_);
}

void mouse_capture_ui::on_lbutton_down(HWND /*hWnd*/)
{
    if (modify_mode_ == modify_mode::move)
        return;

    CPoint pt;
    ::GetCursorPos(&pt);

    capture_rect_drag_start_ = pt;
    select_rect_ = true;
}

void mouse_capture_ui::on_rbutton_down(HWND /*hWnd*/)
{
    if (capture_mode_ != capture_type::fixed)
        return;

    hide();
}

void mouse_capture_ui::on_key_down(HWND /*hWnd*/, WPARAM wParam, LPARAM /*lParam*/)
{
    int nVirtKey = (int)wParam; // virtual-key code

    if (nVirtKey == cancel_vkey_)
    {
        select_rect_ = false;
        hide();
    }
}
