#pragma once

#include <memory>

namespace Gdiplus
{
    struct GdiplusStartupInput;
} // namespace Gdiplus

class gdi
{
public:
    // initialize gdi plus
    gdi();
    // uninitialize gdi plus
    ~gdi();

    std::unique_ptr<Gdiplus::GdiplusStartupInput> input;
    ULONG_PTR gdiplusToken;
};
