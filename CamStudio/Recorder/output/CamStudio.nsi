;--------------------------------
; Include Modern UI

	!include "MUI2.nsh"

;--------------------------------
; Constants
	
	# Set product information...
	!define PRODUCT_NAME "CamStudio"
	!define PRODUCT_VERSION "2.6"
	!define PRODUCT_VERSION_LONG "2.6.0.0"
	!define PRODUCT_PUBLISHER "CamStudio Group"
	!define PRODUCT_WEB_SITE "http://www.camstudio.org/"
	!define PRODUCT_DIR_REGKEY "Software\Microsoft\Windows\CurrentVersion\App Paths\Recorder.exe"
	!define PRODUCT_UNINST_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}"
	!define PRODUCT_UNINST_ROOT_KEY "HKLM"
	!define PRODUCT_STARTMENU_REGVAL "NSIS:StartMenuDir"
	
	# Codec version and years of copyright (YYYY-YYYY)
	!define CODEC_VERSION "1.5"
	!define COPYRIGHT_YEARS "2008"
	
	# Warn the user if they try to close the installer
	!define MUI_ABORTWARNING

	# Set installer and uninstaller icons
	!define MUI_ICON "..\GlobalResources\Recorder.ico"
	!define MUI_UNICON "..\GlobalResources\Uninstall.ico"

;--------------------------------
; Pages and page settings

	# Multi-language settings
	!define MUI_LANGDLL_REGISTRY_ROOT "${PRODUCT_UNINST_ROOT_KEY}"
	!define MUI_LANGDLL_REGISTRY_KEY "${PRODUCT_UNINST_KEY}"
	!define MUI_LANGDLL_REGISTRY_VALUENAME "NSIS:Language"

	# Insert pages
	!insertmacro MUI_PAGE_WELCOME
	!define MUI_LICENSEPAGE_CHECKBOX ; Force user to check an "I agree" box to continue installing
	!insertmacro MUI_PAGE_LICENSE $(license)
	!insertmacro MUI_PAGE_COMPONENTS
	!insertmacro MUI_PAGE_DIRECTORY
	Var StartMenuFolder ; Define variable to hold start menu folder
	!define MUI_STARTMENUPAGE_DEFAULTFOLDER "${PRODUCT_NAME}" ; Set default start menu folder
	!insertmacro MUI_PAGE_STARTMENU Application $StartMenuFolder
	!insertmacro MUI_PAGE_INSTFILES
	!define MUI_FINISHPAGE_NOAUTOCLOSE ; Don't automatically skip past the install details page when it's done
	!define MUI_FINISHPAGE_LINK "$(LANG_CaptionWebsite)" ; Finish page link text
	!define MUI_FINISHPAGE_LINK_LOCATION "${PRODUCT_WEB_SITE}" ; Finish page URL
	!insertmacro MUI_PAGE_FINISH

	!insertmacro MUI_UNPAGE_COMPONENTS
	!insertmacro MUI_UNPAGE_INSTFILES
	!define MUI_UNFINISHPAGE_NOAUTOCLOSE ; Don't automatically skip past the uninstall details page when it's done
	!insertmacro MUI_UNPAGE_FINISH

	# Define languages that the installer has
	!insertmacro MUI_LANGUAGE "English"
	!insertmacro MUI_LANGUAGE "German"

