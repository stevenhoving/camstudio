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
#include "window_util.h"

#include <windows.h>
#include <psapi.h>

namespace utility
{
std::wstring get_window_text(const CWnd &window)
{
    const auto window_text_length = window.GetWindowTextLengthW();
    auto window_text = std::wstring(static_cast<size_t>(window_text_length) + 1, '\0');
    // \todo make safe lowering cast..
    window.GetWindowTextW(window_text.data(), static_cast<int>(window_text.size()));
    window_text.resize(static_cast<size_t>(window_text_length));
    return window_text;
}

auto get_window_title(HWND hwnd) -> std::wstring
{
    std::wstring title;
    const auto title_length = static_cast<std::size_t>(GetWindowTextLength(hwnd));
    title.resize(title_length + 1);
    GetWindowText(hwnd, &title[0], (int)title.size());
    title.resize(title_length);
    return title;
}

void label_auto_size(CStatic *label)
{
    CString s;
    label->GetWindowText(s);

    CDC dc;
    dc.CreateCompatibleDC(NULL);
    dc.SelectObject(label->GetFont());

    CRect r;
    label->GetClientRect(&r);

    if (s.Find('\n') < 0)
        dc.DrawText(s, &r, DT_CALCRECT | DT_NOPREFIX | DT_SINGLELINE | DT_EDITCONTROL);
    else
        dc.DrawText(s, &r, DT_CALCRECT | DT_NOPREFIX | DT_EDITCONTROL);

    label->SetWindowPos(0, 0, 0, r.Width(), r.Height(), SWP_NOMOVE);
}

void draw_bitmap(CDC *pDC, HBITMAP hbitmap, CRect size)
{
    CBitmap *pBitmap = CBitmap::FromHandle(hbitmap);
    BITMAP bm;
    pBitmap->GetBitmap(&bm);

    CDC MemDC;
    MemDC.CreateCompatibleDC(pDC);
    const auto old_bitmap = MemDC.SelectObject(pBitmap);

    pDC->SetStretchBltMode(STRETCH_HALFTONE);
    pDC->StretchBlt(0, 0, size.Width(), size.Height(), &MemDC, 0, 0, bm.bmWidth, bm.bmHeight, SRCCOPY);
    MemDC.SelectObject(old_bitmap);
}

bool is_in_rect(const CRect &rect, const CPoint &point)
{
    const bool result =
        (point.x > rect.left && point.x < rect.right)
        && (point.y > rect.top && point.y < rect.bottom);
    return result;
}

bool rect_empty(const CRect &rect)
{
    return rect.right == rect.left || rect.bottom == rect.top;
}

bool is_top_most(HWND hwnd)
{
    return GetWindowLong(hwnd, GWL_EXSTYLE) & WS_EX_TOPMOST;
}

HWND get_root_parent(HWND hwnd)
{
    HWND result = GetParent(hwnd);
    if (result)
        result = get_root_parent(result);
    return hwnd;
}

auto get_process_name(unsigned int process_id) -> std::tuple<std::wstring, std::wstring>
{
    std::wstring process_name(MAX_PATH, '\0');
    std::wstring process_filepath(MAX_PATH, '\0');

    const auto process = ::OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE,
        process_id);
    if (process)
    {
        HMODULE module = 0;
        DWORD needed = 0;
        if (::EnumProcessModules(process, &module, sizeof(module), &needed))
        {
            const auto process_name_len = ::GetModuleBaseName(process, module, process_name.data(),
                MAX_PATH);
            const auto process_filepath_len = ::GetModuleFileNameEx(process, module,
                process_filepath.data(), MAX_PATH);

            process_name.resize(process_name_len);
            process_filepath.resize(process_filepath_len);
        }
    }
    return std::make_tuple(process_name, process_filepath);
}

} // namespace utility
