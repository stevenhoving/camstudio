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

#include "display_select_data.h"

#include <screen_capture/cam_gdiplus_fwd.h>
#include <memory>

class background_capture_source;

class display_button : public CButton
{
public:
    display_button(CWnd *parent = nullptr);
    display_button(CWnd *parent, const display_data &data);
    ~display_button();

    display_button(const display_button &) = delete;
    display_button &operator = (const display_button &) = delete;
    display_button (display_button &&) = default;
    display_button &operator = (display_button &&) = default;

    void create(const CRect &rect, const unsigned int id);
    const display_data &get_data() const noexcept;

    void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) override;
private:
    CWnd *parent_{nullptr};
    display_data data_{};

    std::unique_ptr<background_capture_source> capture_source_;
    std::unique_ptr<Gdiplus::Bitmap> background_bitmap_;
};
