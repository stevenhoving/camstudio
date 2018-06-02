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
#include "CamCapture/cam_gdiplus.h"
#include "CamCapture/annotations/cam_annotation_cursor.h"
#include "CamCapture/annotations/cam_annotation_systemtime.h"
#include <fmt/printf.h>
#include <memory>
#include <cassert>
#include <ctime>

cam_capture_source::cam_capture_source(HWND hwnd, const rect<int> &view)
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
        //width_ = ::GetSystemMetrics(SM_CXVIRTUALSCREEN);
        //height_ = ::GetSystemMetrics(SM_CYVIRTUALSCREEN);
        //x_ = ::GetSystemMetrics(SM_XVIRTUALSCREEN);
        //y_ = ::GetSystemMetrics(SM_YVIRTUALSCREEN);

        width_ = view.width();//::GetSystemMetrics(SM_CXVIRTUALSCREEN);
        height_ = view.height();//::GetSystemMetrics(SM_CYVIRTUALSCREEN);
        x_ = view.left();//::GetSystemMetrics(SM_XVIRTUALSCREEN);
        y_ = view.top();//::GetSystemMetrics(SM_YVIRTUALSCREEN);
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

    ZeroMemory(&bmp_info_, sizeof(BITMAPINFO));
    bmp_info_.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmp_info_.bmiHeader.biWidth = width_;
    /* \todo we can avoid doing the reverse stride rbg2yuv by making the height negative */
    bmp_info_.bmiHeader.biHeight = height_;
    bmp_info_.bmiHeader.biPlanes = 1;
    bmp_info_.bmiHeader.biBitCount = 32;
    bmp_info_.bmiHeader.biCompression = BI_RGB;

    bitmap_frame_ = ::CreateDIBSection(desktop_dc_, &bmp_info_, DIB_RGB_COLORS, (VOID**)&bmp_data_, NULL, 0);

    frame_.bi = &bmp_info_;
    frame_.lpBitmapBits = bmp_data_;

    /* \todo bitmap_info_ is unused ... cleanup */
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

    old_selected_bitmap_ = ::SelectObject(memory_dc_, bitmap_frame_);

    annotations_.emplace_back(
        std::make_unique<cam_annotation_cursor>(true, size<int>(100, 100), color(127, 255, 0, 0))
    );

    annotations_.emplace_back(
        std::make_unique<cam_annotation_systemtime>(point<int>(10, 10), color(255, 0, 0, 0))
    );
}

cam_capture_source::~cam_capture_source()
{
    ::SelectObject(memory_dc_, old_selected_bitmap_);

    ::DeleteDC(memory_dc_);
    ::ReleaseDC(hwnd_, desktop_dc_);
}

void cam_capture_source::set_capture_dst_rect(const rect<int> &view) noexcept
{
    dst_capture_rect_ = view;
}

bool cam_capture_source::capture_frame(const rect<int> &/*src_capture_rect*/)
{
    BOOL blit_ret = ::BitBlt(memory_dc_, 0, 0,
        dst_capture_rect_.width(), dst_capture_rect_.height(),
        desktop_dc_,
        dst_capture_rect_.left(), dst_capture_rect_.top(),
        SRCCOPY | CAPTUREBLT);

    if (!blit_ret)
        return false;

    _draw_annotations();

    return true;
}

const cam_frame *cam_capture_source::get_frame() const
{
    return &frame_;
}

void cam_capture_source::_draw_cursor()
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

void cam_capture_source::_draw_annotations()
{
    POINT pt;
    ::GetCursorPos(&pt);

    Gdiplus::Graphics canvas(memory_dc_);
    canvas.SetSmoothingMode(Gdiplus::SmoothingMode::SmoothingModeAntiAlias);

    point<int> mouse_point(pt.x, pt.y);

    cam_draw_data draw_data(0.1, dst_capture_rect_, mouse_point);

    for (const auto &annotation : annotations_)
        annotation->draw(canvas, draw_data);

}
