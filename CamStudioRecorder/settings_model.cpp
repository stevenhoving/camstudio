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
namespace capture
{
    constexpr auto settings = "capture-settings";
    constexpr auto type = "capture_type";
    constexpr auto region_fixed = "capture_region_fixed";
    constexpr auto region_mouse_drag = "capture_region_mouse_drag";
    constexpr auto rect = "capture_rect";
}

namespace cursor
{
    constexpr auto settings = "cursor-settings";
    constexpr auto enabled = "cursor_enabled";
    constexpr auto halo_enabled = "cursor_halo_enabled";
    constexpr auto halo_type = "cursor_halo_type";
    constexpr auto halo_color = "cursor_halo_color";
    constexpr auto click_enabled = "cursor_click_enabled";
    constexpr auto click_left_color = "cursor_click_left_color";
    constexpr auto click_right_color = "cursor_click_right_color";
    constexpr auto ring_threshold = "cursor_ring_threshold";
    constexpr auto ring_size = "cursor_ring_size";
    constexpr auto ring_width = "cursor_ring_width";
}

} // namespace config


/* cpptoml table wrapper to make things easier */
class table
{
public:
    table(std::shared_ptr<cpptoml::table> table)
        : table_(table)
    {
    }

    template<typename T>
    void insert(const std::string &key, T value)
    {
        if constexpr(std::is_enum_v<T>)
            table_->insert<int64_t>(key, static_cast<int64_t>(value));
        else
            table_->insert<T>(key, std::move(value));
    }

    template<>
    void insert(const std::string &key, cam::color value)
    {
        table_->insert<uint32_t>(key, value);
    }

    template<>
    void insert(const std::string &key, rect<int> value)
    {
        auto array = cpptoml::make_array();
        array->push_back(value.left());
        array->push_back(value.top());
        array->push_back(value.right());
        array->push_back(value.bottom());
        table_->insert(key, array);
    }

    template<typename T>
    T get_optional(const std::string &key, const T default_value)
    {
        if (!table_)
            return default_value;

        if (!table_->contains(key))
            return default_value;

        if constexpr(std::is_enum_v<T>)
            return static_cast<T>(*table_->get_as<int64_t>(key));
        else
            return *table_->get_as<T>(key);
    }

    template<>
    cam::color get_optional(const std::string &key, const cam::color default_value)
    {
        if (!table_)
            return default_value;

        const auto value = table_->get_as<uint32_t>(key);
        if (!value)
            return default_value;

        return cam::color(*value);
    }

    // cookie cutter of a cookie cutter is preferred.
    template<>
    rect<int> get_optional(const std::string &key, const rect<int> default_value)
    {
        if (!table_)
            return default_value;

        const auto value = table_->get_array_of<int64_t>(key);
        if (!value)
            return default_value;

        assert(value->size() == 4);
        return rect<int>(
            static_cast<int>(value->at(0)),
            static_cast<int>(value->at(1)),
            static_cast<int>(value->at(2)),
            static_cast<int>(value->at(3))
        );
    }

    std::shared_ptr<cpptoml::table> get_table()
    {
        return table_;
    }

private:
    std::shared_ptr<cpptoml::table> table_;
};

void settings_model::save()
{
    auto root = cpptoml::make_table();
    // add a config file format version so we can handle conversions in the future.
    root->insert("version", 1);

    _save_capture_settings(*root);
    _save_cursor_settings(*root);

    std::ofstream stream(utility::create_config_path(L"settings.toml"));
    cpptoml::toml_writer writer{stream, ""};
    root->accept(writer);
}

void settings_model::_save_capture_settings(cpptoml::table &root)
{
    table capture = cpptoml::make_table();

    capture.insert(config::capture::type, capture_type_);
    capture.insert(config::capture::region_fixed, capture_fixed_);
    capture.insert(config::capture::region_mouse_drag, capture_mouse_drag_);
    capture.insert(config::capture::rect, capture_rect_);

    root.insert(config::capture::settings, capture.get_table());
}

void settings_model::_save_cursor_settings(cpptoml::table &root)
{
    table cursor = cpptoml::make_table();

    cursor.insert(config::cursor::enabled, cursor_enabled_);
    cursor.insert(config::cursor::halo_enabled, cursor_halo_enabled_);
    cursor.insert(config::cursor::halo_type, cursor_halo_type_);
    cursor.insert(config::cursor::halo_color, cursor_halo_color_);
    cursor.insert(config::cursor::click_enabled, cursor_click_enabled_);
    cursor.insert(config::cursor::click_left_color, cursor_click_left_color_);
    cursor.insert(config::cursor::click_right_color, cursor_click_right_color_);
    // currently unused
    cursor.insert(config::cursor::ring_threshold, cursor_ring_threshold_);
    cursor.insert(config::cursor::ring_size, cursor_ring_size_);
    cursor.insert(config::cursor::ring_width, cursor_ring_width_);

    root.insert(config::cursor::settings, cursor.get_table());
}

void settings_model::_load_capture_settings(const cpptoml::table &root)
{
    table capture = root.get_table(config::capture::settings);
    capture_type_ = capture.get_optional<capture_type>(config::capture::type, capture_type::fullscreen);
    capture_fixed_ = capture.get_optional<bool>(config::capture::region_fixed, false);
    capture_mouse_drag_ = capture.get_optional<bool>(config::capture::region_mouse_drag, false);
    capture_rect_ = capture.get_optional<rect<int>>(config::capture::rect, {0, 0, 0, 0});
}

void settings_model::_load_cursor_settings(const cpptoml::table &root)
{
    table cursor = root.get_table(config::cursor::settings);
    cursor_enabled_ = cursor.get_optional<bool>(config::cursor::enabled, true);
    cursor_halo_enabled_ = cursor.get_optional<bool>(config::cursor::halo_enabled, false);
    cursor_halo_type_ = cursor.get_optional<cursor_halo_type>(config::cursor::halo_type, cursor_halo_type::circle);
    cursor_halo_color_ = cursor.get_optional<cam::color>(config::cursor::halo_color, 0xFFFFFF80);
    cursor_click_enabled_ = cursor.get_optional<bool>(config::cursor::click_enabled, false);
    cursor_click_left_color_ = cursor.get_optional<cam::color>(config::cursor::click_left_color, 0xa0ff0000);
    cursor_click_right_color_ = cursor.get_optional<cam::color>(config::cursor::click_right_color, 0xa00000ff);
    // currently unused
    cursor_ring_threshold_ = cursor.get_optional<int>(config::cursor::ring_threshold, 1000);
    cursor_ring_size_ = cursor.get_optional<int>(config::cursor::ring_size, 20);
    cursor_ring_width_ = cursor.get_optional<double>(config::cursor::ring_width, 1.5);
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
    _load_capture_settings(*root);
    /* cursor settings */
    _load_cursor_settings(*root);
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
