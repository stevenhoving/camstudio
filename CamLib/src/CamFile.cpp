#include "stdafx.h"
#include "CamLib/CamFile.h"
#include <shlobj.h>

std::filesystem::path get_prog_path()
{
    // get root of Camstudio application
    wchar_t szTemp[1024] = {};
    ::GetModuleFileNameW(nullptr, szTemp, 1023);
    std::filesystem::path program_path(szTemp);
    return program_path.remove_filename();

    //std::wstring path = szTemp;
    //const auto found = path.rfind('\\');
    //if (found != std::wstring::npos)
    //{
    //    path = path.substr(0, found);
    //}
    //return path;
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

std::filesystem::path get_my_documents_path()
{
    wchar_t szPath[2014] = {};

    if (FAILED(SHGetFolderPathW(nullptr, CSIDL_PERSONAL, nullptr, 0, szPath)))
        throw std::runtime_error("Unable to get my documents path");

    return szPath;
}
