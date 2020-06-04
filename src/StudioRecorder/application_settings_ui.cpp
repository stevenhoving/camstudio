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
#include "application_settings_ui.h"
#include "utility/string_convert.h"
#include "utility/window_util.h"
#include <filesystem>

IMPLEMENT_DYNAMIC(application_settings_ui, CDialogEx)

application_settings_ui::application_settings_ui(CWnd* pParent /*=nullptr*/)
    : CDialogEx(IDD_APPLICATION_SETTINGS_UI, pParent)
{
}

application_settings_ui::application_settings_ui(CWnd* pParent, settings_model &model)
    : CDialogEx(IDD_APPLICATION_SETTINGS_UI, pParent)
    , settings_(&model)
{
}

application_settings_ui::~application_settings_ui() = default;

BOOL application_settings_ui::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    /* minimize on record */
    minimize_on_record_checkbox_.SetCheck(settings_->get_application_minimize_on_capture_start());
    /* auto generate filename */
    auto_filename_generation_checkbox_.SetCheck(settings_->get_application_auto_filename());

    /* output directory */
    for (const auto name : application_output_directory::names())
        output_directory_combobox_.AddString(name);
    output_directory_combobox_.SetCurSel(settings_->get_application_output_directory_type());

    _enable_output_directory_user_specified(settings_->get_application_output_directory_type());
    const auto output_directory = std::filesystem::path(settings_->get_application_output_directory());
    output_directory_user_specified_edit_.SetWindowText(output_directory.c_str());

    /* temp directory */
    for (const auto name : temp_output_directory::names())
        temp_directory_combobox_.AddString(name);
    temp_directory_combobox_.SetCurSel(settings_->get_application_temp_directory_type());

    _enable_temp_directory_user_specified(settings_->get_application_temp_directory_type());
    const auto temp_directory = std::filesystem::path(settings_->get_application_temp_directory());
    temp_directory_user_specified_edit_.SetWindowText(temp_directory.c_str());

    return TRUE;
}

void application_settings_ui::_enable_output_directory_user_specified(application_output_directory::type output_type)
{
    const auto is_enabled = output_type == application_output_directory::user_specified;

    output_directory_user_specified_edit_.EnableWindow(is_enabled);
    output_directory_user_specified_browse_button_.EnableWindow(is_enabled);
}

void application_settings_ui::_enable_temp_directory_user_specified(temp_output_directory::type temp_type)
{
    const auto is_enabled = temp_type == temp_output_directory::user_specified;

    temp_directory_user_specified_edit_.EnableWindow(is_enabled);
    temp_directory_user_specified_browse_button_.EnableWindow(is_enabled);
}

void application_settings_ui::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_APPLICATION_MINIMIZE_ON_RECORD, minimize_on_record_checkbox_);
    DDX_Control(pDX, IDC_AUTO_FILENAME_GENERATION, auto_filename_generation_checkbox_);
    DDX_Control(pDX, IDC_OUTPUT_DIRECTORY_CHECKBOX, output_directory_combobox_);
    DDX_Control(pDX, IDC_OUTPUT_DIRECTORY_USER_SPECIFIED_EDIT, output_directory_user_specified_edit_);
    DDX_Control(pDX, IDC_OUTPUT_DIRECTORY_USER_SPECIFIED_BROWSE_BUTTON, output_directory_user_specified_browse_button_);
    DDX_Control(pDX, IDC_TEMP_DIRECTORY_COMBOBOX, temp_directory_combobox_);
    DDX_Control(pDX, IDC_TEMP_DIRECTORY_USER_SPECIFIED_EDIT, temp_directory_user_specified_edit_);
    DDX_Control(pDX, IDC_TEMP_DIRECTORY_USER_SPECIFIED_BROWSE_BUTTON, temp_directory_user_specified_browse_button_);
}

