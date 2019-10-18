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
#include "display_select_ui.h"
#include "utility/window_util.h"
#include "utility/windows_api.h"
#include "utility/fmt_helper.h"
#include "utility/rect_util.h"

#include <psapi.h>
#include <vector>
#include <string>
#include <fmt/printf.h>


IMPLEMENT_DYNAMIC(display_select_ui, CDialogEx)

/* \todo Create a list of processes that have actual visible windows for this user.
 - For each of these processes create a capture source.
 - for each of these processes create a button on the ui.
 - on press call supplied callback to start recording (covering record window use case).
*/

/* \todo no dpi correction */
//constexpr auto button_spacing = 10;
constexpr auto button_width = 250;
constexpr auto button_height = 250;

std::vector<display_data> display_select_ui::get_displays()
{
    std::vector<display_data> result;
    int index = 0;
    winapi::display::enum_monitors(
        [&result, &index](const MONITORINFOEX &monitor)
        {
            result.emplace_back(index++, monitor);
        }
    );

    return result;
}

display_select_ui::display_select_ui(CWnd* pParent, const std::function<void()> &completed)
    : CDialogEx(IDD_DISPLAY_SELECT, pParent)
    , completed_(completed)
{
}

BOOL display_select_ui::OnInitDialog()
{
    fmt::print("display_select_ui::OnInitDialog\n");

    const auto displays = get_displays();

    CRect button_rect(0, 0, button_width, button_height);

    constexpr auto button_horizontal_count = 6;
    const auto windows_size = static_cast<int>(displays.size());
//    const auto columns = windows_size % button_horizontal_count;
    //const auto rows = windows_size / button_horizontal_count;

    //const auto window_width = std::min(windows_size, button_horizontal_count) * button_width;
    //const auto window_height = std::max(rows, 1) * button_height;

    fmt::print("desktop count: {}\n", windows_size);
    // set size
    //SetWindowPos(&CWnd::wndTop, -1, -1,
    SetWindowPos(nullptr, -1, -1,
        std::min(windows_size, button_horizontal_count) * button_width,
        button_height,
        SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOZORDER);

    //const auto desktop_window = ::FindWindow(nullptr, _T("Progman"));
    int i = 0;
    for (const auto &display : displays)
    {
        //auto &btn = *capture_displays_.emplace(capture_displays_.begin(),
            //std::make_unique<display_button>(this, display));

        auto &btn = *capture_displays_.emplace(capture_displays_.begin(),
            std::make_unique<display_thumbnail_widget_base>(display));
        (void)btn;

        const auto x = i % button_horizontal_count;
        const auto y = i / button_horizontal_count;
        //
        auto rect = button_rect;
        rect.MoveToXY(x * button_width, y * button_height);

        //const auto style = WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_OWNERDRAW;
        //const auto style = WS_VISIBLE;
        //btn->Create(_T(""), style, rect, this, IDC_WINDOW_SELECT_BUTTON + i);

        ++i;
    }

    CDialogEx::OnInitDialog();
    return TRUE;
}

BOOL display_select_ui::OnCmdMsg(UINT nID, int nCode, void *pExtra, AFX_CMDHANDLERINFO *pHandlerInfo)
{
    const auto button_id = static_cast<int>(nID) - IDC_WINDOW_SELECT_BUTTON;
    if (button_id < 0 || button_id > static_cast<int>(capture_displays_.size()))
        return CDialogEx::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);

    //const auto &desktop = capture_displays_.at(button_id);
    //const auto &data = desktop->get_data();

    if (completed_)
        //completed_(data.hwnd);
        completed_();

    EndDialog(0);

    return TRUE;
}

void display_select_ui::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    fmt::print("display_select_ui::DoDataExchange\n");

    int i = 0;
    for (auto &frame : capture_displays_)
    {
        const auto index = IDC_WINDOW_SELECT_BUTTON + i;
        DDX_Control(pDX, index, *frame);
    }
}

BEGIN_MESSAGE_MAP(display_select_ui, CDialogEx)
END_MESSAGE_MAP()
