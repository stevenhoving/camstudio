# Microsoft Developer Studio Project File - Name="vscap" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=vscap - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "vscap.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "vscap.mak" CFG="vscap - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "vscap - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "vscap - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "vscap - Win32 Release"

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
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o /win32 "NUL"
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o /win32 "NUL"
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 winmm.lib vfw32.lib msacm32.lib /nologo /subsystem:windows /machine:I386 /out:"Release/Recorder.exe"

!ELSEIF  "$(CFG)" == "vscap - Win32 Debug"

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
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MDd /W3 /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /YX /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o /win32 "NUL"
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o /win32 "NUL"
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 winmm.lib vfw32.lib Nafxcwd.lib Libcmtd.lib msacm32.lib /nologo /subsystem:windows /debug /machine:I386 /nodefaultlib:"Nafxcwd.lib Libcmtd.lib" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "vscap - Win32 Release"
# Name "vscap - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\AudioFormat.cpp
# End Source File
# Begin Source File

SOURCE=.\AutopanSpeed.cpp
# End Source File
# Begin Source File

SOURCE=.\AutoSearchDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\fister\Buffer.cpp
# End Source File
# Begin Source File

SOURCE=.\CBitmapEx.cpp
# End Source File
# Begin Source File

SOURCE=.\CursorOptionsDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\EditImage.cpp
# End Source File
# Begin Source File

SOURCE=.\EditTransparency.cpp
# End Source File
# Begin Source File

SOURCE=.\FixedRegion.cpp
# End Source File
# Begin Source File

SOURCE=.\FlashingWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\FolderDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\FrameGrabber.cpp
# End Source File
# Begin Source File

SOURCE=.\Keyshortcuts.cpp
# End Source File
# Begin Source File

SOURCE=.\LayeredWindowHelperST.cpp
# End Source File
# Begin Source File

SOURCE=.\LayoutList.cpp
# End Source File
# Begin Source File

SOURCE=.\ListManager.cpp
# End Source File
# Begin Source File

SOURCE=.\MainFrm.cpp
# End Source File
# Begin Source File

SOURCE=.\NewShape.cpp
# End Source File
# Begin Source File

SOURCE=.\Picture.cpp
# End Source File
# Begin Source File

SOURCE=.\PresetTime.cpp
# End Source File
# Begin Source File

SOURCE=.\ResizeDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\ScreenAnnotations.cpp
# End Source File
# Begin Source File

SOURCE=.\fister\SoundBase.cpp
# End Source File
# Begin Source File

SOURCE=.\fister\SoundFile.cpp
# End Source File
# Begin Source File

SOURCE=.\SpeakersImpl.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\SyncDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\TextDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\TransparentWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\TransRateDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\TroubleShoot.cpp
# End Source File
# Begin Source File

SOURCE=.\VideoOptions.cpp
# End Source File
# Begin Source File

SOURCE=.\VideoWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\vscap.cpp
# End Source File
# Begin Source File

SOURCE=.\vscap.rc
# End Source File
# Begin Source File

SOURCE=.\vscapDoc.cpp
# End Source File
# Begin Source File

SOURCE=.\vscapView.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\AudioFormat.h
# End Source File
# Begin Source File

SOURCE=.\AudioSpeakers.h
# End Source File
# Begin Source File

SOURCE=.\AutopanSpeed.h
# End Source File
# Begin Source File

SOURCE=.\AutoSearchDialog.h
# End Source File
# Begin Source File

SOURCE=.\fister\Buffer.h
# End Source File
# Begin Source File

SOURCE=.\CBitmapEx.h
# End Source File
# Begin Source File

SOURCE=.\CursorOptionsDlg.h
# End Source File
# Begin Source File

SOURCE=.\EditImage.h
# End Source File
# Begin Source File

SOURCE=.\EditTransparency.h
# End Source File
# Begin Source File

SOURCE=.\FlashingWnd.h
# End Source File
# Begin Source File

SOURCE=.\FrameGrabber.h
# End Source File
# Begin Source File

SOURCE=.\Keyshortcuts.h
# End Source File
# Begin Source File

SOURCE=.\LayeredWindowHelperST.h
# End Source File
# Begin Source File

SOURCE=.\LayoutList.h
# End Source File
# Begin Source File

SOURCE=.\ListManager.h
# End Source File
# Begin Source File

SOURCE=.\MainFrm.h
# End Source File
# Begin Source File

SOURCE=.\NewShape.h
# End Source File
# Begin Source File

SOURCE=.\Picture.h
# End Source File
# Begin Source File

SOURCE=.\PresetTime.h
# End Source File
# Begin Source File

SOURCE=.\ResizeDialog.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\ScreenAnnotations.h
# End Source File
# Begin Source File

SOURCE=.\fister\SoundBase.h
# End Source File
# Begin Source File

SOURCE=.\fister\SoundFile.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\SyncDialog.h
# End Source File
# Begin Source File

