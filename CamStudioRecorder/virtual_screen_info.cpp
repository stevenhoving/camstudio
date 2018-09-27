#include "stdafx.h"
#include "virtual_screen_info.h"
#include <windows.h>

virtual_screen_info get_virtual_screen_info()
{
    virtual_screen_info result{};

    HDC hScreenDC = ::GetDC(nullptr);
    const auto bpp = ::GetDeviceCaps(hScreenDC, BITSPIXEL);
    ::ReleaseDC(nullptr, hScreenDC);

    const auto max_x = ::GetSystemMetrics(SM_CXVIRTUALSCREEN);
    const auto max_y = ::GetSystemMetrics(SM_CYVIRTUALSCREEN);
    const auto min_x = ::GetSystemMetrics(SM_XVIRTUALSCREEN);
    const auto min_y = ::GetSystemMetrics(SM_YVIRTUALSCREEN);
    const auto monitor_count = ::GetSystemMetrics(SM_CMONITORS);

    result.bpp = bpp;
    result.size = { min_x, min_y, min_x + max_x, min_y + max_y };
    result.monitor_count = monitor_count;
    return result;
}