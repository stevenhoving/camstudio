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

using namespace std::chrono_literals;

void mouse_hook::set_instance(HINSTANCE instance)
{
    instance_ = instance;
}

void mouse_hook::attach()
{
    clear_mouse_events();
    hook_ = ::SetWindowsHookEx(WH_MOUSE_LL, mouse_hook::global_message_proc, instance_, 0);

    debug_unhook_ = std::thread([this]()
    {
        while (hook_ != nullptr && !IsDebuggerPresent())
            std::this_thread::sleep_for(0ms);
        fmt::print("shutting down or debugger attached\n");

        // only detach when we are not shutting down.
        if (hook_)
            _detach_impl();
    });
}

void mouse_hook::detach()
{
    _detach_impl();

    if (debug_unhook_.joinable())
        debug_unhook_.join();
}

void mouse_hook::_detach_impl()
{
    ::UnhookWindowsHookEx(hook_);
    hook_ = nullptr;

    clear_mouse_events();
}

LRESULT CALLBACK mouse_hook::message_proc(int nCode, WPARAM wParam, LPARAM lParam)
{
    // for debugging..
    if (IsDebuggerPresent())
        _detach_impl();

    if (nCode >= HC_ACTION)
    {
        switch(wParam)
        {
        case WM_MOUSEWHEEL:
            [[fallthrough]];
        case WM_MOUSEHWHEEL:
            [[fallthrough]];
        case WM_LBUTTONDOWN:
            [[fallthrough]];
        case WM_LBUTTONUP:
            [[fallthrough]];
        case WM_RBUTTONDOWN:
            [[fallthrough]];
        case WM_RBUTTONUP:
        {
            MSLLHOOKSTRUCT mouse_event = *reinterpret_cast<MSLLHOOKSTRUCT *>(lParam);
            mouse_event.dwExtraInfo = static_cast<ULONG_PTR>(wParam);
            {
                std::lock_guard<std::mutex> slock(mouse_events_lock_);
                mouse_events_.emplace_back(mouse_event);

                //fmt::print("mouse action queue size: {}\n", mouse_events_.size());
            }
        } break;

        // ignore
        case WM_MOUSEMOVE:
            break;
        default:
            //fmt::print("unhandled mouse hook event: {}\n", static_cast<unsigned int>(wParam));
            break;
        }
    }

    return CallNextHookEx(hook_, nCode, wParam, lParam);
}

int32_t mouse_hook::get_mouse_events_count()
{
    std::lock_guard<std::mutex> slock(mouse_events_lock_);
    // \todo make lowering cast safe
    return static_cast<int32_t>(mouse_events_.size());
}

// here we must assume that the user has actually allocated enough memory
bool mouse_hook::get_mouse_events(MSLLHOOKSTRUCT *dst, int32_t count)
{
    if (dst == nullptr)
        return false;

    {
        std::lock_guard<std::mutex> slock(mouse_events_lock_);
        const auto copy_count = std::min<int32_t>(count, static_cast<int32_t>(mouse_events_.size()));
        memcpy(dst, mouse_events_.data(), copy_count * sizeof(MSLLHOOKSTRUCT));

        /* \todo fix this so it does not do useless copy stuff, just use a circular buffer instead
         * of a std::vector */
        //auto itr = mouse_events_.begin();
        //std::advance(itr, copy_count);
        //mouse_events_.erase(mouse_events_.begin(), itr);

        fmt::print("mouse_hook - clear\n");
        mouse_events_.clear();
        mouse_events_.shrink_to_fit();

        return true;
    }
}

void mouse_hook::clear_mouse_events()
{
    std::lock_guard<std::mutex> slock(mouse_events_lock_);
    mouse_events_.clear();
    mouse_events_.shrink_to_fit();
}


