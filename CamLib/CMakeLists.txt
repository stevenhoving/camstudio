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

project(CamLib)

set(LIB_SOURCE
    CamError.cpp
    CamFile.cpp
    CamImage.cpp
    CamRect.cpp
    CamSys.cpp
    CBitmapEx.cpp
    CStudiolib.cpp
    Picture.cpp
    TrayIcon.cpp
    targetver.h
    stdafx.h
    stdafx.cpp
)

set(LIB_INCLUDE
    include/CamLib/CamColor.h
    include/CamLib/CamError.h
    include/CamLib/CamFile.h
    include/CamLib/CamImage.h
    include/CamLib/CamRect.h
    include/CamLib/CamSys.h
    include/CamLib/CBitmapEx.h
    include/CamLib/CStudioLib.h
    include/CamLib/Picture.h
    include/CamLib/TrayIcon.h
)

source_group(CamLib FILES
    ${LIB_SOURCE}
    ${LIB_INCLUDE}
)

set(CMAKE_MFC_FLAG 1)

add_library(CamLib STATIC
    ${LIB_SOURCE}
    ${LIB_INCLUDE}
)

target_compile_definitions(CamLib
  PRIVATE
    -D_AFXDLL
)

target_include_directories(CamLib
  PUBLIC
    include
)

install(
    DIRECTORY include
    DESTINATION include
)
