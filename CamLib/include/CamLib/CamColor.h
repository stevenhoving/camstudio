#pragma once

#include <wtypes.h>
#include <cstdint>

static DWORD COLORREFtoARGB(COLORREF color, BYTE alpha)
{
    uint32_t red = GetRValue(color);
    uint32_t green = GetGValue(color);
    uint32_t blue = GetBValue(color);
    uint32_t result = alpha << 24;
    result |= red << 16;
    result |= green << 8;
    result |= blue;
    return result;
}
