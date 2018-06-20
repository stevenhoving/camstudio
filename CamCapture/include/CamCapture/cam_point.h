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

template<typename T>
class point
{
public:
    point() noexcept = default;
    point(const T x, const T y) noexcept
        : x_(x)
        , y_(y)
    {
    }

    T x() const noexcept
    {
        return x_;
    }

    T y() const noexcept
    {
        return y_;
    }

private:
    T x_{ static_cast<T>(0) };
    T y_{ static_cast<T>(0) };
};
