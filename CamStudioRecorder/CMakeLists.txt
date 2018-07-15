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
    # new
    application_settings_ui.h
    application_settings_ui.cpp
    background_capture_source.h
    background_capture_source.cpp
    fmt_helper.h
    mouse_capture_ui.h
    mouse_capture_ui.cpp
    cursor_settings_ui.h
    cursor_settings_ui.cpp
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
    string_convert.h
    capture_thread.h
    capture_thread.cpp
    settings_model.h
    settings_model.cpp
    settings_table.h
    utility/filesystem.h
    utility/make_array.h
)

set(RECORDER_SOURCE
    # legacy
    AutopanSpeedDlg.cpp
    AutopanSpeedDlg.h
    BasicMessageDlg.cpp
    BasicMessageDlg.h
    CamStudioCommandLineInfo.cpp
    FixedRegionDlg.cpp
    FixedRegionDlg.h
    GdiPlusInitializer.cpp
    GdiPlusInitializer.h
    HotKey.cpp
    KeyshortcutsDlg.cpp
    KeyshortcutsDlg.h
    MainFrm.cpp
    Recorder.cpp
    RecorderDoc.cpp
    RecorderView.cpp
    stdafx.cpp
    stdafx.h
    TextDlg.cpp
    TextDlg.h
    CamStudioCommandLineInfo.h
    HotKey.h
    MainFrm.h
    Recorder.h
    RecorderDoc.h
    RecorderVersionReleaseInfo.h
    RecorderView.h
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
    ${RECORDER_RESOURCE}
)

source_group(src FILES
    ${RECORDER_NEW_SOURCE}
)

source_group(resource FILES
    ${RECORDER_RESOURCE}
)

source_group(src\\legacy FILES
    ${RECORDER_SOURCE}
)

target_include_directories(CamStudioRecorder
  PUBLIC
    .
    ${libconfig_SOURCE_DIR}/lib
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
)

set_target_properties(CamStudioRecorder PROPERTIES
    COMPILE_FLAGS /Yustdafx.h)

set_source_files_properties(stdafx.cpp PROPERTIES
    COMPILE_FLAGS /Ycstdafx.h)

target_link_libraries(CamStudioRecorder
    CamLib
    CamHook
    cam_hook2
    CamAudio
    CamCapture
    CamEncoder
    libconfig++
    cpptoml
    CxImage
    localization
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
