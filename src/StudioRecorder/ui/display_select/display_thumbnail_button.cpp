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
#include "display_thumbnail_button.h"
#include "utility/window_util.h"
#include "background_capture_source.h"
#include <fmt/printf.h>

display_button::display_button(CWnd *parent /*= nullptr*/)
    : CButton()
    , parent_(parent)
    , data_()
    , capture_source_()
{
}

display_button::display_button(CWnd *parent, const display_data &data)
    : CButton()
    , parent_(parent)
    , data_(data)
    , capture_source_(std::make_unique<background_capture_source>(nullptr /* capture desktop */, data.info.rcWork))
{
}

display_button::~display_button() = default;

void display_button::create(const CRect &rect, const unsigned int id)
{
    fmt::print("display_button::create\n");

    const auto style = WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_OWNERDRAW;
    Create(_T("desktop nr: "), style, rect, parent_, id);
}

void display_button::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
    fmt::print("display_button::DrawItem\n");
    CRect client_rect;
    GetWindowRect(&client_rect);

    CDC dc;
    dc.Attach(lpDrawItemStruct->hDC);
    CRect rt = lpDrawItemStruct->rcItem;

    capture_source_->capture_frame();
    const auto bitmap = capture_source_->get_frame();
    //background_frame_ = background_capture_->get_frame();
    background_bitmap_ = std::unique_ptr<Gdiplus::Bitmap>(Gdiplus::Bitmap::FromHBITMAP(bitmap, 0));


    //dc.FillSolidRect(rt, RGB(200, 200, 200));

    // Create a compatible DC.
    //auto hdc_mem = ::CreateCompatibleDC(ps.hdc);

    // Create a bitmap big enough for our client rectangle.
    //auto hbmMem = ::CreateCompatibleBitmap(ps.hdc, rc.right - rc.left, rc.bottom - rc.top);

    // Select the bitmap into the off-screen DC.
    //auto hbmOld = (HBITMAP)::SelectObject(hdc_mem, hbmMem);

    // Render the image into the offscreen DC.
    // \note using gdiplus to draw something is not fast...
    Gdiplus::Graphics canvas(lpDrawItemStruct->hDC);

    const Gdiplus::RectF paint_rect(
        static_cast<Gdiplus::REAL>(rt.left),
        static_cast<Gdiplus::REAL>(rt.top),
        static_cast<Gdiplus::REAL>(rt.right - rt.left),
        static_cast<Gdiplus::REAL>(rt.bottom - rt.top));
    const auto ret = canvas.DrawImage(background_bitmap_.get(), paint_rect);
    assert(ret == Gdiplus::Status::Ok);

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

    //std::wstring button_text = L"something";
    //std::wstring &button_text = data_.window_title;
    //if (button_text.empty())
        //button_text = data_.process_name;

    //dc.DrawText(button_text.c_str(), text_rect, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_WORDBREAK);

    if (state & ODS_FOCUS)
    {
        auto focus_rect = rt;
        focus_rect.InflateRect(3, 3);
        dc.DrawFocusRect(&focus_rect);
    }

    dc.Detach();
}

const display_data &display_button::get_data() const noexcept
{
    return data_;
}
