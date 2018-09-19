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
#include "cursor_settings_ui.h"
#include "settings_model.h"
#include "window_utils.h"
#include "afxdialogex.h"
#include <CamCapture/annotations/cam_annotation_cursor.h>
#include <CamCapture/cam_color.h>
#include <CamCapture/cam_rect.h>
#include <CamCapture/cam_gdiplus.h>
#include <CamCapture/cam_draw_data.h>
#include <CamCapture/cam_stop_watch.h>
#include <fmt/printf.h>

IMPLEMENT_DYNAMIC(cursor_settings_ui, CDialogEx)

BEGIN_MESSAGE_MAP(cursor_settings_ui, CDialogEx)
    ON_BN_CLICKED(IDC_SHOW_CURSOR, &cursor_settings_ui::OnBnClickedShowCursor)
    ON_BN_CLICKED(IDC_SHOW_RINGS, &cursor_settings_ui::OnBnClickedShowRings)
    ON_BN_CLICKED(IDC_HALO_COLOR, &cursor_settings_ui::OnBnClickedPickHaloColor)
    ON_BN_CLICKED(IDC_CLICK_LEFT_COLOR, &cursor_settings_ui::OnBnClickedPickClickLeftColor)
    ON_BN_CLICKED(IDC_CLICK_RIGHT_COLOR, &cursor_settings_ui::OnBnClickedPickClickRightColor)
    ON_BN_CLICKED(IDC_CLICK_MIDDLE_COLOR, &cursor_settings_ui::OnBnClickedPickClickMiddleColor)
    ON_BN_CLICKED(IDC_SHOW_HALO, &cursor_settings_ui::OnBnClickedShowHalo)
    ON_BN_CLICKED(IDC_SHOW_MOUSE_CLICKS, &cursor_settings_ui::OnBnClickedShowMouseClicks)
    ON_CBN_SELCHANGE(IDC_HALO_SHAPE, &cursor_settings_ui::OnCbnSelchangeHaloShape)
    ON_WM_HSCROLL()
    ON_WM_CTLCOLOR()
    ON_WM_TIMER()
    ON_BN_CLICKED(IDC_RING_CLICK_LEFT_COLOR, &cursor_settings_ui::OnBnClickedRingClickLeftColor)
    ON_BN_CLICKED(IDC_RING_CLICK_RIGHT_COLOR, &cursor_settings_ui::OnBnClickedRingClickRightColor)
    ON_BN_CLICKED(IDC_RING_CLICK_MIDDLE_COLOR, &cursor_settings_ui::OnBnClickedRingClickMiddleColor)
END_MESSAGE_MAP()


cursor_settings_ui::cursor_settings_ui(CWnd* pParent /*=nullptr*/)
    : CDialogEx(IDD_CURSOR_SETTINGS_UI, pParent)
{
}

cursor_settings_ui::cursor_settings_ui(CWnd* pParent, settings_model *settings)
    : CDialogEx(IDD_CURSOR_SETTINGS_UI, pParent)
    , settings_(settings)
{
}

cursor_settings_ui::~cursor_settings_ui()
{
}

