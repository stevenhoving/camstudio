#pragma once

#include <string>
#include <windef.h>

struct window_data
{
    unsigned int process_id;
    std::wstring process_name;
    std::wstring window_title;
    HWND hwnd;
};