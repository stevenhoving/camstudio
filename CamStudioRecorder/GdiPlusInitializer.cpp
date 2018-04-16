/**
 * Copyright(C) 2018  Steven Hoving
 *
 * This program is free software : you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.If not, see < https://www.gnu.org/licenses/>.
 */

#include "stdafx.h"
#include "GdiPlusInitializer.h"

// for using std::min, std::max in gdiplus
#include <algorithm>
using namespace std;
#define byte uint8_t
#include <gdiplus.h>

gdi::gdi()
{
    input = std::make_unique<Gdiplus::GdiplusStartupInput>();
    Gdiplus::GdiplusStartup(&gdiplusToken, input.get(), nullptr);
}

gdi::~gdi()
{
    Gdiplus::GdiplusShutdown(gdiplusToken);
}
