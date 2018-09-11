# Copyright (C) 2018  Steven Hoving
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.

project(CamStudioRecorder)

set(RECORDER_NEW_SOURCE
    application_settings_ui.h
    application_settings_ui.cpp
    background_capture_source.h
    background_capture_source.cpp
    fmt_helper.h
    mouse_capture_ui.h
    mouse_capture_ui.cpp
    cursor_settings_ui.h
    cursor_settings_ui.cpp
    cursor_settings_preview.h
    cursor_settings_preview.cpp
    shortcut_settings_ui.h
    shortcut_settings_ui.cpp
    shortcut_controller.h
    shortcut_controller.cpp
    video_settings_ui.h
    video_settings_ui.cpp
    video_settings_model.h
    video_settings_model.cpp
    window_select_data.h
    window_select_ui.h
    window_select_ui.cpp
    window_select_dwm.h
    window_select_dwm.cpp
    window_thumbnail_widget.h
    window_thumbnail_widget.cpp
    window_utils.h
    capture_thread.h
    capture_thread.cpp
    settings_model.h
    settings_model.cpp
    settings_table.h
)

set(RECORDER_UTILITY_SOURCE
    utility/filesystem.h
    utility/filesystem.cpp
    utility/make_array.h
    utility/string_convert.h
    utility/string_convert.cpp
)

set(RECORDER_SETTINGS_SOURCE
    settings/application_settings.h
    settings/video_settings.h
    settings/shortcut_settings.h
    settings/settings_enum_type.h
)

set(RECORDER_UI_SOURCE
    ui/listbox_grid_ui.cpp
    ui/listbox_grid_ui.h
    ui/listbox_base_ui.cpp
    ui/listbox_base_ui.h
)

# legacy
set(RECORDER_SOURCE
    AboutDlg.cpp
    AboutDlg.h
    AutopanSpeedDlg.cpp
    AutopanSpeedDlg.h
    BasicMessageDlg.cpp
    BasicMessageDlg.h
    FixedRegionDlg.cpp
    FixedRegionDlg.h
    GdiPlusInitializer.cpp
    GdiPlusInitializer.h
    MainFrm.cpp
    MainFrm.h
    Recorder.cpp
    Recorder.h
    RecorderDoc.cpp
    RecorderDoc.h
    RecorderView.cpp
    RecorderView.h
    stdafx.cpp
    stdafx.h
    RecorderVersionReleaseInfo.h
)

set(RECORDER_RESOURCE
    Recorder.rc
    resource.h
    targetver.h
)

set(CMAKE_MFC_FLAG 2)

add_executable(CamStudioRecorder WIN32
    ${RECORDER_NEW_SOURCE}
    ${RECORDER_SOURCE}
    ${RECORDER_SETTINGS_SOURCE}
    ${RECORDER_UTILITY_SOURCE}
    ${RECORDER_UI_SOURCE}
    ${RECORDER_RESOURCE}
)

source_group(src FILES
    ${RECORDER_NEW_SOURCE}
)

source_group(src\\settings FILES
    ${RECORDER_SETTINGS_SOURCE}
)

source_group(src\\utility FILES
    ${RECORDER_UTILITY_SOURCE}
)

source_group(src\\ui FILES
    ${RECORDER_UI_SOURCE}
)

source_group(src\\legacy FILES
    ${RECORDER_SOURCE}
)

source_group(resource FILES
    ${RECORDER_RESOURCE}
)

target_include_directories(CamStudioRecorder
  PUBLIC
    .
    ${FFMPEG_INCLUDE_DIR}
)

target_compile_definitions(CamStudioRecorder
  PRIVATE
    NOMINMAX
    _AFXDLL=1
    _UNICODE
    UNICODE
    # silence codecvt deprecation warning for now
    _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING
)

target_compile_options(CamStudioRecorder
  PRIVATE
    /experimental:external
    /external:W0
    /external:anglebrackets
    /W4
    /wd4505 # warning C4505: unreferenced local function has been removed
)

set_target_properties(CamStudioRecorder PROPERTIES
    COMPILE_FLAGS /Yustdafx.h)

set_source_files_properties(stdafx.cpp PROPERTIES
    COMPILE_FLAGS /Ycstdafx.h)

target_link_libraries(CamStudioRecorder
    CamLib
    cam_hook
    CamCapture
    CamEncoder
    cpptoml
    fmt
    gdi32.lib
    winmm.lib
    vfw32.lib
    gdiplus.lib
    strmiids.lib
    msacm32.lib
    dwmapi.lib
)

set_target_properties(CamStudioRecorder PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/bin/$(Configuration)
)

install(TARGETS CamStudioRecorder
    RUNTIME DESTINATION ${CMAKE_BINARY_DIR}/bin
    LIBRARY DESTINATION ${CMAKE_BINARY_DIR}/lib
    ARCHIVE DESTINATION ${CMAKE_BINARY_DIR}/lib
)
