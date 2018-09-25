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

#include "CamCapture/cam_mouse_button.h"
#include "CamCapture/cam_annotarion.h"
#include "CamCapture/cam_color.h"
#include "CamCapture/cam_point.h"
#include "CamCapture/cam_size.h"
#include "CamCapture/cam_rect.h"
#include <vector>

enum class cam_halo_type
{
    circle,
    ellipse,
    square,
    rectangle
};

struct mouse_action_config
{
    bool enabled{false};
    cam::size<int> size{ 0, 0 };
    cam::color color{};
};

class cam_mouse_ring_state
{
public:
    constexpr cam_mouse_ring_state(const point<int> &ring_center, const cam_mouse_button::type ring_type) noexcept
        : ring_center_(ring_center)
        , ring_type_(ring_type)
    {
    }

    point<int> ring_center_;
    double lifetime_{ 0.0 };
    cam_mouse_button::type ring_type_;
};

constexpr bool operator == (const cam_mouse_ring_state &lhs, const cam_mouse_ring_state &rhs)
{
    return lhs.lifetime_ == rhs.lifetime_
        && lhs.ring_center_ == rhs.ring_center_
        && lhs.ring_type_ == rhs.ring_type_;
}

class cam_annotation_cursor : public cam_iannotation
{
public:
    cam_annotation_cursor() noexcept = default;
    cam_annotation_cursor(
        const bool cursor_enabled,
        const bool ring_enabled,
        const mouse_action_config &ring_left_click_config,
        const mouse_action_config &ring_right_click_config,
        const mouse_action_config &ring_middle_click_config,
        const cam_halo_type halo_type,
        const mouse_action_config &halo_config,
        const mouse_action_config &halo_left_click_config,
        const mouse_action_config &halo_right_click_config,
        const mouse_action_config &halo_middle_click_config) noexcept;

    ~cam_annotation_cursor() override;

    void draw(Gdiplus::Graphics &canvas, const cam_draw_data &draw_data) override;

    void set_cursor_enabled(const bool enabled) noexcept;
    void set_cursor_ring_enabled(const bool enabled);

    void set_ring_left_click_config(const mouse_action_config &config) noexcept;
    void set_ring_right_click_config(const mouse_action_config &config) noexcept;
    void set_ring_middle_click_config(const mouse_action_config &config) noexcept;

    void set_halo_type(const cam_halo_type halo_type) noexcept;
    void set_halo_config(const mouse_action_config &config) noexcept;

    void set_halo_left_click_config(const mouse_action_config &config) noexcept;
    void set_halo_right_click_config(const mouse_action_config &config) noexcept;
    void set_halo_middle_click_config(const mouse_action_config &config) noexcept;

protected:
    void _handle_ring_button_state_changed(const point<int> &position, const unsigned int mouse_button_state, const cam_mouse_button::type mouse_button_type);

    void _draw_cursor(Gdiplus::Graphics &canvas, const cam::rect<int> &canvas_rect, const point<int> &mouse_pos);
    void _draw_halo(Gdiplus::Graphics &canvas, const cam::rect<int> &canvas_rect, const point<int> &mouse_pos, const mouse_action_config &halo_config);
    void _draw_rings(Gdiplus::Graphics &canvas, const cam::rect<int> &canvas_rect, const double frame_delta);
    auto _draw_ring(Gdiplus::Graphics &canvas, const cam::rect<int> &canvas_rect, cam_mouse_ring_state &ring, const double frame_delta) -> bool;

    auto _get_ring_config(const cam_mouse_button::type mouse_button_type) -> mouse_action_config &;
private:
    // 'cached' config fields
    bool cursor_enabled_{false};

    // Halo config fields
    cam_halo_type halo_type_{cam_halo_type::circle};
    mouse_action_config halo_config_{};
    mouse_action_config halo_left_click_config_{};
    mouse_action_config halo_right_click_config_{};
    mouse_action_config halo_middle_click_config_{};

    // Ring config fields
    bool ring_enabled_{false};
    int ring_speed_{5};
    double ring_width_{1.5};
    mouse_action_config ring_left_click_config_{};
    mouse_action_config ring_right_click_config_{};
    mouse_action_config ring_middle_click_config_{};

    std::vector<cam_mouse_ring_state> queued_rings_;

    // draw 'runtime' settings.
    unsigned int mouse_button_state_{0};
};
