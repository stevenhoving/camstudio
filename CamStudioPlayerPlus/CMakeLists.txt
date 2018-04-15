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

project(CamStudioPlayerPlus)

set(PLAYER_PLUS_SOURCE
    audio.h
    AudioFormat.h
    MainFrm.h
    MULDIV32.h
    playerplus.h
    playplus.h
    playplusDoc.h
    playplusView.h
    resource.h
    SlideToolBar.h
    StdAfx.h
    targetver.h
    audio.cpp
    AudioFormat.cpp
    MainFrm.cpp
    playplus.cpp
    playplusDoc.cpp
    playplusView.cpp
    SlideToolBar.cpp
    StdAfx.cpp
)

set(PLAYER_PLUS_RESOURCE
    playplus.rc
)

set(CMAKE_MFC_FLAG 2)

add_executable(CamStudioPlayerPlus WIN32
    ${PLAYER_PLUS_SOURCE}
    ${PLAYER_PLUS_RESOURCE}
)

target_compile_definitions(CamStudioPlayerPlus
  PRIVATE
    -DNOMINMAX
    -D_AFXDLL=1
    -D_UNICODE
    -DUNICODE
)

target_compile_options(CamStudioPlayerPlus
  PRIVATE
    /experimental:external
    /external:W0
    /external:anglebrackets
)

set_target_properties(CamStudioPlayerPlus PROPERTIES
  COMPILE_FLAGS "/Yustdafx.h")

set_source_files_properties(stdafx.cpp PROPERTIES
    COMPILE_FLAGS "/Ycstdafx.h")

target_link_libraries(CamStudioPlayerPlus
    CamLib
    gdi32.lib
    winmm.lib
    vfw32.lib
)

set_target_properties(CamStudioPlayerPlus PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

install(TARGETS CamStudioPlayerPlus
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)
