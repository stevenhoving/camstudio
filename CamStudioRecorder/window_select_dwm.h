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

#include "CamCapture/cam_rect.h"
#include <windows.h>
#include <dwmapi.h>

// \todo handle enabled/disabled dwm (only for win7, on win8 and higher you can no longer disable dwm)
class dwm_thumbnail
{
public:
    dwm_thumbnail() = default;
    ~dwm_thumbnail();

    void link(HWND dst, HWND src);
    void unlink();
    void set_size(rect<int> dst_size);

private:
    HTHUMBNAIL thumbnail_{ INVALID_HANDLE_VALUE };
};