;--------------------------------
; Language strings
	
	# English
	LangString LANG_SecCoreFiles ${LANG_ENGLISH} "Core files required for CamStudio to operate." ;Core files section description
	LangString LANG_SecCodec ${LANG_ENGLISH} "The CamStudio Lossless Video Codec. Recommended." ;Codec description
	LangString LANG_UnSecCoreFiles ${LANG_ENGLISH} "Removes the CamStudio core files."
	LangString LANG_UnSecCodec ${LANG_ENGLISH} "Removes the CamStudio Lossless Video Codec."
	LangString LANG_UnSecSettings ${LANG_ENGLISH} "Removes the CamStudio settings and preferences files."
	LangString LANG_CaptionPlayerClassic ${LANG_ENGLISH} "CamStudio Player Classic"
	LangString LANG_CaptionPlayerPlus ${LANG_ENGLISH} "CamStudio PlayerPlus"
	LangString LANG_CaptionProducer ${LANG_ENGLISH} "CamStudio SWF Producer"
	LangString LANG_CaptionRecorder ${LANG_ENGLISH} "CamStudio Recorder"
	LangString LANG_CaptionUninstall ${LANG_ENGLISH} "Uninstall CamStudio"
	LangString LANG_CaptionWebsite ${LANG_ENGLISH} "Visit the CamStudio website"
	LangString LANG_UninstallError ${LANG_ENGLISH} "There was a serious error uninstalling CamStudio. Please visit www.camstudio.org for assistance."
	LangString LANG_Website ${LANG_ENGLISH} "Website"
	LangString LANG_Codec ${LANG_ENGLISH} "Codec"
	LangString LANG_Settings ${LANG_ENGLISH} "Settings"
	LicenseLangString license ${LANG_ENGLISH} "..\GlobalResources\License\English.rtf"
	
	# German
	LangString LANG_SecCoreFiles ${LANG_GERMAN} "Für CamStudio erforderliche Kerndateien, um zu funktionieren." ;Core files section description
	LangString LANG_SecCodec ${LANG_GERMAN} "Der CamStudio Lossless Codec Video. Empfohlen." ;Codec description
	LangString LANG_UnSecCoreFiles ${LANG_GERMAN} "Entfernt die CamStudio-Kerndateien."
	LangString LANG_UnSecCodec ${LANG_GERMAN} "Entfernt den CamStudio Lossless Codec Video."
	LangString LANG_UnSecSettings ${LANG_GERMAN} "Entfernt die CamStudio Einstellungen und Vorzugsdateien."
	LangString LANG_CaptionPlayerClassic ${LANG_GERMAN} "CamStudio-Spieler-Klassiker"
	LangString LANG_CaptionPlayerPlus ${LANG_GERMAN} "CamStudio-SpielerPlus"
	LangString LANG_CaptionProducer ${LANG_GERMAN} "CamStudio SWF Erzeuger"
	LangString LANG_CaptionRecorder ${LANG_GERMAN} "CamStudio-Recorder"
	LangString LANG_CaptionUninstall ${LANG_GERMAN} "Deinstallieren Sie CamStudio"
	LangString LANG_CaptionWebsite ${LANG_GERMAN} "Besuchen Sie die CamStudio Website"
	LangString LANG_UninstallError ${LANG_GERMAN} "Es gab einen ernsten Fehler, der CamStudio deinstalliert. Besuchen Sie bitte www.camstudio.org für die Hilfe."
	LangString LANG_Website ${LANG_GERMAN} "Website"
	LangString LANG_Codec ${LANG_GERMAN} "Codec"
	LangString LANG_Settings ${LANG_GERMAN} "Einstellungen"
	LicenseLangString license ${LANG_GERMAN} "..\GlobalResources\License\German.rtf"

;--------------------------------
; General

	# Display name and filename of installer
	Name ${PRODUCT_NAME}
	OutFile "CamStudioInstall${PRODUCT_VERSION}.exe"

	# Default installation folder
	InstallDir "$PROGRAMFILES\${PRODUCT_NAME}"
	
	# Registry install key
	InstallDirRegKey HKLM "${PRODUCT_DIR_REGKEY}" ""

	# Request application privileges for Windows Vista
	RequestExecutionLevel admin

	# Do not show the NSIS message at the bottom
	BrandingText " "
	
	# Don't auto-close the window when install finishes
	AutoCloseWindow false
	
	# Show the language selection dialog
	Function .onInit
		!insertmacro MUI_LANGDLL_DISPLAY
	FunctionEnd
	
	# Make sure to show the installer in the proper language
	Function un.onInit
		!insertmacro MUI_UNGETLANGUAGE
	FunctionEnd

