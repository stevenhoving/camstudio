/**
 * Copyright(C) 2018 - 2020  Steven Hoving
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

#include "screen_capture/annotations/cam_annotation_cursor.h"
#include "screen_capture/cam_gdiplus.h"
#include "screen_capture/cam_draw_data.h"
#include <windows.h>
#include <stdexcept>
#include <cmath>

cam_annotation_cursor::cam_annotation_cursor(const bool cursor_enabled,
                                             const bool ring_enabled,
                                             const mouse_action_config &ring_left_click_config,
                                             const mouse_action_config &ring_right_click_config,
                                             const mouse_action_config &ring_middle_click_config,
                                             const cam_halo_type halo_type,
                                             const mouse_action_config &halo_config,
                                             const mouse_action_config &halo_left_click_config,
                                             const mouse_action_config &halo_right_click_config,
                                             const mouse_action_config &halo_middle_click_config) noexcept
    : cursor_enabled_(cursor_enabled)
    , halo_type_(halo_type)
    , halo_config_(halo_config)
    , halo_left_click_config_(halo_left_click_config)
    , halo_right_click_config_(halo_right_click_config)
    , halo_middle_click_config_(halo_middle_click_config)
    , ring_enabled_(ring_enabled)
    , ring_left_click_config_(ring_left_click_config)
    , ring_right_click_config_(ring_right_click_config)
    , ring_middle_click_config_(ring_middle_click_config)
{
}

cam_annotation_cursor::~cam_annotation_cursor() = default;

void cam_annotation_cursor::draw(Gdiplus::Graphics &canvas, const cam_draw_data &draw_data)
{
    const auto &rect = draw_data.canvas_rect_;
    const auto &position = draw_data.mouse_pos_;

    unsigned int mouse_button_state = 0;
    mouse_button_state |= (draw_data.mouse_button_state_ & cam_mouse_button::left_button_down);
    mouse_button_state |= (draw_data.mouse_button_state_ & cam_mouse_button::right_button_down);
    mouse_button_state |= (draw_data.mouse_button_state_ & cam_mouse_button::middle_button_down);

    if (ring_enabled_ && mouse_button_state != mouse_button_state_)
    {
        _handle_ring_button_state_changed(position, mouse_button_state, cam_mouse_button::left_button_down);
        _handle_ring_button_state_changed(position, mouse_button_state, cam_mouse_button::right_button_down);
        _handle_ring_button_state_changed(position, mouse_button_state, cam_mouse_button::middle_button_down);
    }

    mouse_button_state_ = mouse_button_state;

    /* only draw the normal halo if we don't have a mouse click to draw */
    if (halo_config_.enabled)
    {
        const auto mouse_click_enabled = halo_left_click_config_.enabled
            || halo_right_click_config_.enabled
            || halo_middle_click_config_.enabled;

        if (mouse_click_enabled && mouse_button_state_ == 0)
            _draw_halo(canvas, rect, position, halo_config_);
        else if (mouse_click_enabled == false)
            _draw_halo(canvas, rect, position, halo_config_);
    }

    if (halo_left_click_config_.enabled && (mouse_button_state_ & cam_mouse_button::left_button_down) != 0)
        _draw_halo(canvas, rect, position, halo_left_click_config_);

    if (halo_right_click_config_.enabled && (mouse_button_state_ & cam_mouse_button::right_button_down) != 0)
        _draw_halo(canvas, rect, position, halo_right_click_config_);

    if (halo_middle_click_config_.enabled && (mouse_button_state_ & cam_mouse_button::middle_button_down) != 0)
        _draw_halo(canvas, rect, position, halo_middle_click_config_);

    if (ring_enabled_ && !queued_rings_.empty())
        _draw_rings(canvas, rect, draw_data.frame_delta_);

    if (cursor_enabled_)
        _draw_cursor(canvas, rect, position);

    if (draw_data.mouse_button_state_ & cam_mouse_button::left_button_up)
        mouse_button_state_ &= ~cam_mouse_button::left_button_down;

    if (draw_data.mouse_button_state_ & cam_mouse_button::right_button_up)
        mouse_button_state_ &= ~cam_mouse_button::right_button_down;

    if (draw_data.mouse_button_state_ & cam_mouse_button::middle_button_up)
        mouse_button_state_ &= ~cam_mouse_button::middle_button_down;
}

void cam_annotation_cursor::set_cursor_enabled(const bool enabled) noexcept
{
    cursor_enabled_ = enabled;
}

void cam_annotation_cursor::set_cursor_ring_enabled(const bool enabled)
{
    if (enabled == ring_enabled_)
        return;

    queued_rings_.clear();
    ring_enabled_ = enabled;
}

void cam_annotation_cursor::set_ring_left_click_config(const mouse_action_config &config) noexcept
{
    ring_left_click_config_ = config;
}

void cam_annotation_cursor::set_ring_right_click_config(const mouse_action_config &config) noexcept
{
    ring_right_click_config_ = config;
}

