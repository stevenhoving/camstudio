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

if (NOT TARGET gtest)
    message(FATAL_ERROR "Unittests.cmake requires google test included as submodule.")
endif ()

include(CMakeParseArguments)

function(add_unit_test_suite)
    cmake_parse_arguments(
        UNIT_TEST_PARSED_ARGS
        "NO_GTEST_MAIN"
        "TARGET;FOLDER"
        "SOURCES;LIBRARIES;INCLUDES;LABELS"
        ${ARGN}
    )

    if (NOT UNIT_TEST_PARSED_ARGS_TARGET)
        message(FATAL_ERROR "No target name was given for unit test.")
    endif ()

    if (NOT UNIT_TEST_PARSED_ARGS_SOURCES)
        message(FATAL_ERROR "No sources were given for unit test.")
    endif ()

    foreach(_src ${UNIT_TEST_PARSED_ARGS_SOURCES})
        list (APPEND SRCS "${UNIT_TEST_PARSED_ARGS_TARGET}/${_src}")
    endforeach()

    add_executable(${UNIT_TEST_PARSED_ARGS_TARGET} ${SRCS})

    if (UNIT_TEST_PARSED_ARGS_FOLDER)
        set_target_properties(
            ${UNIT_TEST_PARSED_ARGS_TARGET} PROPERTIES
            FOLDER ${UNIT_TEST_PARSED_ARGS_FOLDER}
            RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
        )
    endif ()

    if (UNIT_TEST_PARSED_ARGS_INCLUDES)
        target_include_directories(${UNIT_TEST_PARSED_ARGS_TARGET} PRIVATE ${UNIT_TEST_PARSED_ARGS_INCLUDES})
    endif ()

    target_link_libraries(
        ${UNIT_TEST_PARSED_ARGS_TARGET}
        gtest
        gmock
    )

    if (NOT ${UNIT_TEST_PARSED_ARGS_NO_GTEST_MAIN})
        target_link_libraries(${UNIT_TEST_PARSED_ARGS_TARGET} gtest_main)
    endif ()

    if (UNIT_TEST_PARSED_ARGS_LIBRARIES)
        target_link_libraries(
            ${UNIT_TEST_PARSED_ARGS_TARGET}
            ${UNIT_TEST_PARSED_ARGS_LIBRARIES}
        )
    endif ()

    add_test(
        NAME ${UNIT_TEST_PARSED_ARGS_TARGET}
        WORKING_DIRECTORY $<TARGET_FILE_DIR:${UNIT_TEST_PARSED_ARGS_TARGET}>
        COMMAND $<TARGET_FILE:${UNIT_TEST_PARSED_ARGS_TARGET}> "--gtest_output=xml:${UNIT_TEST_PARSED_ARGS_TARGET}.xml"
    )

    list(APPEND UNIT_TEST_PARSED_ARGS_LABELS "unittest")
    set_tests_properties(${UNIT_TEST_PARSED_ARGS_TARGET} PROPERTIES LABELS "${UNIT_TEST_PARSED_ARGS_LABELS}")
endfunction()
