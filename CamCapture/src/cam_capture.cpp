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
 * along with this program.If not, see < https://www.gnu.org/licenses/>.
 */
#include "CamCapture/cam_capture.h"
#include "CamCapture/cam_point.h"
#include "CamCapture/cam_size.h"

#include <ctime>
#include <cassert>

 // for using std::min, std::max in gdiplus
#include <algorithm>
namespace Gdiplus
{
using std::max;
using std::min;
}
#include <gdiplus.h>

cam_capture_source::cam_capture_source(HWND hwnd)
    : bitmap_info_{}
    , bitmap_frame_{nullptr}
    , hwnd_{hwnd}
    , desktop_dc_{::GetDC(hwnd_)}
    , memory_dc_{::CreateCompatibleDC(desktop_dc_)}
    , width_{0}
    , height_{0}
    , x_{0}
    , y_{0}
    , show_cursor_{true}
{
    if (hwnd == 0)
    {
        width_ = ::GetSystemMetrics(SM_CXVIRTUALSCREEN);
        height_ = ::GetSystemMetrics(SM_CYVIRTUALSCREEN);
        x_ = ::GetSystemMetrics(SM_XVIRTUALSCREEN);
        y_ = ::GetSystemMetrics(SM_YVIRTUALSCREEN);
    }
    else
    {
        RECT window_rect = {};
        BOOL ret = ::GetWindowRect(hwnd, &window_rect);
        assert(ret != 0 && "Failed to get window rect.");

        width_ = window_rect.right - window_rect.left;
        height_ = window_rect.bottom - window_rect.top;
        x_ = 0;
        y_ = 0;
    }

    bitmap_frame_ = ::CreateCompatibleBitmap(desktop_dc_, width_, height_);

    bitmap_info_.biSize = sizeof(BITMAPINFOHEADER);
    bitmap_info_.biWidth = width_;
    bitmap_info_.biHeight = height_;
    bitmap_info_.biPlanes = 1;
    bitmap_info_.biBitCount = 32;
    bitmap_info_.biCompression = BI_RGB;
    bitmap_info_.biSizeImage = 0;
    bitmap_info_.biXPelsPerMeter = 0;
    bitmap_info_.biYPelsPerMeter = 0;
    bitmap_info_.biClrUsed = 0;
    bitmap_info_.biClrImportant = 0;
}

cam_capture_source::~cam_capture_source()
{
    ::DeleteDC(memory_dc_);
    ::ReleaseDC(hwnd_, desktop_dc_);
}

void cam_capture_source::set_capture_dst_rect(const rect<int> &view) noexcept
{
    dst_capture_rect_ = view;
}

bool cam_capture_source::capture_frame(const rect<int> &/*src_capture_rect*/)
{
    const auto previous_object = ::SelectObject(memory_dc_, bitmap_frame_);

    //BOOL blit_ret = ::BitBlt(memory_dc_, 0, 0, width_, height_, desktop_dc_, x_, y_, SRCCOPY | CAPTUREBLT);
    BOOL blit_ret = ::BitBlt(memory_dc_, 0, 0,
        dst_capture_rect_.width(), dst_capture_rect_.height(),
        desktop_dc_,
        dst_capture_rect_.left(), dst_capture_rect_.top(),
        SRCCOPY | CAPTUREBLT);

    if (!blit_ret)
    {
        ::SelectObject(memory_dc_, previous_object);
        return false;
    }

    if (show_cursor_)
        _draw_cursor();

    ::SelectObject(memory_dc_, previous_object);

    captured_frame_.CreateFromHBITMAP(bitmap_frame_);

    return true;
}

BITMAPINFOHEADER *cam_capture_source::get_frame() const
{
    return static_cast<BITMAPINFOHEADER *>(captured_frame_.GetDIB());
}

void cam_capture_source::_draw_cursor() const
{
    /* \todo cache mouse cursor */
    CURSORINFO cursor_info = {};
    cursor_info.cbSize = sizeof(CURSORINFO);

    BOOL ret = ::GetCursorInfo(&cursor_info);
    if (!ret)
        return;

    if (!(cursor_info.flags & CURSOR_SHOWING))
        return;

    ICONINFO icon_info;
    ::GetIconInfo(cursor_info.hCursor, &icon_info);

    auto cursor_x = cursor_info.ptScreenPos.x;
    auto cursor_y = cursor_info.ptScreenPos.y;

    cursor_x -= x_;
    cursor_y -= y_;

    cursor_x -= icon_info.xHotspot;
    cursor_y -= icon_info.yHotspot;

    ::DrawIconEx(memory_dc_, cursor_x, cursor_y, cursor_info.hCursor, 0, 0, 0, NULL, DI_NORMAL);

    ::DeleteObject(icon_info.hbmColor);
    ::DeleteObject(icon_info.hbmMask);
}
