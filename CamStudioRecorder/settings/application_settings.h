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

#include "settings/settings_enum_type.h"

struct application_output_directory_type
{
    using enum_type = enum
    {
        ask_user, /* x */
        user_my_documents, /* C:/users/<username>/Documents/My CamStudio Videos */
        user_specified /* x */
    };
};


static const wchar_t* application_output_directory_strings[] = {
    L"Ask user",
    L"My Documents",
    L"User specified"
};

using application_output_directory = settings_enum_type<application_output_directory_type,
    std::size(application_output_directory_strings), application_output_directory_strings>;


/************************************************************************/

static const wchar_t* temp_output_directory_strings[] = {
    L"User profile temp directory",
    L"My Documents",
    L"Windows temp directory",
    L"Installation directory",
    L"User specified"
};

struct temp_output_directory_type
{
    using enum_type = enum
    {
        user_temp = 0, /* C:/users/<username>/AppData/Local/temp */
        user_my_documents = 1, /* C:/users/<username>/Documents/My CamStudio Temp Files */
        windows_temp = 2, /* C:/windows/temp */
        install = 3, /* ea. C:/program files/CamStudio/temp */
        user_specified = 4 /* x */
    };
};

using temp_output_directory = settings_enum_type<temp_output_directory_type,
    std::size(temp_output_directory_strings), temp_output_directory_strings>;
