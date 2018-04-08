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

project(jasper C)

set(JASPER_BASE_SOURCE
    base/jas_cm.c
    base/jas_debug.c
    base/jas_getopt.c
    base/jas_icc.c
    base/jas_iccdata.c
    base/jas_image.c
    base/jas_init.c
    base/jas_malloc.c
    base/jas_seq.c
    base/jas_stream.c
    base/jas_string.c
    base/jas_tmr.c
    base/jas_tvp.c
    base/jas_version.c
)

source_group(jasper\\base FILES
    ${JASPER_BASE_SOURCE}
)

set(BMP_SOURCE
    bmp/bmp_cod.c
    bmp/bmp_cod.h
    bmp/bmp_dec.c
    bmp/bmp_enc.c
    bmp/bmp_enc.h
)

source_group(jasper\\bmp FILES
    ${BMP_SOURCE}
)

set(JASPER_INCLUDE
    include/jasper/jasper.h
    include/jasper/jas_cm.h
    include/jasper/jas_config.h
    include/jasper/jas_debug.h
    include/jasper/jas_fix.h
    include/jasper/jas_getopt.h
    include/jasper/jas_icc.h
    include/jasper/jas_image.h
    include/jasper/jas_init.h
    include/jasper/jas_malloc.h
    include/jasper/jas_math.h
    include/jasper/jas_seq.h
    include/jasper/jas_stream.h
    include/jasper/jas_string.h
    include/jasper/jas_tmr.h
    include/jasper/jas_tvp.h
    include/jasper/jas_types.h
    include/jasper/jas_version.h
)

source_group(jasper\\include FILES
    ${JASPER_INCLUDE}
)

set(JP2_SOURCE
    jp2/jp2_cod.c
    jp2/jp2_cod.h
    jp2/jp2_dec.c
    jp2/jp2_dec.h
    jp2/jp2_enc.c
)

source_group(jasper\\jp2 FILES
    ${JP2_SOURCE}
)

set(JPC_SOURCE
    jpc/jpc_bs.c
    jpc/jpc_bs.h
    jpc/jpc_cod.h
    jpc/jpc_cs.c
    jpc/jpc_cs.h
    jpc/jpc_dec.c
    jpc/jpc_dec.h
    jpc/jpc_enc.c
    jpc/jpc_enc.h
    jpc/jpc_fix.h
    jpc/jpc_flt.h
    jpc/jpc_math.c
    jpc/jpc_math.h
    jpc/jpc_mct.c
    jpc/jpc_mct.h
    jpc/jpc_mqcod.c
    jpc/jpc_mqcod.h
    jpc/jpc_mqdec.c
    jpc/jpc_mqdec.h
    jpc/jpc_mqenc.c
    jpc/jpc_mqenc.h
    jpc/jpc_qmfb.c
    jpc/jpc_qmfb.h
    jpc/jpc_t1cod.c
    jpc/jpc_t1cod.h
    jpc/jpc_t1dec.c
    jpc/jpc_t1dec.h
    jpc/jpc_t1enc.c
    jpc/jpc_t1enc.h
    jpc/jpc_t2cod.c
    jpc/jpc_t2cod.h
    jpc/jpc_t2dec.c
    jpc/jpc_t2dec.h
    jpc/jpc_t2enc.c
    jpc/jpc_t2enc.h
    jpc/jpc_tagtree.c
    jpc/jpc_tagtree.h
    jpc/jpc_tsfb.c
    jpc/jpc_tsfb.h
    jpc/jpc_util.c
    jpc/jpc_util.h
)

source_group(jasper\\jpc FILES
    ${JPC_SOURCE}
)

# jpeg support is currently disabled
#set(JPG_SOURCE
#    jpg/jpg_cod.h
#    jpg/jpg_dec.c
#    jpg/jpg_dummy.c
#    jpg/jpg_enc.c
#    jpg/jpg_enc.h
#    jpg/jpg_jpeglib.h
#    jpg/jpg_val.c
#)
#
#source_group(jasper\\jpg FILES
#    ${JPG_SOURCE}
#)

set(MIF_SOURCE
    mif/mif_cod.c
    mif/mif_cod.h
)

source_group(jasper\\mif FILES
    ${MIF_SOURCE}
)

set(PGX_SOURCE
    pgx/pgx_cod.c
    pgx/pgx_cod.h
    pgx/pgx_dec.c
    pgx/pgx_enc.c
    pgx/pgx_enc.h
)

source_group(jasper\\pgx FILES
    ${PGX_SOURCE}
)

set(PNM_SOURCE
    pnm/pnm_cod.c
    pnm/pnm_cod.h
    pnm/pnm_dec.c
    pnm/pnm_enc.c
    pnm/pnm_enc.h
)

source_group(jasper\\pnm FILES
    ${PNM_SOURCE}
)

set(RAS_SOURCE
    ras/ras_cod.c
    ras/ras_cod.h
    ras/ras_dec.c
    ras/ras_enc.c
    ras/ras_enc.h
)

source_group(jasper\\ras FILES
    ${RAS_SOURCE}
)

add_library(jasper STATIC
    ${JASPER_BASE_SOURCE}
    ${JASPER_INCLUDE}
    ${BMP_SOURCE}
    ${JP2_SOURCE}
    ${JPC_SOURCE}
    #${JPG_SOURCE}
    ${MIF_SOURCE}
    ${PGX_SOURCE}
    ${PNM_SOURCE}
    ${RAS_SOURCE}
)

add_definitions(-DEXCLUDE_JPG_SUPPORT)

target_include_directories(jasper
  PUBLIC
    include/
)

set_target_properties(jasper PROPERTIES FOLDER "CxImage")

install(
    DIRECTORY include
    DESTINATION include
)
