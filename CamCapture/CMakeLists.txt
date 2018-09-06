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

set(ANNOTATIONS_SOURCE
    src/annotations/cam_annotation_cursor.cpp
    src/annotations/cam_annotation_systemtime.cpp
    include/CamCapture/annotations/cam_annotation_cursor.h
    include/CamCapture/annotations/cam_annotation_systemtime.h
)

set(CAPTURE_SOURCE
    src/cam_capture.cpp
)

set(CAPTURE_INCLUDE
    include/CamCapture/cam_annotarion.h
    include/CamCapture/cam_capture.h
    include/CamCapture/cam_color.h
    include/CamCapture/cam_draw_data.h
    include/CamCapture/cam_gdiplus.h
    include/CamCapture/cam_gdiplus_fwd.h
    include/CamCapture/cam_mouse_button.h
    include/CamCapture/cam_rect.h
    include/CamCapture/cam_point.h
    include/CamCapture/cam_size.h
    include/CamCapture/cam_stop_watch.h
)

source_group(src FILES
    ${CAPTURE_SOURCE}
    ${CAPTURE_INCLUDE}
)

source_group(src\\annotations FILES
    ${ANNOTATIONS_SOURCE}
)

add_library(CamCapture STATIC
    ${CAPTURE_SOURCE}
    ${CAPTURE_INCLUDE}
    ${ANNOTATIONS_SOURCE}
)

target_include_directories(CamCapture
  PUBLIC
    include
  PRIVATE
    include/CamCapture
)

target_compile_definitions(CamCapture
  PRIVATE
    -DNOMINMAX
    -D_UNICODE
    -DUNICODE
    #-DWIN32_LEAN_AND_MEAN
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
    cam_hook
)

add_subdirectory(tests)
