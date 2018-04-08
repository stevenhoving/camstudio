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

set(CXIMAGE_SOURCE
    xfile.h
    ximabmp.h
    ximacfg.h
    ximadef.h
    ximage.h
    ximagif.h
    ximaico.h
    ximaiter.h
    ximajas.h
    ximajbg.h
    ximajpg.h
    ximamng.h
    ximapcx.h
    ximapng.h
    ximapsd.h
    ximaraw.h
    ximaska.h
    ximatga.h
    ximath.h
    ximatif.h
    ximawbmp.h
    ximawmf.h
    xiofile.h
    xmemfile.cpp
    xmemfile.h
    tif_xfile.cpp
    ximabmp.cpp
    ximadsp.cpp
    ximaenc.cpp
    ximaexif.cpp
    ximage.cpp
    ximagif.cpp
    ximahist.cpp
    ximaico.cpp
    ximainfo.cpp
    ximaint.cpp
    ximajas.cpp
    ximajbg.cpp
    ximajpg.cpp
    ximalpha.cpp
    ximalyr.cpp
    ximamng.cpp
    ximapal.cpp
    ximapcx.cpp
    ximapng.cpp
    ximapsd.cpp
    ximaraw.cpp
    ximasel.cpp
    ximaska.cpp
    ximatga.cpp
    ximath.cpp
    ximatif.cpp
    ximatran.cpp
    ximawbmp.cpp
    ximawmf.cpp
    ximawnd.cpp
)

source_group(CxImage FILES
    ${CXIMAGE_SOURCE}
)

add_library(CxImage STATIC
    ${CXIMAGE_SOURCE}
)

target_include_directories(CxImage
  PUBLIC
    .
)

target_link_libraries(CxImage
    zlibstatic
    jasper
    png_static
)

set_target_properties(CxImage PROPERTIES FOLDER "CxImage")

install(
    DIRECTORY include
    DESTINATION include
)