BEGIN_MESSAGE_MAP(application_settings_ui, CDialogEx)
    ON_BN_CLICKED(IDC_APPLICATION_MINIMIZE_ON_RECORD, &application_settings_ui::OnBnClickedApplicationMinimizeOnRecord)
    ON_BN_CLICKED(IDC_AUTO_FILENAME_GENERATION, &application_settings_ui::OnBnClickedAutoFilenameGeneration)
    ON_CBN_SELCHANGE(IDC_OUTPUT_DIRECTORY_CHECKBOX, &application_settings_ui::OnCbnSelchangeOutputDirectoryCheckbox)
    ON_CBN_SELCHANGE(IDC_TEMP_DIRECTORY_COMBOBOX, &application_settings_ui::OnCbnSelchangeTempDirectoryCombobox)
    ON_BN_CLICKED(IDC_OUTPUT_DIRECTORY_USER_SPECIFIED_BROWSE_BUTTON, &application_settings_ui::OnBnClickedOutputDirectoryUserSpecifiedBrowseButton)
    ON_BN_CLICKED(IDC_TEMP_DIRECTORY_USER_SPECIFIED_BROWSE_BUTTON, &application_settings_ui::OnBnClickedTempDirectoryUserSpecifiedBrowseButton)
    ON_EN_CHANGE(IDC_OUTPUT_DIRECTORY_USER_SPECIFIED_EDIT, &application_settings_ui::OnEnChangeOutputDirectoryUserSpecifiedEdit)
    ON_EN_CHANGE(IDC_TEMP_DIRECTORY_USER_SPECIFIED_EDIT, &application_settings_ui::OnEnChangeTempDirectoryUserSpecifiedEdit)
END_MESSAGE_MAP()


void application_settings_ui::OnBnClickedApplicationMinimizeOnRecord()
{
    settings_->set_application_minimize_on_capture_start(minimize_on_record_checkbox_.GetCheck() != 0);
}

void application_settings_ui::OnBnClickedAutoFilenameGeneration()
{
    settings_->set_application_auto_filename(auto_filename_generation_checkbox_.GetCheck() != 0);
}

void application_settings_ui::OnCbnSelchangeOutputDirectoryCheckbox()
{
    const auto output_directory_type = static_cast<application_output_directory::type>(
        output_directory_combobox_.GetCurSel()
    );

    settings_->set_application_output_directory_type(output_directory_type);

    _enable_output_directory_user_specified(output_directory_type);
}

void application_settings_ui::OnCbnSelchangeTempDirectoryCombobox()
{
    const auto temp_directory_type = static_cast<temp_output_directory::type>(
        temp_directory_combobox_.GetCurSel()
    );

    settings_->set_application_temp_directory_type(temp_directory_type);
    _enable_temp_directory_user_specified(temp_directory_type);
}



void application_settings_ui::OnBnClickedOutputDirectoryUserSpecifiedBrowseButton()
{
    const auto output_directory = utility::utf8_to_wstring(
        settings_->get_application_output_directory());

    CFolderPickerDialog folder_picker(output_directory.c_str(), 0, this);
    if (folder_picker.DoModal() == IDOK)
    {
        // \note we abuse the fact that setting the window text, triggers the OnChange
        output_directory_user_specified_edit_.SetWindowText(folder_picker.GetPathName());
    }
}

void application_settings_ui::OnEnChangeOutputDirectoryUserSpecifiedEdit()
{
    std::filesystem::path new_output_directory_path = utility::get_window_text(
        output_directory_user_specified_edit_
    );

    settings_->set_application_output_directory(new_output_directory_path.generic_string());
}

void application_settings_ui::OnBnClickedTempDirectoryUserSpecifiedBrowseButton()
{
    const auto temp_directory = utility::utf8_to_wstring(
        settings_->get_application_temp_directory());

    CFolderPickerDialog folder_picker(temp_directory.c_str(), 0, this);
    if (folder_picker.DoModal() == IDOK)
    {
        // \note we abuse the fact that setting the window text, triggers the OnChange
        temp_directory_user_specified_edit_.SetWindowText(folder_picker.GetPathName());
    }
}

void application_settings_ui::OnEnChangeTempDirectoryUserSpecifiedEdit()
{
    std::filesystem::path new_temp_directory_path = utility::get_window_text(
        temp_directory_user_specified_edit_
    );

    settings_->set_application_temp_directory(new_temp_directory_path.generic_string());
}
