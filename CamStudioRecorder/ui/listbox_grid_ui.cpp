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

#include "stdafx.h"
#include "listbox_grid_ui.h"

constexpr auto list_id = 1;

IMPLEMENT_DYNAMIC(listbox_grid, listbox_base)
BEGIN_MESSAGE_MAP(listbox_grid, listbox_base)
    ON_NOTIFY(LVN_GETDISPINFO, list_id, &listbox_grid::_on_get_disp_info)
    ON_NOTIFY(LVN_ITEMCHANGED, list_id, &listbox_grid::_on_item_changed)
END_MESSAGE_MAP()

listbox_grid::listbox_grid() = default;
listbox_grid::~listbox_grid() = default;

void listbox_grid::set_text_callback(item_text_callback_type get_text_callback)
{
    get_text_callback_ = get_text_callback;
}

void listbox_grid::set_select_callback(item_select_callback_type on_select_callback)
{
    on_select_callback_ = on_select_callback;
}

HWND listbox_grid::_get_list_hwnd() const
{
    return wnd_list_->GetSafeHwnd();
}

CWnd* listbox_grid::_on_create_list()
{
    if (GetSafeHwnd() == nullptr || wnd_list_ != nullptr)
        return nullptr;

    assert(GetStyle() & WS_CHILD);

    const auto rect_empty = CRect(0, 0, 0, 0);

    wnd_list_ = std::make_unique<CListCtrl>();

    const DWORD window_style = WS_TABSTOP | WS_CHILD | WS_VISIBLE | LVS_AUTOARRANGE | LVS_REPORT |
        LVS_OWNERDATA | LVS_SHOWSELALWAYS | LVS_SINGLESEL;

    wnd_list_->CWnd::CreateEx(WS_EX_CLIENTEDGE, _T("SysListView32"), _T(""), window_style,
        rect_empty, this, list_id);

    const DWORD window_ex_style = LVS_EX_DOUBLEBUFFER | LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT |
        LVS_EX_HEADERINALLVIEWS;

    wnd_list_->SendMessage(LVM_SETEXTENDEDLISTVIEWSTYLE, 0, window_ex_style);
    wnd_list_->InsertColumn(0, _T(""));
    return wnd_list_.get();
}

int listbox_grid::add_item(const CString &text, DWORD_PTR data, int item_index)
{
    if (GetSafeHwnd() == nullptr || wnd_list_ == nullptr)
        return -1;

    ASSERT_VALID(wnd_list_.get());

    if (item_index < 0)
        item_index = wnd_list_->GetItemCount();

    item_index = wnd_list_->InsertItem(item_index, text, I_IMAGECALLBACK);
    wnd_list_->SetItemData(item_index, data);

    if (item_index == 0)
        select_item(0);

    return item_index;
}

int listbox_grid::get_count() const
{
    if (GetSafeHwnd() == nullptr || wnd_list_ == nullptr)
        return -1;

    return wnd_list_->GetItemCount();
}

CString listbox_grid::get_item_text(int item_index) const
{
    if (GetSafeHwnd() == nullptr || wnd_list_ == nullptr)
        return _T("");

    ASSERT_VALID(wnd_list_.get());
    return wnd_list_->GetItemText(item_index, 0);
}

DWORD *listbox_grid::get_item_data(int item_index) const
{
    if (GetSafeHwnd() == nullptr || wnd_list_ == nullptr)
    {
        assert(false);
        return nullptr;
    }

    ASSERT_VALID(wnd_list_.get());
    return reinterpret_cast<DWORD *>(wnd_list_->GetItemData(item_index));
}

void listbox_grid::set_item_data(int item_index, DWORD_PTR data)
{
    if (GetSafeHwnd() == nullptr || wnd_list_ == nullptr)
    {
        assert(false);
        return;
    }

    ASSERT_VALID(wnd_list_.get());
    wnd_list_->SetItemData(item_index, data);
}

int listbox_grid::get_selected_item() const
{
    if (GetSafeHwnd() == nullptr || wnd_list_ == nullptr)
    {
        assert(false);
        return -1;
    }

    ASSERT_VALID(wnd_list_.get());
    return wnd_list_->GetNextItem(-1, LVNI_SELECTED);
}

