#include "stdafx.h"
#include "GdiPlusInitializer.h"

// for using std::min, std::max in gdiplus
#include <algorithm>
using namespace std;
#define byte uint8_t
#include <gdiplus.h>

#pragma comment(lib, "Gdiplus.lib")

gdi::gdi()
{
    input = std::make_unique<Gdiplus::GdiplusStartupInput>();
    Gdiplus::GdiplusStartup(&gdiplusToken, input.get(), nullptr);
}

gdi::~gdi()
{
    Gdiplus::GdiplusShutdown(gdiplusToken);
}
