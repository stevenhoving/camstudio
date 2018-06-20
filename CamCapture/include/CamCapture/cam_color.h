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

#include <cstdint>

class color
{
public:
    color() noexcept = default;
    color(const uint8_t a, const uint8_t r, const uint8_t g, const uint8_t b) noexcept
        : a_(a)
        , r_(r)
        , g_(g)
        , b_(b)
    {
    }

    uint8_t a_{ 255u };
    uint8_t r_{ 0 };
    uint8_t g_{ 0 };
    uint8_t b_{ 0 };
};