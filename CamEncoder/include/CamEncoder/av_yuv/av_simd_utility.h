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

#include <emmintrin.h>

#if defined(__clang__)
#define COMPILER_CLANG 1
#define COMPILER_MSVC 0
#elif defined(_MSC_VER)
#define COMPILER_CLANG 0
#define COMPILER_MSVC 1
#endif

#if COMPILER_MSVC
#define __packed_struct(x) __pragma(pack(push, 1)) struct x
#define __packed_struct_end __pragma(pack(pop))
#define __no_unroll __pragma(loop(no_vector))
#else
#define __packed_struct(x) struct __attribute__((__packed__)) x
#define __packed_struct_end
#define __no_unroll _Pragma("nounroll")
#endif

#if 0
#if defined(_MSC_VER) && !defined(__clang__)
#define __packed_struct(x) __pragma(pack(push, 1)) struct x
#define __packed_struct_end __pragma(pack(pop))

#define __no_unroll __pragma(loop(no_vector))
#else
#define __packed_struct(x) struct __attribute__((__packed__)) x
#define __packed_struct_end

#define __no_unroll _Pragma("nounroll")
#endif
#endif

namespace simd
{
template <typename T>
static inline T align_up(const T size, const T align) noexcept
{
    return ((size + (align - 1)) & ~(align - 1));
}

template <typename T>
static inline T align_down(const T size, const T align) noexcept
{
    return size & ~(align - 1);
}

} // namespace simd
