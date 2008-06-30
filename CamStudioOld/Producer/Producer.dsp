# Microsoft Developer Studio Project File - Name="Producer" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=Producer - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Producer.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Producer.mak" CFG="Producer - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Producer - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "Producer - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Producer - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 vfw32.lib winmm.lib msacm32.lib /nologo /subsystem:windows /machine:I386 /out:"../Release/Producer.exe"

!ELSEIF  "$(CFG)" == "Producer - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /YX /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 vfw32.lib winmm.lib msacm32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "Producer - Win32 Release"
# Name "Producer - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\swfsource\adler32.c
# End Source File
# Begin Source File

SOURCE=.\AdvProperty.cpp
# End Source File
# Begin Source File

SOURCE=.\swfsource\actioncompiler\assembler.cpp
# End Source File
# Begin Source File

SOURCE=.\audio.cpp
# End Source File
# Begin Source File

SOURCE=.\AudioFormat.cpp
# End Source File
# Begin Source File

SOURCE=.\fister\Buffer.cpp
# End Source File
# Begin Source File

SOURCE=.\swfsource\actioncompiler\common.cpp
# End Source File
# Begin Source File

SOURCE=.\swfsource\actioncompiler\compile.cpp
# End Source File
# Begin Source File

SOURCE=.\swfsource\actioncompiler\compileaction.cpp
# End Source File
# Begin Source File

SOURCE=.\swfsource\compress.c
# End Source File
# Begin Source File

SOURCE=.\Conversion.cpp
# End Source File
# Begin Source File

SOURCE=.\swfsource\crc32.c
# End Source File
# Begin Source File

SOURCE=.\swfsource\deflate.c
# End Source File
# Begin Source File

SOURCE=.\swfsource\FAction.cpp
# End Source File
# Begin Source File

SOURCE=.\swfsource\FBase.cpp
# End Source File
# Begin Source File

SOURCE=.\swfsource\FBitmap.cpp
# End Source File
# Begin Source File

SOURCE=.\swfsource\FButton.cpp
# End Source File
# Begin Source File

SOURCE=.\swfsource\FControl.cpp
# End Source File
# Begin Source File

SOURCE=.\swfsource\FDisplay.cpp
# End Source File
# Begin Source File

SOURCE=.\swfsource\FFont.cpp
# End Source File
# Begin Source File

SOURCE=.\swfsource\FImport.cpp
# End Source File
# Begin Source File

SOURCE=.\FlashConversionDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\swfsource\FlashFontObj.cpp
# End Source File
# Begin Source File

SOURCE=.\FlashInterface.cpp
# End Source File
# Begin Source File

SOURCE=.\swfsource\FlashMP3Encoder.cpp
# End Source File
# Begin Source File

SOURCE=.\swfsource\FMorph.cpp
# End Source File
# Begin Source File

SOURCE=.\swfsource\FShape.cpp
# End Source File
# Begin Source File

SOURCE=.\swfsource\FSound.cpp
# End Source File
# Begin Source File

SOURCE=.\flashsdk\FSoundSDK.cpp
# End Source File
# Begin Source File

SOURCE=.\swfsource\FSprite.cpp
# End Source File
# Begin Source File

SOURCE=.\swfsource\FTarga.cpp
# End Source File
# Begin Source File

SOURCE=.\swfsource\gzio.c
# End Source File
# Begin Source File

SOURCE=.\swfsource\infblock.c
# End Source File
# Begin Source File

SOURCE=.\swfsource\infcodes.c
# End Source File
# Begin Source File

SOURCE=.\swfsource\inffast.c
# End Source File
# Begin Source File

SOURCE=.\swfsource\inflate.c
# End Source File
# Begin Source File

SOURCE=.\swfsource\inftrees.c
# End Source File
# Begin Source File

SOURCE=.\swfsource\infutil.c
# End Source File
# Begin Source File

SOURCE=.\swfsource\actioncompiler\lex.swf4.cpp
# End Source File
# Begin Source File

SOURCE=.\swfsource\actioncompiler\lex.swf5.cpp
# End Source File
# Begin Source File

SOURCE=.\swfsource\actioncompiler\listaction.cpp
# End Source File
# Begin Source File

SOURCE=.\MainFrm.cpp
# End Source File
# Begin Source File

SOURCE=.\swfsource\md5\md5.c
# End Source File
# Begin Source File

SOURCE=.\MessageWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\Picture.cpp
# End Source File
# Begin Source File

SOURCE=.\playplus.cpp
# End Source File
# Begin Source File

SOURCE=.\playplusDoc.cpp
# End Source File
# Begin Source File

SOURCE=.\playplusView.cpp
# End Source File
# Begin Source File

SOURCE=.\Producer.rc
# End Source File
# Begin Source File

SOURCE=.\ProgressDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\SlideToolBar.cpp
# End Source File
# Begin Source File

SOURCE=.\fister\SoundBase.cpp
# End Source File
# Begin Source File

SOURCE=.\fister\SoundFile.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\swfsource\actioncompiler\swf4compiler.tab.cpp
# End Source File
# Begin Source File

SOURCE=.\swfsource\actioncompiler\swf5compiler.tab.cpp
# End Source File
# Begin Source File

SOURCE=.\SWFPropertySheet.cpp
# End Source File
# Begin Source File

SOURCE=.\swfsource\trees.c
# End Source File
# Begin Source File

SOURCE=.\swfsource\uncompr.c
# End Source File
# Begin Source File

SOURCE=.\Conversion\WAVEIO.CPP
# End Source File
# Begin Source File

SOURCE=.\swfsource\zutil.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\AdvProperty.h
# End Source File
# Begin Source File

SOURCE=.\AudioFormat.h
# End Source File
# Begin Source File

SOURCE=.\Conversion.h
# End Source File
# Begin Source File

SOURCE=.\FlashConversionDlg.h
# End Source File
# Begin Source File

SOURCE=.\FlashInterface.h
# End Source File
# Begin Source File

SOURCE=.\MainFrm.h
# End Source File
# Begin Source File

SOURCE=.\playplus.h
# End Source File
# Begin Source File

SOURCE=.\playplusDoc.h
# End Source File
# Begin Source File

SOURCE=.\playplusView.h
# End Source File
# Begin Source File

SOURCE=.\ProgressDlg.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\SlideToolBar.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\SWFPropertySheet.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\ABOUT.BMP
# End Source File
# Begin Source File

SOURCE=.\Aboutdubber.bmp
# End Source File
# Begin Source File

SOURCE=.\Aboutplayer.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bitmap3.bmp
# End Source File
# Begin Source File

SOURCE=.\Logo.bmp
# End Source File
# Begin Source File

SOURCE=.\Logodubber.bmp
# End Source File
# Begin Source File

SOURCE=.\Logoplayer.bmp
# End Source File
# Begin Source File

SOURCE=.\res\playplus.ico
# End Source File
# Begin Source File

SOURCE=.\res\playplus.rc2
# End Source File
# Begin Source File

SOURCE=.\res\playplusDoc.ico
# End Source File
# Begin Source File

SOURCE=.\res\Toolbar.bmp
# End Source File
# Begin Source File

SOURCE=.\res\toolbar1.bmp
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
