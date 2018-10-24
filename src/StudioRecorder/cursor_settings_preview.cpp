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
#include "cursor_settings_preview.h"

IMPLEMENT_DYNAMIC(cursor_settings_preview, CStatic)
BEGIN_MESSAGE_MAP(cursor_settings_preview, CStatic)
    ON_WM_CREATE()
    ON_WM_GETDLGCODE()
    ON_WM_LBUTTONDOWN()
    ON_WM_RBUTTONDOWN()
    ON_WM_MBUTTONDOWN()
    ON_WM_LBUTTONUP()
    ON_WM_RBUTTONUP()
    ON_WM_MBUTTONUP()
    ON_WM_MOUSEWHEEL()
    ON_WM_MOUSEHWHEEL()
END_MESSAGE_MAP()

cursor_settings_preview::cursor_settings_preview() = default;
cursor_settings_preview::~cursor_settings_preview() = default;

BOOL cursor_settings_preview::Create(LPCTSTR lpszText, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID /*= 0xffff*/)
{
    return CStatic::Create(lpszText, dwStyle, rect, pParentWnd, nID);
}

void cursor_settings_preview::PreSubclassWindow()
{
    CStatic::PreSubclassWindow();

    const auto thread_state = AfxGetThreadState();
    if (thread_state->m_pWndInit == nullptr)
        _init();
}

void cursor_settings_preview::set_on_left_down(const std::function<void()> &on_left_down)
{
    on_left_down_ = on_left_down;
}

void cursor_settings_preview::set_on_right_down(const std::function<void()> &on_right_down)
{
    on_right_down_ = on_right_down;
}

void cursor_settings_preview::set_on_middle_down(const std::function<void()> &on_middle_down)
{
    on_middle_down_ = on_middle_down;
}

void cursor_settings_preview::set_on_left_up(const std::function<void()> &on_left_up)
{
    on_left_up_ = on_left_up;
}

void cursor_settings_preview::set_on_right_up(const std::function<void()> &on_right_up)
{
    on_right_up_ = on_right_up;
}

void cursor_settings_preview::set_on_middle_up(const std::function<void()> &on_middle_up)
{
    on_middle_up_ = on_middle_up;
}

int cursor_settings_preview::OnCreate(LPCREATESTRUCT create_struct)
{
    return CStatic::OnCreate(create_struct);
}

UINT cursor_settings_preview::OnGetDlgCode()
{
    return DLGC_WANTALLKEYS;
}

void cursor_settings_preview::OnLButtonDown(UINT /*nFlags*/, CPoint /*point*/)
{
    if (on_left_down_)
        on_left_down_();
}

void cursor_settings_preview::OnRButtonDown(UINT /*nFlags*/, CPoint /*point*/)
{
    if (on_right_down_)
        on_right_down_();
}

void cursor_settings_preview::OnMButtonDown(UINT /*nFlags*/, CPoint /*point*/)
{
    if (on_middle_down_)
        on_middle_down_();
}

void cursor_settings_preview::OnLButtonUp(UINT /*nFlags*/, CPoint /*point*/)
{
    if (on_left_up_)
        on_left_up_();
}

void cursor_settings_preview::OnRButtonUp(UINT /*nFlags*/, CPoint /*point*/)
{
    if (on_right_up_)
        on_right_up_();
}

void cursor_settings_preview::OnMButtonUp(UINT /*nFlags*/, CPoint /*point*/)
{
    if (on_middle_up_)
        on_middle_up_();
}

BOOL cursor_settings_preview::OnMouseWheel(UINT /*nFlags*/, short /*zDelta*/, CPoint /*pt*/)
{
    /* \todo implement mouse wheel annotation animation */
    return TRUE;
}

void cursor_settings_preview::OnMouseHWheel(UINT /*nFlags*/, short /*zDelta*/, CPoint /*pt*/)
{
    /* \todo implement mouse wheel annotation animation */
}

void cursor_settings_preview::_init()
{
    ModifyStyle(0, SS_USERITEM | SS_NOTIFY);
}
