# Copyright (c) 2012-2018 Robin Degen
#
# Permission is hereby granted, free of charge, to any person
# obtaining a copy of this software and associated documentation
# files (the "Software"), to deal in the Software without
# restriction, including without limitation the rights to use,
# copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the
# Software is furnished to do so, subject to the following
# conditions:
#
# The above copyright notice and this permission notice shall be
# included in all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
# EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
# OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
# NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
# HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
# WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
# FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
# OTHER DEALINGS IN THE SOFTWARE.

include(GitDescribe)

set(AEON_CMAKE_ROOT_DIR ${CMAKE_CURRENT_LIST_DIR})

function(generate_build_info_header copyright_file_path destination)
    get_git_describe_tag(git_describe_tag)

    file(READ ${copyright_file_path} AEON_CMAKE_COPYRIGHT)

    set(AEON_CMAKE_FULL_VERSION ${git_describe_tag})
    string(TIMESTAMP AEON_CMAKE_BUILD_DATE "%Y-%m-%d")

    configure_file(
        ${AEON_CMAKE_ROOT_DIR}/buildinfo.h.in
        ${destination}
        @ONLY
    )
endfunction()