BOOL cursor_settings_ui::OnInitDialog()
{
    CDialog::OnInitDialog();

    // setup
    halo_size_ctrl_.SetRange(1, 128, TRUE);
    ring_size_ctrl_.SetRange(1, 128, TRUE);
    //halo_shape_ctrl_.SetItemData() // Circle;Ellipse;Square;Rectangle;

    const auto halo_size = cam::size(
        settings_->get_cursor_halo_size(),
        settings_->get_cursor_halo_size()
    );

    annotation_ = std::make_unique<cam_annotation_cursor>(
        settings_->get_cursor_enabled(),
        settings_->get_cursor_ring_enabled(),

        mouse_action_config{ settings_->get_cursor_ring_enabled(), halo_size, settings_->get_cursor_ring_click_left_color() },
        mouse_action_config{ settings_->get_cursor_ring_enabled(), halo_size, settings_->get_cursor_ring_click_right_color() },
        mouse_action_config{ settings_->get_cursor_ring_enabled(), halo_size, settings_->get_cursor_ring_click_right_color() },

        static_cast<cam_halo_type>(settings_->get_cursor_halo_type()),
        //mouse_action_config
        mouse_action_config{ settings_->get_cursor_halo_enabled(), halo_size, settings_->get_cursor_halo_color() },
        mouse_action_config{ settings_->get_cursor_click_enabled(), halo_size, settings_->get_cursor_click_left_color() },
        mouse_action_config{ settings_->get_cursor_click_enabled(), halo_size, settings_->get_cursor_click_right_color() },
        mouse_action_config{ settings_->get_cursor_click_enabled(), halo_size, settings_->get_cursor_click_middle_color() }
    );

    // set config
    // halo
    const auto halo_shape_size = settings_->get_cursor_halo_size();
    halo_size_ctrl_.SetPos(halo_shape_size);

    const auto halo_shape_type = static_cast<int>(settings_->get_cursor_halo_type());
    halo_shape_ctrl_.SetCurSel(halo_shape_type);
    _set_cursor_halo_size(settings_->get_cursor_halo_size());

    show_cursor_checkbox_.SetCheck(settings_->get_cursor_enabled());
    show_cursor_mouse_halo_checkbox_.SetCheck(settings_->get_cursor_halo_enabled());
    show_cursor_mouse_click_checkbox_.SetCheck(settings_->get_cursor_click_enabled());

    // ring
    show_cursor_rings_checkbox_.SetCheck(settings_->get_cursor_ring_enabled());
    const auto ring_size = settings_->get_cursor_ring_size();
    ring_size_ctrl_.SetPos(ring_size);
    _set_cursor_ring_size(ring_size);

    cursor_preview_.set_on_left_down([this](){mouse_button_state_ |= cam_mouse_button::left_button_down;});
    cursor_preview_.set_on_right_down([this](){mouse_button_state_ |= cam_mouse_button::right_button_down;});
    cursor_preview_.set_on_middle_down([this](){mouse_button_state_ |= cam_mouse_button::middle_button_down;});
    cursor_preview_.set_on_left_up([this]() {mouse_button_state_ |= cam_mouse_button::left_button_up; });
    cursor_preview_.set_on_right_up([this]() {mouse_button_state_ |= cam_mouse_button::right_button_up; });
    cursor_preview_.set_on_middle_up([this]() {mouse_button_state_ |= cam_mouse_button::middle_button_up; });

    // update preview
    SetTimer(0, 50, nullptr);

    return TRUE;
}

void cursor_settings_ui::_set_annotation_settings()
{
    annotation_->set_cursor_enabled(settings_->get_cursor_enabled());
    annotation_->set_cursor_ring_enabled(settings_->get_cursor_ring_enabled());

    // ring config
    const auto ring_size = cam::size(
        settings_->get_cursor_ring_size(),
        settings_->get_cursor_ring_size()
    );

    annotation_->set_ring_left_click_config(
        { settings_->get_cursor_ring_enabled(), ring_size, settings_->get_cursor_ring_click_left_color() }
    );

    annotation_->set_ring_right_click_config(
        { settings_->get_cursor_ring_enabled(), ring_size, settings_->get_cursor_ring_click_right_color() }
    );

    annotation_->set_ring_middle_click_config(
        { settings_->get_cursor_ring_enabled(), ring_size, settings_->get_cursor_ring_click_middle_color() }
    );

    // halo config
    annotation_->set_halo_type(static_cast<cam_halo_type>(settings_->get_cursor_halo_type()));

    const auto halo_size = cam::size(
        settings_->get_cursor_halo_size(),
        settings_->get_cursor_halo_size()
    );

    annotation_->set_halo_config(
        { settings_->get_cursor_halo_enabled(), halo_size, settings_->get_cursor_halo_color() }
    );

    annotation_->set_halo_left_click_config(
        { settings_->get_cursor_click_enabled(), halo_size, settings_->get_cursor_click_left_color() }
    );

    annotation_->set_halo_right_click_config(
        { settings_->get_cursor_click_enabled(), halo_size, settings_->get_cursor_click_right_color() }
    );

    annotation_->set_halo_middle_click_config(
        { settings_->get_cursor_click_enabled(), halo_size, settings_->get_cursor_click_middle_color() }
    );
}

