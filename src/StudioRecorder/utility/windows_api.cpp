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

#include "stdafx.h"
#include "windows_api.h"
#include <cassert>

namespace winapi
{
namespace detail
{
BOOL CALLBACK enum_windows_callback(HWND hwnd, LPARAM lparam) noexcept
{
    const auto user_data = reinterpret_cast<window::enum_windows_lambda*>(lparam);
    if (user_data == nullptr)
        return TRUE;

    std::invoke(*user_data, hwnd);
    return TRUE;
}
} // namespace detail

namespace window
{

void enum_windows(enum_windows_lambda pred)
{
    ::EnumWindows(detail::enum_windows_callback, reinterpret_cast<LPARAM>(&pred));
}

auto get_thread_process_id(HWND hWnd) -> DWORD
{
    DWORD window_thread_process_id;
    const auto ret = ::GetWindowThreadProcessId(hWnd, &window_thread_process_id);
    assert(ret);
    return window_thread_process_id;
}

auto get_title(HWND hwnd) -> std::wstring
{
    std::wstring title;
    const auto title_length = static_cast<std::size_t>(::GetWindowTextLength(hwnd));
    title.resize(title_length + 1);

    const auto window_text_length = ::GetWindowText(hwnd, &title[0], (int)title.size());
    title.resize(window_text_length);
    return title;
}

} // namespace window

namespace detail
{
BOOL CALLBACK enum_display_monitor_callback(HMONITOR monitor, HDC /*hdc*/, LPRECT /*rect*/, LPARAM lparam)
{
    const auto user_data = reinterpret_cast<display::enum_monitor_lambda*>(lparam);
    if (user_data == nullptr)
        return TRUE;

    MONITORINFOEX info{ sizeof(MONITORINFOEX) };
    if (::GetMonitorInfo(monitor, &info))
        std::invoke(*user_data, info);

    return TRUE;
}
} // namespace detail

namespace display
{
void display::enum_monitors(enum_monitor_lambda pred)
{
    ::EnumDisplayMonitors(nullptr, nullptr, detail::enum_display_monitor_callback,
        reinterpret_cast<LPARAM>(&pred));
}

} // namespace display


} // namespace winapi
