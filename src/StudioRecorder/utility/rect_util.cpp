/**
 * Copyright(C) 2018 - 2020  Steven Hoving
 *
 * This program is free software : you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#include "stdafx.h"
#include "utility/rect_util.h"

#include <screen_capture/cam_rect.h>

namespace utility
{

auto from_rect(const CRect &rect) -> cam::rect<int>
{
    return { rect.left, rect.top, rect.right, rect.bottom };
}

auto from_rect(const cam::rect<int> &rect) -> CRect
{
    return { rect.left(), rect.top(), rect.right(), rect.bottom() };
}

} // namespace utility

