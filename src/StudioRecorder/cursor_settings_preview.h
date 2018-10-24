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

#include <functional>

class cursor_settings_preview : public CStatic
{
    DECLARE_DYNAMIC(cursor_settings_preview)
public:
    cursor_settings_preview();
    ~cursor_settings_preview() override;
    BOOL Create(LPCTSTR lpszText, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID = 0xffff) override;
    void PreSubclassWindow() override;

    void set_on_left_down(const std::function<void()> &on_left_down);
    void set_on_right_down(const std::function<void()> &on_right_down);
    void set_on_middle_down(const std::function<void()> &on_middle_down);

    void set_on_left_up(const std::function<void()> &on_left_down);
    void set_on_right_up(const std::function<void()> &on_right_down);
    void set_on_middle_up(const std::function<void()> &on_middle_down);
private:
    void _init();
    std::function<void()> on_left_down_;
    std::function<void()> on_right_down_;
    std::function<void()> on_middle_down_;

    std::function<void()> on_left_up_;
    std::function<void()> on_right_up_;
    std::function<void()> on_middle_up_;
protected:
    afx_msg int OnCreate(LPCREATESTRUCT create_struct);
    afx_msg UINT OnGetDlgCode();
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
    afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
    afx_msg void OnMButtonUp(UINT nFlags, CPoint point);
    afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
    afx_msg void OnMouseHWheel(UINT nFlags, short zDelta, CPoint pt);

    //(static_cast< BOOL (AFX_MSG_CALL CWnd::*)(UINT, short, CPoint) > ( &ThisClass :: OnMouseWheel)) },

    DECLARE_MESSAGE_MAP()
};
