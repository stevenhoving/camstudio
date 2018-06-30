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

#include "cam_hook/cam_hook.h"
#include <fmt/printf.h>

void mouse_hook::set_instance(HINSTANCE instance)
{
    instance_ = instance;
}

void mouse_hook::attach()
{
    hook_ = ::SetWindowsHookEx(WH_MOUSE_LL, mouse_hook::global_message_proc, instance_, 0);
}

void mouse_hook::detach()
{
    ::UnhookWindowsHookEx(hook_);
    hook_ = nullptr;
}

LRESULT CALLBACK mouse_hook::message_proc(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (nCode >= HC_ACTION)
    {
        switch(wParam)
        {
        case WM_LBUTTONDOWN:
        case WM_LBUTTONUP:
        case WM_RBUTTONDOWN:
        case WM_RBUTTONUP:
            fmt::print("mouse action\n");
            break;

        // ignore
        case WM_MOUSEMOVE:
        case WM_MOUSEWHEEL:
        case WM_MOUSEHWHEEL:
            break;
        }
        LPMSG msg = reinterpret_cast<LPMSG>(lParam);

        if (msg->message == WM_LBUTTONUP)
        {
            POINT pt;
            GetCursorPos(&pt);
            HWND hwnd = WindowFromPoint(pt);
        }
    }

    return CallNextHookEx(hook_, nCode, wParam, lParam);
}
