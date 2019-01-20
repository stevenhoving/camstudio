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
#include "ui/dwm/dwm_thumbnail.h"
#include "logging/logging.h"
#include <fmt/printf.h>

static auto logger = logging::get_logger("dwm::thumbnail");

namespace dwm
{

thumbnail::~thumbnail()
{
    unlink();
}

void thumbnail::link(HWND dst, HWND src)
{
    if (const auto ret = DwmRegisterThumbnail(dst, src, &thumbnail_); ret != S_OK)
    {
        logger->error("unable to register dwm thumbnail relation - {}", ret);
        fmt::print("unable to register dwm thumbnail relation - {}\n", ret);
    }
}

void thumbnail::unlink()
{
    DwmUnregisterThumbnail(thumbnail_);
    thumbnail_ = nullptr;
}

void thumbnail::set_viewport(const cam::rect<int> &src_rect, const cam::rect<int> &dst_rect)
{
    DWM_THUMBNAIL_PROPERTIES dskThumbProps;
    dskThumbProps.dwFlags = DWM_TNP_RECTDESTINATION | DWM_TNP_VISIBLE | DWM_TNP_SOURCECLIENTAREAONLY;
    dskThumbProps.fSourceClientAreaOnly = FALSE;
    dskThumbProps.fVisible = TRUE;
    dskThumbProps.opacity = 255; //(255 * 70) / 100;
    dskThumbProps.rcSource = {src_rect.left(), src_rect.top(), src_rect.right(), src_rect.bottom()};
    dskThumbProps.rcDestination = {dst_rect.left(), dst_rect.top(), dst_rect.right(), dst_rect.bottom()};

    if (const auto ret = DwmUpdateThumbnailProperties(thumbnail_, &dskThumbProps); ret < 0)
    {
        logger->error("unable to update thumbnail properties");
        fmt::print("unable to update thumbnail properties\n");
    }
}

cam::rect<int> thumbnail::_get_src_size()
{
    SIZE src_size;
    if (const auto ret = DwmQueryThumbnailSourceSize(thumbnail_, &src_size); ret != S_OK)
    {
        logger->error("failed to query source file\n");
        return {0, 0, 0, 0};
    }
    return {0, 0, src_size.cx, src_size.cy};
}

} // namespace dwm
