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

project(CamCapture)

set(CAPTURE_SOURCE
    src/cam_capture.cpp
)

set(CAPTURE_INCLUDE
    include/CamCapture/cam_capture.h
    include/CamCapture/cam_rect.h
    include/CamCapture/cam_point.h
    include/CamCapture/cam_size.h
)

source_group(src FILES
    ${CAPTURE_SOURCE}
    ${CAPTURE_INCLUDE}
)

add_library(CamCapture STATIC
    ${CAPTURE_SOURCE}
    ${CAPTURE_INCLUDE}
)

target_include_directories(CamCapture
  PUBLIC
    include
)

target_compile_definitions(CamCapture
  PRIVATE
    -DNOMINMAX
    -D_UNICODE
    -DUNICODE
)

target_compile_options(CamCapture
  PRIVATE
    /experimental:external
    /external:W0
    /external:anglebrackets
)

target_link_libraries(CamCapture
  PUBLIC
    fmt
    CxImage
)

add_subdirectory(tests)
