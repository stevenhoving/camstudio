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

set(RECORDER_SOURCE
    AviWriter.cpp
    AviWriter.h
    AudioFormatDlg.cpp
    AudioFormatDlg.h
    AudioMixer.cpp
    AudioMixer.h
    AudioSpeakersDlg.cpp
    AudioSpeakersDlg.h
    AutopanSpeedDlg.cpp
    AutopanSpeedDlg.h
    AutoSearchDialog.cpp
    AVI.cpp
    AVICompressorFilter.cpp
    BasicMessage.cpp
    CamCursor.cpp
    CamStudioCommandLineInfo.cpp
    CursorOptionsDlg.cpp
    EditImage.cpp
    EditTransparency.cpp
    FixedRegion.cpp
    FlashingWnd.cpp
    FolderDialog.cpp
    FrameGrabber.cpp
    GdiPlusInitializer.cpp
    HotKey.cpp
    Keyshortcuts.cpp
    LayeredWindowHelperST.cpp
    LayoutList.cpp
    ListManager.cpp
    MainFrm.cpp
    MCI.cpp
    MouseCaptureWnd.cpp
    MP4Converter.cpp
    NewShape.cpp
    PresetTime.cpp
    Profile.cpp
    ProgressDlg.cpp
    Recorder.cpp
    RecorderDoc.cpp
    RecorderView.cpp
    ResizeDialog.cpp
    Screen.cpp
    ScreenAnnotations.cpp
    SpeakersImpl.cpp
    StdAfx.cpp
    SyncDialog.cpp
    TextDialog.cpp
    TransparentWnd.cpp
    TransRateDialog.cpp
    TroubleShoot.cpp
    VideoOptions.cpp
    VideoWnd.cpp
    AutoSearchDialog.h
    AVI.h
    AVICompressorFilter.h
    BasicMessage.h
    CamCursor.h
    CamStudioCommandLineInfo.h
    CursorOptionsDlg.h
    EditImage.h
    EditTransparency.h
    FixedRegion.h
    FlashingWnd.h
    FolderDialog.h
    FrameGrabber.h
    GdiPlusInitializer.h
    HotKey.h
    Keyshortcuts.h
    LayeredWindowHelperST.h
    LayoutList.h
    ListManager.h
    MainFrm.h
    MCI.h
    MouseCaptureWnd.h
    MP4Converter.h
    NewShape.h
    PresetTime.h
    Profile.h
    ProgressDlg.h
    Recorder.h
    RecorderDoc.h
    RecorderVersionReleaseInfo.h
    RecorderView.h
    ResizeDialog.h
    resource.h
    Screen.h
    ScreenAnnotations.h
    StdAfx.h
    SyncDialog.h
    targetver.h
    TextDialog.h
    TransparentWnd.h
    TransRateDialog.h
    TroubleShoot.h
    VideoOptions.h
    VideoWnd.h
    string_convert.h
)

set(RECORDER_VFW_SOURCE
    vfw/ACM.cpp
    vfw/ACM.h
    vfw/VCM.cpp
    vfw/VCM.h
)

set(RECORDER_ADDONS_SOURCE
    addons/AnnotationEffectsOptions.cpp
    addons/AnnotationEffectsOptions.h
    addons/EffectsOptions.cpp
    addons/EffectsOptions.h
    addons/EffectsOptions2.cpp
    addons/EffectsOptions2.h
    addons/ImageAttributes.h
    addons/TextAttributes.h
)

set(RECORDER_RESOURCE
    Recorder.rc
)

set(CMAKE_MFC_FLAG 2)

add_executable(CamStudioRecorder WIN32
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
    ${RECORDER_SOURCE}
    ${RECORDER_RESOURCE}
)

target_include_directories(CamStudioRecorder
  PUBLIC
    .
    ${libconfig_SOURCE_DIR}/lib
)

target_compile_definitions(CamStudioRecorder
  PRIVATE
    -DNOMINMAX
    -D_AFXDLL=1
    -D_UNICODE
    -DUNICODE
    # silence codecvt deprication warning for now
    -D_SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING
)

target_compile_options(CamStudioRecorder
  PRIVATE
    /experimental:external
    /external:W0
    /external:anglebrackets
)

set_target_properties(CamStudioRecorder PROPERTIES
  COMPILE_FLAGS "/Yustdafx.h")

set_source_files_properties(stdafx.cpp PROPERTIES
    COMPILE_FLAGS "/Ycstdafx.h")

target_link_libraries(CamStudioRecorder
    CamLib
    CamHook
    CamAudio
    libconfig++
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
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)
