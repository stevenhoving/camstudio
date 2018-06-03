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

add_subdirectory(zlib)

#set(SKIP_INSTALL_HEADERS ON CACHE BOOL "" FORCE)
add_subdirectory(libpng)
add_subdirectory(minilzo)
add_subdirectory(CxImage)
add_subdirectory(localization)
add_subdirectory(fmt)
add_subdirectory(googletest)
add_subdirectory(cpptoml)

# libconfig settings
set(BUILD_EXAMPLES OFF CACHE BOOL "Disable libconfig examples" FORCE)
set(BUILD_TESTS OFF CACHE BOOL "Disable libconfig examples" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
add_subdirectory(libconfig)

set_target_properties(libconfig libconfig++
  PROPERTIES
    FOLDER "External/libconfig"
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# fmt format library settings.
set_target_properties(fmt
  PROPERTIES
    FOLDER "External/fmt"
)

