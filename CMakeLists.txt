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

set(GTEST_CATCH_EXCEPTIONS 0)

include(CTest)
enable_testing()

list(APPEND CMAKE_MODULE_PATH
  "${CMAKE_SOURCE_DIR}/dep/cmake"
  "${CMAKE_SOURCE_DIR}/dep/cmake_utility"
)

if(NOT DEFINED MSVC OR MSVC_VERSION LESS 1914)
  message(FATAL_ERROR "camstudio currently only builds with visual studio 2017 15.7 for now.")
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -D_DEBUG")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_DEBUG")

#add_compile_options(/WX) # treat warnings as errors.
add_compile_options(/permissive-) # vs conformance mode.
add_compile_options(/Zc:twoPhase-) # 2 phase name lookup.
add_compile_options(/Zc:rvalueCast)


###############################################################################

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_INSTALL_PREFIX
    ${CMAKE_BINARY_DIR}/lib
)

###############################################################################

add_subdirectory(dep)
add_subdirectory(CamCodec)
add_subdirectory(CamHook)
add_subdirectory(CamLib)
add_subdirectory(CamAudio)
add_subdirectory(CamCapture)
add_subdirectory(CamEncoder)
add_subdirectory(CamStudioCmd)
add_subdirectory(CamStudioRecorder)