void cursor_settings_ui::_set_cursor_halo_size(const int halo_size)
{
    settings_->set_cursor_halo_size(halo_size);

    const auto halo_size_string = std::to_wstring(halo_size) + L"px";
    cursor_halo_size_label_.SetWindowText(halo_size_string.c_str());
    label_auto_size(&cursor_halo_size_label_);

    _draw_cursor_preview(cam_mouse_button::none);
}

void cursor_settings_ui::_set_cursor_ring_size(const int ring_size)
{
    settings_->set_cursor_ring_size(ring_size);

    const auto ring_size_string = std::to_wstring(ring_size) + L"px";
    cursor_ring_size_label_.SetWindowText(ring_size_string.c_str());
    label_auto_size(&cursor_ring_size_label_);

    _draw_cursor_preview(cam_mouse_button::none);
}

void cursor_settings_ui::_draw_cursor_preview(cam_mouse_button::type mouse_buttons_state, double dt /*= 0.1*/)
{
    // force update annotation settings
    _set_annotation_settings();

    CRect preview_rect;
    cursor_preview_.GetWindowRect(&preview_rect);

    Gdiplus::Bitmap image(preview_rect.Width(), preview_rect.Height(), PixelFormat32bppPARGB);

    Gdiplus::Graphics canvas(&image);
    canvas.SetSmoothingMode(Gdiplus::SmoothingMode::SmoothingModeAntiAlias);
    canvas.SetCompositingMode(Gdiplus::CompositingMode::CompositingModeSourceOver);

    // draw a raster so we can see what has alpha and what is not...
    //Gdiplus::Pen pen(Gdiplus::Color::Gray, 1);
    //const auto raster_line_count = 5;
    //preview_rect.Width() / raster_line_count;

    point<int> mouse_point(preview_rect.Width()/2, preview_rect.Height()/2);

    cam::rect<int> prev_rect(0, 0, preview_rect.Width(), preview_rect.Height());
    cam_draw_data draw_data(dt, prev_rect, mouse_point, mouse_buttons_state);

    annotation_->draw(canvas, draw_data);

    HBITMAP bitmap;
    if (const auto ret = image.GetHBITMAP(Gdiplus::Color(255, 255, 255, 255), &bitmap); ret != Gdiplus::Status::Ok)
        fmt::print("failed to get bitmap from bitmap\n");

    HBITMAP hbmOld = cursor_preview_.SetBitmap(bitmap);
    if (hbmOld != nullptr)
        ::DeleteObject(hbmOld);
}

void cursor_settings_ui::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);

    DDX_Control(pDX, IDC_SHOW_CURSOR, show_cursor_checkbox_);
    DDX_Control(pDX, IDC_SHOW_RINGS, show_cursor_rings_checkbox_);
    DDX_Control(pDX, IDC_SHOW_HALO, show_cursor_mouse_halo_checkbox_);
    DDX_Control(pDX, IDC_SHOW_MOUSE_CLICKS, show_cursor_mouse_click_checkbox_);

    DDX_Control(pDX, IDC_HALO_SIZE, halo_size_ctrl_);
    DDX_Control(pDX, IDC_HALO_SHAPE, halo_shape_ctrl_);

    DDX_Control(pDX, IDC_CURSOR_HALO_COLOR_EXAMPLE, cursor_halo_color_example_);
    DDX_Control(pDX, IDC_CURSOR_SIZE_LABEL, cursor_halo_size_label_);
    DDX_Control(pDX, IDC_CURSOR_PREVIEW, cursor_preview_);
    DDX_Control(pDX, IDC_RING_CURSOR_SIZE_LABEL, cursor_ring_size_label_);
    DDX_Control(pDX, IDC_RING_SIZE, ring_size_ctrl_);
}

