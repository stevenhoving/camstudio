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

#include "background_capture_source.h"
#include "window_select_dwm.h"
#include "window_thumbnail_widget.h"
#include "window_select_data.h"
#include <windef.h>
#include <functional>
#include <vector>

struct get_windows_info
{
    std::vector<window_data> windows;

    // hwnd's to ignore
    HWND ignore_camstudio_wnd;
    HWND ignore_shell_tray_wnd;
    HWND ignore_progman_wnd;
    DWORD ignore_current_process_id;
};

using message_handler_type = LRESULT WINAPI (HWND hWnd, UINT wMessage, WPARAM wParam, LPARAM lParam);

class window_select_ui : public CDialogEx
{
    DECLARE_DYNAMIC(window_select_ui)

public:
    window_select_ui(CWnd* pParent, const std::function<void(const HWND window_handle)> &completed);
    virtual ~window_select_ui();
    window_select_ui(const window_select_ui &) = delete;
    window_select_ui &operator = (const window_select_ui &) = delete;

    std::vector<window_data> get_windows();

    BOOL OnInitDialog() override;
    BOOL OnCmdMsg(UINT nID, int nCode, void *pExtra, AFX_CMDHANDLERINFO *pHandlerInfo) override;

    message_handler_type *old_message_handler_;
    void *old_user_data_{nullptr};

    std::vector<std::unique_ptr<window_button>> capture_windows_;
    std::function<void(const HWND window_handle)> completed_;

// Dialog Data
#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_WINDOW_SELECT };
#endif

protected:
    void DoDataExchange(CDataExchange* pDX) override;

    DECLARE_MESSAGE_MAP()
};
