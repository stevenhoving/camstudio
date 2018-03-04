#pragma once

static const int USE_WINDOWS_TEMP_DIR = 0;
static const int USE_INSTALLED_DIR = 1;
static const int USE_USER_SPECIFIED_DIR = 2;

CString GetTempFolder(int iAccess, const CString &strFolder = _T(""), bool bOut = false);
CString GetProgPath();
CString FindExtension(const CString &name);
CString GetMyVideoPath();
CString GetAppDataPath();
CString GetMyDocumentsPath();
bool DoesFileExist(const CString &name);
bool DoesDefaultOutDirExist(const CString &dir);
