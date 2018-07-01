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
#include <limits>

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
        {
            MSLLHOOKSTRUCT mouse_event = *reinterpret_cast<MSLLHOOKSTRUCT *>(lParam);
            mouse_event.dwExtraInfo = static_cast<ULONG_PTR>(wParam);
            {
                std::unique_lock<std::mutex> slock(mouse_events_lock_);
                mouse_events_.emplace_back(mouse_event);

                fmt::print("mouse action queue size: {}\n", mouse_events_.size());
            }
        } break;

        // ignore
        case WM_MOUSEMOVE:
        case WM_MOUSEWHEEL:
        case WM_MOUSEHWHEEL:
            break;
        }
    }

    return CallNextHookEx(hook_, nCode, wParam, lParam);
}

int32_t mouse_hook::get_mouse_events_count()
{
    std::unique_lock<std::mutex> slock(mouse_events_lock_);
    constexpr auto mouse_events_max = std::numeric_limits<int32_t>::max();
    if (mouse_events_.size() > mouse_events_max)
    {
        // maybe this is a unwanted side effect, but it keeps things at bay.
        // \todo this is wrong, now we are removing the newest first, but we should remove the oldest.
        mouse_events_.resize(mouse_events_max);
        mouse_events_.shrink_to_fit();
        return mouse_events_max;
    }

    return static_cast<int32_t>(mouse_events_.size());
}

// here we must assume that the user has actually allocated enough memory
bool mouse_hook::get_mouse_events(MSLLHOOKSTRUCT *dst, int32_t count)
{
    if (dst == nullptr)
        return false;

    {
        std::unique_lock<std::mutex> slock(mouse_events_lock_);
        const auto copy_count = std::min<int32_t>(count, mouse_events_.size());
        memcpy(dst, mouse_events_.data(), count * sizeof(MSLLHOOKSTRUCT));

        /* \todo fix this so it does not do useless copy stuff, just use a circular buffer instead
         * of a std::vector */
        auto itr = mouse_events_.begin();
        std::advance(itr, copy_count);
        mouse_events_.erase(mouse_events_.begin(), itr);
        return true;
    }
}
