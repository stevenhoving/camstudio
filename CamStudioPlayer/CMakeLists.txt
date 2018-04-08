project(CamStudioPlayer)

set(PLAYER_SOURCE
    player.cpp
    player.h
    player.rc
    resource.h
)

add_executable(CamStudioPlayer WIN32
    ${PLAYER_SOURCE}
)

target_link_libraries(CamStudioPlayer
    CamLib
    gdi32.lib
    winmm.lib
    vfw32.lib
)

# todo figure out how todo install
#install(TARGETS CamStudioPlayer
#    DIRECTORY include
#    DESTINATION include
#)