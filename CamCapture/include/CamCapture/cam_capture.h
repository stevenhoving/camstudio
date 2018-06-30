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

#pragma once

#include "cam_rect.h"
#include "cam_annotarion.h"
#include <windows.h>
#include <memory>
#include <vector>

struct cam_frame
{
    BITMAPINFO *bitmap_info{nullptr};
    unsigned char *bitmap_data{nullptr};
    int width{0};
    int height{0};
    int stride{0};
};

class cam_capture_source
{
public:
    cam_capture_source() = delete;
    cam_capture_source(HWND hwnd, const rect<int> &view);
    ~cam_capture_source();

    /*!
     * \param[in] src_capture_rect the rectangle of the capture source.
     * \todo the src_capture_rect does not belong here...
     */
    bool capture_frame(const rect<int> &capture_rect);
    const cam_frame *get_frame();

    void enable_annotations();

    void add_annotation(std::unique_ptr<cam_iannotation> annotation);

protected:
    void _draw_annotations(const rect<int> &capture_rect);

private:
    BITMAPINFO bitmap_info_;
    HBITMAP bitmap_frame_;
    HWND hwnd_;
    HDC desktop_dc_;
    HDC memory_dc_;
    rect<int> src_rect_;
    bool show_cursor_;

    unsigned char *bitmap_data_{nullptr};
    cam_frame frame_;
    rect<int> captured_rect_;

    HGDIOBJ old_selected_bitmap_{nullptr};

    bool enable_annotations_{false};
    std::vector<std::unique_ptr<cam_iannotation>> annotations_;
};
