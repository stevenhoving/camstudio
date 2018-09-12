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

find_program(INNOSETUP_PATH
  NAMES
    iscc
    ISCC
  HINTS
    "C:/Program Files (x86)/Inno Setup 5"
    "C:/Program Files/Inno Setup 5"
)

if(NOT INNOSETUP_PATH)
    message(WARNING "Inno Setup Compiler not found.")
    set(INNOSETUP_COMPILER_FOUND off CACHE BOOL "" FORCE)
else()
    set(INNOSETUP_COMPILER_FOUND on CACHE BOOL "" FORCE)
endif()

