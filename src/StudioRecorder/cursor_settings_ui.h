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

#include "cursor_settings_preview.h"
#include <CamCapture/cam_annotarion.h>
#include <CamCapture/cam_mouse_button.h>
#include <vector>

class settings_model;
class cam_annotation_cursor;

class cursor_settings_ui : public CDialogEx
{
    DECLARE_DYNAMIC(cursor_settings_ui)
public:
    cursor_settings_ui(CWnd* pParent = nullptr);
    cursor_settings_ui(CWnd* pParent, settings_model *settings);
    virtual ~cursor_settings_ui();
    cursor_settings_ui(const cursor_settings_ui &) = delete;
    cursor_settings_ui &operator = (const cursor_settings_ui &) = delete;

    BOOL OnInitDialog() override;

    // Dialog Data
#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_CURSOR_SETTINGS_UI };
#endif

private:
    void _set_annotation_settings();
    void _set_cursor_halo_size(const int halo_size);
    void _set_cursor_ring_size(const int halo_size);
    void _draw_cursor_preview(cam_mouse_button::type mouse_buttons_state, double dt = 0.1);

    // hack for now not following MVP or MVC
    settings_model *settings_{ nullptr };
    std::unique_ptr<cam_annotation_cursor> annotation_{};
    unsigned int mouse_button_state_{cam_mouse_button::none};

protected:
    void DoDataExchange(CDataExchange* pDX) override;

    DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnBnClickedShowCursor();
    afx_msg void OnBnClickedShowRings();
    afx_msg void OnBnClickedShowHalo();
    afx_msg void OnBnClickedShowMouseClicks();
    afx_msg void OnBnClickedPickHaloColor();
    afx_msg void OnBnClickedPickClickLeftColor();
    afx_msg void OnBnClickedPickClickRightColor();
    afx_msg void OnBnClickedPickClickMiddleColor();
    afx_msg void OnBnClickedRingClickLeftColor();
    afx_msg void OnBnClickedRingClickRightColor();
    afx_msg void OnBnClickedRingClickMiddleColor();
    afx_msg void OnCbnSelchangeHaloShape();
    afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
    afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
    afx_msg void OnTimer(UINT_PTR nIDEvent);

    CButton show_cursor_checkbox_;
    CButton show_cursor_rings_checkbox_;
    CButton show_cursor_mouse_halo_checkbox_;
    CButton show_cursor_mouse_click_checkbox_;

    CSliderCtrl halo_size_ctrl_;
    CSliderCtrl ring_size_ctrl_;

    CComboBox halo_shape_ctrl_;

    CStatic cursor_halo_color_example_;
    CStatic cursor_halo_size_label_;
    CStatic cursor_ring_size_label_;

    cursor_settings_preview cursor_preview_;
};
