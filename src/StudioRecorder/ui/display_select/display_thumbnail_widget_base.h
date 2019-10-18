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
#include "display_thumbnail_button.h"

// \note the class is postfixed with _base only for unique naming...
class display_thumbnail_widget_base : public CStatic
{
    DECLARE_DYNAMIC(display_thumbnail_widget_base)

public:
    display_thumbnail_widget_base(const display_data &data);

    // CWnd
    void DoDataExchange(CDataExchange* pDX) override;

protected:
    //CStatic
    void PreSubclassWindow() override;

    // display_thumbnail_widget_base
    void _init();
    void _create();
    void _adjust_layout();

    CFont font_;

    std::unique_ptr<display_button> display_select_button_;
    std::unique_ptr<CStatic> display_select_widget_title_;

    display_data data_;

protected:
    afx_msg int OnCreate(LPCREATESTRUCT create_struct);
    afx_msg void OnSize(UINT type, int cx, int cy);
    afx_msg BOOL OnEraseBkgnd(CDC *dc);
    afx_msg void OnSetFocus(CWnd *old_wnd);
    afx_msg void OnEnable(BOOL enable);
    afx_msg void OnSetFont(CFont *font, BOOL redraw);
    afx_msg HFONT OnGetFont();

    DECLARE_MESSAGE_MAP()
};
