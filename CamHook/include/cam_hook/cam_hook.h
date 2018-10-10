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
#pragma once

#include "cam_hook_export.h"
#include "cam_common.h"
#include <memory>

#define WM_USER_RECORDPAUSED_MSG L"WM_USER_RECORDPAUSED_MSG"
#define WM_USER_GENERIC_MSG L"WM_USER_GENERIC_MSG"
#define WM_USER_RECORDSTART_MSG L"WM_USER_RECORDSTART_MSG"

class mouse_hook
{
public:
    CAMHOOK_EXPORT static auto get() -> mouse_hook &;

    CAMHOOK_EXPORT mouse_hook();
    CAMHOOK_EXPORT ~mouse_hook();
    CAMHOOK_EXPORT void set_instance(HINSTANCE instance);
    CAMHOOK_EXPORT void attach();
    CAMHOOK_EXPORT void detach();
    CAMHOOK_EXPORT void pause();
    CAMHOOK_EXPORT void unpause();

    CAMHOOK_EXPORT auto get_mouse_events_count() -> int32_t;
    CAMHOOK_EXPORT auto get_mouse_events(MSLLHOOKSTRUCT *dst, int32_t count) -> bool;
    CAMHOOK_EXPORT void clear_mouse_events();

protected:
    static auto CALLBACK global_message_proc(int nCode, WPARAM wParam, LPARAM lParam) -> LRESULT;
    auto message_proc(int nCode, WPARAM wParam, LPARAM lParam) -> LRESULT;

private:
    void _detach_impl();
private:
    std::unique_ptr<struct hook_pimpl> pimpl_;
};
