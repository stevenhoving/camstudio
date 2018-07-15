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
#include "window_thumbnail_widget.h"
#include "window_utils.h"
#include <fmt/printf.h>
#include "fmt_helper.h"
#include <shellapi.h>
#include <CommonControls.h>

HICON get_shell_icon(const std::wstring &filepath)
{
    SHFILEINFOW sfi;
    ::SHGetFileInfo(filepath.c_str(), 0, &sfi, sizeof(sfi), SHGFI_SYSICONINDEX);

    IImageList *piml = nullptr;
    if (::SHGetImageList(SHIL_LARGE, IID_IImageList, (void **)&piml) == S_OK)
    {
        HICON hIcon;
        if (piml->GetIcon(sfi.iIcon, ILD_TRANSPARENT, &hIcon) == S_OK)
            return hIcon;
    }

    return nullptr;
}

HICON get_icon(HWND hwnd, const std::wstring &filepath)
{
    HICON result = 0;
    const auto ret = ::SendMessageTimeout(hwnd, WM_GETICON, ICON_BIG, 0, SMTO_ABORTIFHUNG, 2000,
        reinterpret_cast<DWORD_PTR*>(&result));
    if (ret != 0 && result == nullptr)
        result = reinterpret_cast<HICON>(::GetClassLongPtr(hwnd, GCLP_HICON));

    if (result == nullptr)
        result = get_shell_icon(filepath);

    return result;
}

window_button::window_button(CWnd *parent, const window_data &data)
    : CButton()
    , parent_(parent)
    , data_(data)
    , window_icon_(get_icon(data.hwnd, data.process_filepath))
{
}

void window_button::create(CRect rect, const unsigned int id)
{
    const auto style = WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_OWNERDRAW;
    Create(data_.window_title.c_str(), style, rect, parent_, id);
}

void window_button::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
    CRect client_rect;
    GetWindowRect(&client_rect);

    CDC dc;
    dc.Attach(lpDrawItemStruct->hDC);
    CRect rt = lpDrawItemStruct->rcItem;

    dc.FillSolidRect(rt, RGB(200, 200, 200));

    auto state = lpDrawItemStruct->itemState;
    if (state & ODS_SELECTED)
    {
        dc.DrawEdge(rt, EDGE_SUNKEN, BF_RECT);
    }
    else
    {
        dc.DrawEdge(rt, EDGE_RAISED, BF_RECT);
    }

    // draw text if needed
    dc.SetTextColor(RGB(0, 0, 0));
    auto text_rect = rt;
    text_rect.DeflateRect(3, 3);

    if (!window_icon_.empty())
    {
        text_rect.bottom = text_rect.top + window_icon_.height;
        dc.DrawIcon(text_rect.TopLeft(), window_icon_.window_icon_);
        text_rect.left = window_icon_.width + 5;
    }

    std::wstring &button_text = data_.window_title;
    if (button_text.empty())
        button_text = data_.process_name;

    dc.DrawText(button_text.c_str(), text_rect, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_WORDBREAK);

    if (state & ODS_FOCUS)
    {
        auto focus_rect = rt;
        focus_rect.InflateRect(3, 3);
        dc.DrawFocusRect(&focus_rect);
    }

    dc.Detach();
}

window_data &window_button::get_data() noexcept
{
    return data_;
}
