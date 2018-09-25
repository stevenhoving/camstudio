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
#include "window_select_dwm.h"
#include <fmt/printf.h>

dwm_thumbnail::~dwm_thumbnail()
{
}

void dwm_thumbnail::link(HWND dst, HWND src)
{
    if (const auto ret = DwmRegisterThumbnail(dst, src, &thumbnail_); ret != S_OK)
        fmt::print("unable to register dwm thumbnail relation - {}\n", ret);
}

void dwm_thumbnail::unlink()
{
    DwmUnregisterThumbnail(thumbnail_);
    thumbnail_ = nullptr;
}

void dwm_thumbnail::set_size(cam::rect<int> dst_size)
{
    DWM_THUMBNAIL_PROPERTIES dskThumbProps;
    dskThumbProps.dwFlags = DWM_TNP_RECTDESTINATION | DWM_TNP_VISIBLE | DWM_TNP_SOURCECLIENTAREAONLY;
    dskThumbProps.fSourceClientAreaOnly = FALSE;
    dskThumbProps.fVisible = TRUE;
    dskThumbProps.opacity = 255; //(255 * 70) / 100;
    dskThumbProps.rcDestination = {dst_size.left(), dst_size.top(), dst_size.right(), dst_size.bottom()};
    if (const auto ret = DwmUpdateThumbnailProperties(thumbnail_, &dskThumbProps); ret < 0)
        fmt::print("unable to update thumbnail props\n");
}

cam::rect<int> dwm_thumbnail::_get_src_size()
{
    SIZE src_size;
    if (const auto ret = DwmQueryThumbnailSourceSize(thumbnail_, &src_size); ret != S_OK)
    {
        fmt::print("failed to query source file\n");
        return {0, 0, 0, 0};
    }
    return {0, 0, src_size.cx, src_size.cy};
}
