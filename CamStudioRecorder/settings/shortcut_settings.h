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

#include "utility/make_array.h"

class shortcut_action
{
public:
    enum type
    {
        record_start_or_pause = 0,
        record_stop = 1,
        record_cancel = 2,
        zoom = 3, // currently not supported
        autopan = 4
    };

    static constexpr auto names()
    {
        return make_array(
            L"Start a new or pause current recording",
            L"Stop the current recording",
            L"Cancel the current recording",
            L"Zoom",
            L"Autopan"
        );
    }

    static constexpr auto setting_keys()
    {
        return make_array(
            "start_pause",
            "stop",
            "cancel",
            "zoom",
            "autopan"
        );
    }
};

class shortcut_enabled
{
public:
    enum type
    {
        yes = 0,
        no = 1,
        unsupported = 2
    };

    static constexpr auto names()
    {
        return make_array(
            L"Enabled",
            L"Disabled",
            L"Unsupported"
        );
    }

    shortcut_enabled(type new_type) noexcept
        : type_(new_type)
    {
    }

#if 0
    
    auto get_index() const noexcept
    {
        return static_cast<int>(type_);
    }
    void set_index(const type new_type) noexcept
    {
        type_ = new_type;
    }
    void set_index(const int new_type) noexcept
    {
        type_ = static_cast<type>(new_type);
    }
#endif

private:
    type type_;
};

struct shortcut_definition
{
    shortcut_action::type action{shortcut_action::record_start_or_pause};
    shortcut_enabled::type is_enabled{shortcut_enabled::unsupported};
    std::wstring shortcut; // in the form of <modifiers-key>, example: "Ctrl-Shift-W".
};