void cursor_settings_ui::OnCbnSelchangeHaloShape()
{
    const auto index = halo_shape_ctrl_.GetCurSel();
    settings_->set_cursor_halo_type(static_cast<cursor_halo_type>(index));

    //_draw_cursor_preview(cam_mouse_button::none);
}

void cursor_settings_ui::OnBnClickedShowCursor()
{
    const auto checked = show_cursor_checkbox_.GetCheck() != 0;
    settings_->set_cursor_enabled(checked);
    //_draw_cursor_preview(cam_mouse_button::none);
}

void cursor_settings_ui::OnBnClickedShowRings()
{
    const auto checked = show_cursor_rings_checkbox_.GetCheck() != 0;
    settings_->set_cursor_ring_enabled(checked);
}

void cursor_settings_ui::OnBnClickedShowHalo()
{
    const auto checked = show_cursor_mouse_halo_checkbox_.GetCheck() != 0;
    settings_->set_cursor_halo_enabled(checked);
    //_draw_cursor_preview(cam_mouse_button::none);
}

void cursor_settings_ui::OnBnClickedShowMouseClicks()
{
    const auto checked = show_cursor_mouse_click_checkbox_.GetCheck() != 0;
    settings_->set_cursor_click_enabled(checked);
    //_draw_cursor_preview(cam_mouse_button::none);
}

template<typename Callback>
void handle_color_change(const cam::color &color, Callback save_color_callback)
{
    const auto rgb_color = RGB(color.r_, color.g_, color.b_);
    CColorDialog color_dialog(rgb_color, CC_ANYCOLOR | CC_FULLOPEN | CC_RGBINIT);
    if (color_dialog.DoModal() == IDOK)
    {
        const auto out_color = color_dialog.GetColor();
        const auto save_color = cam::color(127, GetRValue(out_color), GetGValue(out_color), GetBValue(out_color));
        save_color_callback(save_color);
    }
}

void cursor_settings_ui::OnBnClickedPickHaloColor()
{
    handle_color_change(settings_->get_cursor_halo_color(),
        [this](const cam::color &save_color)
        {settings_->set_cursor_halo_color(save_color); Invalidate();}
    );
}

void cursor_settings_ui::OnBnClickedPickClickLeftColor()
{
    handle_color_change(settings_->get_cursor_click_left_color(),
        [this](const cam::color &save_color)
        {settings_->set_cursor_click_left_color(save_color); Invalidate();}
    );
}

void cursor_settings_ui::OnBnClickedPickClickRightColor()
{
    handle_color_change(settings_->get_cursor_click_right_color(),
        [this](const cam::color &save_color)
        {settings_->set_cursor_click_right_color(save_color); Invalidate();}
    );
}

void cursor_settings_ui::OnBnClickedPickClickMiddleColor()
{
    handle_color_change(settings_->get_cursor_click_middle_color(),
        [this](const cam::color &save_color)
        {settings_->set_cursor_click_middle_color(save_color); Invalidate();}
    );
}

void cursor_settings_ui::OnBnClickedRingClickLeftColor()
{
    handle_color_change(settings_->get_cursor_ring_click_left_color(),
        [this](const cam::color &save_color)
        {settings_->set_cursor_ring_click_left_color(save_color); Invalidate(); }
    );
}

void cursor_settings_ui::OnBnClickedRingClickRightColor()
{
    handle_color_change(settings_->get_cursor_ring_click_right_color(),
        [this](const cam::color &save_color)
        {settings_->set_cursor_ring_click_right_color(save_color); Invalidate(); }
    );
}

