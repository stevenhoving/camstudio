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

function(copy_to_target_config_path)

    cmake_parse_arguments(
        FUNCTION_ARGS
        ""
        "TARGET"
        "FILES"
        ${ARGN}
    )

    get_target_property(TARGET_RUNTIME_DIRECTORY ${FUNCTION_ARGS_TARGET} RUNTIME_OUTPUT_DIRECTORY)

    foreach (CONFIGURATION ${CMAKE_CONFIGURATION_TYPES})
        foreach (FILEPATH ${FUNCTION_ARGS_FILES})
            get_filename_component(FILENAME ${FILEPATH} NAME)
            configure_file("${FILEPATH}" "${TARGET_RUNTIME_DIRECTORY}/${CONFIGURATION}/${FILENAME}" COPYONLY)
        endforeach ()
    endforeach ()
endfunction()