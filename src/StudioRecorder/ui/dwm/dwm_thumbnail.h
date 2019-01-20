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
#pragma once

#include <screen_capture/cam_rect.h>
#include <windef.h>
#include <dwmapi.h>

namespace dwm
{

/*
 * \note only for win 7 it is possible to disable DWM. This means that we have to check for OS
 * version and supply a alternative method.
 * From win 8 it is no longer possible to disable DWM.
 */
class thumbnail
{
public:
    thumbnail() = default;
    ~thumbnail();

    void link(HWND dst, HWND src);
    void unlink();
    void set_viewport(const cam::rect<int> &src_rect, const cam::rect<int> &dst_rect);
private:
    cam::rect<int> _get_src_size();

private:
    HTHUMBNAIL thumbnail_{ INVALID_HANDLE_VALUE };
};

} // namespace dwm
