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

#include "annotations/cam_annotation_cursor.h"
#include "cam_gdiplus.h"
#include <fmt/printf.h>
#include <windows.h>

cam_annotation_cursor::cam_annotation_cursor(bool halo_enabled, size<int> halo_size, color halo_color) noexcept
    : halo_enabled_(halo_enabled)
    , halo_size_(halo_size)
    , halo_color_(halo_color)
{
}

cam_annotation_cursor::~cam_annotation_cursor()
{
}

void cam_annotation_cursor::draw(Gdiplus::Graphics &canvas, const cam_draw_data &draw_data)
{
    _draw_extras(canvas, draw_data.canvast_rect_, draw_data.mouse_pos_);
    _draw_cursor(canvas, draw_data.canvast_rect_, draw_data.mouse_pos_);
}

void cam_annotation_cursor::_draw_cursor(Gdiplus::Graphics &canvas, const rect<int> &canvast_rect,
                                         const point<int> &mouse_pos)
{
    CURSORINFO cursor_info = {};
    cursor_info.cbSize = sizeof(CURSORINFO);

    BOOL ret = ::GetCursorInfo(&cursor_info);
    if (!ret)
        return;

    if (!(cursor_info.flags & CURSOR_SHOWING))
        return;

    ICONINFO icon_info;
    ret = ::GetIconInfo(cursor_info.hCursor, &icon_info);
    if (!ret)
        return;

    auto cursor_x = cursor_info.ptScreenPos.x;
    auto cursor_y = cursor_info.ptScreenPos.y;

    cursor_x -= canvast_rect.left();
    cursor_y -= canvast_rect.top();

    cursor_x -= icon_info.xHotspot;
    cursor_y -= icon_info.yHotspot;

    const auto hdc = canvas.GetHDC();
    ::DrawIconEx(hdc, cursor_x, cursor_y, cursor_info.hCursor, 0, 0, 0, NULL, DI_NORMAL);
    canvas.ReleaseHDC(hdc);

    ::DeleteObject(icon_info.hbmColor);
    ::DeleteObject(icon_info.hbmMask);
}

void cam_annotation_cursor::_draw_extras(Gdiplus::Graphics &canvas, const rect<int> &canvast_rect,
                                         const point<int> &mouse_pos)
{
    const auto r = Gdiplus::Rect(
        mouse_pos.x() - halo_size_.width() / 2,
        mouse_pos.y() - halo_size_.height() / 2,
        halo_size_.width(),
        halo_size_.height()
    );
    const Gdiplus::Color c(halo_color_.a_, halo_color_.r_, halo_color_.g_, halo_color_.b_);
    const Gdiplus::SolidBrush b(c);
    canvas.FillEllipse(&b, r);
}
