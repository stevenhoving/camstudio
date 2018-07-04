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

#include "cam_size.h"
#include "cam_point.h"
#include <optional>
#include <string>

template<typename T>
class rect
{
public:
    using size_type = cam::size<T>;

    rect() noexcept = default;
    rect(const T left, const T top, const T right, const T bottom) noexcept
        : left_(left)
        , top_(top)
        , right_(right)
        , bottom_(bottom)
    {
    }

    rect(const point<T> left_top, const size_type width_height) noexcept
        : left_(left_top.x())
        , top_(left_top.y())
        , right_(left_top.x() + width_height.width())
        , bottom_(left_top.y() + width_height.height())
    {
    }

    T left() const noexcept
    {
        return left_;
    }

    T top() const noexcept
    {
        return top_;
    }

    T right() const noexcept
    {
        return right_;
    }

    T bottom() const noexcept
    {
        return bottom_;
    }

    T width() const noexcept
    {
        return (right_ - left_);
    }

    T height() const noexcept
    {
        return (bottom_ - top_);
    }

    void width(const T new_width) noexcept
    {
        right_ = left_ + new_width;
    }

    void height(const T new_height) noexcept
    {
        bottom_ = top_ + new_height;
    }


    size_type size() const noexcept
    {
        return { width(), height() };
    }

    T left_{ static_cast<T>(0) };
    T top_{ static_cast<T>(0) };
    T right_{ static_cast<T>(0) };
    T bottom_{ static_cast<T>(0) };
};
