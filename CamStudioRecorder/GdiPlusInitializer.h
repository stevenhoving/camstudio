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
 * along with this program.If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once

#include <memory>

#include <CamCapture/cam_gdiplus_fwd.h>

class gdi_plus
{
public:
    // initialize gdi plus
    gdi_plus();
    // uninitialize gdi plus
    ~gdi_plus();

    gdi_plus(const gdi_plus &) = delete;
    gdi_plus &operator = (const gdi_plus &) = delete;


    std::unique_ptr<Gdiplus::GdiplusStartupInput> input;
    ULONG_PTR gdiplusToken;
};
