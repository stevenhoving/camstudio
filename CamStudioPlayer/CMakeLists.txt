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

target_compile_definitions(CamStudioPlayer
  PRIVATE
    _UNICODE
    UNICODE
)

target_link_libraries(CamStudioPlayer
    CamLib
    gdi32.lib
    winmm.lib
    vfw32.lib
)

set_target_properties(CamStudioPlayer PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

install(TARGETS CamStudioPlayer
    RUNTIME DESTINATION ${CMAKE_BINARY_DIR}/bin
    LIBRARY DESTINATION ${CMAKE_BINARY_DIR}/lib
    ARCHIVE DESTINATION ${CMAKE_BINARY_DIR}/lib
)
