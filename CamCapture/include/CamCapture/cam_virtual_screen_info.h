#pragma once

#include <CamCapture/cam_rect.h>

namespace cam
{
struct virtual_screen_info
{
    // the virtual screen rect
    cam::rect<int> size{ 0, 0, 0, 0 };
    // the bits per pixel
    int bpp{ 0 };
    // the monitor count
    int monitor_count{ 0 };
};

virtual_screen_info get_virtual_screen_info();
} // namespace cam
