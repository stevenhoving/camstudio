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
//#include "display_thumbnail_button.h"
#include "display_thumbnail_widget.h"
#include "display_thumbnail_widget_base.h"
#include "display_select_data.h"
#include <windef.h>
#include <functional>
#include <vector>
#include <memory>

struct get_display_info
{
    std::vector<display_data> windows{};
};

using message_handler_type = LRESULT WINAPI (HWND hWnd, UINT wMessage, WPARAM wParam, LPARAM lParam);

class display_select_ui : public CDialogEx
{
    DECLARE_DYNAMIC(display_select_ui)

public:
    display_select_ui(CWnd* pParent, const std::function<void()> &completed);
    display_select_ui(const display_select_ui &) = delete;
    display_select_ui &operator = (const display_select_ui &) = delete;

    std::vector<display_data> get_displays();

    //CWnd
    //void PreSubclassWindow() override;

    BOOL OnInitDialog() override;
    BOOL OnCmdMsg(UINT nID, int nCode, void *pExtra, AFX_CMDHANDLERINFO *pHandlerInfo) override;

    message_handler_type *old_message_handler_{nullptr};
    void *old_user_data_{nullptr};

    std::vector<std::unique_ptr<display_thumbnail_widget_base>> capture_displays_;
    //std::function<void(const HWND window_handle)> completed_;
    std::function<void()> completed_;

// Dialog Data
#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_DISPLAY_SELECT };
#endif

protected:
    void DoDataExchange(CDataExchange* pDX) override;

    DECLARE_MESSAGE_MAP()
};
