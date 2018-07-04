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

namespace cam
{
template<typename T>
class size
{
public:
    size() noexcept = default;
    size(const T width, const T height) noexcept
        : width_(width)
        , height_(height)
    {
    }

    T width() const noexcept
    {
        return width_;
    }

    T height() const noexcept
    {
        return height_;
    }

private:
    T width_{ static_cast<T>(0) };
    T height_{ static_cast<T>(0) };
};
} // namespace cam
