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
#include "afxwinappex.h"
#include "afxdialogex.h"
#include "video_settings_ui.h"

IMPLEMENT_DYNAMIC(video_settings_ui, CDialogEx)

video_settings_ui::video_settings_ui(CWnd* pParent /*=nullptr*/)
    : CDialogEx(IDD_VIDEO_SETTINGS_UI, pParent)
{
}

video_settings_ui::video_settings_ui(CWnd *pParent /*= nullptr*/, video_settings_model &model)
    : CDialogEx(IDD_VIDEO_SETTINGS_UI, pParent)
    , model_(&model)
{
}

video_settings_ui::~video_settings_ui()
{
}

BOOL video_settings_ui::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    /* source fps */
    const auto fps_text = std::to_wstring(model_->video_source_fps_);
    video_source_fps_.SetWindowText(fps_text.c_str());

    /* source name */
    for (const auto video_source_name : video_source::names())
        video_source_combo_.AddString(video_source_name);
    video_source_combo_.SetCurSel(model_->video_source_.get_index());

    /* video container */
    for (const auto video_container_name : video_container::names())
        video_container_combo_.AddString(video_container_name);
    video_container_combo_.SetCurSel(model_->video_container_.get_index());

    /* codec name */
    for (const auto video_codec_name : video_codec::names())
        video_codec_combo_.AddString(video_codec_name);
    video_codec_combo_.SetCurSel(model_->video_codec_.get_index());

    /* codec preset */
    const auto preset_max = video_codec_preset::names().size() - 1;
    video_codec_preset_slider_.SetRange(0, static_cast<int>(preset_max));
    video_codec_preset_slider_.SetPos(model_->video_codec_.get_index());

    /* codec tune */
    for (const auto video_encoder_tune_name : video_codec_tune::names())
        video_codec_tune_.AddString(video_encoder_tune_name);
    video_codec_tune_.SetCurSel(model_->video_codec_tune_.get_index());

    /* codec profile */
    for (const auto video_encoder_profile_name : video_codec_profile::names())
        video_codec_profile_.AddString(video_encoder_profile_name);
    video_codec_profile_.SetCurSel(model_->video_codec_profile_.get_index());

    /* codec level */
    for (const auto video_codec_level_name : video_codec_level::names())
        video_codec_level_.AddString(video_codec_level_name);
    video_codec_level_.SetCurSel(model_->video_codec_level_.get_index());

    /* codec quality */
    video_codec_constant_quality_slider_.SetRange(0, 51);
    _set_codec_quality_mode(model_->video_codec_quality_type_);

    const auto quality = model_->video_codec_quality_constant_;
    video_codec_constant_quality_slider_.SetPos(quality);
    const auto quality_str = std::to_wstring(quality);
    video_codec_quality_value_label_.SetWindowText(quality_str.c_str());

    const auto bitrate = std::to_wstring(model_->video_codec_quality_bitrate_);
    codec_quality_bitrate_edit_.SetWindowText(bitrate.c_str());

    return TRUE;
}

void video_settings_ui::_set_codec_quality_mode(video_quality_type mode)
{
    switch (mode)
    {
        case video_quality_type::constant_quality:
            codec_constant_quality_radio_.SetCheck(TRUE);
            codec_constant_bitrate_radio_.SetCheck(FALSE);
            video_codec_constant_quality_slider_.EnableWindow(TRUE);
            code_constant_quality_low_label_.EnableWindow(TRUE);
            codec_constant_quality_high_label_.EnableWindow(TRUE);
            video_codec_quality_value_label_.EnableWindow(TRUE);
            codec_quality_bitrate_edit_.EnableWindow(FALSE);
            break;
        case video_quality_type::constant_bitrate:
            codec_constant_quality_radio_.SetCheck(FALSE);
            codec_constant_bitrate_radio_.SetCheck(TRUE);
            video_codec_constant_quality_slider_.EnableWindow(FALSE);
            code_constant_quality_low_label_.EnableWindow(FALSE);
            codec_constant_quality_high_label_.EnableWindow(FALSE);
            video_codec_quality_value_label_.EnableWindow(FALSE);
            codec_quality_bitrate_edit_.EnableWindow(TRUE);
            break;
    }

    model_->video_codec_quality_type_ = mode;
}

