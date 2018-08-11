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
#include "shortcut_controller.h"
#include "string_convert.h"
#include <fmt/format.h>
#include <fmt/printf.h>
#include <stdexcept>
#include <string>
#include <vector>
#include <algorithm>
#include <windows.h>

using namespace std::string_literals;

std::vector<std::wstring> tokenize_string(const std::wstring &text, const wchar_t token)
{
    std::vector<std::wstring> result;

    size_t offset_found = std::wstring::npos;
    size_t offset_prev = 0;
    while((offset_found = text.find(token, offset_found)) != std::wstring::npos)
    {
        result.emplace_back(text.substr(offset_prev, offset_found - offset_prev));
        offset_prev = offset_found + 1;
    }

    if (!text.empty())
    {
        result.emplace_back(text.substr(offset_prev, text.size() - offset_prev));
    }

    return result;
}

template<typename C, typename T>
auto vector_contains(const C& v, const T& x) -> decltype(std::end(v), true)
{
    return std::end(v) != std::find(begin(v), std::end(v), x);
}

template<typename C, typename T>
auto vector_remove(C& v, const T& x) -> void
{
    v.erase(std::remove(std::begin(v), std::end(v), x), std::end(v));
}

// parse ea. <shift+ctrl+F9> to shift+ctrl
UINT parse_shortcut_modifiers(const std::wstring &shortcut)
{
    const auto tokens = tokenize_string(shortcut, L'+');

    UINT result = 0;
    if (vector_contains(tokens, L"shift"s))
        result |= MOD_SHIFT;

    if (vector_contains(tokens, L"alt"s))
        result |= MOD_ALT;

    if (vector_contains(tokens, L"ctrl"s))
        result |= MOD_CONTROL;

    return result;
}

static const std::map<std::wstring, int> virtual_key_to_nr = {
    { L"F1"s, VK_F1  },
    { L"F2"s, VK_F2 },
    { L"F3"s, VK_F3 },
    { L"F4"s, VK_F4 },
    { L"F5"s, VK_F5 },
    { L"F6"s, VK_F6 },
    { L"F7"s, VK_F7 },
    { L"F8"s, VK_F8 },
    { L"F9"s, VK_F9 },
    { L"F10"s, VK_F10 },
    { L"F11"s, VK_F11 },
    { L"F12"s, VK_F12 },
    { L"F13"s, VK_F13 },
    { L"F14"s, VK_F14 },
    { L"F15"s, VK_F15 },
    { L"F16"s, VK_F16 },
    { L"F17"s, VK_F17 },
    { L"F18"s, VK_F18 },
    { L"F19"s, VK_F19 },
    { L"F20"s, VK_F20 },
    { L"F21"s, VK_F21 },
    { L"F22"s, VK_F22 },
    { L"F23"s, VK_F23 },
    { L"F24"s, VK_F24 },
    { L"0"s, '0'},
    { L"0"s, '0'},
    { L"1"s, '1'},
    { L"2"s, '2'},
    { L"3"s, '3'},
    { L"4"s, '4'},
    { L"5"s, '5'},
    { L"6"s, '6'},
    { L"7"s, '7'},
    { L"8"s, '8'},
    { L"9"s, '9'},
    { L"A"s, 'A'},
    { L"B"s, 'B'},
    { L"C"s, 'C'},
    { L"D"s, 'D'},
    { L"E"s, 'E'},
    { L"F"s, 'F'},
    { L"G"s, 'G'},
    { L"H"s, 'H'},
    { L"I"s, 'I'},
    { L"J"s, 'J'},
    { L"K"s, 'K'},
    { L"L"s, 'L'},
    { L"M"s, 'M'},
    { L"N"s, 'N'},
    { L"O"s, 'O'},
    { L"P"s, 'P'},
    { L"Q"s, 'Q'},
    { L"R"s, 'R'},
    { L"S"s, 'S'},
    { L"T"s, 'T'},
    { L"U"s, 'U'},
    { L"V"s, 'V'},
    { L"W"s, 'W'},
    { L"X"s, 'X'},
    { L"Y"s, 'Y'},
    { L"Z"s, 'Z'},
};

// parse ea. <shift+ctr+F9> to F9
UINT parse_shortcut_virtual_key(const std::wstring &shortcut)
{
    auto tokens = tokenize_string(shortcut, L'+');
    vector_remove(tokens, L"shift"s);
    vector_remove(tokens, L"alt"s);
    vector_remove(tokens, L"ctrl"s);

    return virtual_key_to_nr.at(tokens.front());
}

shortcut_controller::shortcut_controller(HWND hwnd)
    : shortcut_window_handle_(hwnd)
{

}

shortcut_controller::~shortcut_controller()
{
    clear();
}

void shortcut_controller::clear()
{
    for (const auto itr : shortcut_lambda_map_)
        ::UnregisterHotKey(shortcut_window_handle_, itr.first);
    shortcut_lambda_map_.clear();
}

void shortcut_controller::register_action(const shortcut_definition &definition, shortcut_function_type shortcut_function)
{
    const auto modifiers = parse_shortcut_modifiers(definition.shortcut);
    const auto virtual_key = parse_shortcut_virtual_key(definition.shortcut);
    const auto shortcut_id = shortcut_id_generator_++;
    if (const auto ret = ::RegisterHotKey(shortcut_window_handle_, shortcut_id, modifiers, virtual_key); ret == 0)
        throw std::runtime_error(fmt::format("failed to register shortcut: '{}'", wstring_to_utf8(definition.shortcut)));
    shortcut_lambda_map_.emplace(shortcut_id, shortcut_function);
}

void shortcut_controller::handle_action(int shortcut_id)
{
    fmt::print("handle shortcut action\n");
    shortcut_lambda_map_.at(shortcut_id)();
}
