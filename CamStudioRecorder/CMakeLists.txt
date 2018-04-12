project(CamStudioRecorder)

set(RECORDER_SOURCE
    AudioFormat.cpp
    AudioMixer.cpp
    AudioSpeakers.cpp
    AutopanSpeed.cpp
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
    UpdateDialog.cpp
    VideoOptions.cpp
    VideoWnd.cpp
    AudioFormat.h
    AudioMixer.h
    AudioSpeakers.h
    AudioVolume.h
    AutopanSpeed.h
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
    MULDIV32.h
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
    UpdateDialog.h
    VideoOptions.h
    VideoWnd.h
    string_convert.h

    # todo move these into seperate source group
    vfw/ACM.cpp
    vfw/ACM.h
    vfw/VCM.cpp
    vfw/VCM.h

    # todo move these info seperate source group
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
    gdi32.lib
    winmm.lib
    vfw32.lib
    Gdiplus.lib
    strmiids.lib
    Msacm32.lib
)

# todo figure out how todo install
#install(TARGETS CamStudioPlayer
#    DIRECTORY include
#    DESTINATION include
#)