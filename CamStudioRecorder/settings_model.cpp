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

#include "stdafx.h"
#include "settings_model.h"

#include "utility/make_array.h"
#include "utility/filesystem.h"
#include <fmt/format.h>
#include <cpptoml.h>
#include <string_view>
#include <optional>
#include <iostream>

namespace config
{
    constexpr auto capture_settings = "capture-settings";
    constexpr auto capture_type = "capture_type";
    constexpr auto capture_region_fixed = "capture_region_fixed";
    constexpr auto capture_region_mouse_drag = "capture_region_mouse_drag";
    constexpr auto capture_rect = "capture_rect";
} // namespace config


void settings_model::save()
{
    auto root = cpptoml::make_table();
    // add a config file format version so we can handle conversions in the future.
    root->insert("version", 1);

    auto capture_settings = cpptoml::make_table();
    capture_settings->insert(config::capture_type, static_cast<int>(capture_type_));
    capture_settings->insert(config::capture_region_fixed, capture_fixed_);
    capture_settings->insert(config::capture_region_mouse_drag, capture_mouse_drag_);

    auto capture_rect = cpptoml::make_array();
    capture_rect->push_back(capture_rect_.left());
    capture_rect->push_back(capture_rect_.top());
    capture_rect->push_back(capture_rect_.right());
    capture_rect->push_back(capture_rect_.bottom());

    capture_settings->insert(config::capture_rect, capture_rect);

    root->insert(config::capture_settings, capture_settings);

    std::ofstream stream(utility::create_config_path(L"settings.toml"));
    cpptoml::toml_writer writer{stream, ""};
    root->accept(writer);
}

void settings_model::load()
{
    const auto config_filepath = utility::create_config_path(L"settings.toml");

    // non existing config file is not an error.
    if (!std::filesystem::exists(config_filepath))
        return;

    const auto root = cpptoml::parse_file(config_filepath.string());
    const auto version = *root->get_as<int>("version");
    assert(version == 1);

    /* capture settings */
    const auto capture = root->get_table(config::capture_settings);
    capture_type_ = static_cast<capture_type>(*capture->get_as<int>(config::capture_type));
    capture_fixed_ = *capture->get_as<bool>(config::capture_region_fixed);
    capture_mouse_drag_ = *capture->get_as<bool>(config::capture_region_mouse_drag);
    const auto capture_rect = *capture->get_array_of<int64_t>(config::capture_rect);

    // ignore if capture_rect does not contain the value data
    if (capture_rect.size() == 4)
    {
        capture_rect_ = rect<int>(
            static_cast<int>(capture_rect.at(0)),
            static_cast<int>(capture_rect.at(1)),
            static_cast<int>(capture_rect.at(2)),
            static_cast<int>(capture_rect.at(3))
        );
    }
}

void settings_model::set_capture_mode(capture_type type)
{
    capture_type_ = type;
}

capture_type settings_model::get_capture_mode()
{
    return capture_type_;
}

void settings_model::set_capture_rect(const rect<int> &capture_rect)
{
    capture_rect_ = capture_rect;
}

rect<int> settings_model::get_capture_rect()
{
    return capture_rect_;
}

void settings_model::set_region_mouse_drag(bool capture_mouse_drag)
{
    capture_mouse_drag_ = capture_mouse_drag;
}

bool settings_model::get_region_mouse_drag()
{
    return capture_mouse_drag_;
}

void settings_model::set_region_fixed(bool capture_fixed)
{
    capture_fixed_ = capture_fixed;
}

bool settings_model::get_region_fixed()
{
    return capture_fixed_;
}
