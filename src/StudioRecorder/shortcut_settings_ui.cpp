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
#include "shortcut_settings_ui.h"
#include "settings_model.h"
#include "utility/make_array.h"


using namespace std::string_literals;

#define WM_UPDATE_SHORTCUT_TEXT_MESSAGE (WM_USER + 100)

IMPLEMENT_DYNAMIC(shortcut_settings_ui, CDialogEx)
BEGIN_MESSAGE_MAP(shortcut_settings_ui, CDialogEx)
    ON_NOTIFY(EN_MSGFILTER, IDC_SHORTCUT_EDIT, &shortcut_settings_ui::OnEnMsgfilterRichedit)
    ON_MESSAGE(WM_UPDATE_SHORTCUT_TEXT_MESSAGE, &shortcut_settings_ui::OnUpdateShortcutTextMessage)
    ON_BN_CLICKED(IDC_SHORTCUT_SET, &shortcut_settings_ui::OnBnClickedShortcutSet)
    ON_BN_CLICKED(IDC_SHORTCUT_RESTORE, &shortcut_settings_ui::OnBnClickedShortcutRestore)
    ON_BN_CLICKED(IDC_SHORTCUT_ENABLED, &shortcut_settings_ui::OnBnClickedShortcutEnabled)
END_MESSAGE_MAP()

shortcut_settings_ui::shortcut_settings_ui(CWnd* pParent /*=nullptr*/)
    : shortcut_settings_ui(pParent, nullptr)
{
}

shortcut_settings_ui::shortcut_settings_ui(CWnd* pParent, settings_model *settings)
    : CDialogEx(IDD_SHORTCUTS_SETTINGS_UI, pParent)
    , shortcut_table_()
    , settings_(settings)
{
}

shortcut_settings_ui::~shortcut_settings_ui()
{
}

BOOL shortcut_settings_ui::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    shortcut_edit_.SetEventMask(ENM_KEYEVENTS);

    shortcut_table_.set_text_callback(
        [this](int item_index, int subitem_index) -> auto { return get_shortcut_data(item_index, subitem_index); }
    );

    shortcut_table_.set_select_callback(
        [this](int item_index){ on_shortcut_select(item_index); }
    );

    shortcut_table_.add_column(L"Action", 200);
    shortcut_table_.add_column(L"Shortcut", 100);
    shortcut_table_.add_column(L"State", 100);

    const auto &shortcut_map = settings_->get_shortcut_map();
    for (auto &itr : shortcut_map)
    {
        auto entry = const_cast<shortcut_definition *>(&itr.second);
        shortcut_table_data_.emplace_back(entry);
    }

    shortcut_table_.set_row_count(static_cast<int>(shortcut_table_data_.size()));

    return TRUE;
}

auto shortcut_settings_ui::get_shortcut_data(int item_index, int itemsub_index) -> const wchar_t *
{
    // just failsafe for now
    if (item_index >= static_cast<int>(shortcut_table_data_.size()))
        return nullptr;

    const auto &row_entry = shortcut_table_data_.at(item_index);

    constexpr auto shortcut_enabled_names = shortcut_enabled::names();
    constexpr auto shortcut_action_names = shortcut_action::names();
    switch(itemsub_index)
    {
    case 0: // Action
        return shortcut_action_names.at(row_entry->action);
    case 1: // Shortcut
        return row_entry->shortcut.c_str();
    case 2: // State
        return shortcut_enabled_names.at(row_entry->is_enabled);
    }
    return nullptr;
}

auto shortcut_settings_ui::on_shortcut_select(int item_index) -> void
{
    current_entry_ = item_index;

    if (item_index == -1)
    {
        _set_current_shortcut(L""s);
        return;
    }

    const auto shortcut = shortcut_table_data_.at(item_index);
    const auto ui_enabled =  shortcut->is_enabled != shortcut_enabled::unsupported;
    shortcut_edit_.EnableWindow(ui_enabled);
    shortcut_enabled_checkbox_.EnableWindow(ui_enabled);
    set_shortcut_button_.EnableWindow(ui_enabled);
    // \todo implement restore shortcut...
    restore_shortcut_button_.EnableWindow(false);

    shortcut_enabled_checkbox_.SetCheck(shortcut->is_enabled == shortcut_enabled::yes);
    _set_current_shortcut(shortcut->shortcut);
}

void shortcut_settings_ui::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_SHORTCUTS_TABLE, shortcut_table_);
    DDX_Control(pDX, IDC_SHORTCUT_EDIT, shortcut_edit_);
    DDX_Control(pDX, IDC_SHORTCUT_ENABLED, shortcut_enabled_checkbox_);
    DDX_Control(pDX, IDC_SHORTCUT_SET, set_shortcut_button_);
    DDX_Control(pDX, IDC_SHORTCUT_RESTORE, restore_shortcut_button_);
}

