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

#include "window_select_data.h"

class window_icon
{
public:
    window_icon(HICON icon)
        : window_icon_(icon)
    {
        BITMAP window_icon_bitmap = {};
        if (!::GetIconInfo(window_icon_, &window_icon_info_))
            return;

        if (window_icon_info_.hbmColor)
        {
            if (::GetObject(window_icon_info_.hbmColor, sizeof(window_icon_bitmap), &window_icon_bitmap))
            {
                width = window_icon_bitmap.bmWidth;
                height = window_icon_bitmap.bmHeight;
                bpp = window_icon_bitmap.bmBitsPixel;
                DeleteObject(window_icon_info_.hbmColor);
            }
        }
        else if (window_icon_info_.hbmMask)
        {
            if (::GetObject(window_icon_info_.hbmMask, sizeof(window_icon_bitmap), &window_icon_bitmap))
            {
                width = window_icon_bitmap.bmWidth;
                height = window_icon_bitmap.bmHeight / 2;
                bpp = 1;
                DeleteObject(window_icon_info_.hbmMask);
            }
        }

        if (width == 0 || height == 0)
        {
            width = ::GetSystemMetrics(SM_CXICON);
            height = ::GetSystemMetrics(SM_CYICON);
            bpp = 0;
        }
    }

    bool empty()
    {
        return window_icon_ == nullptr;
    }

    HICON window_icon_{nullptr};
    ICONINFO window_icon_info_{};

    int width = {0};
    int height = {0};
    int bpp = {0};
};

class window_button : public CButton
{
public:
    window_button(CWnd *parent, const window_data &data);
    window_button(const window_button &) = delete;
    window_button &operator = (const window_button &) = delete;
    window_button (window_button &&) = delete;
    window_button &operator = (window_button &&) = delete;

    void create(CRect rect, const unsigned int id);
    window_data &get_data() noexcept;

    void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) override;
private:
    CWnd *parent_{nullptr};
    window_data data_;
    window_icon window_icon_;
};
