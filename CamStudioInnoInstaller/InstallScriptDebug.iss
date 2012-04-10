;#######################################################################
; NOTE: The value of AppId uniquely identifies this application.
; Do not use the same AppId value in installers for other applications.
; (To generate a new GUID, click Tools | Generate GUID inside the IDE.)
;#######################################################################

[Setup]
AppId={{FD9C31B6-F572-414D-81E3-89368C97A125}
AppName=CamStudio OSS Desktop Recorder
AppVerName=CamStudio 2.6 Beta (310 Testers only)
AppPublisher=CamStudio Open Source Dev Team
AppPublisherURL=http://camstudio.org/
AppSupportURL=http://camstudio.org/forum/
AppUpdatesURL=http://camstudio.org/
DefaultDirName={pf}\CamStudio
SourceDir=../
DefaultGroupName=CamStudio
AllowNoIcons=true
LicenseFile=CamStudio\license.txt
OutputBaseFilename=CamStudio_v2.6b_r310-Setup
Compression=lzma/ultra64
SolidCompression=true
InternalCompressLevel=ultra64
WizardImageFile=compiler:WizModernImage-IS.bmp
WizardSmallImageFile=compiler:WizModernSmallImage-IS.bmp
VersionInfoCompany=CamStudio Open Source Dev Team
VersionInfoCopyright=CamStudio Dev Team
VersionInfoProductName=CamStudio
AppVersion=2.6 Beta (310 Testers only)
UninstallDisplayName=CamStudio OSS Desktop Recorder
OutputDir=CamStudioInnoInstaller\Output
MergeDuplicateFiles=true

[Languages]
Name: english; MessagesFile: compiler:Default.isl

[Tasks]
Name: desktopicon; Description: {cm:CreateDesktopIcon}; GroupDescription: {cm:AdditionalIcons}; Flags: unchecked

[Files]
Source: CamStudio\Debug\CamCodec.dll; DestDir: {sys}; Flags: restartreplace 32bit; Components: arch32Bit
Source: CamStudio\Debug\CamCodec.dll; DestDir: {sys}; Flags: restartreplace 64bit; Components: arch64Bit

;############################################################################################
;The codec supports 32bit under system32 folder and x64 under the sysWOW64 Folder..
;These folders are selected by the use of the {sys} under the OS's specific architecture.
;The code added should help the installer figure out which folder camcodec.dll should go in.
;############################################################################################

Source: CamStudio\Debug\*.*; Excludes: CamStudio\Debug\CamCodec.dll; DestDir: {app}; Flags: ignoreversion recursesubdirs  createallsubdirs
Source: CamStudio\Debug\controller\*.*; DestDir: {app}\controller; Flags: ignoreversion recursesubdirs

;####################################################################
; NOTE: Don't use "Flags: ignoreversion" on any shared system files.
;####################################################################

[Components]
Name: arch32Bit; Description: Install CamCodec 1.5; Types: full custom; Flags: checkablealone; Check: IsWin32
Name: arch64Bit; Description: Install CamCodec 1.5; Types: full custom; Flags: checkablealone; Check: IsWin64


[Icons]
Name: {group}\CamStudio; Filename: {app}\Recorder.exe
Name: {group}\{cm:UninstallProgram,CamStudio}; Filename: {uninstallexe}
Name: {commondesktop}\CamStudio; Filename: {app}\Recorder.exe; Tasks: desktopicon

[Run]
Filename: {app}\Recorder.exe; Description: {cm:LaunchProgram,CamStudio}; Flags: nowait postinstall skipifsilent

[Code]
function IsWin32: Boolean;
begin
 Result := not IsWin64;
end;
