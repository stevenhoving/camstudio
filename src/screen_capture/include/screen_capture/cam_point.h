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

#pragma once

template<typename T>
class point
{
public:
    constexpr point() noexcept = default;
    constexpr point(const T x, const T y) noexcept
        : x_(x)
        , y_(y)
    {
    }

    constexpr auto x() const noexcept -> T
    {
        return x_;
    }

    constexpr void x(const T new_x) noexcept
    {
        x_ = new_x;
    }

    constexpr auto y() const noexcept -> T
    {
        return y_;
    }

    constexpr void y(const T new_y) noexcept
    {
        y_ = new_y;
    }

private:
    T x_{ static_cast<T>(0) };
    T y_{ static_cast<T>(0) };
};

template<typename T>
constexpr auto operator == (const point<T> &lhs, const point<T> &rhs) noexcept -> bool
{
    return lhs.x() == rhs.x()
        && lhs.y() == rhs.y();
}
