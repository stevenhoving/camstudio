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

namespace cam
{
template<typename T>
class size
{
public:
    constexpr size() noexcept = default;
    constexpr size(const T width, const T height) noexcept
        : width_(width)
        , height_(height)
    {
    }

    constexpr T width() const noexcept
    {
        return width_;
    }

    constexpr T height() const noexcept
    {
        return height_;
    }

private:
    T width_{ static_cast<T>(0) };
    T height_{ static_cast<T>(0) };
};

// Y must be int/float
// \todo do float rounding?
template<typename T, typename Y>
constexpr auto operator * (const size<T> &lhs, const Y rhs) -> size<T>
{
    return {static_cast<T>(lhs.width() * rhs), static_cast<T>(lhs.height() * rhs)};
}

template<typename T>
constexpr auto operator == (const size<T> &lhs, const size<T> &rhs) noexcept -> bool
{
    return lhs.width() == rhs.width()
        && lhs.height() == rhs.height();
}

} // namespace cam
