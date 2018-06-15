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

#include <CamCapture/cam_rect.h>

enum class capture_type
{
    fixed,
    variable,
    fullscreen,
    window,
    allscreens
};

class settings_model
{
public:
    void set_capture_mode(capture_type type);
    capture_type get_capture_mode();

    void set_capture_rect(const rect<int>& capture_rect);
    rect<int> get_capture_rect();

    void set_region_mouse_drag(bool capture_mouse_drag);
    bool get_region_mouse_drag();

    void set_region_fixed(bool capture_fixed);
    bool get_region_fixed();

    /* capture settings */
    capture_type capture_type_{capture_type::fullscreen};
    rect<int> capture_rect_{0, 0, 0, 0};
    bool capture_fixed_{false};
    bool capture_mouse_drag_{false};

    /* For now we will fall back to a simple save and load strategy.
     * The current implementation has a couple of problems. No input validation. No format version
     * validation in release mode. etc.
     */
    void save();

    void load();

    
};
