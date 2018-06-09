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

void settings_model::save()
{
    auto root = cpptoml::make_table();
    // add a config file format version so we can handle conversions in the future.
    root->insert("version", 1);

    auto capture_settings = cpptoml::make_table();
    capture_settings->insert("capture_type", static_cast<int>(capture_type_));
    capture_settings->insert("capture_mouse_drag", capture_mouse_drag_);

    auto capture_rect = cpptoml::make_array();
    capture_rect->push_back(capture_rect_.left());
    capture_rect->push_back(capture_rect_.top());
    capture_rect->push_back(capture_rect_.right());
    capture_rect->push_back(capture_rect_.bottom());

    capture_settings->insert("capture_rect", capture_rect);

    root->insert("capture-settings", capture_settings);

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
    const auto capture = root->get_table("capture-settings");
    capture_type_ = static_cast<capture_type>(*capture->get_as<int>("capture_type"));
    capture_mouse_drag_ = *capture->get_as<bool>("capture_mouse_drag");
    const auto capture_rect = *capture->get_array_of<int64_t>("capture_rect");

    // ignore if capture_rect does not contain the value data
    if (capture_rect.size() == 4)
    {
        capture_rect_ = rect<int>(
            capture_rect.at(0),
            capture_rect.at(1),
            capture_rect.at(2),
            capture_rect.at(3)
        );
    }
}
