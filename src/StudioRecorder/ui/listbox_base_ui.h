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

class listbox_base : public CStatic
{
    DECLARE_DYNAMIC(listbox_base)

public:
    virtual int on_get_image(LV_ITEM * /*pItem*/) { return -1; }
    virtual void on_selection_changed() {}

    // interface
    virtual int add_item(const CString &text, DWORD_PTR data = 0, int item_index = -1) = 0;
    virtual bool remove_item(int item_index) = 0;
    virtual int get_count() const = 0;
    virtual int get_selected_item() const = 0;
    virtual bool select_item(int item_index) = 0;
    virtual CString get_item_text(int item_index) const = 0;
    virtual void set_item_text(int item_index, const CString &text) = 0;
    virtual void set_item_text(int item_index, int sub_index, const CString &text) = 0;
    virtual DWORD *get_item_data(int item_index) const = 0;
    virtual void set_item_data(int item_index, DWORD_PTR data) = 0;

protected:
    virtual HWND _get_list_hwnd() const = 0;
    virtual CWnd *_on_create_list() = 0;
    virtual void _on_size_list() = 0;

    //CStatic
    void PreSubclassWindow() override;

protected:
    listbox_base();
    ~listbox_base() override;

    void _init();
    void _adjust_layout();

    CFont font_;

protected:
    afx_msg int OnCreate(LPCREATESTRUCT create_struct);
    afx_msg void OnSize(UINT type, int cx, int cy);
    afx_msg BOOL OnEraseBkgnd(CDC *dc);
    afx_msg void OnSetFocus(CWnd *old_wnd);
    afx_msg void OnEnable(BOOL enable);
    afx_msg UINT OnGetDlgCode();
    afx_msg void OnSetFont(CFont *font, BOOL redraw);
    afx_msg HFONT OnGetFont();

    DECLARE_MESSAGE_MAP()
};
