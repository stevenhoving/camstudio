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

project(jbig C)

set(JBIG_SOURCE
    jbig.c
    jbig.h
    jbig_tab.c
)

source_group(jbig FILES
    ${JBIG_SOURCE}
)

add_library(jbig STATIC
    ${JBIG_SOURCE}
)

target_include_directories(jbig
  PUBLIC
    .
)

install(
    DIRECTORY include
    DESTINATION include
)
