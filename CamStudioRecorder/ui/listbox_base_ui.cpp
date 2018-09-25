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
#include "listbox_base_ui.h"
#include <algorithm>
#include <cassert>

IMPLEMENT_DYNAMIC(listbox_base, CStatic)
BEGIN_MESSAGE_MAP(listbox_base, CStatic)
    ON_WM_CREATE()
    ON_WM_PAINT()
    ON_WM_SIZE()
    ON_WM_ERASEBKGND()
    ON_WM_SETFOCUS()
    ON_WM_ENABLE()
    ON_WM_GETDLGCODE()
    ON_WM_SETFONT()
    ON_WM_GETFONT()
END_MESSAGE_MAP()

listbox_base::listbox_base() = default;
listbox_base::~listbox_base() = default;

void listbox_base::PreSubclassWindow()
{
    CStatic::PreSubclassWindow();

    const auto thread_state = AfxGetThreadState();
    if (thread_state->m_pWndInit == nullptr)
        _init();
}

int listbox_base::OnCreate(LPCREATESTRUCT create_struct)
{
    const auto ret = CStatic::OnCreate(create_struct);
    if (ret == -1)
        return -1;

    _init();
    return ret;
}

void listbox_base::_init()
{
    ModifyStyle(0, SS_USERITEM);

    if (_on_create_list() == nullptr)
        return;

    _adjust_layout();
}

void listbox_base::OnSize(UINT type, int cx, int cy)
{
    CStatic::OnSize(type, cx, cy);
    _adjust_layout();
}

void listbox_base::_adjust_layout()
{
    if (GetSafeHwnd() == nullptr)
        return;

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
}

BOOL listbox_base::OnEraseBkgnd(CDC * /*dc*/)
{
    return TRUE;
}

void listbox_base::OnSetFocus(CWnd * /*old_wnd*/)
{
    auto wnd_list = CWnd::FromHandle(_get_list_hwnd());
    assert(wnd_list != nullptr);
    wnd_list->SetFocus();
}

HFONT listbox_base::OnGetFont()
{
    return static_cast<HFONT>(font_.GetSafeHandle());
}

void listbox_base::OnSetFont(CFont *font, BOOL redraw)
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

void listbox_base::OnEnable(BOOL enable)
{
    CStatic::OnEnable(enable);

    if (CWnd* wnd_list = CWnd::FromHandle(_get_list_hwnd()); wnd_list != nullptr)
        wnd_list->EnableWindow(enable);

    RedrawWindow();
}

UINT listbox_base::OnGetDlgCode()
{
    return DLGC_WANTALLKEYS;
}
