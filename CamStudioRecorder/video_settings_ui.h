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
 * along with this program.If not, see < https://www.gnu.org/licenses/>.
 */

#pragma once

#include "video_settings_model.h"

class video_settings_ui : public CDialogEx
{
    DECLARE_DYNAMIC(video_settings_ui)

public:
    video_settings_ui(CWnd* pParent = nullptr);
    video_settings_ui(CWnd* pParent, video_settings_model &model);

    virtual ~video_settings_ui();

    BOOL OnInitDialog() override;

     // hack for now not following MVP or MVC
    video_settings_model *model_{nullptr};

// Dialog Data
#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_VIDEO_SETTINGS_UI };
#endif

private:
    void _set_codec_quality_mode(video_quality_type mode);
    void _set_codec_quality_constant(int q);
    void _set_codec_preset_index(int index);

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

    DECLARE_MESSAGE_MAP()
private:
    CEdit video_source_fps_;
    CComboBox video_source_combo_;
    CComboBox video_container_combo_;
    CComboBox video_codec_combo_;
    CComboBox video_codec_tune_;
    CComboBox video_codec_profile_;
    CComboBox video_codec_level_;
    CSliderCtrl video_codec_preset_slider_;
    CButton codec_constant_quality_radio_;
    CButton codec_constant_bitrate_radio_;
    CSliderCtrl video_codec_constant_quality_slider_;
    CStatic code_constant_quality_low_label_;
    CStatic codec_constant_quality_high_label_;
    CEdit codec_quality_bitrate_edit_;
    CStatic video_codec_quality_value_label_;
    CStatic video_codec_preset_name_;
public:
    afx_msg void OnCbnSelchangeVideoSourceCombo();
    afx_msg void OnCbnSelchangeVideoContainerCombo();
    afx_msg void OnCbnSelchangeVideoCodecCombo();
    afx_msg void OnCbnSelchangeCodecTuneCombo();
    afx_msg void OnCbnSelchangeCodecProfileCombo();
    afx_msg void OnCbnSelchangeCodecLevelCombo();
    afx_msg void OnEnChangeFps();
    afx_msg void OnBnClickedCodecQualityConstant();
    afx_msg void OnBnClickedCodecQualityBitrate();
    afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
    afx_msg void OnEnChangeCodecQualityBitrateEdit();
    afx_msg void OnBnClickedOk();
};
