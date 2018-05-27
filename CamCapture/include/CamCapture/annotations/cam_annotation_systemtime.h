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
#include "CamCapture/cam_point.h"
#include "CamCapture/cam_color.h"
#include <memory>


class cam_annotation_systemtime : public cam_iannotation
{
public:
    cam_annotation_systemtime() noexcept = default;
    cam_annotation_systemtime(point<int> systemtime_position, color systemtime_color) noexcept;
    ~cam_annotation_systemtime() override;

    void draw(Gdiplus::Graphics &canvas, const cam_draw_data &draw_data) override;
private:
    point<int> systemtime_position_;
    color systemtime_color_;
    std::unique_ptr<struct font_ptr> font_pimpl_;
};
