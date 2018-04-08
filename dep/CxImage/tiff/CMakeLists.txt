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

project(tiff C)

set(TIFF_SOURCE
    mkg3states.c
    tif_aux.c
    tif_close.c
    tif_codec.c
    tif_color.c
    tif_compress.c
    tif_dir.c
    tif_dirinfo.c
    tif_dirread.c
    tif_dirwrite.c
    tif_dumpmode.c
    tif_error.c
    tif_extension.c
    tif_fax3.c
    tif_fax3sm.c
    tif_flush.c
    tif_getimage.c
    tif_getimage_.c
    tif_jbig.c
    tif_jpeg.c
    tif_luv.c
    tif_lzw.c
    tif_next.c
    tif_ojpeg.c
    tif_open.c
    tif_packbits.c
    tif_pixarlog.c
    tif_predict.c
    tif_print.c
    tif_read.c
    tif_strip.c
    tif_swab.c
    tif_thunder.c
    tif_tile.c
    tif_version.c
    tif_warning.c
    tif_win32.c
    tif_write.c
    tif_zip.c
    t4.h
    tiff.h
    tiffconf.h
    tiffio.h
    tiffiop.h
    tiffvers.h
    tif_config.h
    tif_config.wince.h
    tif_dir.h
    tif_fax3.h
    tif_predict.h
    uvcode.h
)

source_group(tiff FILES
    ${TIFF_SOURCE}
)

add_library(tiff STATIC
    ${TIFF_SOURCE}
)

target_link_libraries(tiff
    zlibstatic
)

target_include_directories(tiff
  PUBLIC
    .
)

set_target_properties(tiff PROPERTIES FOLDER "CxImage")

install(
    DIRECTORY include
    DESTINATION include
)