bool listbox_grid::select_item(int item_index)
{
    if (GetSafeHwnd() == nullptr || wnd_list_ == nullptr)
    {
        assert(false);
        return false;
    }

    ASSERT_VALID(wnd_list_.get());

    if (!wnd_list_->SetItemState(item_index, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED))
        return false;

    return wnd_list_->EnsureVisible(item_index, FALSE);
}

bool listbox_grid::remove_item(int item_index)
{
    if (GetSafeHwnd() == nullptr || wnd_list_ == nullptr)
    {
        assert(false);
        return false;
    }

    ASSERT_VALID(wnd_list_.get());

    const auto is_selected = (get_selected_item() == item_index);
    if (!wnd_list_->DeleteItem(item_index))
        return false;

    if (!is_selected || get_count() == 0)
        return false;

    if (item_index >= get_count())
        item_index--;

    select_item(item_index);
    return true;
}

void listbox_grid::_on_get_disp_info(NMHDR *notify_message_header, LRESULT *result)
{
    ENSURE(notify_message_header != nullptr);

    auto item = &reinterpret_cast<LV_DISPINFO*>(notify_message_header)->item;

    ENSURE(item != nullptr);

    if (item->mask & LVIF_IMAGE)
        item->iImage = on_get_image(item);

    if (item->mask & LVIF_TEXT)
        on_get_text(item);

    *result = 0;
}

void listbox_grid::_on_item_changed(NMHDR *notify_message_header, LRESULT *result)
{
    ENSURE(notify_message_header != nullptr);

    auto notify_message_list_view = reinterpret_cast<NM_LISTVIEW*>(notify_message_header);
    ENSURE(notify_message_list_view != nullptr);

    if (notify_message_list_view->uChanged == LVIF_STATE
        && (notify_message_list_view->uOldState & LVIS_SELECTED) != (notify_message_list_view->uNewState & LVIS_SELECTED))
        on_selection_changed();

    *result = 0;
}

void listbox_grid::_on_size_list()
{
    if (GetSafeHwnd() == nullptr || wnd_list_ == nullptr)
    {
        assert(false);
        return;
    }

    ASSERT_VALID(wnd_list_.get());

    CRect rect_client;
    GetClientRect(rect_client);

    const auto column_width = rect_client.Width() - 2 * ::GetSystemMetrics(SM_CXEDGE) - ::GetSystemMetrics(SM_CXVSCROLL);
    wnd_list_->SetColumnWidth(0, column_width);
}

void listbox_grid::set_item_text(int item_index, const CString& text)
{
    set_item_text(item_index, 0, text);
}

void listbox_grid::set_item_text(int item_index, int subindex, const CString& text)
{
    if (GetSafeHwnd() == nullptr || wnd_list_ == nullptr)
    {
        assert(false);
        return;
    }

    ASSERT_VALID(wnd_list_.get());
    wnd_list_->SetItemText(item_index, subindex, text);
}

void listbox_grid::on_selection_changed()
{
    const auto item_index = get_selected_item();
    if (on_select_callback_)
        on_select_callback_(item_index);
}

int listbox_grid::add_column(const wchar_t *title, int width /*= 100*/)
{
    LV_COLUMN lvColumn = {};
    lvColumn.mask = LVCF_FMT | LVCF_TEXT | LVCF_SUBITEM | LVCF_WIDTH;
    lvColumn.fmt = LVCFMT_LEFT;
    lvColumn.pszText = static_cast<LPWSTR>(const_cast<wchar_t *>(title));
    lvColumn.cx = width;
    wnd_list_->InsertColumn(static_cast<int>(header_.size()), &lvColumn);
    header_.emplace_back(title);
    return 0;
}

void listbox_grid::set_row_count(int count)
{
    wnd_list_->SetItemCount(count);
}

void listbox_grid::on_get_text(LV_ITEM *item)
{
    const auto item_text = const_cast<wchar_t *>(get_text_callback_(item->iItem, item->iSubItem));
    item->pszText = reinterpret_cast<LPWSTR>(item_text);
}