void cam_annotation_cursor::set_ring_middle_click_config(const mouse_action_config &config) noexcept
{
    ring_middle_click_config_ = config;
}

void cam_annotation_cursor::set_halo_type(const cam_halo_type halo_type) noexcept
{
    halo_type_ = halo_type;
}

void cam_annotation_cursor::set_halo_config(const mouse_action_config &config) noexcept
{
    halo_config_ = config;
}

void cam_annotation_cursor::set_halo_left_click_config(const mouse_action_config &config) noexcept
{
    halo_left_click_config_ = config;
}

void cam_annotation_cursor::set_halo_right_click_config(const mouse_action_config &config) noexcept
{
    halo_right_click_config_ = config;
}

void cam_annotation_cursor::set_halo_middle_click_config(const mouse_action_config &config) noexcept
{
    halo_middle_click_config_ = config;
}

void cam_annotation_cursor::_handle_ring_button_state_changed(const point<int> &position, const unsigned int mouse_button_state, const cam_mouse_button::type mouse_button_type)
{
    const auto mouse_button_state_delta = mouse_button_state ^ mouse_button_state_;
    const auto is_state_changed = mouse_button_state_delta & mouse_button_type;
    const auto is_state_new = mouse_button_state & mouse_button_type;
    if (is_state_changed && is_state_new)
    {
        queued_rings_.emplace_back(
            cam_mouse_ring_state(position, mouse_button_type)
        );
    }
}

void cam_annotation_cursor::_draw_cursor(Gdiplus::Graphics &canvas, const cam::rect<int> &canvas_rect,
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

    cursor_x -= canvas_rect.left();
    cursor_y -= canvas_rect.top();

    cursor_x -= icon_info.xHotspot;
    cursor_y -= icon_info.yHotspot;

    const auto hdc = canvas.GetHDC();
    ::DrawIconEx(hdc, cursor_x, cursor_y, cursor_info.hCursor, 0, 0, 0, NULL, DI_NORMAL);
    canvas.ReleaseHDC(hdc);

    ::DeleteObject(icon_info.hbmColor);
    ::DeleteObject(icon_info.hbmMask);
}

void cam_annotation_cursor::_draw_halo(Gdiplus::Graphics &canvas, const cam::rect<int> &canvas_rect,
                                       const point<int> &mouse_pos,
                                       const mouse_action_config &halo_config)
{
    auto cursor_x = mouse_pos.x();
    auto cursor_y = mouse_pos.y();

    cursor_x -= canvas_rect.left();
    cursor_y -= canvas_rect.top();

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

void cam_annotation_cursor::_draw_rings(Gdiplus::Graphics &canvas, const cam::rect<int> &canvas_rect,
                                        const double frame_delta)
{
    std::vector<cam_mouse_ring_state *> dead_rings;
    for (auto &ring : queued_rings_)
    {
        if (_draw_ring(canvas, canvas_rect, ring, frame_delta))
            dead_rings.push_back(&ring);
    }

    for (const auto &dead_ring : dead_rings)
    {
        if (const auto itr = std::find(begin(queued_rings_), end(queued_rings_), *dead_ring); itr != end(queued_rings_))
            queued_rings_.erase(itr);
    }
}

bool cam_annotation_cursor::_draw_ring(Gdiplus::Graphics &canvas, const cam::rect<int> &canvas_rect,
                                       cam_mouse_ring_state &ring, const double frame_delta)
{
    const auto config = _get_ring_config(ring.ring_type_);

    ring.lifetime_ += frame_delta * ring_speed_;

    // \todo make sure that ring size is always 'odd'.
    const auto ring_size = static_cast<int>(std::round(ring.lifetime_ * 20));

    const auto center = ring.ring_center_;
    auto cursor_x = center.x();
    auto cursor_y = center.y();

    cursor_x -= canvas_rect.left();
    cursor_y -= canvas_rect.top();

    cursor_x -= ring_size / 2;
    cursor_y -= ring_size / 2;

    const auto ring_rect = Gdiplus::Rect(
        cursor_x,
        cursor_y,
        ring_size,
        ring_size
    );

    const auto color = Gdiplus::Color(config.color.a_, config.color.r_, config.color.g_,
        config.color.b_);

    const auto pen = Gdiplus::Pen(color, static_cast<Gdiplus::REAL>(ring_width_));
    canvas.DrawEllipse(&pen, ring_rect);

    // return true when this ring needs to be deleted.
    return ring_size > config.size.width();
}

auto cam_annotation_cursor::_get_ring_config(const cam_mouse_button::type mouse_button_type) -> mouse_action_config &
{
    switch(mouse_button_type)
    {
    case cam_mouse_button::left_button_down: return ring_left_click_config_;
    case cam_mouse_button::right_button_down: return ring_right_click_config_;
    case cam_mouse_button::middle_button_down: return ring_middle_click_config_;
    }

    throw std::runtime_error("invalid ring mouse button type");
}
