# Microsoft Developer Studio Project File - Name="camcodec" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 5.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=camcodec - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "camcodec.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "camcodec.mak" CFG="camcodec - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "camcodec - Win32 Release" (based on\
 "Win32 (x86) Dynamic-Link Library")
!MESSAGE "camcodec - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "camcodec - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "CAMCODEC_EXPORTS" /YX /FD /c
# ADD CPP /nologo /G6 /MD /W3 /O2 /Ob2 /I "." /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_USRDLL" /D "CAMCODEC_EXPORTS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 winmm.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "camcodec - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /GX /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "CAMCODEC_EXPORTS" /YX /FD /ZI /GZ /c
# ADD CPP /nologo /MDd /W3 /GX /Od /I "." /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_USRDLL" /D "CAMCODEC_EXPORTS" /YX /FD /ZI /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 winmm.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /out:"c:\windows\system\camcodec.dll" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "camcodec - Win32 Release"
# Name "camcodec - Win32 Debug"
# Begin Source File

SOURCE=.\zlib\adler32.c
# End Source File
# Begin Source File

SOURCE=.\camcodec.cpp
# End Source File
# Begin Source File

SOURCE=.\camcodec.def
# End Source File
# Begin Source File

SOURCE=.\camcodec.h
# End Source File
# Begin Source File

SOURCE=.\camcodec.rc
# End Source File
# Begin Source File

SOURCE=.\zlib\compress.c
# End Source File
# Begin Source File

SOURCE=.\zlib\crc32.c
# End Source File
# Begin Source File

SOURCE=.\zlib\deflate.c
# End Source File
# Begin Source File

SOURCE=.\drvproc.cpp
# End Source File
# Begin Source File

SOURCE=.\zlib\gzio.c
# End Source File
# Begin Source File

SOURCE=.\zlib\infblock.c
# End Source File
# Begin Source File

SOURCE=.\zlib\infcodes.c
# End Source File
# Begin Source File

SOURCE=.\zlib\inffast.c
# End Source File
# Begin Source File

SOURCE=.\zlib\inflate.c
# End Source File
# Begin Source File

SOURCE=.\zlib\inftrees.c
# End Source File
# Begin Source File

SOURCE=.\zlib\infutil.c
# End Source File
# Begin Source File

SOURCE=.\minilzo.c
# End Source File
# Begin Source File

SOURCE=.\zlib\trees.c
# End Source File
# Begin Source File

SOURCE=.\zlib\uncompr.c
# End Source File
# Begin Source File

SOURCE=.\zlib\zutil.c
# End Source File
# End Target
# End Project
