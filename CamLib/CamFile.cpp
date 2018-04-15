#include "stdafx.h"
#include "CamLib/CamFile.h"
#include <shlobj.h>
#include <filesystem>

/////////////////////////////////////////////////////////////////////////////
// GetTempPath
// n.b. Overloads DWORD GetTempPath(DWORD nBufferLength, LPTSTR lpBuffer)
// There is a conversion from LPTSTR to CString
// TODO: Rename; this is needlessly dangerous
// TODO: GetTempFolder does not contain the temp folder but returns the udf target folder. Hence the function
// name is not correct..!
/////////////////////////////////////////////////////////////////////////////
CString GetTempFolder(int iAccess, const CString &strFolder, bool bOut)
{
    if (iAccess == USE_USER_SPECIFIED_DIR)
    {
        const CString &tempdir = strFolder;

        // Verify the chosen temp path is valid
        WIN32_FIND_DATA wfd;
        ::ZeroMemory(&wfd, sizeof(wfd));
        HANDLE hdir = ::FindFirstFile(LPCTSTR(tempdir), &wfd);
        if (!hdir)
        {
            return GetProgPath();
        }
        ::FindClose(hdir);

        // If valid directory, return Windows\temp as temp directory
        if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            return tempdir;
        }
    }
    // handles USE_INSTALLED_DIR value of iAccess
    // attempts to create <CamStudio root dir>\\temp dir
    // if failed to create just return CamStudio root dir
    if (iAccess == USE_INSTALLED_DIR)
    {
        CString tempdir;
        // tempdir.Format("%s\\Videos", GetProgPath());
        if (bOut)
        {
            tempdir.Format(_T("%s\\My CamStudio Videos"), GetMyDocumentsPath().GetString());
        }
        else
        {
            tempdir.Format(_T("%s\\My CamStudio Temp Files"), GetMyDocumentsPath().GetString());
        }
        if (!CreateDirectory(tempdir, nullptr))
        {
            DWORD err = GetLastError();
            if (ERROR_ALREADY_EXISTS == err)
            {
                return tempdir;
            }
            if (ERROR_PATH_NOT_FOUND == err)
            {
                return GetMyDocumentsPath();
            }
        }
        else
        {
            return tempdir;
        }
    }
    TCHAR tempPath[_MAX_PATH + 1] = {'\0'};
    if (GetTempPath(_MAX_PATH, tempPath))
    {
        CString tempdir;
        tempdir = tempPath;
        if (0 < tempdir.ReverseFind('\\'))
        {
            tempdir = tempdir.Left(tempdir.ReverseFind('\\'));
        }
        return tempdir;
    }

    // This code looks for an old style temp directory. NOT standard windows.
    TCHAR dirx[_MAX_PATH];
    ::GetWindowsDirectory(dirx, _MAX_PATH);
    CString tempdir;
    tempdir.Format(_T("%s\\temp"), dirx);

    // Verify the chosen temp path is valid

    WIN32_FIND_DATA wfd;
    ::ZeroMemory(&wfd, sizeof(wfd));
    HANDLE hdir = ::FindFirstFile(LPCTSTR(tempdir), &wfd);
    if (!hdir)
    {
        return GetProgPath();
    }
    ::FindClose(hdir);

    // If valid directory, return Windows\temp as temp directory
    if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
    {
        return tempdir;
    }

    // else return program path as temp directory
    // iAccess = USE_INSTALLED_DIR;    // TODO: nonsense; iAccess is NOT an output
    return GetProgPath();
}

std::wstring get_temp_folder(dir_access iAccess, const std::wstring &folder /*= ""*/, bool bOut /*= false*/)
{
    switch (iAccess)
    {
        case dir_access::user_specified_dir:
        {
            // fallback behavior, use application root when user supplied temp folder does not exist.
            if (!std::experimental::filesystem::exists(folder))
                return get_prog_path();

            const auto status = std::experimental::filesystem::status(folder);

            // If valid directory, return the user supplied directory as temp directory.
            if (status.type() == std::experimental::filesystem::file_type::directory)
                return folder;
            else
                throw std::runtime_error("failed to get user supplied temp folder");
        } break;

        case dir_access::install_dir:
        {
            // handles USE_INSTALLED_DIR value of iAccess
            // attempts to create <CamStudio root dir>\\temp dir
            // if failed to create just return CamStudio root dir

            std::wstring tempdir;
            if (bOut)
            {
                tempdir = get_my_documents_path() + _T("\\My CamStudio Videos");
            }
            else
            {
                tempdir = get_my_documents_path() + _T("\\My CamStudio Temp Files");
            }

            if (std::experimental::filesystem::exists(tempdir))
                return tempdir;

            if (std::experimental::filesystem::create_directories(tempdir))
                return tempdir;

            // \note this is actually the failure path...
            return get_my_documents_path();
        } break;
    }

    wchar_t tempPath[MAX_PATH + 1] = {0};
    if (GetTempPathW(MAX_PATH, tempPath))
    {
        std::wstring tempdir = tempPath;
        if (tempdir.back() == '\\')
            tempdir.resize(tempdir.size() - 1);
        return tempdir;
    }

    // This code looks for an old style temp directory. NOT standard windows.
    wchar_t dirx[_MAX_PATH];
    ::GetWindowsDirectoryW(dirx, _MAX_PATH);
    std::wstring tempdir = dirx;
    tempdir += _T("\\temp");

    // Verify the chosen temp path is valid
    if (!std::experimental::filesystem::exists(tempdir))
        return get_prog_path();

    // If valid directory, return Windows\temp as temp directory
    if (std::experimental::filesystem::status(tempdir).type() == std::experimental::filesystem::file_type::directory)
        return tempdir;

    // else return program path as temp directory
    // iAccess = USE_INSTALLED_DIR;    // TODO: nonsense; iAccess is NOT an output
    return get_prog_path();
}

