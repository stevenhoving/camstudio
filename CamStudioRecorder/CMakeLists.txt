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
    video_settings_ui.h
    video_settings_ui.cpp
    video_settings_model.h
    video_settings_model.cpp
    string_convert.h
    capture_thread.h
    capture_thread.cpp
    utility/filesystem.h
    utility/make_array.h
)

set(RECORDER_SOURCE
    # legacy
    AudioFormatDlg.cpp
    AudioFormatDlg.h
    AudioMixer.cpp
    AudioMixer.h
    AudioSpeakersDlg.cpp
    AudioSpeakersDlg.h
    AutopanSpeedDlg.cpp
    AutopanSpeedDlg.h
    AutoSearchDlg.cpp
    AutoSearchDlg.h
    BasicMessageDlg.cpp
    BasicMessageDlg.h
    CamCursor.cpp
    CamCursor.h
    CamStudioCommandLineInfo.cpp
    CursorOptionsDlg.cpp
    CursorOptionsDlg.h
    EditImageDlg.cpp
    EditImageDlg.h
    EditTransparencyDlg.cpp
    EditTransparencyDlg.h
    FixedRegionDlg.cpp
    FixedRegionDlg.h
    FlashingWnd.cpp
    FolderDlg.cpp
    FolderDlg.h
    GdiPlusInitializer.cpp
    GdiPlusInitializer.h
    HotKey.cpp
    KeyshortcutsDlg.cpp
    KeyshortcutsDlg.h
    LayeredWindowHelperST.cpp
    MainFrm.cpp
    MCI.cpp
    MouseCaptureWnd.cpp
    PresetTimeDlg.cpp
    PresetTimeDlg.h
    Profile.cpp
    ProgressDlg.cpp
    Recorder.cpp
    RecorderDoc.cpp
    RecorderView.cpp
    ResizeDlg.cpp
    Screen.cpp
    Screen.h
    SpeakersImpl.cpp
    stdafx.cpp
    stdafx.h
    SyncDlg.cpp
    SyncDlg.h
    TextDlg.cpp
    TextDlg.h
    TransparentWnd.cpp
    CamStudioCommandLineInfo.h
    FlashingWnd.h
    HotKey.h
    LayeredWindowHelperST.h
    MainFrm.h
    MCI.h
    MouseCaptureWnd.h
    Profile.h
    ProgressDlg.h
    Recorder.h
    RecorderDoc.h
    RecorderVersionReleaseInfo.h
    RecorderView.h
    ResizeDlg.h
    TransparentWnd.h
)

set(RECORDER_VFW_SOURCE
    vfw/ACM.cpp
    vfw/ACM.h
    vfw/VCM.cpp
    vfw/VCM.h
)

set(RECORDER_ADDONS_SOURCE
    addons/AnnotationEffectsOptionsDlg.cpp
    addons/AnnotationEffectsOptionsDlg.h
    addons/EffectsOptionsDlg.cpp
    addons/EffectsOptionsDlg.h
    addons/EffectsOptions2Dlg.cpp
    addons/EffectsOptions2Dlg.h
    addons/ImageAttributes.h
    addons/TextAttributes.h
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
    ${RECORDER_VFW_SOURCE}
    ${RECORDER_ADDONS_SOURCE}
)

source_group(vfw FILES
    ${RECORDER_VFW_SOURCE}
)

source_group(addons FILES
    ${RECORDER_ADDONS_SOURCE}
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
    -DNOMINMAX
    -D_AFXDLL=1
    -D_UNICODE
    -DUNICODE
    # silence codecvt deprecation warning for now
    -D_SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING
)

target_compile_options(CamStudioRecorder
  PRIVATE
    /experimental:external
    /external:W0
    /external:anglebrackets
)

set_target_properties(CamStudioRecorder PROPERTIES
    COMPILE_FLAGS /Yustdafx.h)

set_source_files_properties(stdafx.cpp PROPERTIES
    COMPILE_FLAGS /Ycstdafx.h)

target_link_libraries(CamStudioRecorder
    CamLib
    CamHook
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
    Gdiplus.lib
    strmiids.lib
    Msacm32.lib
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
