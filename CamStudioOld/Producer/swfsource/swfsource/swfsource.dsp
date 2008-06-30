# Microsoft Developer Studio Project File - Name="swfsource" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=swfsource - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "swfsource.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "swfsource.mak" CFG="swfsource - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "swfsource - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "swfsource - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "swfsource - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "swfsource - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "swfsource - Win32 Release"
# Name "swfsource - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\actioncompiler\assembler.cpp
# End Source File
# Begin Source File

SOURCE=..\actioncompiler\common.cpp
# End Source File
# Begin Source File

SOURCE=..\actioncompiler\compile.cpp
# End Source File
# Begin Source File

SOURCE=..\actioncompiler\compileaction.cpp
# End Source File
# Begin Source File

SOURCE=..\compress.c
# End Source File
# Begin Source File

SOURCE=..\crc32.c
# End Source File
# Begin Source File

SOURCE=..\deflate.c
# End Source File
# Begin Source File

SOURCE=..\FAction.cpp
# End Source File
# Begin Source File

SOURCE=..\FBase.cpp
# End Source File
# Begin Source File

SOURCE=..\FBitmap.cpp
# End Source File
# Begin Source File

SOURCE=..\FButton.cpp
# End Source File
# Begin Source File

SOURCE=..\FControl.cpp
# End Source File
# Begin Source File

SOURCE=..\FDisplay.cpp
# End Source File
# Begin Source File

SOURCE=..\FFont.cpp
# End Source File
# Begin Source File

SOURCE=..\FImport.cpp
# End Source File
# Begin Source File

SOURCE=..\FlashMP3Encoder.cpp
# End Source File
# Begin Source File

SOURCE=..\FMorph.cpp
# End Source File
# Begin Source File

SOURCE=..\FShape.cpp
# End Source File
# Begin Source File

SOURCE=..\FSound.cpp
# End Source File
# Begin Source File

SOURCE=..\FSprite.cpp
# End Source File
# Begin Source File

SOURCE=..\FTarga.cpp
# End Source File
# Begin Source File

SOURCE=..\gzio.c
# End Source File
# Begin Source File

SOURCE=..\infblock.c
# End Source File
# Begin Source File

SOURCE=..\infcodes.c
# End Source File
# Begin Source File

SOURCE=..\inffast.c
# End Source File
# Begin Source File

SOURCE=..\inflate.c
# End Source File
# Begin Source File

SOURCE=..\inftrees.c
# End Source File
# Begin Source File

SOURCE=..\infutil.c
# End Source File
# Begin Source File

SOURCE=..\actioncompiler\lex.swf4.cpp
# End Source File
# Begin Source File

SOURCE=..\actioncompiler\lex.swf5.cpp
# End Source File
# Begin Source File

SOURCE=..\actioncompiler\listaction.cpp
# End Source File
# Begin Source File

SOURCE=..\actioncompiler\main.cpp
# End Source File
# Begin Source File

SOURCE=..\maketree.c
# End Source File
# Begin Source File

SOURCE=..\md5\md5.c
# End Source File
# Begin Source File

SOURCE=..\actioncompiler\swf4compiler.tab.cpp
# End Source File
# Begin Source File

SOURCE=..\actioncompiler\swf5compiler.tab.cpp
# End Source File
# Begin Source File

SOURCE=..\trees.c
# End Source File
# Begin Source File

SOURCE=..\uncompr.c
# End Source File
# Begin Source File

SOURCE=..\zutil.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\actioncompiler\action.h
# End Source File
# End Group
# End Target
# End Project
