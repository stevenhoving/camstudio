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

#include <windows.h>
#include <functional>
#include <string>

namespace winapi
{
namespace display
{
using enum_monitor_lambda = std::function<void(const MONITORINFOEX &monitor)>;
void enum_monitors(enum_monitor_lambda pred);
} // namespace display

namespace window
{
using enum_windows_lambda = std::function<void(HWND hwnd)>;
void enum_windows(enum_windows_lambda pred);

auto get_thread_process_id(HWND hWnd) -> DWORD;
auto get_title(HWND hwnd) -> std::wstring;

} // namespace window
} // namespace winapi