void video_settings_ui::_set_codec_quality_constant(int q)
{
    const auto index_text = std::to_wstring(q);
    model_->video_codec_quality_constant_ = q;
    video_codec_quality_value_label_.SetWindowText(index_text.c_str());
}

void video_settings_ui::_set_codec_preset_index(int index)
{
    const auto index_text = video_codec_preset::names().at(static_cast<size_t>(index));
    model_->video_codec_preset_.set_index(static_cast<video_codec_preset::type>(index));
    video_codec_preset_name_.SetWindowText(index_text);
}

void video_settings_ui::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_VIDEO_SOURCE_COMBO, video_source_combo_);
    DDX_Control(pDX, IDC_VIDEO_CONTAINER_COMBO, video_container_combo_);
    DDX_Control(pDX, IDC_VIDEO_CODEC_COMBO, video_codec_combo_);
    DDX_Control(pDX, IDC_CODEC_PRESET_SLIDER, video_codec_preset_slider_);
    DDX_Control(pDX, IDC_CODEC_TUNE_COMBO, video_codec_tune_);
    DDX_Control(pDX, IDC_CODEC_PROFILE_COMBO, video_codec_profile_);
    DDX_Control(pDX, IDC_CODEC_LEVEL_COMBO_, video_codec_level_);
    DDX_Control(pDX, IDC_FPS, video_source_fps_);
    DDX_Control(pDX, IDC_CODEC_QUALITY_CONSTANT, codec_constant_quality_radio_);
    DDX_Control(pDX, IDC_CODEC_QUALITY_BITRATE, codec_constant_bitrate_radio_);
    DDX_Control(pDX, IDC_CODEC_CONSTANT_QUALITY_SLIDER, video_codec_constant_quality_slider_);
    DDX_Control(pDX, IDC_CODEC_CONSTANT_QUALITY_LOW_LABEL, code_constant_quality_low_label_);
    DDX_Control(pDX, IDC_CODEC_CONSTANT_QUALITY_HIGH_LABEL, codec_constant_quality_high_label_);
    DDX_Control(pDX, IDC_CODEC_QUALITY_BITRATE_EDIT, codec_quality_bitrate_edit_);
    DDX_Control(pDX, IDC_CODEC_QUALITY_VALUE, video_codec_quality_value_label_);
    DDX_Control(pDX, IDC_CODEC_PRESET_NAME, video_codec_preset_name_);
}

BEGIN_MESSAGE_MAP(video_settings_ui, CDialogEx)
    ON_CBN_SELCHANGE(IDC_VIDEO_SOURCE_COMBO, &video_settings_ui::OnCbnSelchangeVideoSourceCombo)
    ON_CBN_SELCHANGE(IDC_VIDEO_CONTAINER_COMBO, &video_settings_ui::OnCbnSelchangeVideoContainerCombo)
    ON_CBN_SELCHANGE(IDC_VIDEO_CODEC_COMBO, &video_settings_ui::OnCbnSelchangeVideoCodecCombo)
    ON_CBN_SELCHANGE(IDC_CODEC_TUNE_COMBO, &video_settings_ui::OnCbnSelchangeCodecTuneCombo)
    ON_CBN_SELCHANGE(IDC_CODEC_PROFILE_COMBO, &video_settings_ui::OnCbnSelchangeCodecProfileCombo)
    ON_CBN_SELCHANGE(IDC_CODEC_LEVEL_COMBO_, &video_settings_ui::OnCbnSelchangeCodecLevelCombo)
    ON_EN_CHANGE(IDC_FPS, &video_settings_ui::OnEnChangeFps)
    ON_BN_CLICKED(IDC_CODEC_QUALITY_CONSTANT, &video_settings_ui::OnBnClickedCodecQualityConstant)
    ON_BN_CLICKED(IDC_CODEC_QUALITY_BITRATE, &video_settings_ui::OnBnClickedCodecQualityBitrate)
    ON_WM_HSCROLL()
    ON_EN_CHANGE(IDC_CODEC_QUALITY_BITRATE_EDIT, &video_settings_ui::OnEnChangeCodecQualityBitrateEdit)
    ON_BN_CLICKED(IDOK, &video_settings_ui::OnBnClickedOk)
