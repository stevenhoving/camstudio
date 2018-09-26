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

#include "stdafx.h"
#include "utility/filesystem.h"

#include <windows.h>
#include <shlobj.h>

namespace utility
{

std::filesystem::path get_app_runtime_path()
{
    wchar_t szTemp[1024] = {};
    ::GetModuleFileNameW(nullptr, szTemp, 1023);
    std::filesystem::path program_path(szTemp);
    return program_path.remove_filename();
}

std::filesystem::path get_app_data_path()
{
    wchar_t app_data_path[MAX_PATH + 1] = {};
    if (FAILED(SHGetFolderPath(nullptr, CSIDL_LOCAL_APPDATA, nullptr, 0, app_data_path)))
        throw std::runtime_error("unable to get app data path");

    return std::filesystem::path(app_data_path) / L"CamStudio";
}

std::filesystem::path create_config_path(const std::wstring &filename)
{
    return get_app_data_path() / filename;
}

} // namespace utility

