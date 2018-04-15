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

set(LOCALIZATION_SOURCE
    Localization.cpp
    include/localization/Localization.h
)

source_group(localization FILES
    ${LOCALIZATION_SOURCE}
)

add_library(localization STATIC
    ${LOCALIZATION_SOURCE}
)

target_include_directories(localization
  PUBLIC
    .
    include
  PRIVATE
    include/localization
)

set_target_properties(localization PROPERTIES FOLDER "External/localization")

install(
    DIRECTORY include
    DESTINATION include
)
