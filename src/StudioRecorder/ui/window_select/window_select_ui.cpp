/**
 * Copyright(C) 2018 - 2020  Steven Hoving
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
#include "window_select_ui.h"
#include "utility/window_util.h"
#include "utility/fmt_helper.h"
#include "utility/windows_api.h"

#include <psapi.h>
#include <vector>
#include <string>

IMPLEMENT_DYNAMIC(window_select_ui, CDialogEx)

/* \todo Create a list of processes that have actual visible windows for this user.
 - For each of these processes create a capture source.
 - for each of these processes create a button on the ui.
 - on press call supplied callback to start recording (covering record window use case).
*/

/* \todo no dpi correction */
//constexpr auto button_spacing = 10;
constexpr auto button_width = 250;
constexpr auto button_height = 250;

/* weirdness that seems to work... okey ish for win 7 - 10 (I hope) */
bool is_window_candidate(HWND hwnd)
{
    HWND hwnd_candidate = nullptr;
    HWND hwnd_walk_prev = nullptr;
    bool result = false;

    CRect rect;
    const auto style = static_cast<unsigned int>(::GetWindowLongPtr(hwnd, GWL_STYLE));
    if ((::GetWindowRect(hwnd, &rect) < 1 || !utility::rect_empty(rect))
        && (style & WS_VISIBLE) ==  WS_VISIBLE
        /*&& (style & WS_MINIMIZE) != WS_MINIMIZE*/)
    {
        HWND hwnd_walk = hwnd;
        do
        {
            hwnd_walk_prev = hwnd_walk;
            hwnd_walk = ::GetWindow(hwnd_walk, GW_OWNER);
            CRect rect2;
            if (!hwnd_candidate && hwnd_walk && ::GetWindowRect(hwnd_walk, &rect2) >= 1 && !utility::rect_empty(rect2))
                hwnd_candidate = hwnd_walk;

        } while(hwnd_walk);

        const auto exstyle_prev = static_cast<unsigned int>(::GetWindowLongPtr(hwnd_walk_prev, GWL_EXSTYLE));
        const auto exstyle_hwnd = static_cast<unsigned int>(::GetWindowLongPtr(hwnd, GWL_EXSTYLE));

        unsigned int exstyle;
        if (hwnd_walk_prev != hwnd)
            exstyle = exstyle_hwnd;
        else
            exstyle = exstyle_prev;

        if (hwnd_walk_prev == hwnd || !hwnd_candidate || (exstyle & WS_EX_APPWINDOW) == WS_EX_APPWINDOW)
        {
            result =
                  !(exstyle_prev & WS_EX_TOOLWINDOW)
                || (exstyle & WS_EX_APPWINDOW)
                || (!(exstyle & WS_EX_TOOLWINDOW) && (exstyle & WS_EX_CONTROLPARENT));
        }
    }
    return result;
}

std::vector<window_data> window_select_ui::get_windows() const
{
    get_windows_info result;

    result.ignore_camstudio_wnd = utility::get_root_parent(GetSafeHwnd());
    result.ignore_shell_tray_wnd = ::FindWindow(L"Shell_TrayWnd", 0);
    result.ignore_progman_wnd = ::FindWindow(L"Progman", 0); // DWM Thumbnail desktop
    result.ignore_current_process_id = ::GetCurrentProcessId();

    winapi::window::enum_windows(
        [&result](HWND hwnd)
        {
            const auto process_id = winapi::window::get_thread_process_id(hwnd);
            if (process_id != result.ignore_current_process_id && is_window_candidate(hwnd))
            {
                const auto[process_name, process_filepath] = utility::get_process_name(process_id);

                if (process_name.empty())
                    return;

                const auto window_title = winapi::window::get_title(hwnd);

                window_data data = { process_id, process_name, process_filepath, window_title, hwnd };
                result.windows.emplace_back(data);
            }
        }
    );
    return result.windows;
}

window_select_ui::window_select_ui(CWnd* pParent, const std::function<void(const HWND window_handle)> &completed)
    : CDialogEx(IDD_WINDOW_SELECT, pParent)
    , completed_(completed)
{
}

window_select_ui::~window_select_ui()
{
}

BOOL window_select_ui::OnInitDialog()
{
    const auto windows = get_windows();

    CRect button_rect(0, 0, button_width, button_height);

    constexpr auto button_horizontal_count = 6;
    const auto windows_size = static_cast<int>(windows.size());
//    const auto columns = windows_size % button_horizontal_count;
    const auto rows = windows_size / button_horizontal_count;

    //const auto window_width = std::min(windows_size, button_horizontal_count) * button_width;
    //const auto window_height = std::max(rows, 1) * button_height;

    // set size
    //SetWindowPos(&CWnd::wndTop, -1, -1,
    SetWindowPos(nullptr, -1, -1,
        std::min(windows_size, button_horizontal_count) * button_width,
        rows * button_height,
        SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOZORDER);

    int i = 0;
    for (const auto &window : windows)
    {
        auto btn = std::make_unique<window_button>(this, window);

        const auto x = i % button_horizontal_count;
        const auto y = i / button_horizontal_count;

        auto rect = button_rect;
        rect.MoveToXY(x * button_width, y * button_height);

        btn->create(rect, IDC_WINDOW_SELECT_BUTTON + i);

        capture_windows_.emplace_back(std::move(btn));
        ++i;
    }
    return TRUE;
}

BOOL window_select_ui::OnCmdMsg(UINT nID, int nCode, void *pExtra, AFX_CMDHANDLERINFO *pHandlerInfo)
{
    const auto button_id = static_cast<int>(nID) - IDC_WINDOW_SELECT_BUTTON;
    if (button_id < 0 || button_id > static_cast<int>(capture_windows_.size()))
        return CDialogEx::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);

    const auto &window = capture_windows_.at(button_id);
    const auto &window_data = window->get_data();
    //const auto process_name = std::string(window_data.process_name.begin(), window_data.process_name.end());

    if (completed_)
        completed_(window_data.hwnd);

    EndDialog(0);

    return TRUE;
}

void window_select_ui::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(window_select_ui, CDialogEx)
END_MESSAGE_MAP()
