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

project(raw C)

set(RAW_SOURCE
    dcr.c
    dcraw.c
    libdcr.c
    libdcr.h
)

source_group(raw FILES
    ${RAW_SOURCE}
)

add_library(raw STATIC
    ${RAW_SOURCE}
)

target_link_libraries(raw
    png_static
)

target_include_directories(raw
  PUBLIC
    .
)

install(
    DIRECTORY include
    DESTINATION include
)
