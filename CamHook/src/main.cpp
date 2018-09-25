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

#include "cam_hook/cam_hook.h"

BOOL APIENTRY DllMain(HINSTANCE hInstance, DWORD Reason, LPVOID /*Reserved*/)
{
    switch (Reason)
    {
        case DLL_PROCESS_ATTACH:
            mouse_hook::get().set_instance(hInstance);
            break;
        case DLL_PROCESS_DETACH:
            mouse_hook::get().detach();
            break;
    }

    return TRUE;
}
