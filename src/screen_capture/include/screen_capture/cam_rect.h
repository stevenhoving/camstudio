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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once

#include "cam_size.h"
#include "cam_point.h"

namespace cam
{

/* \note no restrictions on what you feed this template class. Maybe we should. */
template<typename T>
class rect
{
public:
    using size_type = cam::size<T>;

    constexpr rect() noexcept = default;
    constexpr rect(const T left, const T top, const T right, const T bottom) noexcept
        : left_(left)
        , top_(top)
        , right_(right)
        , bottom_(bottom)
    {
    }

    constexpr rect(const point<T> &left_top, const size_type &width_height) noexcept
        : left_(left_top.x())
        , top_(left_top.y())
        , right_(left_top.x() + width_height.width())
        , bottom_(left_top.y() + width_height.height())
    {
    }

    constexpr auto left() const noexcept
    {
        return left_;
    }

    constexpr void left(const T new_left) noexcept
    {
        left_ = new_left;
    }

    constexpr auto top() const noexcept
    {
        return top_;
    }

    constexpr void top(const T new_top) noexcept
    {
        top_ = new_top;
    }

    constexpr auto right() const noexcept
    {
        return right_;
    }

    constexpr void right(const T new_right) noexcept
    {
        right_ = new_right;
    }

    constexpr auto bottom() const noexcept
    {
        return bottom_;
    }

    constexpr void bottom(const T new_bottom) noexcept
    {
        bottom_ = new_bottom;
    }

    constexpr T width() const noexcept
    {
        return (right_ - left_);
    }

    constexpr T height() const noexcept
    {
        return (bottom_ - top_);
    }

    constexpr void width(const T new_width) noexcept
    {
        right_ = left_ + new_width;
    }

    constexpr void height(const T new_height) noexcept
    {
        bottom_ = top_ + new_height;
    }

    constexpr size_type size() const noexcept
    {
        return { width(), height() };
    }

    constexpr bool empty() const noexcept
    {
        return left_ == right_ || top_ == bottom_;
    }

    T left_{ static_cast<T>(0) };
    T top_{ static_cast<T>(0) };
    T right_{ static_cast<T>(0) };
    T bottom_{ static_cast<T>(0) };
};

template<typename T>
constexpr auto operator == (const rect<T> &lhs, const rect<T> &rhs) noexcept -> bool
{
    return lhs.left() == rhs.left()
        && lhs.right() == rhs.right()
        && lhs.top() == rhs.top()
        && lhs.bottom() == rhs.bottom();
}
} // namespace cam
