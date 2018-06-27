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

//#include "CamCapture/cam_rect.h"
#include "window_select_data.h"
#include "window_select_dwm.h"

enum class button_state
{
    normal,
    hover,
    down
};

class window_button : public CButton
{
public:
    window_button(const window_data &data);
    //void create(CRect rect, const int id);

    // void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) override;
    void OnCreate(HWND hwnd);
    void OnMouseMove(UINT nFlags, CPoint point);
    BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT *pResult) override;

    window_data &get_data();
    void set_state(button_state state)
    {
        prev_button_state_ = button_state_;
        button_state_ = state;
    }

private:
    //dwm_thumbnail dwm_;
    button_state button_state_{button_state::normal};
    button_state prev_button_state_{button_state::normal};
    window_data data_;
};
