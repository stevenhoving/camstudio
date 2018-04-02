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

set(LIB_MINI_LZO_SOURCE
    minilzo/lzoconf.h
    minilzo/lzodefs.h
    minilzo/minilzo.c
    minilzo/minilzo.h
)

source_group(libminilzo FILES
    ${LIB_MINI_LZO_SOURCE}
)

add_library(libminilzo STATIC
    ${LIB_MINI_LZO_SOURCE}
)

target_include_directories(libminilzo
  PUBLIC
    minilzo
)

install(
    DIRECTORY include
    DESTINATION include
)