SOURCE=.\TextDialog.h
# End Source File
# Begin Source File

SOURCE=.\TransparentWnd.h
# End Source File
# Begin Source File

SOURCE=.\TransRateDialog.h
# End Source File
# Begin Source File

SOURCE=.\TroubleShoot.h
# End Source File
# Begin Source File

SOURCE=.\VideoOptions.h
# End Source File
# Begin Source File

SOURCE=.\VideoWnd.h
# End Source File
# Begin Source File

SOURCE=.\vscap.h
# End Source File
# Begin Source File

SOURCE=.\vscapDoc.h
# End Source File
# Begin Source File

SOURCE=.\vscapView.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\About.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bitmap1.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bitmap2.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp00001.bmp
# End Source File
# Begin Source File

SOURCE=.\res\cur00001.cur
# End Source File
# Begin Source File

SOURCE=.\res\cur00002.cur
# End Source File
# Begin Source File

SOURCE=.\res\cursor4.cur
# End Source File
# Begin Source File

SOURCE=.\res\customic.ico
# End Source File
# Begin Source File

SOURCE=.\res\ico00001.ico
# End Source File
# Begin Source File

SOURCE=.\res\ico00002.ico
# End Source File
# Begin Source File

SOURCE=.\res\ico00003.ico
# End Source File
# Begin Source File

SOURCE=.\res\ico00004.ico
# End Source File
# Begin Source File

SOURCE=.\res\ico00005.ico
# End Source File
# Begin Source File

SOURCE=.\res\ico00006.ico
# End Source File
# Begin Source File

SOURCE=.\res\ico00007.ico
# End Source File
# Begin Source File

SOURCE=.\res\ico00008.ico
# End Source File
# Begin Source File

SOURCE=.\res\ico00009.ico
# End Source File
# Begin Source File

SOURCE=.\res\ico00010.ico
# End Source File
# Begin Source File

SOURCE=.\res\ico00011.ico
# End Source File
# Begin Source File

SOURCE=.\res\ico00012.ico
# End Source File
# Begin Source File

SOURCE=.\res\ico00013.ico
# End Source File
# Begin Source File

SOURCE=.\res\ico00014.ico
# End Source File
# Begin Source File

SOURCE=.\res\ico00015.ico
# End Source File
# Begin Source File

SOURCE=.\res\ico00016.ico
# End Source File
# Begin Source File

SOURCE=.\res\ico00017.ico
# End Source File
# Begin Source File

SOURCE=.\res\ico00018.ico
# End Source File
# Begin Source File

SOURCE=.\res\ico00019.ico
# End Source File
# Begin Source File

SOURCE=.\res\ico00020.ico
# End Source File
# Begin Source File

SOURCE=.\res\ico00021.ico
# End Source File
# Begin Source File

SOURCE=.\res\ico00022.ico
# End Source File
# Begin Source File

SOURCE=.\res\ico00023.ico
# End Source File
# Begin Source File

SOURCE=.\res\ico00024.ico
# End Source File
# Begin Source File

SOURCE=.\res\ico00025.ico
# End Source File
# Begin Source File

SOURCE=.\res\ico00026.ico
# End Source File
# Begin Source File

SOURCE=.\res\ico00027.ico
# End Source File
# Begin Source File

SOURCE=.\res\ico00028.ico
# End Source File
# Begin Source File

SOURCE=.\res\ico00029.ico
# End Source File
# Begin Source File

SOURCE=.\res\ico00030.ico
# End Source File
# Begin Source File

SOURCE=.\res\ico00031.ico
# End Source File
# Begin Source File

SOURCE=.\res\ico00032.ico
# End Source File
# Begin Source File

SOURCE=.\res\ico00033.ico
# End Source File
# Begin Source File

SOURCE=.\res\ico00034.ico
# End Source File
# Begin Source File

SOURCE=.\res\ico00035.ico
# End Source File
# Begin Source File

SOURCE=.\res\icon1.ico
# End Source File
# Begin Source File

SOURCE=.\res\icon2.ico
# End Source File
# Begin Source File

SOURCE=.\res\icon3.ico
# End Source File
# Begin Source File

SOURCE=.\res\icon4.ico
# End Source File
# Begin Source File

SOURCE=.\res\idr_main.ico
# End Source File
# Begin Source File

SOURCE=.\res\idr_vsca.ico
# End Source File
# Begin Source File

SOURCE=.\Logo.bmp
# End Source File
# Begin Source File

SOURCE=.\res\mainfram.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Toolbar.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Toolbar256.bmp
# End Source File
# Begin Source File

SOURCE=.\res\ToolbarMask.bmp
# End Source File
# Begin Source File

SOURCE=.\res\ToolbarPalette256.bmp
# End Source File
# Begin Source File

SOURCE=.\res\vscap.ico
# End Source File
# Begin Source File

SOURCE=.\res\vscap.rc2
# End Source File
# Begin Source File

SOURCE=.\res\vscapDoc.ico
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
