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

project(logging)

set(LOGGING_SOURCE
    src/logging.cpp
)

set(LOGGING_INCLUDE
    include/logging/logging.h
)

source_group(src FILES
    ${LOGGING_SOURCE}
    ${LOGGING_INCLUDE}
)

add_library(logging STATIC
    ${LOGGING_SOURCE}
    ${LOGGING_INCLUDE}
)

target_link_libraries(logging
  PUBLIC
    spdlog
    fmt
)

target_include_directories(logging
  PUBLIC
    include
)


#target_compile_definitions(CamLib
#  PRIVATE
#    _UNICODE
#    UNICODE
#    _AFXDLL
#)


#add_subdirectory(tests)
