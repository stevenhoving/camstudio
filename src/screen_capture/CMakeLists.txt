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

project(screen_capture)

set(ANNOTATIONS_SOURCE
    src/annotations/cam_annotation_cursor.cpp
    src/annotations/cam_annotation_systemtime.cpp
    include/screen_capture/annotations/cam_annotation_cursor.h
    include/screen_capture/annotations/cam_annotation_systemtime.h
)

set(CAPTURE_SOURCE
    src/cam_capture.cpp
    src/cam_virtual_screen_info.cpp
)

set(CAPTURE_INCLUDE
    include/screen_capture/cam_annotarion.h
    include/screen_capture/cam_capture.h
    include/screen_capture/cam_color.h
    include/screen_capture/cam_draw_data.h
    include/screen_capture/cam_gdiplus.h
    include/screen_capture/cam_gdiplus_fwd.h
    include/screen_capture/cam_mouse_button.h
    include/screen_capture/cam_rect.h
    include/screen_capture/cam_point.h
    include/screen_capture/cam_size.h
    include/screen_capture/cam_stop_watch.h
    include/screen_capture/cam_virtual_screen_info.h
)

source_group(src FILES
    ${CAPTURE_SOURCE}
    ${CAPTURE_INCLUDE}
)

source_group(src\\annotations FILES
    ${ANNOTATIONS_SOURCE}
)

add_library(screen_capture STATIC
    ${CAPTURE_SOURCE}
    ${CAPTURE_INCLUDE}
    ${ANNOTATIONS_SOURCE}
)

target_include_directories(screen_capture
  PUBLIC
    include
  PRIVATE
    include/screen_capture
)

target_compile_definitions(screen_capture
  PRIVATE
    -DNOMINMAX
    -D_UNICODE
    -DUNICODE
    #-DWIN32_LEAN_AND_MEAN
)

target_compile_options(screen_capture
  PRIVATE
    /experimental:external
    /external:W0
    /external:anglebrackets
)

target_link_libraries(screen_capture
  PUBLIC
    fmt
    cam_hook
	screen_capture_desktop_duplication
)

add_subdirectory(tests)