END_MESSAGE_MAP()


// video_settings_ui message handlers

void video_settings_ui::OnEnChangeFps()
{
    wchar_t fps_text[10 + 1] = {};
    video_source_fps_.GetWindowText(fps_text, 10);
    try
    {
        const auto fps = std::stoi(fps_text);
        model_->video_source_fps_ = fps;
    }
    catch (std::exception &)
    {
        // blindly ignore the exception on purpose
    }
}

void video_settings_ui::OnCbnSelchangeVideoSourceCombo()
{
    const auto  index = video_source_combo_.GetCurSel();
    model_->video_source_.set_index(static_cast<video_source::type>(index));
}

void video_settings_ui::OnCbnSelchangeVideoContainerCombo()
{
    const auto index = video_container_combo_.GetCurSel();
    model_->video_container_.set_index(static_cast<video_container::type>(index));
}

void video_settings_ui::OnCbnSelchangeVideoCodecCombo()
{
    const auto index = video_codec_combo_.GetCurSel();
    model_->video_codec_.set_index(static_cast<video_codec::type>(index));
}

void video_settings_ui::OnCbnSelchangeCodecTuneCombo()
{
    const auto index = video_codec_tune_.GetCurSel();
    model_->video_codec_tune_.set_index(static_cast<video_codec_tune::type>(index));
}

void video_settings_ui::OnCbnSelchangeCodecProfileCombo()
{
    const auto index = video_codec_profile_.GetCurSel();
    model_->video_codec_profile_.set_index(static_cast<video_codec_profile::type>(index));
}

void video_settings_ui::OnCbnSelchangeCodecLevelCombo()
{
    const auto index = video_codec_level_.GetCurSel();
    model_->video_codec_level_.set_index(static_cast<video_codec_level::type>(index));
}

void video_settings_ui::OnBnClickedCodecQualityConstant()
{
    _set_codec_quality_mode(video_quality_type::constant_quality);
}

void video_settings_ui::OnBnClickedCodecQualityBitrate()
{
    _set_codec_quality_mode(video_quality_type::constant_bitrate);
}

void video_settings_ui::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
    if (nSBCode == SB_THUMBPOSITION || nSBCode == SB_THUMBTRACK)
    {
        const auto id = pScrollBar->GetDlgCtrlID();
        switch(id)
        {
        case IDC_CODEC_CONSTANT_QUALITY_SLIDER:
            _set_codec_quality_constant(static_cast<int>(nPos));
            break;
        case IDC_CODEC_PRESET_SLIDER:
            _set_codec_preset_index(static_cast<int>(nPos));
            break;
        }
    }

    CDialogEx::OnHScroll(nSBCode, nPos, pScrollBar);
}

void video_settings_ui::OnEnChangeCodecQualityBitrateEdit()
{
    wchar_t bitrate_text[10 + 1];
    codec_quality_bitrate_edit_.GetWindowText(bitrate_text, 10);

    try
    {
        const auto bitrate = std::stoi(bitrate_text);
        model_->video_codec_quality_bitrate_ = bitrate;
    }
    catch (std::exception &)
    {
        // blindly ignore the exception on purpose
    }
}

void video_settings_ui::OnBnClickedOk()
{
    model_->save();
    CDialogEx::OnOK();
}