;--------------------------------
; Version

	# Insert version tab into the installer file (sorry, this has to stay in English... not like most will look at it anyways)
	VIProductVersion ${PRODUCT_VERSION_LONG}
	VIAddVersionKey "ProductName" "CamStudio Desktop Screen Recorder"
	VIAddVersionKey "Comments" "CamStudio Desktop Screen Recorder"
	VIAddVersionKey "CompanyName" "CamStudio Group"
	VIAddVersionKey "LegalCopyright" "Copyright © ${COPYRIGHT_YEARS} CamStudio Group & Contributors"
	VIAddVersionKey "FileDescription" "CamStudio Desktop Screen Recorder"
	VIAddVersionKey "FileVersion" ${PRODUCT_VERSION_LONG}

;--------------------------------
; Installer Sections

	# Core files required to install the CamStudio recording suite
	Section "${PRODUCT_NAME} ${PRODUCT_VERSION}" SecCoreFiles
		CreateDirectory "$INSTDIR"
		CreateDirectory "$INSTDIR\controller\"
		CreateDirectory "$INSTDIR\help_files\"
		CreateDirectory "$INSTDIR\helpProducer_files\"
		SetOutPath "$INSTDIR"
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
		File "..\GlobalResources\Web.ico"
		SetOutPath "$INSTDIR\controller"
		File "controller\controller.ini"
		File "controller\controller_backup.ini"
		File "controller\leftpiece.bmp"
		File "controller\loadnode.bmp"
		File "controller\loadpiece.bmp"
		File "controller\pausebutton.bmp"
		File "controller\pausebutton2.bmp"
		File "controller\playbutton.bmp"
		File "controller\playbutton2.bmp"
		File "controller\rightpiece.bmp"
		File "controller\stopbutton.bmp"
		File "controller\stopbutton2.bmp"
		SetOutPath "$INSTDIR\help_files"
		File "help_files\filelist.xml"
		File "help_files\image001.gif"
		File "help_files\image002.gif"
		File "help_files\image003.gif"
		File "help_files\image004.png"
		File "help_files\image005.jpg"
		File "help_files\image006.jpg"
		File "help_files\image007.jpg"
		File "help_files\image008.png"
		File "help_files\image009.png"
		File "help_files\image010.png"
		File "help_files\image011.png"
		File "help_files\image012.png"
		File "help_files\image013.png"
		File "help_files\image014.png"
		File "help_files\image015.png"
		File "help_files\image016.png"
		File "help_files\image017.png"
		File "help_files\image018.png"
		File "help_files\image019.png"
		File "help_files\image020.png"
		File "help_files\image021.png"
		File "help_files\image022.gif"
		File "help_files\image023.png"
		File "help_files\image024.png"
		File "help_files\image025.png"
		File "help_files\image026.png"
		File "help_files\image027.png"
		File "help_files\image028.png"
		File "help_files\image029.png"
		File "help_files\image030.png"
		File "help_files\image031.png"
		File "help_files\image032.png"
		File "help_files\image033.png"
		File "help_files\image034.png"
		File "help_files\image035.png"
		File "help_files\image036.png"
		File "help_files\image037.png"
		File "help_files\image038.png"
		File "help_files\image039.gif"
		File "help_files\image040.gif"
		File "help_files\image041.png"
		File "help_files\image042.png"
		File "help_files\image043.png"
		File "help_files\image044.png"
		File "help_files\image045.png"
		File "help_files\image046.png"
		File "help_files\image047.png"
		File "help_files\image048.png"
		File "help_files\image049.png"
		File "help_files\image050.png"
		File "help_files\image051.png"
		File "help_files\image052.png"
		File "help_files\image053.png"
		File "help_files\image054.png"
		File "help_files\image055.png"
		File "help_files\image056.png"
		File "help_files\image057.png"
		File "help_files\image058.png"
		File "help_files\image059.png"
		File "help_files\image060.png"
		File "help_files\image061.png"
		File "help_files\image062.png"
		File "help_files\image063.png"
		File "help_files\image064.png"
		File "help_files\image065.png"
		File "help_files\image066.png"
		File "help_files\image067.gif"
		File "help_files\oledata.mso"
		SetOutPath "$INSTDIR\helpProducer_files"
		File "helpProducer_files\filelist.xml"
		File "helpProducer_files\image001.gif"
		File "helpProducer_files\image002.gif"
		File "helpProducer_files\image003.gif"
		File "helpProducer_files\image004.png"
		File "helpProducer_files\image005.jpg"
		File "helpProducer_files\image006.png"
		File "helpProducer_files\image007.png"
		File "helpProducer_files\image008.png"
		File "helpProducer_files\image009.png"
		File "helpProducer_files\image010.png"
		File "helpProducer_files\oledata.mso"
		# Create start menu shortcuts only if the user selected them
		!insertmacro MUI_STARTMENU_WRITE_BEGIN Application
		CreateDirectory "$SMPROGRAMS\$StartMenuFolder"
		CreateShortCut "$SMPROGRAMS\$StartMenuFolder\$(LANG_CaptionRecorder).lnk" "$INSTDIR\Recorder.exe" \
		"" "$INSTDIR\Recorder.exe" 0 SW_SHOWNORMAL \
		"" $(LANG_CaptionRecorder)
		CreateShortCut "$SMPROGRAMS\$StartMenuFolder\$(LANG_CaptionPlayerClassic).lnk" "$INSTDIR\Player.exe" \
		"" "$INSTDIR\Player.exe" 0 SW_SHOWNORMAL \
		"" $(LANG_CaptionPlayerClassic)
		CreateShortCut "$SMPROGRAMS\$StartMenuFolder\$(LANG_CaptionPlayerPlus).lnk" "$INSTDIR\PlayerPlus.exe" \
		"" "$INSTDIR\PlayerPlus.exe" 0 SW_SHOWNORMAL \
		"" $(LANG_CaptionPlayerPlus)
		CreateShortCut "$SMPROGRAMS\$StartMenuFolder\$(LANG_CaptionProducer).lnk" "$INSTDIR\Producer.exe" \
		"" "$INSTDIR\Producer.exe" 0 SW_SHOWNORMAL \
		"" $(LANG_CaptionProducer)
		CreateShortCut "$SMPROGRAMS\$StartMenuFolder\$(LANG_CaptionUninstall).lnk" "$INSTDIR\Uninstall.exe" \
		"" "$INSTDIR\Uninstall.exe" 0 SW_SHOWNORMAL \
		"" $(LANG_CaptionUninstall)
		CreateShortCut "$SMPROGRAMS\$StartMenuFolder\$(LANG_Website).lnk" "${PRODUCT_WEB_SITE}" \
		"" "$INSTDIR\Web.ico" 0 SW_SHOWNORMAL \
		"" $(LANG_CaptionWebsite)
		!insertmacro MUI_STARTMENU_WRITE_END
	SectionEnd

	# CamStudio codec dll file - written to the system or system32 directory if chosen
	Section "${PRODUCT_NAME} $(LANG_Codec) ${CODEC_VERSION}" SecCodec
		SetOutPath $SYSDIR
		File "CamCodec.dll"
	SectionEnd
	
	# Actions performed after the installation has been completed
	Section -Post
		WriteUninstaller "$INSTDIR\Uninstall.exe" ; Create the uninstaller program
		WriteRegStr HKLM "${PRODUCT_DIR_REGKEY}" "" "$INSTDIR\Recorder.exe"
		WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayName" "$(^Name)"
		WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "UninstallString" "$INSTDIR\Uninstall.exe"
		WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayIcon" "$INSTDIR\Recorder.exe"
		WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayVersion" "${PRODUCT_VERSION}"
		WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "URLInfoAbout" "${PRODUCT_WEB_SITE}"
		WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "Publisher" "${PRODUCT_PUBLISHER}"
	SectionEnd
 
