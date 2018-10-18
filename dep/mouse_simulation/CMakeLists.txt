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

project(mouse_simulation)

set(LIB_SOURCE
    src/mouse_simulation.cpp
    src/main.cpp
)

set(LIB_INCLUDE
    include/mouse_simulation/mouse_simulation.h
)

source_group(mouse_simulation FILES
    ${LIB_SOURCE}
    ${LIB_INCLUDE}
)

add_executable(mouse_simulation
    ${LIB_SOURCE}
    ${LIB_INCLUDE}
)

target_link_libraries(mouse_simulation
  PRIVATE
    fmt
)

target_include_directories(mouse_simulation
  PUBLIC
    include
)

target_compile_options(mouse_simulation
  PRIVATE
    /experimental:external
    /external:W0
    /external:anglebrackets
)

set_target_properties(mouse_simulation PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

install(
    TARGETS mouse_simulation
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)
