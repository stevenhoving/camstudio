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

#pragma once

#include "cam_rect.h"
#include "cam_point.h"

namespace Gdiplus
{
class Graphics;
} // namespace Gdiplus

class cam_draw_data
{
public:
    cam_draw_data(const double frame_delta, const rect<int> &canvast_rect, const point<int> &mouse_pos)
        : frame_delta_(frame_delta)
        , canvast_rect_(canvast_rect)
        , mouse_pos_(mouse_pos)
    {
    }

    const double frame_delta_;
    const rect<int> &canvast_rect_;
    const point<int> &mouse_pos_;
};

class cam_iannotation
{
public:
    virtual ~cam_iannotation() = default;
    virtual void draw(Gdiplus::Graphics &canvas, const cam_draw_data &draw_data) = 0;
};