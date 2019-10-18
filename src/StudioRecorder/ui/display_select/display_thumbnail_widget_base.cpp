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
#include "display_thumbnail_widget_base.h"
//#include <algorithm>
//#include <cassert>

#include <fmt/format.h>
#include <fmt/printf.h>

IMPLEMENT_DYNAMIC(display_thumbnail_widget_base, CStatic)
BEGIN_MESSAGE_MAP(display_thumbnail_widget_base, CStatic)
    ON_WM_CREATE()
    ON_WM_PAINT()
    ON_WM_SIZE()
    ON_WM_ERASEBKGND()
    ON_WM_SETFOCUS()
    ON_WM_ENABLE()
    ON_WM_SETFONT()
    ON_WM_GETFONT()
END_MESSAGE_MAP()

display_thumbnail_widget_base::display_thumbnail_widget_base(const display_data &data)
    : CStatic()
    , data_(data)
{
    fmt::print("display_thumbnail_widget_base::display_thumbnail_widget_base\n");
}

void display_thumbnail_widget_base::PreSubclassWindow()
{
    fmt::print("display_thumbnail_widget_base::PreSubclassWindow\n");

    const auto thread_state = AfxGetThreadState();
    if (thread_state->m_pWndInit == nullptr)
        _init();
    CStatic::PreSubclassWindow();
}

void display_thumbnail_widget_base::DoDataExchange(CDataExchange* pDX)
{
    fmt::print("display_thumbnail_widget::DoDataExchange\n");

    CStatic::DoDataExchange(pDX);

    //CFrameWndEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_DISPLAY_SELECT_BUTTON, *display_select_button_);
    DDX_Control(pDX, IDC_DISPLAY_SELECT_WIDGET_TITLE, *display_select_widget_title_);

    const auto &data = display_select_button_->get_data();

    const auto title_format = fmt::format("Dispay: {}", data.index);
    auto title = CString(title_format.c_str());
    DDX_LBString(pDX, IDC_DISPLAY_SELECT_WIDGET_TITLE, title);
}

int display_thumbnail_widget_base::OnCreate(LPCREATESTRUCT create_struct)
{
    fmt::print("display_thumbnail_widget_base::OnCreate\n");
    const auto ret = CStatic::OnCreate(create_struct);
    if (ret == -1)
        return -1;

    _init();
    return ret;
}

void display_thumbnail_widget_base::_init()
{
    //ModifyStyle(0, DS_SETFONT | DS_FIXEDSYS | WS_CHILD | WS_SYSMENU);
    ModifyStyle(0, WS_VISIBLE);

    _create();
    _adjust_layout();
}

void display_thumbnail_widget_base::_create()
{
    if (GetSafeHwnd() == nullptr)
        return;

    //assert(GetStyle() & WS_CHILD);

    const auto rect_empty = CRect(0, 0, 0, 0);

    //IDD_DISPLAY_SELECT_WIDGET DIALOGEX 0, 0, 107, 102
    //    STYLE DS_SETFONT | DS_FIXEDSYS | WS_CHILD | WS_SYSMENU
    //    EXSTYLE WS_EX_NOACTIVATE
    //    FONT 8, "MS Shell Dlg", 400, 0, 0x1
    //    BEGIN
    //    PUSHBUTTON      "Button1", IDC_DISPLAY_SELECT_BUTTON, 7, 22, 93, 73
    //    CTEXT           "Title", IDC_DISPLAY_SELECT_WIDGET_TITLE, 7, 7, 93, 13, SS_CENTERIMAGE | WS_BORDER
    //    END

    display_select_widget_title_ = std::make_unique<CStatic>();

    const auto style = WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_OWNERDRAW;
    display_select_widget_title_->Create(
        _T("Something"),
        style,
        {7, 7, 93, 13},
        this,
        IDC_DISPLAY_SELECT_WIDGET_TITLE
    );

    display_select_button_ = std::make_unique<display_button>(this, data_);
    display_select_button_->Create(
        _T("Button1"),
        0,
        {7, 22, 93, 73},
        this,
        IDC_DISPLAY_SELECT_BUTTON
    );


    //const DWORD window_style = WS_TABSTOP | WS_CHILD | WS_VISIBLE | LVS_AUTOARRANGE | LVS_REPORT |
        //LVS_OWNERDATA | LVS_SHOWSELALWAYS | LVS_SINGLESEL;

    //wnd_list_->CWnd::CreateEx(WS_EX_CLIENTEDGE, _T("SysListView32"), _T(""), window_style,
        //rect_empty, this, list_id);

    //const DWORD window_ex_style = LVS_EX_DOUBLEBUFFER | LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT |
        //LVS_EX_HEADERINALLVIEWS;

    //wnd_list_->SendMessage(LVM_SETEXTENDEDLISTVIEWSTYLE, 0, window_ex_style);
    //wnd_list_->InsertColumn(0, _T(""));
    //return wnd_list_.get();
}

void display_thumbnail_widget_base::OnSize(UINT type, int cx, int cy)
{
    CStatic::OnSize(type, cx, cy);
    _adjust_layout();
}

void display_thumbnail_widget_base::_adjust_layout()
{
    if (GetSafeHwnd() == nullptr)
        return;
#if 0

    auto wnd_list = CWnd::FromHandle(_get_list_hwnd());
    if (wnd_list == nullptr)
        return;

    CRect rect_client;
    GetClientRect(rect_client);

    // \note HACK, abuse adjust layout function for setting the font
    if (font_.GetSafeHandle() == nullptr)
    {
        if (const auto parent_font = GetParent()->GetFont(); parent_font != nullptr)
            wnd_list->SetFont(parent_font, FALSE);
    }
    else
    {
        wnd_list->SetFont(&font_, FALSE);
    }

    wnd_list->MoveWindow(rect_client.left, rect_client.top, rect_client.Width(),
        rect_client.Height());
    _on_size_list();
#endif
}

BOOL display_thumbnail_widget_base::OnEraseBkgnd(CDC * /*dc*/)
{
    fmt::print("display_thumbnail_widget_base::OnEraseBkgnd\n");
    return TRUE;
}

void display_thumbnail_widget_base::OnSetFocus(CWnd * /*old_wnd*/)
{
    //auto wnd_list = CWnd::FromHandle(_get_list_hwnd());
    //assert(wnd_list != nullptr);
    //wnd_list->SetFocus();
}

HFONT display_thumbnail_widget_base::OnGetFont()
{
    return static_cast<HFONT>(font_.GetSafeHandle());
}

void display_thumbnail_widget_base::OnSetFont(CFont *font, BOOL redraw)
{
    Default();

    if (font != nullptr)
    {
        LOGFONT log_font;
        font->GetLogFont(&log_font);

        font_.DeleteObject();
        font_.CreateFontIndirect(&log_font);
    }

    if (::IsWindow(GetSafeHwnd()) != FALSE)
    {
        _adjust_layout();

        if (redraw)
        {
            Invalidate();
            UpdateWindow();
        }
    }
}

void display_thumbnail_widget_base::OnEnable(BOOL enable)
{
    CStatic::OnEnable(enable);

    //if (CWnd* wnd_list = CWnd::FromHandle(_get_list_hwnd()); wnd_list != nullptr)
    //    wnd_list->EnableWindow(enable);

    RedrawWindow();
}
