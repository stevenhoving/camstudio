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

class display_thumbnail_widget : public CDialogEx
//class display_thumbnail_widget : public CFrameWndEx
{
    DECLARE_DYNAMIC(display_thumbnail_widget)

public:
    display_thumbnail_widget(CWnd* pParent = nullptr);
    display_thumbnail_widget(CWnd* pParent, const display_data &data);

    //BOOL Create(UINT nIDTemplate, CWnd* pParentWnd = NULL) = override;

// Dialog Data
#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_DISPLAY_SELECT_WIDGET };
#endif

protected:
    virtual void DoDataExchange(CDataExchange* pDX);

    DECLARE_MESSAGE_MAP()
public:
    display_button display_select_button_;
    CStatic display_select_widget_title_;
};
