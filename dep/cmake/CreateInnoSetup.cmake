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

include(GitDescribe)

function(create_inno_setup)

    get_git_describe_tag(git_describe_tag)

    #message(STATUS "Using Inno Setup Compiler from: ${INNOSETUP_PATH}")
    #message(STATUS "install: ${CMAKE_INSTALL_PREFIX}")
    #message(STATUS "current source dir: ${CMAKE_CURRENT_SOURCE_DIR}")

    #file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/setup/" DESTINATION "${CMAKE_INSTALL_PREFIX}/bin")

    configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/setup/setup.iss"
        "${CMAKE_INSTALL_PREFIX}/bin/setup.iss"
    )

    add_custom_target(CreateInstaller
                      COMMAND ${INNOSETUP_PATH} "${CMAKE_INSTALL_PREFIX}/bin/setup.iss" /FCamStudio-${git_describe_tag}
                      DEPENDS INSTALL
                      VERBATIM)

endfunction()