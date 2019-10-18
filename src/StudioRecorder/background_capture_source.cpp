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
#include "background_capture_source.h"
#include "utility/rect_util.h"

 background_capture_source::background_capture_source(HWND hwnd)
    : hwnd_{hwnd}
    , desktop_dc_{::GetDC(hwnd_)}
    , memory_dc_{::CreateCompatibleDC(desktop_dc_)}
{
    assert(desktop_dc_);
    assert(memory_dc_);
    if (hwnd == nullptr)
    {
        src_rect_.left(::GetSystemMetrics(SM_XVIRTUALSCREEN));
        src_rect_.top(::GetSystemMetrics(SM_YVIRTUALSCREEN));
        src_rect_.width(::GetSystemMetrics(SM_CXVIRTUALSCREEN));
        src_rect_.bottom(::GetSystemMetrics(SM_CYVIRTUALSCREEN));
    }
    else
    {
        RECT window_rect = {};
        BOOL ret = ::GetWindowRect(hwnd, &window_rect);
        assert(ret != 0 && "Failed to get window rect.");
        (void)ret;

        src_rect_.left(window_rect.left);
        src_rect_.top(window_rect.top);
        src_rect_.right(window_rect.right);
        src_rect_.bottom(window_rect.bottom);
    }

    bitmap_info_.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bitmap_info_.bmiHeader.biWidth = src_rect_.width();
    bitmap_info_.bmiHeader.biHeight = -src_rect_.height();
    bitmap_info_.bmiHeader.biPlanes = 1;
    bitmap_info_.bmiHeader.biBitCount = 32;
    bitmap_info_.bmiHeader.biCompression = BI_RGB;

    bitmap_frame_ = ::CreateCompatibleBitmap(desktop_dc_, src_rect_.width(), src_rect_.height());
    assert(bitmap_frame_);
}

 background_capture_source::background_capture_source(HWND hwnd, RECT rect)
     : hwnd_{ hwnd }
     , desktop_dc_{ ::GetDC(hwnd_) }
     , memory_dc_{ ::CreateCompatibleDC(desktop_dc_) }
 {
     assert(desktop_dc_);
     assert(memory_dc_);
     if (hwnd == nullptr)
     {
         //src_rect_.left(::GetSystemMetrics(SM_XVIRTUALSCREEN));
         //src_rect_.top(::GetSystemMetrics(SM_YVIRTUALSCREEN));
         //src_rect_.width(::GetSystemMetrics(SM_CXVIRTUALSCREEN));
         //src_rect_.bottom(::GetSystemMetrics(SM_CYVIRTUALSCREEN));
         src_rect_ = utility::from_rect(rect);
     }
     else
     {
         RECT window_rect = {};
         BOOL ret = ::GetWindowRect(hwnd, &window_rect);
         assert(ret != 0 && "Failed to get window rect.");
         (void)ret;

         src_rect_ = utility::from_rect(rect);

         //src_rect_.left(window_rect.left);
         //src_rect_.top(window_rect.top);
         //src_rect_.right(window_rect.right);
         //src_rect_.bottom(window_rect.bottom);
     }

     bitmap_info_.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
     bitmap_info_.bmiHeader.biWidth = src_rect_.width();
     bitmap_info_.bmiHeader.biHeight = -src_rect_.height();
     bitmap_info_.bmiHeader.biPlanes = 1;
     bitmap_info_.bmiHeader.biBitCount = 32;
     bitmap_info_.bmiHeader.biCompression = BI_RGB;

     bitmap_frame_ = ::CreateCompatibleBitmap(desktop_dc_, src_rect_.width(), src_rect_.height());
     assert(bitmap_frame_);
 }

void background_capture_source::capture_frame()
{
    const auto old_bitmap_ = ::SelectObject(memory_dc_, bitmap_frame_);
    const auto ret = ::BitBlt(memory_dc_, 0, 0, src_rect_.width(), src_rect_.height(), desktop_dc_, src_rect_.left(),
                              src_rect_.top(), SRCCOPY | CAPTUREBLT);
    ::SelectObject(memory_dc_, old_bitmap_);

    assert(ret);
    if (!ret)
    {
        return;
    }
}

HBITMAP background_capture_source::get_frame()
{
    return bitmap_frame_;
}

cam::rect<int> background_capture_source::get_size()
{
    return src_rect_;
}
