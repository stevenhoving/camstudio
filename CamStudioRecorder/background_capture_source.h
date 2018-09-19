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

#include <CamCapture/cam_rect.h>
#include <windows.h>
#include <cassert>

// \todo move this to camcapture library

// capture source that creates compatible bitmaps.
class background_capture_source
{
public:
    background_capture_source(HWND hwnd);

    void capture_frame();
    HBITMAP get_frame();
    cam::rect<int> get_size();

private:
    BITMAPINFO bitmap_info_{};
    HBITMAP bitmap_frame_{nullptr};
    HWND hwnd_{nullptr};
    HDC desktop_dc_{nullptr};
    HDC memory_dc_{nullptr};
    cam::rect<int> src_rect_{0, 0, 0, 0};
};
