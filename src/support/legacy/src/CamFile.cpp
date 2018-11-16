#include "stdafx.h"
#include "CamLib/CamFile.h"
#include <shlobj.h>

std::filesystem::path get_my_documents_path()
{
    wchar_t szPath[2014] = {};

    if (FAILED(SHGetFolderPathW(nullptr, CSIDL_PERSONAL, nullptr, 0, szPath)))
        throw std::runtime_error("Unable to get my documents path");

    return szPath;
}