/* \todo code can be a lot better... */
void shortcut_settings_ui::OnEnMsgfilterRichedit(NMHDR *pNMHDR, LRESULT *result)
{
    const auto message_filter = reinterpret_cast<MSGFILTER *>(pNMHDR);

    if (message_filter->msg != WM_KEYDOWN && message_filter->msg != WM_SYSKEYDOWN)
    {
        *result = 0;
        return;
    }

    const auto virtual_key = message_filter->wParam;
    bool ignore_key = false;
    switch(virtual_key)
    {
    case VK_CONTROL:
    case VK_LCONTROL:
    case VK_RCONTROL:
    case VK_SHIFT:
    case VK_LSHIFT:
    case VK_RSHIFT:
    case VK_MENU:
    case VK_LMENU:
    case VK_RMENU:
        ignore_key = true;
        break;
    }

    if (!ignore_key)
    {
        const auto control_key_state = (GetKeyState(VK_CONTROL) & 0x8000) != 0;
        const auto shift_key_state = (GetKeyState(VK_SHIFT) & 0x8000) != 0;
        const auto alt_key_state = HIWORD(message_filter->lParam) & KF_ALTDOWN;

        std::wstring key_combo;
        if (control_key_state)
            key_combo += L"ctrl";
        if (shift_key_state)
            key_combo += L"+shift";
        if (alt_key_state)
            key_combo += L"+alt";

        bool is_alpha_numberic = (virtual_key >= '0' && virtual_key <= '9')
            || (virtual_key >= 'A' && virtual_key <= 'Z');

        bool is_function_key = virtual_key >= VK_F1 && virtual_key <= VK_F24;

        bool is_numpad = virtual_key >= VK_MULTIPLY && virtual_key <= VK_DIVIDE;

        bool is_space = virtual_key == VK_SPACE;

        if (is_alpha_numberic && !is_function_key)
        {
            if (!key_combo.empty()) key_combo += L"+";
            key_combo += static_cast<wchar_t>(tolower(static_cast<int>(virtual_key)));
        }
        else if (is_function_key)
        {
            constexpr auto function_key_names = make_array(L"F1",L"F2",L"F3",L"F4",L"F5",L"F6",
                L"F7",L"F8",L"F9",L"F10",L"F11",L"F12",L"F13",L"F14",L"F15",L"F16",L"F17",L"F18",L"F19",L"F20",L"F21",L"F22",L"F23",
                L"F24"
            );
            if (!key_combo.empty()) key_combo += L"+";
            key_combo += function_key_names.at(virtual_key - VK_F1);
        }
        else if (is_numpad)
        {
            constexpr auto function_key_names = make_array( L"*", L"+", L" ",L"-",L".",L"/");

            if (!key_combo.empty()) key_combo += L"+";
            key_combo += function_key_names.at(virtual_key - VK_MULTIPLY);
        }
        else if (is_space)
        {
            if (!key_combo.empty()) key_combo += L"+";
            key_combo += L"space";
        }

        _set_current_shortcut(key_combo);
    }

    *result = 1;
}

LRESULT shortcut_settings_ui::OnUpdateShortcutTextMessage(WPARAM /*wParam*/, LPARAM /*lParams*/)
{
    shortcut_edit_.SetWindowText(current_shortcut_.c_str());
    return 0;
}

void shortcut_settings_ui::_set_current_shortcut(std::wstring shortcut)
{
    current_shortcut_ = std::move(shortcut);
    PostMessage(WM_UPDATE_SHORTCUT_TEXT_MESSAGE);
}

void shortcut_settings_ui::OnBnClickedShortcutSet()
{
    if (current_entry_ == -1)
        return;

    shortcut_table_data_.at(current_entry_)->shortcut = current_shortcut_;
    shortcut_table_.Invalidate();
}

void shortcut_settings_ui::OnBnClickedShortcutRestore()
{
    // reset to the default
}

void shortcut_settings_ui::OnBnClickedShortcutEnabled()
{
    if (current_entry_ == -1)
        return;

    auto &shortcut = shortcut_table_data_.at(current_entry_);

    const auto is_enabled_checked = (shortcut_enabled_checkbox_.GetCheck() & BST_CHECKED) != 0;
    const auto enabled = is_enabled_checked ? shortcut_enabled::yes : shortcut_enabled::no;
    shortcut->is_enabled = enabled;
    shortcut_table_.Invalidate();
}
