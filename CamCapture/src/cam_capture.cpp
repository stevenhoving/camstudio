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

cam_capture_source::cam_capture_source(HWND hwnd, const rect<int> & /*view*/)
    : bitmap_info_{}
    , bitmap_frame_{nullptr}
    , hwnd_{hwnd}
    , desktop_dc_{::GetDC(hwnd_)}
    , memory_dc_{::CreateCompatibleDC(desktop_dc_)}
    , src_rect_()
    , show_cursor_{true}
    , annotations_()
{
    if (hwnd == nullptr)
    {
        src_rect_.right_ = ::GetSystemMetrics(SM_CXVIRTUALSCREEN);
        src_rect_.bottom_ = ::GetSystemMetrics(SM_CYVIRTUALSCREEN);
        src_rect_.left_ = ::GetSystemMetrics(SM_XVIRTUALSCREEN);
        src_rect_.top_ = ::GetSystemMetrics(SM_YVIRTUALSCREEN);
    }
    else
    {
        RECT window_rect = {};
        BOOL ret = ::GetWindowRect(hwnd, &window_rect);
        assert(ret != 0 && "Failed to get window rect.");

        src_rect_.left_ = window_rect.left;
        src_rect_.top_ = window_rect.top;
        src_rect_.right_ = window_rect.right;
        src_rect_.bottom_ = window_rect.bottom;
    }

    bitmap_info_.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bitmap_info_.bmiHeader.biWidth = src_rect_.width();
    bitmap_info_.bmiHeader.biHeight = -src_rect_.height();
    bitmap_info_.bmiHeader.biPlanes = 1;
    bitmap_info_.bmiHeader.biBitCount = 32;
    bitmap_info_.bmiHeader.biCompression = BI_RGB;

    bitmap_frame_ = ::CreateDIBSection(desktop_dc_, &bitmap_info_, DIB_RGB_COLORS,
        reinterpret_cast<void **>(&bitmap_data_), nullptr, 0);

    /* \todo handle CreateDIBSection failure */
    assert(bitmap_frame_);

    frame_.bitmap_info = &bitmap_info_;
    frame_.bitmap_data = bitmap_data_;

#if 0
    /* \todo make adding these annotations optional */
    annotations_.emplace_back(
        std::make_unique<cam_annotation_cursor>(true, true, cam_halo_type::circle,
            cam::size<int>(100, 100), color(127, 255, 0, 0))
    );

    annotations_.emplace_back(
        std::make_unique<cam_annotation_systemtime>(point<int>(10, 10), color(255, 0, 0, 0))
    );
#endif
}

cam_capture_source::~cam_capture_source()
{
    ::DeleteDC(memory_dc_);
    ::ReleaseDC(hwnd_, desktop_dc_);
}

bool cam_capture_source::capture_frame(const rect<int> &capture_rect)
{
    old_selected_bitmap_ = ::SelectObject(memory_dc_, bitmap_frame_);
    const auto ret = ::BitBlt(memory_dc_, 0, 0,
        capture_rect.width(), capture_rect.height(),
        desktop_dc_,
        capture_rect.left(), capture_rect.top(),
        SRCCOPY | CAPTUREBLT);

    assert(ret);
    if (!ret)
    {
        ::SelectObject(memory_dc_, old_selected_bitmap_);
        return false;
    }

    captured_rect_ = capture_rect;

    _draw_annotations(capture_rect);

    ::SelectObject(memory_dc_, old_selected_bitmap_);

    return true;
}

const cam_frame *cam_capture_source::get_frame()
{
    frame_.width = captured_rect_.width();
    frame_.height = captured_rect_.height();
    /* \todo make the bytes per pixel optional or based on the actual Bpp */
    frame_.stride = src_rect_.width() * 4;
    return &frame_;
}

void cam_capture_source::enable_annotations()
{
    enable_annotations_ = true;
}

void cam_capture_source::add_annotation(std::unique_ptr<cam_iannotation> annotation)
{
    annotations_.emplace_back(std::move(annotation));
}

void cam_capture_source::_draw_annotations(const rect<int> &capture_rect)
{
    if (!enable_annotations_)
        return;
    if (annotations_.empty())
        return;

    {
        POINT pt;
        ::GetCursorPos(&pt);

        Gdiplus::Graphics canvas(memory_dc_);
        canvas.SetSmoothingMode(Gdiplus::SmoothingMode::SmoothingModeAntiAlias);

        point<int> mouse_point(pt.x, pt.y);

        cam_draw_data draw_data(0.1, capture_rect, mouse_point);

        for (const auto &annotation : annotations_)
            annotation->draw(canvas, draw_data);
    }
}
