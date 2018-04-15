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

project(mng C)

set(MNG_SOURCE
    libmng_callback_xs.c
    libmng_chunk_descr.c
    libmng_chunk_io.c
    libmng_chunk_prc.c
    libmng_chunk_xs.c
    libmng_cms.c
    libmng_display.c
    libmng_dither.c
    libmng_error.c
    libmng_filter.c
    libmng_hlapi.c
    libmng_jpeg.c
    libmng_object_prc.c
    libmng_pixels.c
    libmng_prop_xs.c
    libmng_read.c
    libmng_trace.c
    libmng_write.c
    libmng_zlib.c
    libmng.h
    libmng_chunks.h
    libmng_chunk_descr.h
    libmng_chunk_io.h
    libmng_chunk_prc.h
    libmng_cms.h
    libmng_conf.h
    libmng_data.h
    libmng_display.h
    libmng_dither.h
    libmng_error.h
    libmng_filter.h
    libmng_jpeg.h
    libmng_memory.h
    libmng_objects.h
    libmng_object_prc.h
    libmng_pixels.h
    libmng_read.h
    libmng_trace.h
    libmng_types.h
    libmng_write.h
    libmng_zlib.h
)

source_group(mng FILES
    ${MNG_SOURCE}
)

add_library(mng STATIC
    ${MNG_SOURCE}
)

target_compile_definitions(mng
  PUBLIC
    -DMNG_SUPPORT_DISPLAY
    -DMNG_SUPPORT_READ
    -DMNG_SUPPORT_WRITE
    -DMNG_ACCESS_CHUNKS
    -DMNG_STORE_CHUNKS
)

target_link_libraries(mng
    zlibstatic
)

target_include_directories(mng
  PUBLIC
    .
)

set_target_properties(mng PROPERTIES FOLDER "CxImage")

install(
    DIRECTORY include
    DESTINATION include
)
