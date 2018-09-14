; Copyright(C) 2018  Steven Hoving
;
; This program is free software : you can redistribute it and/or modify
; it under the terms of the GNU General Public License as published by
; the Free Software Foundation, either version 3 of the License, or
; (at your option) any later version.
;
; This program is distributed in the hope that it will be useful,
; but WITHOUT ANY WARRANTY; without even the implied warranty of
; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
; GNU General Public License for more details.
;
; You should have received a copy of the GNU General Public License
; along with this program.If not, see <https://www.gnu.org/licenses/>.

[Setup]
AppName=CamStudio
AppVersion=@CAMSTUDIO_VERSION@
AppCopyright=Copyright (C) 2018 Steven Hoving. Copyright (C) 2008 CamStudio Group & Contributors. Copyright (C) 2001 RenderSoft Software and Web Publishing.
AppPublisherURL=http://www.github.com/stevenhoving/camstudio
AppSupportURL=http://www.github.com/stevenhoving/camstudio
AppUpdatesURL=http://www.github.com/stevenhoving/camstudio
DefaultDirName={pf}\CamStudio
DefaultGroupName=CamStudio
UninstallDisplayIcon={app}\CamStudioRecorder.exe
Compression=lzma2
SolidCompression=yes
OutputDir=Installer
ArchitecturesAllowed=x64
ArchitecturesInstallIn64BitMode=x64
LicenseFile=license.txt

[Files]
Source: "CamStudioRecorder.exe"; DestDir: "{app}"
Source: "cam_hook.dll"; DestDir: "{app}"
Source: "avcodec-58.dll"; DestDir: "{app}"
Source: "avdevice-58.dll"; DestDir: "{app}"
Source: "avfilter-7.dll"; DestDir: "{app}"
Source: "avformat-58.dll"; DestDir: "{app}"
Source: "avutil-56.dll"; DestDir: "{app}"
Source: "postproc-55.dll"; DestDir: "{app}"
Source: "swresample-3.dll"; DestDir: "{app}"
Source: "swscale-5.dll"; DestDir: "{app}"
Source: "license.txt"; DestDir: "{app}"
Source: "README.md"; DestDir: "{app}"; Flags: isreadme

[Icons]
Name: "{group}\CamStudio Recorder"; Filename: "{app}\CamStudioRecorder.exe"

[UninstallDelete]
; Delete CamStudio setting files.
Type: filesandordirs; Name: "{localappdata}\CamStudio"
