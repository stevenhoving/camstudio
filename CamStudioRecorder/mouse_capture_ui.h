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

#include "settings_model.h"
#include "virtual_screen_info.h"

#include <CamCapture/cam_gdiplus_fwd.h>

#include <atltypes.h>
#include <functional>
#include <vector>

class background_capture_source;

// ui state
enum class modify_mode
{
    /// move the already configured rectangle around
    move,
    /// select a new rectangle
    select
};

class mouse_capture_ui
{
public:
    mouse_capture_ui(HINSTANCE instance, HWND parent, const virtual_screen_info &screen_info,
        const std::function<void(const CRect &capture_rect)> &completed);
    ~mouse_capture_ui();
    mouse_capture_ui(const mouse_capture_ui &) = delete;
    mouse_capture_ui &operator = (const mouse_capture_ui &) = delete;

    void show(const cam::rect<int> region, const capture_type type);
    void hide();
    void set_modify_mode(modify_mode mode);
    void set_cancel_hotkey(int hotkey);
    LRESULT WINAPI message_handler(HWND hWnd, UINT wMessage, WPARAM wParam, LPARAM lParam);

private:
    void register_window_class(HINSTANCE instance);
    HWND create_capture_window(HINSTANCE instance, HWND parent, CRect size);

    void on_mouse_move(HWND hWnd);
    void on_lbutton_up(HWND hWnd);
    void on_lbutton_down(HWND hWnd);
    void on_rbutton_down(HWND hWnd);
    void on_key_down(HWND hWnd, WPARAM wParam, LPARAM lParam);
    void on_paint(HWND hWnd, WPARAM wParam, LPARAM lParam);
    void on_mouse_wheel(HWND hWnd, WPARAM wParam, LPARAM lParam);

    // translate mouse position from virtual 'space' domain.
    POINT translate_from_virtual(const POINT &mouse_position);

    // used for variable
    CPoint capture_rect_drag_start_{0, 0};
    CPoint capture_rect_drag_end_{0, 0};
    CRect capture_rect_drag_rect_old_{0, 0, 0, 0};
    CRect capture_rect_drag_rect_{0, 0, 0, 0};

    // used for fixed
    CRect capture_rect_{0, 0, 0, 0};

    // used to clamp
    CRect max_screen_rect_;

    HBRUSH hatched_brush_;
    HWND hwnd_{nullptr};
    HINSTANCE instance_{nullptr};

    std::unique_ptr<background_capture_source> background_capture_;

    HBITMAP background_frame_{nullptr};
    std::unique_ptr<Gdiplus::Bitmap> background_bitmap_;

    std::function<void(CRect capture_rect)> completed_;

    capture_type capture_mode_{capture_type::fixed};
    /// used to specify the rectangle modification mode (move or create new selection).
    modify_mode modify_mode_{modify_mode::select};
    /// used in 'select' mode to indicate we are have selected a starting point
    bool select_rect_{false};
    /// the hotkey used to cancel the mouse capture operation.
    int cancel_vkey_{VK_ESCAPE};

    virtual_screen_info virtual_screen_info_{};
};
