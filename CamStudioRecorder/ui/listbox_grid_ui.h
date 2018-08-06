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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once

#include "listbox_base_ui.h"
#include <functional>
#include <memory>
#include <string>
#include <vector>

class listbox_grid : public listbox_base
{
    DECLARE_DYNAMIC(listbox_grid)

    using item_type = LVITEM;
    using item_text_type = const wchar_t *;
    using item_text_callback_type = std::function<item_text_type(int row, int col)>;
public:
    listbox_grid();
    ~listbox_grid() override;

    int add_column(const wchar_t *title, int width = 100);
    void set_row_count(int count);

    void on_get_text(LV_ITEM *item);
    void set_text_callback(item_text_callback_type get_text_callback);

    // listbox_base
    int add_item(const CString &text, DWORD_PTR data = 0, int item_index = -1) override;
    bool remove_item(int item_index) override;
    int get_count() const override;

    int get_selected_item() const override;
    bool select_item(int item_index) override;

    CString get_item_text(int item_index) const override;
    DWORD* get_item_data(int item_index) const override;
    void set_item_data(int item_index, DWORD_PTR data) override;

    void set_item_text(int item_index, const CString &text) override;
    void set_item_text(int item_index, int sub_index, const CString &text) override;

protected:
    HWND _get_list_hwnd() const override;
    CWnd* _on_create_list() override;
    void _on_size_list() override;

    std::unique_ptr<CListCtrl> wnd_list_;
    CVSListBoxEditCtrl wnd_edit_;
    std::vector<std::wstring> header_;
    item_text_callback_type get_text_callback_;

protected:
    afx_msg void _on_get_disp_info(NMHDR *notify_message_header, LRESULT *result);
    afx_msg void _on_item_changed(NMHDR *notify_message_header, LRESULT *result);
    DECLARE_MESSAGE_MAP()
};
