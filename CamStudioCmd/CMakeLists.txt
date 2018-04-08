project(CamStudioCmd)

add_executable(CamStudioCmd
    Commandline.cpp
    Commandline.h
)

target_link_libraries(CamStudioCmd
    CamLib
    gdi32.lib
    winmm.lib
    vfw32.lib
)
