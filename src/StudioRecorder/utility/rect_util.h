#pragma once

#include <CamCapture/cam_rect.h>

namespace utility
{

static auto from_rect(const CRect &rect) -> cam::rect<int>
{
    return { rect.left, rect.top, rect.right, rect.bottom };
}

static auto from_rect(const cam::rect<int> &rect) -> CRect
{
    return { rect.left(), rect.top(), rect.right(), rect.bottom() };
}

} // namespace utility

