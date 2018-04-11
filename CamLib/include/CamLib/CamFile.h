#pragma once

#include <string>

static const int USE_WINDOWS_TEMP_DIR = 0;
static const int USE_INSTALLED_DIR = 1;
static const int USE_USER_SPECIFIED_DIR = 2;


enum class dir_access : int
{
    windows_temp_dir = 0,
    install_dir = 1,
    user_specified_dir = 2
};

std::wstring get_temp_folder(dir_access iAccess, const std::wstring &folder = _T(""), bool bOut = false);
std::wstring get_prog_path();
std::wstring get_my_video_path();
std::wstring get_my_documents_path();


CString GetTempFolder(int iAccess, const CString &strFolder = _T(""), bool bOut = false);
CString GetProgPath();
CString FindExtension(const CString &name);
CString GetMyVideoPath();
CString GetAppDataPath();
CString GetMyDocumentsPath();

bool DoesFileExist(const CString &name);
bool DoesDefaultOutDirExist(const CString &dir);
