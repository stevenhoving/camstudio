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

class cam_mouse_button
{
public:
    enum type : unsigned int
    {
        none = 0,
        left_button_down = 1 << 0,
        right_button_down = 1 << 1,
        middle_button_down = 1 << 2,
        left_button_up = 1 << 3,
        right_button_up = 1 << 4,
        middle_button_up = 1 << 5
    };
};