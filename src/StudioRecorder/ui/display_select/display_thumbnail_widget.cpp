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

#include "stdafx.h"
#include "display_thumbnail_widget.h"
//#include "afxdialogex.h"
#include <fmt/format.h>
#include <fmt/printf.h>


IMPLEMENT_DYNAMIC(display_thumbnail_widget, CDialogEx)

display_thumbnail_widget::display_thumbnail_widget(CWnd* pParent /*=nullptr*/)
    : CDialogEx(IDD_DISPLAY_SELECT_WIDGET, pParent)
    //: CFrameWndEx()
    , display_select_button_()
{

}

display_thumbnail_widget::display_thumbnail_widget(CWnd* pParent, const display_data &data)
    : CDialogEx(IDD_DISPLAY_SELECT_WIDGET, pParent)
    //: CFrameWndEx()
    , display_select_button_(this, data)
{
    fmt::print("display_thumbnail_widget::display_thumbnail_widget\n");
}

void display_thumbnail_widget::DoDataExchange(CDataExchange* pDX)
{
    fmt::print("display_thumbnail_widget::DoDataExchange\n");

    CDialogEx::DoDataExchange(pDX);
    //CFrameWndEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_DISPLAY_SELECT_BUTTON, display_select_button_);
    DDX_Control(pDX, IDC_DISPLAY_SELECT_WIDGET_TITLE, display_select_widget_title_);

    const auto &data = display_select_button_.get_data();

    const auto title_format = fmt::format("Dispay: {}", data.index);
    auto title = CString(title_format.c_str());
    DDX_LBString(pDX, IDC_DISPLAY_SELECT_WIDGET_TITLE, title);
}

BEGIN_MESSAGE_MAP(display_thumbnail_widget, CDialogEx)
//BEGIN_MESSAGE_MAP(display_thumbnail_widget, CFrameWndEx)
END_MESSAGE_MAP()
