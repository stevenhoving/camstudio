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
    src/av_audio.cpp
    src/av_muxer.cpp
    src/av_video.cpp
    src/av_log.h
)

set(ENCODER_INCLUDE

    include/CamEncoder/av_audio.h
    include/CamEncoder/av_config.h
    include/CamEncoder/av_dict.h
    include/CamEncoder/av_error.h
    include/CamEncoder/av_muxer.h
    include/CamEncoder/av_icodec.h
    include/CamEncoder/av_video.h
    include/CamEncoder/av_ffmpeg.h
    include/CamEncoder/av_encoder.h
)

set(ENCODER_CAM_ENCODER_SOURCE
    src/av_cam_codec/av_cam_codec.cpp
)

set(ENCODER_CAM_ENCODER_INCLUDE
    include/CamEncoder/av_cam_codec/av_cam_codec.h
)

set(ENCODER_YUV_SOURCE
    src/av_yuv/av_rgb2yuv.cpp
    src/av_yuv/av_rgba2yuv.cpp
    src/av_yuv/av_rgb2yuv_ssse3.cpp
    src/av_yuv/av_rgba2yuv_ssse3.cpp
)

set(ENCODER_YUV_INCLUDE
    include/CamEncoder/av_yuv/av_rgb2yuv.h
    include/CamEncoder/av_yuv/av_rgba2yuv.h
    include/CamEncoder/av_yuv/av_rgb2yuv_ssse3.h
    include/CamEncoder/av_yuv/av_rgba2yuv_ssse3.h
    include/CamEncoder/av_yuv/av_yuv_pixel_type.h
    include/CamEncoder/av_yuv/av_simd_utility.h
    include/CamEncoder/av_yuv/av_simd_debug.h
    include/CamEncoder/av_yuv/av_simd_common.h
    include/CamEncoder/av_yuv/av_simd_vec.h
)

source_group(src FILES
    ${ENCODER_SOURCE}
    ${ENCODER_INCLUDE}
)

source_group(src\\cam_codec FILES
    ${ENCODER_CAM_ENCODER_SOURCE}
    ${ENCODER_CAM_ENCODER_INCLUDE}
)

source_group(src\\av_yuv FILES
    ${ENCODER_YUV_SOURCE}
    ${ENCODER_YUV_INCLUDE}
)

add_library(CamEncoder STATIC
    ${ENCODER_SOURCE}
    ${ENCODER_INCLUDE}
    ${ENCODER_CAM_ENCODER_SOURCE}
    ${ENCODER_CAM_ENCODER_INCLUDE}
    ${ENCODER_YUV_SOURCE}
    ${ENCODER_YUV_INCLUDE}
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
    libminilzo
    zlibstatic
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
