#.rst:
# FindFFMPEG
# ----------
#
# Find the native FFMPEG includes and library
#
# This module defines::
#
#  FFMPEG_INCLUDE_DIR, where to find avcodec.h, avformat.h ...
#  FFMPEG_LIBRARIES, the libraries to link against to use FFMPEG.
#  FFMPEG_FOUND, If false, do not try to use FFMPEG.
#
# also defined, but not for general use are::
#
#   FFMPEG_avformat_LIBRARY, where to find the FFMPEG avformat library.
#   FFMPEG_avcodec_LIBRARY, where to find the FFMPEG avcodec library.
#
# This is useful to do it this way so that we can always add more libraries
# if needed to ``FFMPEG_LIBRARIES`` if ffmpeg ever changes...

#=============================================================================
# Copyright: 1993-2008 Ken Martin, Will Schroeder, Bill Lorensen
#
# Distributed under the OSI-approved BSD License (the "License");
# see accompanying file Copyright.txt for details.
#
# This software is distributed WITHOUT ANY WARRANTY; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# See the License for more information.
#=============================================================================
# (To distribute this file outside of YCM, substitute the full
#  License text for the above reference.)

# Originally from VTK project


find_path(FFMPEG_INCLUDE_DIR1 libavformat/avformat.h
  ${CMAKE_SOURCE_DIR}/dep/ffmpeg/include
)

find_path(FFMPEG_INCLUDE_DIR2 libavutil/avutil.h
  ${CMAKE_SOURCE_DIR}/dep/ffmpeg/include
)

find_path(FFMPEG_INCLUDE_DIR3 libavcodec/avcodec.h
  ${CMAKE_SOURCE_DIR}/dep/ffmpeg/include
)

find_path(FFMPEG_INCLUDE_DIR4 libswscale/swscale.h
  ${CMAKE_SOURCE_DIR}/dep/ffmpeg/include
)

find_path(FFMPEG_INCLUDE_DIR5 libavdevice/avdevice.h
  ${CMAKE_SOURCE_DIR}/dep/ffmpeg/include
)

if(FFMPEG_INCLUDE_DIR1)
  set(FFMPEG_INCLUDE_DIR
    ${FFMPEG_INCLUDE_DIR1}
  )
  
  set(FFMPEG_BIN_DIR
    ${CMAKE_SOURCE_DIR}/dep/ffmpeg/bin
  )
endif()

find_library(FFMPEG_avformat_LIBRARY avformat
  ${CMAKE_SOURCE_DIR}/dep/ffmpeg/lib
)

find_library(FFMPEG_avcodec_LIBRARY avcodec
  ${CMAKE_SOURCE_DIR}/dep/ffmpeg/lib
)

find_library(FFMPEG_avutil_LIBRARY avutil
  ${CMAKE_SOURCE_DIR}/dep/ffmpeg/lib
)

if(NOT DISABLE_SWSCALE)
  find_library(FFMPEG_swscale_LIBRARY swscale
    ${CMAKE_SOURCE_DIR}/dep/ffmpeg/lib
  )
endif(NOT DISABLE_SWSCALE)

find_library(FFMPEG_swresample_LIBRARY swresample
    ${CMAKE_SOURCE_DIR}/dep/ffmpeg/lib
)

find_library(FFMPEG_avdevice_LIBRARY avdevice
  ${CMAKE_SOURCE_DIR}/dep/ffmpeg/lib
)

if(FFMPEG_INCLUDE_DIR)
  if(FFMPEG_avformat_LIBRARY)
    if(FFMPEG_avcodec_LIBRARY)
      if(FFMPEG_avutil_LIBRARY)
        set(FFMPEG_FOUND "YES")
        set(FFMPEG_LIBRARIES ${FFMPEG_avformat_LIBRARY}
                             ${FFMPEG_avcodec_LIBRARY}
                             ${FFMPEG_avutil_LIBRARY}
          )
        if(FFMPEG_swscale_LIBRARY)
          set(FFMPEG_LIBRARIES ${FFMPEG_LIBRARIES}
                               ${FFMPEG_swscale_LIBRARY}
          )
        endif()
        if(FFMPEG_avdevice_LIBRARY)
          set(FFMPEG_LIBRARIES ${FFMPEG_LIBRARIES}
                               ${FFMPEG_avdevice_LIBRARY}
          )
        endif()
        if(FFMPEG_swresample_LIBRARY)
          set(FFMPEG_LIBRARIES ${FFMPEG_LIBRARIES}
                               ${FFMPEG_swresample_LIBRARY}
          )
        endif()
      endif()
    endif()
  endif()
endif()

mark_as_advanced(
  FFMPEG_INCLUDE_DIR
  FFMPEG_INCLUDE_DIR1
  FFMPEG_INCLUDE_DIR2
  FFMPEG_INCLUDE_DIR3
  FFMPEG_INCLUDE_DIR4
  FFMPEG_INCLUDE_DIR5
  FFMPEG_avformat_LIBRARY
  FFMPEG_avcodec_LIBRARY
  FFMPEG_avutil_LIBRARY
  FFMPEG_swscale_LIBRARY
  FFMPEG_avdevice_LIBRARY
)

# Set package properties if FeatureSummary was included
if(COMMAND set_package_properties)
  set_package_properties(FFMPEG PROPERTIES DESCRIPTION "A complete, cross-platform solution to record, convert and stream audio and video")
  set_package_properties(FFMPEG PROPERTIES URL "http://ffmpeg.org/")
endif()
