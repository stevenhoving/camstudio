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

project(cam_hook)

set(SOURCE
    src/main.cpp
    src/cam_hook.cpp
)

set(INCLUDE
    include/cam_hook/cam_hook.h
    include/cam_hook/cam_hook_export.h
    include/cam_hook/cam_common.h
)

source_group(src FILES
    ${SOURCE}
    ${INCLUDE}
)

add_library(cam_hook SHARED
    ${SOURCE}
    ${INCLUDE}
)

target_compile_definitions(cam_hook
  PRIVATE
    NOMINMAX
    _UNICODE
    UNICODE
    CAM_HOOK_EXPORT
)

target_include_directories(cam_hook
  PUBLIC
    include
)

target_link_libraries(cam_hook
    fmt
)

set_target_properties(cam_hook PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

install(TARGETS cam_hook
    RUNTIME DESTINATION ${CMAKE_BINARY_DIR}/bin
    LIBRARY DESTINATION ${CMAKE_BINARY_DIR}/lib
    ARCHIVE DESTINATION ${CMAKE_BINARY_DIR}/lib
)
