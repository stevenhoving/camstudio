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

cmake_minimum_required(VERSION 3.10)

project(CamStudio)

if(NOT DEFINED MSVC OR MSVC_VERSION LESS 1910)
  message(FATAL_ERROR "camstudio currently only builds with visual studio 2017 for now.")
endif()

set(CMAKE_CXX_STANDARD 17)

add_compile_options(/WX) # treat warnings as errors.
add_compile_options(/permissive-) # vs conformance mode.
add_compile_options(/Zc:twoPhase-) # 2 phase name lookup.

###############################################################################

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

###############################################################################

add_subdirectory(dep)



