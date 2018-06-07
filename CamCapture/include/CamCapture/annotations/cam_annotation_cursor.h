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

#include "CamCapture/cam_annotarion.h"
#include "CamCapture/cam_size.h"
#include "CamCapture/cam_color.h"

class cam_annotation_cursor : public cam_iannotation
{
public:
    cam_annotation_cursor() noexcept = default;
    cam_annotation_cursor(bool halo_enabled, const size<int> &halo_size, color halo_color) noexcept;
    ~cam_annotation_cursor() override;

    void draw(Gdiplus::Graphics &canvas, const cam_draw_data &draw_data) override;
protected:
    void _draw_cursor(Gdiplus::Graphics &canvas, const rect<int> &canvast_rect, const point<int> &mouse_pos);
    void _draw_extras(Gdiplus::Graphics &canvas, const rect<int> &canvast_rect, const point<int> &mouse_pos);
private:
    //bool halo_enabled_{false};
    bool halo_enabled_{true};
    size<int> halo_size_{0,0};
    color halo_color_{};
};
