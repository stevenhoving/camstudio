;NSIS Modern User Interface
;Start Menu Folder Selection Example Script
;Written by Joost Verburg

;--------------------------------
;Include Modern UI

	!include "MUI2.nsh"

;--------------------------------
;General

	;Name, file, icon
	Name "CamStudio Desktop Screen Recorder"
	OutFile "CamStudioInstall.exe"
	!define MUI_ICON "..\GlobalResources\Recorder.ico"
	!define MUI_UNICON "..\GlobalResources\Recorder.ico"

	;Default installation folder
	InstallDir "$PROGRAMFILES\CamStudio"

	;Request application privileges for Windows Vista
	RequestExecutionLevel user

	;Do not show the NSIS message at the bottom
	BrandingText " "

;--------------------------------
;Version

	VIProductVersion "2.6.0.0"
	VIAddVersionKey "ProductName" "CamStudio Desktop Screen Recorder"
	VIAddVersionKey "Comments" "CamStudio Desktop Screen Recorder"
	VIAddVersionKey "CompanyName" "CamStudio Group"
	VIAddVersionKey "LegalCopyright" "Copyright © 2008 CamStudio Group & Contributors"
	VIAddVersionKey "FileDescription" "CamStudio Desktop Screen Recorder"
	VIAddVersionKey "FileVersion" "2.6.0.0"

;--------------------------------
;Variables

	Var StartMenuFolder

;--------------------------------
;Interface Settings

	!define MUI_ABORTWARNING

;--------------------------------
;Pages

	!insertmacro MUI_PAGE_WELCOME
	!insertmacro MUI_PAGE_LICENSE "..\GlobalResources\License.rtf"
	!insertmacro MUI_PAGE_COMPONENTS
	!insertmacro MUI_PAGE_DIRECTORY

	!insertmacro MUI_PAGE_STARTMENU Application $StartMenuFolder

	!insertmacro MUI_PAGE_INSTFILES

	!insertmacro MUI_UNPAGE_CONFIRM
	!insertmacro MUI_UNPAGE_INSTFILES

;--------------------------------
;Languages
 
  !insertmacro MUI_LANGUAGE "English"

;--------------------------------
;Installer Sections

	Section "CamStudio" SecCoreFiles
		SectionIn RO
		SetOutPath $INSTDIR
		File /r "controller"
		File /r "help_files"
		File /r "helpProducer_files"
		File "camstudio_cl.exe"
		File "default.shapes"
		File "dialog.bmp"
		File "help.htm"
		File "helpProducer.htm"
		File "hook.dll"
		File "Player.exe"
		File "PlayerPlus.exe"
		File "Producer.exe"
		File "Recorder.exe"
		File "stlport_vc645.dll"
		File "testsnd.wav"
		File "WhatsNew.htm"
		!insertmacro MUI_STARTMENU_WRITE_BEGIN Application
		CreateDirectory "$SMPROGRAMS\$StartMenuFolder"
		CreateShortCut "$SMPROGRAMS\$StartMenuFolder\Recorder.lnk" "$INSTDIR\Recorder.exe" \
		"" "$INSTDIR\Recorder.exe" 0 SW_SHOWNORMAL \
		ALT|CONTROL|SHIFT|F5 "CamStudio Recorder"
		CreateShortCut "$SMPROGRAMS\$StartMenuFolder\Player.lnk" "$INSTDIR\Player.exe" \
		"" "$INSTDIR\Player.exe" 0 SW_SHOWNORMAL \
		ALT|CONTROL|SHIFT|F5 "CamStudio Player"
		CreateShortCut "$SMPROGRAMS\$StartMenuFolder\PlayerPlus.lnk" "$INSTDIR\PlayerPlus.exe" \
		"" "$INSTDIR\PlayerPlus.exe" 0 SW_SHOWNORMAL \
		ALT|CONTROL|SHIFT|F5 "CamStudio PlayerPlus"
		CreateShortCut "$SMPROGRAMS\$StartMenuFolder\SWF Producer.lnk" "$INSTDIR\Producer.exe" \
		"" "$INSTDIR\Producer.exe" 0 SW_SHOWNORMAL \
		ALT|CONTROL|SHIFT|F5 "CamStudio SWF Producer"
		CreateShortCut "$SMPROGRAMS\$StartMenuFolder\Uninstall.lnk" "$INSTDIR\Uninstall.exe" \
		"" "$INSTDIR\Uninstall.exe" 0 SW_SHOWNORMAL \
		ALT|CONTROL|SHIFT|F5 "Uninstall"
		!insertmacro MUI_STARTMENU_WRITE_END
		WriteUninstaller "Uninstall.exe"
	SectionEnd

	Section /o "CamStudio Codec" SecCodec
		SectionIn RO
		;SetOutPath $INSTDIR
		;File "..."
	SectionEnd

;--------------------------------
;Descriptions

	;Language strings
	LangString DESC_SecCoreFiles ${LANG_ENGLISH} "Core files required for CamStudio to operate."
	LangString DESC_SecCodec ${LANG_ENGLISH} "The CamStudio lossless video codec, version 1.4. Recommended."

	;Assign language strings to sections
	!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
	!insertmacro MUI_DESCRIPTION_TEXT ${SecCoreFiles} $(DESC_SecCoreFiles)
	!insertmacro MUI_DESCRIPTION_TEXT ${SecCodec} $(DESC_SecCodec)
	!insertmacro MUI_FUNCTION_DESCRIPTION_END
 
;--------------------------------
;Uninstaller Section

	Section "Uninstall"
		Delete "$INSTDIR\Uninstall.exe"
		!insertmacro MUI_STARTMENU_GETFOLDER Application $StartMenuFolder
		RMDir /r "$INSTDIR"
		RMDir /r "$SMPROGRAMS\$StartMenuFolder"
	SectionEnd