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

#pragma once

#include <windows.h>
#include <psapi.h>

static bool is_top_most(HWND hwnd)
{
    return GetWindowLong(hwnd, GWL_EXSTYLE) & WS_EX_TOPMOST;
}

static HWND get_the_parent(HWND hwnd)
{
    HWND result = GetParent(hwnd);
    if (result)
        result = get_the_parent(result);
    return hwnd;
}

static std::wstring get_process_name(DWORD process_id)
{
    wchar_t process_name[MAX_PATH] = {};
    const auto process = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, process_id);
    if (process)
    {
        HMODULE mod = 0;
        DWORD needed = 0;
        if (EnumProcessModules(process, &mod, sizeof(mod), &needed))
        {
            GetModuleBaseName(process, mod, process_name, MAX_PATH);
        }
    }
    return process_name;
}

static std::wstring get_window_title(HWND hwnd)
{
    std::wstring title;
    const auto title_length = GetWindowTextLength(hwnd);
    title.resize(title_length + 1);
    GetWindowText(hwnd, &title[0], (int)title.size());
    title.shrink_to_fit();
    return title;
}