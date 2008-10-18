;--------------------------------
;Include Modern UI

	!include "MUI2.nsh"

;--------------------------------
;Constants

	!define MUI_ICON "..\GlobalResources\Recorder.ico"
	!define MUI_UNICON "..\GlobalResources\Uninstall.ico"
	
	!define COPYRIGHT_YEARS "2008"
	
	!define CAMST_VERSION "2.6.0.0"
	!define CAMST_VERSION_SHORT "2.6"
	!define CODEC_VERSION_SHORT "1.5"

;--------------------------------
;Language strings
	
	;English
	LangString LANG_Name ${LANG_ENGLISH} "CamStudio Desktop Screen Recorder" ;Name of the installer
	LangString LANG_SecCoreFiles ${LANG_ENGLISH} "Core files required for CamStudio to operate." ;Core files section description
	LangString LANG_SecCodec ${LANG_ENGLISH} "The CamStudio Lossless Video Codec ${CODEC_VERSION_SHORT}. Recommended." ;Codec description
	LangString LANG_CaptionPlayerClassic ${LANG_ENGLISH} "CamStudio Player Classic"
	LangString LANG_CaptionPlayerPlus ${LANG_ENGLISH} "CamStudio PlayerPlus"
	LangString LANG_CaptionProducer ${LANG_ENGLISH} "CamStudio SWF Producer"
	LangString LANG_CaptionRecorder ${LANG_ENGLISH} "CamStudio Recorder"
	LangString LANG_CaptionUninstall ${LANG_ENGLISH} "Uninstall CamStudio"
	LangString LANG_UninstallCodec ${LANG_ENGLISH} "Do you wish to uninstall the CamStudio Lossless Video Codec ${CODEC_VERSION_SHORT}?"
	LangString LANG_UninstallError ${LANG_ENGLISH} "There was a serious error uninstalling CamStudio. Please visit www.camstudio.org for assistance."
	LangString LANG_Codec ${LANG_ENGLISH} "Codec"

;--------------------------------
;General

	;Name, file, icon
	Name $(LANG_NAME)
	OutFile "CamStudioInstall${CAMST_VERSION_SHORT}.exe"

	;Default installation folder
	InstallDir "$PROGRAMFILES\CamStudio"

	;Request application privileges for Windows Vista
	RequestExecutionLevel user

	;Do not show the NSIS message at the bottom
	BrandingText " "

;--------------------------------
;Version

	VIProductVersion ${CAMST_VERSION}
	VIAddVersionKey "ProductName" "CamStudio Desktop Screen Recorder"
	VIAddVersionKey "Comments" "CamStudio Desktop Screen Recorder"
	VIAddVersionKey "CompanyName" "CamStudio Group"
	VIAddVersionKey "LegalCopyright" "Copyright © ${COPYRIGHT_YEARS} CamStudio Group & Contributors"
	VIAddVersionKey "FileDescription" "CamStudio Desktop Screen Recorder"
	VIAddVersionKey "FileVersion" ${CAMST_VERSION}

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
		WriteUninstaller "Uninstall.exe"
		!insertmacro MUI_STARTMENU_WRITE_BEGIN Application
		CreateDirectory "$SMPROGRAMS\$StartMenuFolder"
		CreateShortCut "$SMPROGRAMS\$StartMenuFolder\Recorder.lnk" "$INSTDIR\Recorder.exe" \
		"" "$INSTDIR\Recorder.exe" 0 SW_SHOWNORMAL \
		"" $(LANG_CaptionRecorder)
		CreateShortCut "$SMPROGRAMS\$StartMenuFolder\Player Classic.lnk" "$INSTDIR\Player.exe" \
		"" "$INSTDIR\Player.exe" 0 SW_SHOWNORMAL \
		"" $(LANG_CaptionPlayerClassic)
		CreateShortCut "$SMPROGRAMS\$StartMenuFolder\PlayerPlus.lnk" "$INSTDIR\PlayerPlus.exe" \
		"" "$INSTDIR\PlayerPlus.exe" 0 SW_SHOWNORMAL \
		"" $(LANG_CaptionPlayerPlus)
		CreateShortCut "$SMPROGRAMS\$StartMenuFolder\SWF Producer.lnk" "$INSTDIR\Producer.exe" \
		"" "$INSTDIR\Producer.exe" 0 SW_SHOWNORMAL \
		"" $(LANG_CaptionProducer)
		CreateShortCut "$SMPROGRAMS\$StartMenuFolder\Uninstall.lnk" "$INSTDIR\Uninstall.exe" \
		"" "$INSTDIR\Uninstall.exe" 0 SW_SHOWNORMAL \
		"" $(LANG_CaptionUninstall)
		!insertmacro MUI_STARTMENU_WRITE_END
	SectionEnd

	Section "CamStudio $(LANG_Codec)" SecCodec
		SetOutPath $SYSDIR
		File "CamCodec.dll"
	SectionEnd

;--------------------------------
;Descriptions

	;Assign language strings to sections
	!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
	!insertmacro MUI_DESCRIPTION_TEXT ${SecCoreFiles} $(LANG_SecCoreFiles)
	!insertmacro MUI_DESCRIPTION_TEXT ${SecCodec} $(LANG_SecCodec)
	!insertmacro MUI_FUNCTION_DESCRIPTION_END
 
;--------------------------------
;Uninstaller Section

	;Uninstall path safety check
	!macro BadPathsCheck
	StrCpy $R0 $INSTDIR "" -2
	StrCmp $R0 ":\" bad
	StrCpy $R0 $INSTDIR "" -14
	StrCmp $R0 "\Program Files" bad
	StrCpy $R0 $INSTDIR "" -8
	StrCmp $R0 "\Windows" bad
	StrCpy $R0 $INSTDIR "" -6
	StrCmp $R0 "\WinNT" bad
	StrCpy $R0 $INSTDIR "" -9
	StrCmp $R0 "\system32" bad
	StrCpy $R0 $INSTDIR "" -8
	StrCmp $R0 "\Desktop" bad
	StrCpy $R0 $INSTDIR "" -22
	StrCmp $R0 "\Documents and Settings" bad
	StrCpy $R0 $INSTDIR "" -13
	StrCmp $R0 "\My Documents" bad done
	bad:
		MessageBox MB_OK|MB_ICONSTOP $(LANG_UninstallError)
		Abort
	done:
	!macroend
	
	Section "Uninstall"
		;Make sure the uninstall path isn't dangerous
		!insertmacro BadPathsCheck
		
		Delete "$INSTDIR\Uninstall.exe"
		!insertmacro MUI_STARTMENU_GETFOLDER Application $StartMenuFolder
		RMDir /r "$INSTDIR"
		RMDir /r "$SMPROGRAMS\$StartMenuFolder"
		IfFileExists "$SYSDIR\CamCodec.dll" 0 +2
			MessageBox MB_YESNO $(LANG_UninstallCodec) IDYES true
				true: Delete "$SYSDIR\CamCodec.dll"
	SectionEnd