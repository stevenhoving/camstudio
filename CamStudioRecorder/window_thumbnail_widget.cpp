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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.If not, see <https://www.gnu.org/licenses/>.
 */

#include "stdafx.h"
#include "window_thumbnail_widget.h"
#include <fmt/printf.h>
#include "fmt_helper.h"

window_button::window_button(const window_data &data)
    : CButton()
    , data_(data)
    //, window_capture_source_(std::make_unique<background_capture_source>(data.hwnd))
{
}

void DrawBitmap(CDC *pDC, HBITMAP hbitmap, CRect size)
{
    CBitmap *pBitmap = CBitmap::FromHandle(hbitmap);
    BITMAP bm;
    pBitmap->GetBitmap(&bm);

    CDC MemDC;
    MemDC.CreateCompatibleDC(pDC);
    const auto old_bitmap = MemDC.SelectObject(pBitmap);

    pDC->SetStretchBltMode(STRETCH_HALFTONE);
    pDC->StretchBlt(0, 0, size.Width(), size.Height(), &MemDC, 0, 0, bm.bmWidth, bm.bmHeight, SRCCOPY);
    MemDC.SelectObject(old_bitmap);
}

#if 0
void window_button::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{

    CRect rt = lpDrawItemStruct->rcItem;
    CDC bmpDC;
    auto dc = CDC::FromHandle(lpDrawItemStruct->hDC);
    bmpDC.CreateCompatibleDC(dc);

    window_capture_source_->capture_frame();
    const auto bitmap = window_capture_source_->get_frame();
    const auto src_size = window_capture_source_->get_size();
    const auto dst_size = size<int>(button_width, button_height);

    auto pOldBitmap = bmpDC.SelectObject(bitmap);

    dc->SetStretchBltMode(STRETCH_HALFTONE);
    dc->StretchBlt(0, 0, dst_size.width(), dst_size.height(), &bmpDC, 0, 0, src_size.width(), src_size.height(),
                   SRCCOPY | CAPTUREBLT);
    bmpDC.SelectObject(pOldBitmap);

    if (button_state_ == button_state::hover)
    {
        
    }
    else
    {
        dc->FillSolidRect(rt, RGB(0, 0, 255));
    }

    bmpDC.DeleteDC();



    //CDC dc;
    //dc.Attach(lpDrawItemStruct->hDC);
    //CRect rt = lpDrawItemStruct->rcItem;
    //
    //window_capture_source_->capture_frame();
    //const auto bitmap = window_capture_source_->get_frame();
    //DrawBitmap(&dc, bitmap, rt);
    //
    ////dc.FillSolidRect(rt, RGB(0, 0, 255));
    //
    //auto state = lpDrawItemStruct->itemState;
    //if (state & ODS_SELECTED)
    //{
    //    dc.DrawEdge(rt, EDGE_SUNKEN, BF_RECT);
    //}
    //else
    //{
    //    dc.DrawEdge(rt, EDGE_RAISED, BF_RECT);
    //}
    //
    //// draw text if needed
    ////dc.SetTextColor(RGB(255, 255, 120));
    //
    //
    //
    ////dc.GetSafeHdc()
    //
    ////background_bitmap_ = std::unique_ptr<Gdiplus::Bitmap>(Gdiplus::Bitmap::FromHBITMAP(background_frame_, 0));
    //
    //if (state & ODS_FOCUS)
    //{
    //    auto focus_rect = rt;
    //    focus_rect.InflateRect(3, 3);
    //    dc.DrawFocusRect(&focus_rect);
    //}

    //dc.Detach();
}
#endif

void window_button::OnCreate(HWND hwnd)
{
    // CRect pos;
    // GetWindowRect(&pos);
    //::SetWindowPos(hwnd, HWND_TOP, pos.left, pos.top, pos.Width(), pos.Height(),SWP_SHOWWINDOW);

    // auto src = get_the_parent(data_.hwnd);
    //dwm_.link(hwnd, data_.hwnd);
}

bool is_in_rect(CRect rect, CPoint point)
{
    bool result = point.x > rect.left && point.x < rect.right;
    result = result && point.y > rect.top && point.y < rect.bottom;
    return result;
}

void window_button::OnMouseMove(UINT nFlags, CPoint point)
{
    CRect ButtonRect;

    std::string title(data_.process_name.begin(), data_.process_name.end());
    GetWindowRect(&ButtonRect);
    // fmt::print("mouse move {} - {} | {}\n", title, ButtonRect, point);
    if (button_state_ == button_state::down)
    {
        // fmt::print("down\n");
        // if (!is_in_rect(ButtonRect, point))
        {
            set_state(button_state::hover);
            Invalidate(FALSE);
        }
    }
    else if (prev_button_state_ == button_state::down && button_state_ == button_state::hover)
    {
        // fmt::print("down\n");
        set_state(button_state::down);
    }
    else
    {
        // if (is_in_rect(ButtonRect, point))
        {
            // fmt::print("hover\n");
            set_state(button_state::hover);
            Invalidate(FALSE);
        }
        // else
        //{
        //    //fmt::print("normal\n");
        //    set_state(button_state::normal);
        //}
    }
#if 1
    TRACKMOUSEEVENT tme;
    CWnd::OnMouseMove(nFlags, point);
    tme.cbSize = sizeof(TRACKMOUSEEVENT);
    tme.dwFlags = TME_LEAVE;
    tme.hwndTrack = m_hWnd;
    _TrackMouseEvent(&tme);
#endif
}

BOOL window_button::OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT *pResult)
{
    switch (message)
    {
        case WM_CREATE:
            fmt::print("on create\n");
            break;
        case WM_SHOWWINDOW:
        {
            //OnCreate(GetSafeHwnd());
            //CRect dest;
            //GetClientRect(&dest);

            //fmt::print("WM_SHOWWINDOW size: {}\n", dest);

            //dwm_.set_size({dest.left, dest.top, dest.right, dest.bottom});
        }
        break;
        case WM_MOUSEMOVE:
        {
            const auto x_pos = GET_X_LPARAM(lParam);
            const auto y_pos = GET_Y_LPARAM(lParam);
            OnMouseMove(0, {x_pos, y_pos});
        }
        break;

        case WM_MOUSELEAVE:
            fmt::print("mouse leave\n");
            set_state(button_state::normal);
            Invalidate(FALSE);
            break;

        case WM_PAINT:
            break;

        case WM_SIZE:
        {
            // CRect dest;
            // GetClientRect(&dest);
            //
            // fmt::print("button on size: {}\n", dest);
            //
            // dwm_.set_size({dest.left, dest.top, dest.right, dest.bottom});
        }
        break;

        default:
            break;
    }
    // fmt::print("on wnd msg\n");

    return CWnd::OnWndMsg(message, wParam, lParam, pResult);
}

window_data &window_button::get_data()
{
    return data_;
}