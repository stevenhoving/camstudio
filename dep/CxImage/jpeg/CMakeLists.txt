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

project(jpeg C)

set(JPEG_SOURCE
    cdjpeg.c
    cjpeg.c
    ckconfig.c
    djpeg.c
    example.c
    jaricom.c
    jcapimin.c
    jcapistd.c
    jcarith.c
    jccoefct.c
    jccolor.c
    jcdctmgr.c
    jchuff.c
    jcinit.c
    jcmainct.c
    jcmarker.c
    jcmaster.c
    jcomapi.c
    jcparam.c
    jcprepct.c
    jcsample.c
    jctrans.c
    jdapimin.c
    jdapistd.c
    jdarith.c
    jdatadst.c
    jdatasrc.c
    jdcoefct.c
    jdcolor.c
    jddctmgr.c
    jdhuff.c
    jdinput.c
    jdmainct.c
    jdmarker.c
    jdmaster.c
    jdmerge.c
    jdpostct.c
    jdsample.c
    jdtrans.c
    jerror.c
    jfdctflt.c
    jfdctfst.c
    jfdctint.c
    jidctflt.c
    jidctfst.c
    jidctint.c
    jmemansi.c
    jmemmgr.c
    jmemname.c
    jmemnobs.c
    jpegtran.c
    jquant1.c
    jquant2.c
    jutils.c
    rdbmp.c
    rdcolmap.c
    rdgif.c
    rdjpgcom.c
    rdppm.c
    rdrle.c
    rdswitch.c
    rdtarga.c
    transupp.c
    wrbmp.c
    wrgif.c
    wrjpgcom.c
    wrppm.c
    wrrle.c
    wrtarga.c
    cderror.h
    cdjpeg.h
    jconfig.h
    jdct.h
    jerror.h
    jinclude.h
    jmemsys.h
    jmorecfg.h
    jpegint.h
    jpeglib.h
    jversion.h
    transupp.h
)

source_group(jpeg FILES
    ${JPEG_SOURCE}
)

add_library(jpeg STATIC
    ${JPEG_SOURCE}
)

target_include_directories(jpeg
  PUBLIC
    .
)

set_target_properties(jpeg PROPERTIES FOLDER "CxImage")

install(
    DIRECTORY include
    DESTINATION include
)
