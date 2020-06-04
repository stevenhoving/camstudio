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

#include <map>
#include <functional>
#include <windows.h> // only for HWND
#include "settings_model.h"

class shortcut_controller
{
    using shortcut_function_type = std::function<void()>;
public:
    shortcut_controller(HWND hwnd);
    ~shortcut_controller();

    void clear();

    void register_action(const shortcut_definition &definition, shortcut_function_type shortcut_function);
    void handle_action(int shortcut_id);
private:
    int shortcut_id_generator_{0};
    HWND shortcut_window_handle_{nullptr};
    std::map<int, shortcut_function_type> shortcut_lambda_map_;
};
