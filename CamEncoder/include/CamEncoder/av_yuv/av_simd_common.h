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

#pragma once

#include "av_simd_vec.h"

namespace simd
{
// *y++ = ((66 * r1 + 129 * g1 + 25 * b1) >> 8) + 16;
static const vec3 y_mul = simd::vec3_set(25, 129, 66);

// *u++ = ((-38 * r1 + -74 * g1 + 112 * b1) >> 8) + 128;
static const vec3 u_mul = simd::vec3_set(112, -74, -38);

// *v++ = ((112 * r1 + -94 * g1 + -18 * b1) >> 8) + 128;
static const vec3 v_mul = simd::vec3_set(-18, -94, 112);

static const auto y_add = _mm_set1_epi8(16);
static const auto uv_add = _mm_set1_epi16(128);

} // namespace simd
