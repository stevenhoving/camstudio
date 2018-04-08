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

project(CamAudio)

set(AUDIO_SOURCE
    src/buffer.cpp
    src/sound_base.cpp
    src/sound_file.cpp
    src/sound_in.cpp
    src/sound_out.cpp
    src/stdafx.cpp
    src/stdafx.h
    src/targetver.h
)

set(AUDIO_INCLUDE
    include/CamAudio/buffer.h
    include/CamAudio/sound_base.h
    include/CamAudio/sound_file.h
    include/CamAudio/sound_in.h
    include/CamAudio/sound_out.h
)

source_group(CamAudio FILES
    ${AUDIO_SOURCE}
    ${AUDIO_INCLUDE}
)

set(CMAKE_MFC_FLAG 1)

add_library(CamAudio STATIC
    ${AUDIO_SOURCE}
    ${AUDIO_INCLUDE}
)

target_compile_definitions(CamAudio
  PRIVATE
    -D_AFXDLL
)

target_include_directories(CamAudio
  PUBLIC
    include
)

target_link_libraries(CamAudio
    CamLib
)

install(
    DIRECTORY include
    DESTINATION include
)