;--------------------------------
; Uninstaller Section

	# Uninstall path safety check - this makes sure the path isn't any of the following...
	# otherwise the user may get a blank hard drive due to an improper registry key.
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
	StrCpy $R0 $INSTDIR "" -23
	StrCmp $R0 "\Documents and Settings" bad
	StrCpy $R0 $INSTDIR "" -13
	StrCmp $R0 "\My Documents" bad done
	bad:
		MessageBox MB_OK|MB_ICONSTOP $(LANG_UninstallError)
		Abort
	done:
	!macroend
	
	# Macro to remove uninstaller only IF all components are gone
	!macro RemoveUninstaller
		!insertmacro MUI_STARTMENU_GETFOLDER Application $StartMenuFolder
	
		;The way this works, +1 means simply go to the next instruction.
		;So unless the files DO exist (in which case it jumps to LeaveUninstaller and does nothing),
		;it continues to next instruction - doing exactly what we want.
		IfFileExists "$INSTDIR\Recorder.exe" LeaveUninstaller +1
		IfFileExists "$SYSDIR\CamCodec.dll" LeaveUninstaller +1
			Delete "$INSTDIR\Uninstall.exe"
			RMDir "$INSTDIR"
			Delete "$SMPROGRAMS\$StartMenuFolder\$(LANG_CaptionUninstall).lnk"
			Delete "$SMPROGRAMS\$StartMenuFolder\$(LANG_Website).lnk"
			RMDir "$SMPROGRAMS\$StartMenuFolder"
			DeleteRegKey ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}"
			DeleteRegKey HKLM "${PRODUCT_DIR_REGKEY}"
		LeaveUninstaller:
	!macroend
	
	# Uninstalls the CamStudio Core files
	Section "un.${PRODUCT_NAME} ${PRODUCT_VERSION}" UnSecCoreFiles
		!insertmacro BadPathsCheck ; Make sure the uninstall path isn't dangerous
		!insertmacro MUI_STARTMENU_GETFOLDER Application $StartMenuFolder ; Get the application's start menu folder so the variable is available
		
		Delete "$INSTDIR\camstudio_cl.exe"
		Delete "$INSTDIR\default.shapes"
		Delete "$INSTDIR\dialog.bmp"
		Delete "$INSTDIR\help.htm"
		Delete "$INSTDIR\helpProducer.htm"
		Delete "$INSTDIR\hook.dll"
		Delete "$INSTDIR\Player.exe"
		Delete "$INSTDIR\PlayerPlus.exe"
		Delete "$INSTDIR\Producer.exe"
		Delete "$INSTDIR\Recorder.exe"
		Delete "$INSTDIR\stlport_vc645.dll"
		Delete "$INSTDIR\testsnd.wav"
		Delete "$INSTDIR\WhatsNew.htm"
		Delete "$INSTDIR\Web.ico"
		# Not removing INSTDIR here because the finalizer does that
		Delete "$INSTDIR\controller\controller.ini"
		Delete "$INSTDIR\controller\controller_backup.ini"
		Delete "$INSTDIR\controller\leftpiece.bmp"
		Delete "$INSTDIR\controller\loadnode.bmp"
		Delete "$INSTDIR\controller\loadpiece.bmp"
		Delete "$INSTDIR\controller\pausebutton.bmp"
		Delete "$INSTDIR\controller\pausebutton2.bmp"
		Delete "$INSTDIR\controller\playbutton.bmp"
		Delete "$INSTDIR\controller\playbutton2.bmp"
		Delete "$INSTDIR\controller\rightpiece.bmp"
		Delete "$INSTDIR\controller\stopbutton.bmp"
		Delete "$INSTDIR\controller\stopbutton2.bmp"
		RMDir "$INSTDIR\controller"
		Delete "$INSTDIR\help_files\filelist.xml"
		Delete "$INSTDIR\help_files\image001.gif"
		Delete "$INSTDIR\help_files\image002.gif"
		Delete "$INSTDIR\help_files\image003.gif"
		Delete "$INSTDIR\help_files\image004.png"
		Delete "$INSTDIR\help_files\image005.jpg"
		Delete "$INSTDIR\help_files\image006.jpg"
		Delete "$INSTDIR\help_files\image007.jpg"
		Delete "$INSTDIR\help_files\image008.png"
		Delete "$INSTDIR\help_files\image009.png"
		Delete "$INSTDIR\help_files\image010.png"
		Delete "$INSTDIR\help_files\image011.png"
		Delete "$INSTDIR\help_files\image012.png"
		Delete "$INSTDIR\help_files\image013.png"
		Delete "$INSTDIR\help_files\image014.png"
		Delete "$INSTDIR\help_files\image015.png"
		Delete "$INSTDIR\help_files\image016.png"
		Delete "$INSTDIR\help_files\image017.png"
		Delete "$INSTDIR\help_files\image018.png"
		Delete "$INSTDIR\help_files\image019.png"
		Delete "$INSTDIR\help_files\image020.png"
		Delete "$INSTDIR\help_files\image021.png"
		Delete "$INSTDIR\help_files\image022.gif"
		Delete "$INSTDIR\help_files\image023.png"
		Delete "$INSTDIR\help_files\image024.png"
		Delete "$INSTDIR\help_files\image025.png"
		Delete "$INSTDIR\help_files\image026.png"
		Delete "$INSTDIR\help_files\image027.png"
		Delete "$INSTDIR\help_files\image028.png"
		Delete "$INSTDIR\help_files\image029.png"
		Delete "$INSTDIR\help_files\image030.png"
		Delete "$INSTDIR\help_files\image031.png"
		Delete "$INSTDIR\help_files\image032.png"
		Delete "$INSTDIR\help_files\image033.png"
		Delete "$INSTDIR\help_files\image034.png"
		Delete "$INSTDIR\help_files\image035.png"
		Delete "$INSTDIR\help_files\image036.png"
		Delete "$INSTDIR\help_files\image037.png"
		Delete "$INSTDIR\help_files\image038.png"
		Delete "$INSTDIR\help_files\image039.gif"
		Delete "$INSTDIR\help_files\image040.gif"
		Delete "$INSTDIR\help_files\image041.png"
		Delete "$INSTDIR\help_files\image042.png"
		Delete "$INSTDIR\help_files\image043.png"
		Delete "$INSTDIR\help_files\image044.png"
		Delete "$INSTDIR\help_files\image045.png"
		Delete "$INSTDIR\help_files\image046.png"
		Delete "$INSTDIR\help_files\image047.png"
		Delete "$INSTDIR\help_files\image048.png"
		Delete "$INSTDIR\help_files\image049.png"
		Delete "$INSTDIR\help_files\image050.png"
		Delete "$INSTDIR\help_files\image051.png"
		Delete "$INSTDIR\help_files\image052.png"
		Delete "$INSTDIR\help_files\image053.png"
		Delete "$INSTDIR\help_files\image054.png"
		Delete "$INSTDIR\help_files\image055.png"
		Delete "$INSTDIR\help_files\image056.png"
		Delete "$INSTDIR\help_files\image057.png"
		Delete "$INSTDIR\help_files\image058.png"
		Delete "$INSTDIR\help_files\image059.png"
		Delete "$INSTDIR\help_files\image060.png"
		Delete "$INSTDIR\help_files\image061.png"
		Delete "$INSTDIR\help_files\image062.png"
		Delete "$INSTDIR\help_files\image063.png"
		Delete "$INSTDIR\help_files\image064.png"
		Delete "$INSTDIR\help_files\image065.png"
		Delete "$INSTDIR\help_files\image066.png"
		Delete "$INSTDIR\help_files\image067.gif"
		Delete "$INSTDIR\help_files\oledata.mso"
		RMDir "$INSTDIR\help_files"
		Delete "$INSTDIR\helpProducer_files\filelist.xml"
		Delete "$INSTDIR\helpProducer_files\image001.gif"
		Delete "$INSTDIR\helpProducer_files\image002.gif"
		Delete "$INSTDIR\helpProducer_files\image003.gif"
		Delete "$INSTDIR\helpProducer_files\image004.png"
		Delete "$INSTDIR\helpProducer_files\image005.jpg"
		Delete "$INSTDIR\helpProducer_files\image006.png"
		Delete "$INSTDIR\helpProducer_files\image007.png"
		Delete "$INSTDIR\helpProducer_files\image008.png"
		Delete "$INSTDIR\helpProducer_files\image009.png"
		Delete "$INSTDIR\helpProducer_files\image010.png"
		Delete "$INSTDIR\helpProducer_files\oledata.mso"
		RMDir "$INSTDIR\helpProducer_files"
		
		Delete "$SMPROGRAMS\$StartMenuFolder\$(LANG_CaptionRecorder).lnk"
		Delete "$SMPROGRAMS\$StartMenuFolder\$(LANG_CaptionPlayerClassic).lnk"
		Delete "$SMPROGRAMS\$StartMenuFolder\$(LANG_CaptionPlayerPlus).lnk"
		Delete "$SMPROGRAMS\$StartMenuFolder\$(LANG_CaptionProducer).lnk"
		# Not removing StartMenuFolder here because the finalizer does that
		
		!insertmacro RemoveUninstaller ; Remove the uninstaller if it should be removed
	SectionEnd
	
	# Uninstalls the CamStudio Lossless Video Codec
	Section "un.${PRODUCT_NAME} $(LANG_Codec) ${CODEC_VERSION}" UnSecCodec
		Delete "$SYSDIR\CamCodec.dll"
		!insertmacro RemoveUninstaller ; Remove the uninstaller if it should be removed
	SectionEnd
	
	# Optionally uninstalls user settings and preferences
	Section /o "un.${PRODUCT_NAME} $(LANG_Settings)" UnSecSettings ; We have /o here to uncheck this by default
		Delete "$INSTDIR\CamData.ini"
		Delete "$INSTDIR\CamLayout.ini"
		Delete "$INSTDIR\CamShapes.ini"
		Delete "$INSTDIR\CamStudio.ini"
		Delete "$INSTDIR\CamStudio.Producer.Data.ini"
		Delete "$INSTDIR\CamStudio.Producer.ini"
		!insertmacro RemoveUninstaller ; Remove the uninstaller if it should be removed
	SectionEnd

;--------------------------------
; Descriptions

	# Assign language strings to installer sections
	!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
	!insertmacro MUI_DESCRIPTION_TEXT ${SecCoreFiles} $(LANG_SecCoreFiles)
	!insertmacro MUI_DESCRIPTION_TEXT ${SecCodec} $(LANG_SecCodec)
	!insertmacro MUI_FUNCTION_DESCRIPTION_END
	
	# Assign language strings to uninstaller sections
	!insertmacro MUI_UNFUNCTION_DESCRIPTION_BEGIN
	!insertmacro MUI_DESCRIPTION_TEXT ${UnSecCoreFiles} $(LANG_UnSecCoreFiles)
	!insertmacro MUI_DESCRIPTION_TEXT ${UnSecCodec} $(LANG_UnSecCodec)
	!insertmacro MUI_DESCRIPTION_TEXT ${UnSecSettings} $(LANG_UnSecSettings)
	!insertmacro MUI_UNFUNCTION_DESCRIPTION_END