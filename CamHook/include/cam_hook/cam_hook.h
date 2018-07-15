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

#include "cam_hook_export.h"
#include "cam_common.h"
#include <vector>
#include <mutex>
#include <thread>
#include <cstdint>
#include <atomic>

/* \todo which is used... and which isn't */
#define WM_USER_RECORDINTERRUPTED_MSG L"WM_USER_RECORDINTERRUPTED_MSG"
#define WM_USER_RECORDPAUSED_MSG L"WM_USER_RECORDPAUSED_MSG"
#define WM_USER_SAVECURSOR_MSG L"WM_USER_SAVECURSOR_MSG"
#define WM_USER_GENERIC_MSG L"WM_USER_GENERIC_MSG"
#define WM_USER_RECORDSTART_MSG L"WM_USER_RECORDSTART_MSG"
#define WM_USER_RECORDAUTO_MSG L"WM_USER_RECORDAUTO_MSG"

class CAMHOOK_EXPORT mouse_hook
{
public:
    static mouse_hook &get()
    {
        static mouse_hook hook;
        return hook;
    }

    static LRESULT CALLBACK global_message_proc(int nCode, WPARAM wParam, LPARAM lParam)
    {
        return mouse_hook::get().message_proc(nCode, wParam, lParam);
    }

    void set_instance(HINSTANCE instance);
    void attach();
    void detach();
    LRESULT message_proc(int nCode, WPARAM wParam, LPARAM lParam);

    int32_t get_mouse_events_count();
    bool get_mouse_events(MSLLHOOKSTRUCT *dst, int32_t count);
    void clear_mouse_events();
private:
    void _detach_impl();
private:
    HINSTANCE instance_{nullptr};
    HHOOK hook_{nullptr};

    // the mouse hook tracking part
    std::mutex mouse_events_lock_;
    std::vector<MSLLHOOKSTRUCT> mouse_events_;
    std::thread debug_unhook_;
};
