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

#include "settings/application_settings.h"
#include "settings/shortcut_settings.h"
#include "utility/make_array.h"
#include <CamCapture/cam_rect.h>
#include <CamCapture/cam_color.h>
#include <map>

enum class capture_type
{
    fixed,
    variable,
    fullscreen,
    window,
    allscreens
};

enum class cursor_halo_type
{
    circle,
    ellipse,
    square,
    rectangle
};

class table;

namespace cpptoml
{
    class table;
} // namespace cpptoml

class settings_model
{
    using shortcut_map_type = std::map<shortcut_action::type, shortcut_definition>;
public:
    /* capture */
    void set_capture_mode(capture_type type);
    auto get_capture_mode() -> capture_type;

    void set_capture_rect(const cam::rect<int>& capture_rect);
    auto get_capture_rect() -> cam::rect<int>;

    void set_region_mouse_drag(bool capture_mouse_drag);
    auto get_region_mouse_drag() -> bool;

    void set_region_fixed(bool capture_fixed);
    auto get_region_fixed() -> bool;

    /* cursor */
    void set_cursor_enabled(bool enabled);
    auto get_cursor_enabled() const -> bool;

    void set_cursor_ring_enabled(bool enabled);
    bool get_cursor_ring_enabled() const;

    void set_cursor_halo_enabled(bool enabled);
    bool get_cursor_halo_enabled() const;
    void set_cursor_halo_type(cursor_halo_type halo_type);
    auto get_cursor_halo_type() const -> cursor_halo_type;
    void set_cursor_halo_color(cam::color color);
    auto get_cursor_halo_color() const -> cam::color;
    void set_cursor_halo_size(const int size);
    auto get_cursor_halo_size() const -> int;
    void set_cursor_click_enabled(bool enabled);
    bool get_cursor_click_enabled() const;
    void set_cursor_click_left_color(cam::color color);
    auto get_cursor_click_left_color() const -> cam::color;
    void set_cursor_click_right_color(cam::color color);
    auto get_cursor_click_right_color() const -> cam::color;
    void set_cursor_click_middle_color(cam::color color);
    auto get_cursor_click_middle_color() const->cam::color;

    void set_cursor_ring_threshold(int threshold);
    auto get_cursor_ring_threshold() const -> int;
    void set_cursor_ring_size(int size);
    auto get_cursor_ring_size() const -> int;
    void set_cursor_ring_width(double width);
    auto get_cursor_ring_width() const -> double;

    void set_cursor_ring_click_left_color(cam::color color);
    auto get_cursor_ring_click_left_color() const->cam::color;
    void set_cursor_ring_click_right_color(cam::color color);
    auto get_cursor_ring_click_right_color() const->cam::color;
    void set_cursor_ring_click_middle_color(cam::color color);
    auto get_cursor_ring_click_middle_color() const->cam::color;

    /* shortcuts */
    void set_shortcut(shortcut_action::type shortcut_type, std::wstring shortcut);
    auto get_shortcut(shortcut_action::type shortcut_type) -> std::wstring;
    auto get_shortcut_data(shortcut_action::type shortcut_type) -> const shortcut_definition &;
    auto get_shortcut_map() -> const shortcut_map_type &;

    /* application */
    void set_application_auto_filename(bool auto_filename) noexcept;
    bool get_application_auto_filename() const noexcept;
    void set_application_minimize_on_capture_start(bool minimize_on_capture_start) noexcept;
    bool get_application_minimize_on_capture_start() const noexcept;
    void set_application_temp_directory_type(temp_output_directory::type temp_directory_access) noexcept;
    auto get_application_temp_directory_type() const noexcept -> temp_output_directory::type;
    /* \todo store/load std::wstring directory paths in utf8 */
    void set_application_temp_directory(std::string output_directory);
    auto get_application_temp_directory() const noexcept -> const std::string &;

    void set_application_output_directory_type(application_output_directory::type output_directory_access) noexcept;
    auto get_application_output_directory_type() const noexcept -> application_output_directory::type;
    /* \todo store/load std::wstring directory paths in utf8 */
    void set_application_output_directory(std::string output_directory);
    auto get_application_output_directory() const noexcept -> const std::string &;

    /* For now we will fall back to a simple save and load strategy.
     * The current implementation has a couple of problems. No input validation. No format version
     * validation in release mode. etc.
     */
    void save();
    void load();

private:
    void _save_capture_settings(cpptoml::table &root);
    void _save_cursor_settings(cpptoml::table &root);
    void _save_application_settings(cpptoml::table &root);
    void _save_shotcut_settings(cpptoml::table &root);

    void _load_capture_settings(const cpptoml::table &root);
    void _load_cursor_settings(const cpptoml::table &root);
    void _load_application_settings(const cpptoml::table &root);
    void _load_shortcut_settings(const cpptoml::table &root);

    void _load_shortcut(const table &shortcuts, shortcut_action::type shortcut);

private:

    /* capture settings */
    capture_type capture_type_{capture_type::allscreens};
    cam::rect<int> capture_rect_{0, 0, 0, 0};
    bool capture_fixed_{false};
    bool capture_mouse_drag_{false};

    /* cursor settings */
    bool cursor_enabled_{true};

    // halo config
    bool cursor_halo_enabled_{false};
    cursor_halo_type cursor_halo_type_{cursor_halo_type::circle};
    cam::color cursor_halo_color_{0xa0FFFF80};
    int cursor_halo_size_{100};
    bool cursor_click_enabled_{false};
    cam::color cursor_click_left_color_{0xa0, 0xff, 0, 0};
    cam::color cursor_click_right_color_{0xa0, 0, 0, 0xff};
    cam::color cursor_click_middle_color_{0xa0, 0, 0xff, 0};

    // ring config
    bool cursor_ring_enabled_{false};
    int cursor_ring_threshold_{1000};
    int cursor_ring_size_{20};
    double cursor_ring_width_{1.5};
    cam::color cursor_ring_click_left_color_{ 0xa0, 0xff, 0, 0 };
    cam::color cursor_ring_click_right_color_{ 0xa0, 0, 0, 0xff };
    cam::color cursor_ring_click_middle_color_{ 0xa0, 0, 0xff, 0 };

    /* application settings */
    bool application_auto_filename_{false};
    bool application_minimize_on_capture_start_{false};

    // where we will be storing our temporary recording
    temp_output_directory::type application_temp_directory_access_{temp_output_directory::user_temp};
    std::string application_temp_directory_{""};

    // where we will open the move recording file dialog in when we finished a recording.
    application_output_directory::type application_output_directory_access_{application_output_directory::ask_user};
    // user defined output directory
    std::string application_output_directory_{""};
    // AutoPan = false; // not implemented
    // MaxPan = 20; // not implemented

    /* shortcuts */
    std::map<shortcut_action::type, shortcut_definition> shortcut_settings_{
        {shortcut_action::record_start_or_pause, {shortcut_action::record_start_or_pause, shortcut_enabled::yes, L"F7"}},
        {shortcut_action::record_stop, {shortcut_action::record_stop, shortcut_enabled::yes, L"F8"}},
        {shortcut_action::record_cancel, {shortcut_action::record_cancel, shortcut_enabled::yes, L"F9"}},
        {shortcut_action::zoom, {shortcut_action::zoom, shortcut_enabled::unsupported, L""}},
        {shortcut_action::autopan, {shortcut_action::autopan, shortcut_enabled::unsupported, L""}},
    };
};
