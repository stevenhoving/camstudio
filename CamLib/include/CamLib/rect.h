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

#include <atltypes.h>

/* \note this function assumes that rect_boundary is always bigger than rect */
static CRect clamp_rect(CRect rect, const CRect rect_boundary)
{
    if (rect.left < rect_boundary.left)
        rect += CPoint(rect_boundary.left - rect.left, 0);

    if (rect.top < rect_boundary.top)
        rect += CPoint(0, rect_boundary.top - rect.top);

    if (rect.right > rect_boundary.right)
        rect += CPoint(rect_boundary.right - rect.right, 0);

    if (rect.bottom > rect_boundary.bottom)
        rect += CPoint(0, rect_boundary.bottom - rect.bottom);

    return rect;
}

CRect merge_rect(const CRect a, const CRect b)
{
    CRect result = a;
    if (b.left < a.left)
        result.left = b.left;

    if (b.top < a.top)
        result.top = b.top;

    if (b.right > a.right)
        result.right = b.right;

    if (b.bottom > a.bottom)
        result.bottom = b.bottom;
    return result;
}

CRect align_rect(const CRect rect)
{
    CRect result = rect;
    if (result.Width() % 2 != 0)
        result.right++;

    if (result.Height() % 2 != 0)
        result.bottom++;

    return result;
}