void cursor_settings_ui::OnBnClickedRingClickMiddleColor()
{
    handle_color_change(settings_->get_cursor_ring_click_middle_color(),
        [this](const cam::color &save_color)
        {settings_->set_cursor_ring_click_middle_color(save_color); Invalidate(); }
    );
}

HBRUSH cursor_settings_ui::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
    HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

    if (nCtlColor == CTLCOLOR_STATIC)
    {
        switch(pWnd->GetDlgCtrlID())
        {
            // halo
            case IDC_CURSOR_HALO_COLOR_EXAMPLE:
            {
                const auto color = settings_->get_cursor_halo_color();
                return (HBRUSH)CreateSolidBrush(RGB(color.r_, color.g_, color.b_));
            }
            case IDC_CURSOR_LEFT_CLICK_COLOR_EXAMPLE:
            {
                const auto color = settings_->get_cursor_click_left_color();
                return (HBRUSH)CreateSolidBrush(RGB(color.r_, color.g_, color.b_));
            }
            case IDC_CURSOR_RIGHT_CLICK_COLOR_EXAMPLE:
            {
                const auto color = settings_->get_cursor_click_right_color();
                return (HBRUSH)CreateSolidBrush(RGB(color.r_, color.g_, color.b_));
            }
            case IDC_CURSOR_MIDDLE_CLICK_COLOR_EXAMPLE:
            {
                const auto color = settings_->get_cursor_click_middle_color();
                return (HBRUSH)CreateSolidBrush(RGB(color.r_, color.g_, color.b_));
            }
            // ring
            case IDC_RING_CURSOR_LEFT_CLICK_COLOR_EXAMPLE:
            {
                const auto color = settings_->get_cursor_ring_click_left_color();
                return (HBRUSH)CreateSolidBrush(RGB(color.r_, color.g_, color.b_));
            }
            case IDC_RING_CURSOR_RIGHT_CLICK_COLOR_EXAMPLE:
            {
                const auto color = settings_->get_cursor_ring_click_right_color();
                return (HBRUSH)CreateSolidBrush(RGB(color.r_, color.g_, color.b_));
            }
            case IDC_RING_CURSOR_MIDDLE_CLICK_COLOR_EXAMPLE:
            {
                const auto color = settings_->get_cursor_ring_click_middle_color();
                return (HBRUSH)CreateSolidBrush(RGB(color.r_, color.g_, color.b_));
            }
        }
    }

    return hbr;
}

void cursor_settings_ui::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
    if (nSBCode == SB_THUMBPOSITION || nSBCode == SB_THUMBTRACK)
    {
        const auto id = pScrollBar->GetDlgCtrlID();
        switch (id)
        {
        case IDC_HALO_SIZE:
            _set_cursor_halo_size(static_cast<int>(nPos));
            break;
        case IDC_RING_SIZE:
            _set_cursor_ring_size(static_cast<int>(nPos));
            break;
        }
    }

    CDialogEx::OnHScroll(nSBCode, nPos, pScrollBar);
}

void cursor_settings_ui::OnTimer(UINT_PTR /*nIDEvent*/)
{
    //assert(nIDEvent == 0);
    static auto stopwatch = [](){cam::stop_watch temp; temp.time_start(); return temp;}();
    const auto dt = stopwatch.time_since();
    _draw_cursor_preview(static_cast<cam_mouse_button::type>(mouse_button_state_), dt);
    stopwatch.time_start();

    if (mouse_button_state_ & cam_mouse_button::left_button_up)
        mouse_button_state_ &= ~(cam_mouse_button::left_button_down | cam_mouse_button::left_button_up);

    if (mouse_button_state_ & cam_mouse_button::right_button_up)
        mouse_button_state_ &= ~(cam_mouse_button::right_button_down | cam_mouse_button::right_button_up);

    if (mouse_button_state_ & cam_mouse_button::middle_button_up)
        mouse_button_state_ &= ~(cam_mouse_button::middle_button_down | cam_mouse_button::middle_button_up);
}





