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

project(CamEncoder)

find_package(ffmpeg)

set(ENCODER_SOURCE
    src/avi_writer.cpp
    src/av_audio.cpp
    src/av_muxer.cpp
    src/av_video.cpp
    src/video_writer.cpp
)

set(ENCODER_INCLUDE
    include/CamEncoder/avi_writer.h
    include/CamEncoder/avi_config.h
    include/CamEncoder/av_audio.h
    include/CamEncoder/av_config.h
    include/CamEncoder/av_muxer.h
    include/CamEncoder/av_video.h
    include/CamEncoder/video_writer.h
    include/CamEncoder/av_ffmpeg.h
)

source_group(src FILES
    ${ENCODER_SOURCE}
    ${ENCODER_INCLUDE}
)

add_library(CamEncoder STATIC
    ${ENCODER_SOURCE}
    ${ENCODER_INCLUDE}
)

target_include_directories(CamEncoder
  PUBLIC
    include
    ${FFMPEG_INCLUDE_DIR}
)

target_compile_definitions(CamEncoder
  PRIVATE
    -DNOMINMAX
    -D_UNICODE
    -DUNICODE
)

target_compile_options(CamEncoder
  PRIVATE
    /experimental:external
    /external:W0
    /external:anglebrackets
)

target_link_libraries(CamEncoder
  PUBLIC
    fmt
    ${FFMPEG_LIBRARIES}
)

#install(
#    DIRECTORY include
#    DESTINATION include
#)

# Really hacky way to make sure we deploy the ffmpeg binaries.
#set(FFMPEG_BIN_DIR
#    "${FFMPEG_INCLUDE_DIR}/../bin"
#)

#install(
#  FILES
#    "${FFMPEG_BIN_DIR}/avcodec-58.dll"
#    "${FFMPEG_BIN_DIR}/avdevice-58.dll"
#    "${FFMPEG_BIN_DIR}/avfilter-7.dll"
#    "${FFMPEG_BIN_DIR}/avformat-58.dll"
#    "${FFMPEG_BIN_DIR}/avutil-56.dll"
#    "${FFMPEG_BIN_DIR}/postproc-55.dll"
#    "${FFMPEG_BIN_DIR}/swresample-3.dll"
#    "${FFMPEG_BIN_DIR}/swscale-5.dll"
#  DESTINATION
#    ${CMAKE_BINARY_DIR}/bin
#)

#SET(SRC_FILES head1.h head2.h head3.h)
#COPY_IF_DIFFERENT( /from_dir /to_dir ${SRC_FILES} IncludeTargets "Includes")
#ADD_TARGET(CopyIncludes ALL DEPENDS ${IncludeTargets})

add_custom_command(TARGET CamEncoder POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${FFMPEG_BIN_DIR}/avcodec-58.dll"
    $<TARGET_FILE_DIR:CamEncoder>/avcodec-58.dll
    "${FFMPEG_BIN_DIR}/avdevice-58.dll"
    $<TARGET_FILE_DIR:CamEncoder>
    "${FFMPEG_BIN_DIR}/avfilter-7.dll"
    $<TARGET_FILE_DIR:CamEncoder>
    "${FFMPEG_BIN_DIR}/avformat-58.dll"
    $<TARGET_FILE_DIR:CamEncoder>
    "${FFMPEG_BIN_DIR}/avutil-56.dll"
    $<TARGET_FILE_DIR:CamEncoder>
    "${FFMPEG_BIN_DIR}/postproc-55.dll"
    $<TARGET_FILE_DIR:CamEncoder>
    "${FFMPEG_BIN_DIR}/swresample-3.dll"
    $<TARGET_FILE_DIR:CamEncoder>
    "${FFMPEG_BIN_DIR}/swscale-5.dll"
    $<TARGET_FILE_DIR:CamEncoder>
)

add_subdirectory(tests)
