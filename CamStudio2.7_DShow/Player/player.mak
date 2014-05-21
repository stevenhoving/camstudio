# Nmake macros for building Windows 32-Bit apps

!include <win32.mak>

!if "$(CPU)" == "i386" # .syms are useful for Win95
SYM = player.sym
!endif

all: player.exe $(SYM)

OBJS= player.obj

OTHERCLOPTS=-DWIN32_LEAN_AND_MEAN -nologo -I. -I..\include
OTHERRCOPTS=-DWIN32_LEAN_AND_MEAN -I. -I..\include


# Update the resource if necessary

player.res: player.rc player.h player.ico
    $(rc) -r -DWIN32 $(OTHERRCOPTS) player.rc


# Update the object file if necessary

player.obj: player.cpp player.h
    $(cc) $(cdebug) $(cflags) $(cvars) $(OTHERCLOPTS) player.cpp


# Update the executable file if necessary, and if so, add the resource back in.

player.exe player.map:  $(OBJS) player.res
    $(link) $(linkdebug) $(guilflags) -out:player.exe $(OBJS) player.res $(guilibs) \
	 winmm.lib vfw32.lib -map:$*.map
    
player.sym:	$*.map
	mapsym $*.map

clean:
    @if exist player.exe del player.exe
    @if exist *.obj del *.obj
    @if exist *.map del *.map
    @if exist *.sym del *.sym
    @if exist *.res del *.res
    @if exist *.pdb del *.pdb
    @if exist *.exp del *.exp
    @if exist *.lib del *.lib

