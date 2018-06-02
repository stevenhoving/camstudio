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
#include "cam_annotarion.h"
#include <windows.h>
#include <memory>
#include <vector>

struct cam_frame
{
    BITMAPINFO *bi{nullptr};
    unsigned char *lpBitmapBits{nullptr};
};

class cam_capture_source
{
public:
    cam_capture_source() = delete;
    cam_capture_source(HWND hwnd, const rect<int> &view);
    ~cam_capture_source();

    void set_capture_dst_rect(const rect<int> &view) noexcept;

    /*!
     * \param[in] src_capture_rect the rectangle of the capture source.
     * \todo the src_capture_rect does not belong here...
     */
    bool capture_frame(const rect<int> &src_capture_rect);
    const cam_frame *get_frame() const;

protected:
    // \todo is a cursor a annotation?
    void _draw_cursor();
    void _draw_annotations();

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

    BITMAPINFO bmp_info_;
    unsigned char *bmp_data_{nullptr};
    cam_frame frame_;

    HGDIOBJ old_selected_bitmap_{nullptr};

    // rect to actually capture
    rect<int> dst_capture_rect_;
    std::vector<std::unique_ptr<cam_iannotation>> annotations_;
};
