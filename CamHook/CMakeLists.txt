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

project(CamHook)

set(HOOK_SOURCE
    CamHook.cpp
    CamHook.rc
    ClickQueue.cpp
    resource.h
    stdafx.h
    stdafx.cpp
)

set(HOOK_INCLUDE
    include/CamHook/CamHook.h
    include/CamHook/ClickQueue.h
)

source_group(CamHook FILES
    ${HOOK_SOURCE}
    ${HOOK_INCLUDE}
)

set(CMAKE_MFC_FLAG 2)

add_library(CamHook SHARED
    ${HOOK_SOURCE}
    ${HOOK_INCLUDE}
)


target_compile_definitions(CamHook
  PRIVATE
    -D_AFXDLL
    -DCAM_HOOK_EXPORT
    -D_UNICODE
    -DUNICODE
)

target_include_directories(CamHook
  PUBLIC
    include
)

install(
    DIRECTORY include
    DESTINATION include
)