CString GetProgPath()
{
    // get root of Camstudio application
    TCHAR szTemp[300];
    ::GetModuleFileName(nullptr, szTemp, 300);
    CString path = szTemp;
    if (0 < path.ReverseFind('\\'))
    {
        path = path.Left(path.ReverseFind('\\'));
    }
    return path;
}

std::wstring get_prog_path()
{
    // get root of Camstudio application
    wchar_t szTemp[300];
    ::GetModuleFileNameW(nullptr, szTemp, 300);
    std::wstring path = szTemp;
    const auto found = path.rfind('\\');
    if (found != std::wstring::npos)
    {
        path = path.substr(0, found);
    }
    return path;
}

#ifndef CSIDL_MYVIDEO
#define CSIDL_MYVIDEO 0x000e // "My Videos" folder
#endif

std::wstring get_my_video_path()
{
    wchar_t szPath[MAX_PATH + 100] = { 0 };

    // Get the user's video path
    if (SUCCEEDED(SHGetFolderPath(nullptr, CSIDL_MYVIDEO, nullptr, 0, szPath)))
    {
        return szPath;
    }
    else if (SUCCEEDED(SHGetFolderPath(nullptr, CSIDL_PERSONAL, nullptr, 0, szPath)))
    {
        return szPath;
    }

    throw std::runtime_error("error while getting the users video path");

    return std::wstring();
}



CString GetMyVideoPath()
{
    // Get the user's video path
    int folder = CSIDL_MYVIDEO;

    TCHAR szPath[MAX_PATH + 100];
    szPath[0] = 0;
    CString path = szPath;

    if (SUCCEEDED(SHGetFolderPath(nullptr, folder, nullptr, 0, szPath)))
    {
        path = szPath;
    }
    else if (SUCCEEDED(SHGetFolderPath(nullptr, CSIDL_PERSONAL, nullptr, 0, szPath)))
    {
        path = szPath;
    }
    return path;
}

CString GetMyDocumentsPath()
{
    // Get the user's video path
    int folder = CSIDL_PERSONAL;

    TCHAR szPath[MAX_PATH + 100];
    szPath[0] = 0;
    CString path = szPath;

    if (SUCCEEDED(SHGetFolderPath(nullptr, folder, nullptr, 0, szPath)))
    {
        path = szPath;
    }
    /*else if (SUCCEEDED(SHGetFolderPath(nullptr, CSIDL_PERSONAL, 0, 0, szPath)))
    {
        path = szPath;
    }*/
    return path;
}

std::wstring get_my_documents_path()
{
    wchar_t szPath[MAX_PATH + 100] = { 0 };

    if (FAILED(SHGetFolderPathW(nullptr, CSIDL_PERSONAL, nullptr, 0, szPath)))
        throw std::runtime_error("Unable to get my documents path");

    return szPath;
}

CString GetAppDataPath()
{
    int folder = CSIDL_APPDATA;

    TCHAR szPath[MAX_PATH + 100];
    szPath[0] = 0;
    CString path = szPath;

    if (SUCCEEDED(SHGetFolderPath(nullptr, folder, nullptr, 0, szPath)))
    {
        path = szPath;
    }
    else if (SUCCEEDED(SHGetFolderPath(nullptr, CSIDL_PERSONAL, nullptr, 0, szPath)))
    {
        path = szPath;
    }
    return path;
}

bool DoesFileExist(const CString &name)
{
    return GetFileAttributes(name.GetString()) != 0xffffffff;
}

bool DoesDefaultOutDirExist(const CString &dir)
{
    DWORD ftyp = GetFileAttributes(dir);
    if (ftyp == INVALID_FILE_ATTRIBUTES)
    {
        return false; // something is wrong with the path
    }

    if (ftyp & FILE_ATTRIBUTE_DIRECTORY)
    {
        return true; // this is a directory
    }

    return false; // this is not a directory
}
CString FindExtension(const CString &name)
{
    int len = name.GetLength();
    for (int i = len - 1; i >= 0; i--)
    {
        if (name[i] == '.')
        {
            return name.Mid(i + 1);
        }
    }
    return "";
}
