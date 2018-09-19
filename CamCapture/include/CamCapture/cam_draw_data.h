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

template<typename T>
class rect;

template<typename T>
class point;

class cam_draw_data
{
public:
    constexpr cam_draw_data(const double frame_delta, const cam::rect<int> &canvas_rect,
                            const point<int> &mouse_pos, const cam_mouse_button::type mouse_button_state) noexcept
        : frame_delta_(frame_delta)
        , canvas_rect_(canvas_rect)
        , mouse_pos_(mouse_pos)
        , mouse_button_state_(mouse_button_state)
    {
    }

    double frame_delta_;
    const cam::rect<int> &canvas_rect_;
    const point<int> &mouse_pos_;
    cam_mouse_button::type mouse_button_state_;
};
