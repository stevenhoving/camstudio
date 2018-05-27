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

#include "annotations/cam_annotation_systemtime.h"
#include "cam_gdiplus.h"
#include <fmt/time.h>
#include <windows.h>
#include <chrono>

struct font_ptr
{
    std::unique_ptr<Gdiplus::Font> myFont;
    std::unique_ptr<Gdiplus::SolidBrush> blackBrush;
};

cam_annotation_systemtime::cam_annotation_systemtime(point<int> systemtime_position, color systemtime_color) noexcept
    : systemtime_position_(systemtime_position)
    , systemtime_color_(systemtime_color)
{
    font_pimpl_ = std::make_unique<font_ptr>();
    font_pimpl_->myFont = std::make_unique<Gdiplus::Font>(L"Arial", 16.f);
    font_pimpl_->blackBrush = std::make_unique<Gdiplus::SolidBrush>(Gdiplus::Color(255, 255, 0, 0));
}

cam_annotation_systemtime::~cam_annotation_systemtime()
{
}

void cam_annotation_systemtime::draw(Gdiplus::Graphics &canvas, const rect<int> &canvast_rect,
    const point<int> &mouse_pos)
{
    const auto tp = std::chrono::system_clock::now();
    const auto t = std::chrono::system_clock::to_time_t(tp);

    const auto seconds = std::chrono::time_point_cast<std::chrono::seconds>(tp);
    const auto fraction = tp - seconds;
    const auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(fraction);

    const auto str = fmt::format(L"{:%Y-%m-%d %H:%M:%S}.{:03d}", *std::localtime(&t),
        milliseconds.count());

    Gdiplus::PointF pos;
    pos.X = (float)systemtime_position_.x();
    pos.Y = (float)systemtime_position_.y();

    canvas.DrawString(str.c_str(), (INT)str.size(), font_pimpl_->myFont.get(),
        pos,
        font_pimpl_->blackBrush.get()
    );
}
