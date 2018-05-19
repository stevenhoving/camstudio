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

set(GTEST_CATCH_EXCEPTIONS 0)

include(Unittests)

add_unit_test_suite(
    TARGET test_cam_encoder
    SOURCES
        test_dict.cpp
        test_video_encoder.cpp
        test_muxer.cpp
        test_utilities.h
    INCLUDES
        ${CMAKE_CURRENT_BINARY_DIR}
    LIBRARIES CamEncoder
    FOLDER tests/CamEncoder
)

set_target_properties(test_cam_encoder PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/bin/$(Configuration)
)