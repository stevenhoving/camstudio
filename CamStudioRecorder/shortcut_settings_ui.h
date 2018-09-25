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

#include <afxcmn.h>
#include "ui/listbox_grid_ui.h"


#include <vector>
#include <string>
#include <array>

class settings_model;
struct shortcut_definition;

class shortcut_settings_ui : public CDialogEx
{
    DECLARE_DYNAMIC(shortcut_settings_ui)

public:
    shortcut_settings_ui(CWnd* pParent = nullptr);
    shortcut_settings_ui(CWnd* pParent, settings_model *settings);
    virtual ~shortcut_settings_ui();

    BOOL OnInitDialog() override;

    auto get_shortcut_data(int item_index, int subitem_index) -> const wchar_t *;
    auto on_shortcut_select(int item_index) -> void;

// Dialog Data
#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_SHORTCUTS_SETTINGS_UI };
#endif

protected:
    void DoDataExchange(CDataExchange* pDX) override;
    afx_msg void OnEnMsgfilterRichedit(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnBnClickedShortcutSet();
    afx_msg void OnBnClickedShortcutRestore();
    afx_msg LRESULT OnUpdateShortcutTextMessage(WPARAM wParam, LPARAM lParams);
    afx_msg void OnBnClickedShortcutEnabled();
    DECLARE_MESSAGE_MAP()

private:
    void _set_current_shortcut(std::wstring shortcut);

    listbox_grid shortcut_table_;
    CRichEditCtrl shortcut_edit_;
    CButton shortcut_enabled_checkbox_;
    CButton set_shortcut_button_;
    CButton restore_shortcut_button_;

    settings_model *settings_{nullptr};
    std::vector<shortcut_definition *> shortcut_table_data_{};
    int current_entry_{-1};
    std::wstring current_shortcut_{};
public:

};
