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

#pragma once

#include "cam_rect.h"
#include <ximage.h>
#include <windows.h>

class cam_capture_source
{
public:
    cam_capture_source() = delete;
    cam_capture_source(HWND hwnd);
    ~cam_capture_source();

    void set_capture_dst_rect(const rect<int> &view) noexcept;

    /*!
     * \param[in] src_capture_rect the rectangle of the capture source.
     * \todo the src_capture_rect does not belong here...
     */
    bool capture_frame(const rect<int> &src_capture_rect);
    BITMAPINFOHEADER *get_frame() const;

protected:
    // \todo is a cursor a annotation?
    void _draw_cursor() const;

private:
    BITMAPINFOHEADER bitmap_info_;
    HBITMAP bitmap_frame_;
    HWND hwnd_;
    HDC desktop_dc_;
    HDC memory_dc_;
    int width_;
    int height_;
    int x_;
    int y_;
    bool show_cursor_;

    CxImage captured_frame_;

    // rect to actually capture
    rect<int> dst_capture_rect_;
};
