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

#include "annotations/cam_annotation_cursor.h"
#include "cam_gdiplus.h"
#include <fmt/printf.h>
#include <windows.h>

cam_annotation_cursor::cam_annotation_cursor(bool cursor_enabled, const cam_halo_type halo_type,
                                             const mouse_action_config &halo_config,
                                             const mouse_action_config &left_click_config,
                                             const mouse_action_config &right_click_config) noexcept
    : cursor_enabled_(cursor_enabled)
    , halo_type_(halo_type)
    , halo_config_(halo_config)
    , left_click_config_(left_click_config)
    , right_click_config_(right_click_config)
{
}

cam_annotation_cursor::~cam_annotation_cursor() = default;

void cam_annotation_cursor::draw(Gdiplus::Graphics &canvas, const cam_draw_data &draw_data)
{
    if (draw_data.mouse_button_state_ & cam_mouse_button::left_button_down)
        mouse_button_state_ |= cam_mouse_button::left_button_down;

    if (draw_data.mouse_button_state_ & cam_mouse_button::right_button_down)
        mouse_button_state_ |= cam_mouse_button::right_button_down;

    /* only draw the normal halo if we don't have a mouse click to draw */
    if (halo_config_.enabled && mouse_button_state_ == 0)
        _draw_halo(canvas, draw_data.canvast_rect_, draw_data.mouse_pos_, halo_config_);

    if (left_click_config_.enabled && (mouse_button_state_ & cam_mouse_button::left_button_down) != 0)
        _draw_halo(canvas, draw_data.canvast_rect_, draw_data.mouse_pos_, left_click_config_);

    if (right_click_config_.enabled && (mouse_button_state_ & cam_mouse_button::right_button_down) != 0)
        _draw_halo(canvas, draw_data.canvast_rect_, draw_data.mouse_pos_, right_click_config_);

    if (cursor_enabled_)
        _draw_cursor(canvas, draw_data.canvast_rect_, draw_data.mouse_pos_);

    if (draw_data.mouse_button_state_ & cam_mouse_button::left_button_up)
        mouse_button_state_ &= ~cam_mouse_button::left_button_down;

    if (draw_data.mouse_button_state_ & cam_mouse_button::right_button_up)
        mouse_button_state_ &= ~cam_mouse_button::right_button_down;
}

void cam_annotation_cursor::_draw_cursor(Gdiplus::Graphics &canvas, const rect<int> &canvast_rect,
                                         const point<int> &mouse_pos)
{
    CURSORINFO cursor_info = {};
    cursor_info.cbSize = sizeof(CURSORINFO);

    auto ret = ::GetCursorInfo(&cursor_info);
    if (!ret)
        return;

    if (!(cursor_info.flags & CURSOR_SHOWING))
        return;

    ICONINFO icon_info;
    ret = ::GetIconInfo(cursor_info.hCursor, &icon_info);
    if (!ret)
        return;

    auto cursor_x = mouse_pos.x();
    auto cursor_y = mouse_pos.y();

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

void cam_annotation_cursor::_draw_halo(Gdiplus::Graphics &canvas, const rect<int> &canvast_rect,
                                       const point<int> &mouse_pos, const mouse_action_config &halo_config)
{
    auto cursor_x = mouse_pos.x();
    auto cursor_y = mouse_pos.y();

    cursor_x -= canvast_rect.left();
    cursor_y -= canvast_rect.top();

    cursor_x -= halo_config.size.width() / 2;
    cursor_y -= halo_config.size.height() / 2;

    const auto halo_rect = Gdiplus::Rect(
        cursor_x,
        cursor_y,
        halo_config.size.width(),
        halo_config.size.height()
    );
    const Gdiplus::Color c(
        halo_config.color.a_,
        halo_config.color.r_,
        halo_config.color.g_,
        halo_config.color.b_);
    const Gdiplus::SolidBrush b(c);

    switch(halo_type_)
    {
    case cam_halo_type::circle:
        [[fallthrough]];
    case cam_halo_type::ellipse:
        canvas.FillEllipse(&b, halo_rect);
        break;

    case cam_halo_type::square:
        [[fallthrough]];
    case cam_halo_type::rectangle:
        canvas.FillRectangle(&b, halo_rect);
        break;
    }
}
