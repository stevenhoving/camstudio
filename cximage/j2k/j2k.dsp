# Microsoft Developer Studio Project File - Name="j2k" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=j2k - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "j2k.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "j2k.mak" CFG="j2k - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "j2k - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "j2k - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "j2k - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "LIBJ2K_EXPORTS" /D "_AFXDLL" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x410 /d "NDEBUG"
# ADD RSC /l 0x410 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "j2k - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /D "LIBJ2K_EXPORTS" /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x410 /d "_DEBUG"
# ADD RSC /l 0x410 /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "j2k - Win32 Release"
# Name "j2k - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\bio.c
# End Source File
# Begin Source File

SOURCE=.\cio.c
# End Source File
# Begin Source File

SOURCE=.\dwt.c
# End Source File
# Begin Source File

SOURCE=.\fix.c
# End Source File
# Begin Source File

SOURCE=.\int.c
# End Source File
# Begin Source File

SOURCE=.\j2k.c
# End Source File
# Begin Source File

SOURCE=.\mct.c
# End Source File
# Begin Source File

SOURCE=.\mqc.c
# End Source File
# Begin Source File

SOURCE=.\pi.c
# End Source File
# Begin Source File

SOURCE=.\t1.c
# End Source File
# Begin Source File

SOURCE=.\t2.c
# End Source File
# Begin Source File

SOURCE=.\tcd.c
# End Source File
# Begin Source File

SOURCE=.\tgt.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\bio.h
# End Source File
# Begin Source File

SOURCE=.\cio.h
# End Source File
# Begin Source File

SOURCE=.\dwt.h
# End Source File
# Begin Source File

SOURCE=.\fix.h
# End Source File
# Begin Source File

SOURCE=.\int.h
# End Source File
# Begin Source File

SOURCE=.\j2k.h
# End Source File
# Begin Source File

SOURCE=.\mct.h
# End Source File
# Begin Source File

SOURCE=.\mqc.h
# End Source File
# Begin Source File

SOURCE=.\pi.h
# End Source File
# Begin Source File

SOURCE=.\t1.h
# End Source File
# Begin Source File

SOURCE=.\t2.h
# End Source File
# Begin Source File

SOURCE=.\tcd.h
# End Source File
# Begin Source File

SOURCE=.\tgt.h
# End Source File
# End Group
# End Target
# End Project
