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

#include <xmmintrin.h>
#include <emmintrin.h>
#include <immintrin.h>
#include <tmmintrin.h>
#include <cstdint>

namespace simd
{
struct vec3
{
    __m128i b, g, r;
};

struct vec2
{
    __m128i b, g;
};

constexpr auto mask = (char)0x80;

static __forceinline vec3 vec3_set(int16_t a, int16_t b, int16_t c)
{
    const auto value_a = _mm_set1_epi16(a);
    const auto value_b = _mm_set1_epi16(b);
    const auto value_c = _mm_set1_epi16(c);
    return {value_a, value_b, value_c};
}

static __forceinline vec3 vec3_set_shuffle_lo(int d, int c, int b, int a)
{
    const auto value_a = _mm_set_epi8(
        mask, mask,
        mask, mask,
        mask, mask,
        mask, mask,
        mask, d,
        mask, c,
        mask, b,
        mask, a);

    const auto value_b = _mm_set_epi8(
        mask, mask,
        mask, mask,
        mask, mask,
        mask, mask,
        mask, d+1,
        mask, c+1,
        mask, b+1,
        mask, a+1);

    const auto value_c = _mm_set_epi8(
        mask, mask,
        mask, mask,
        mask, mask,
        mask, mask,
        mask, d+2,
        mask, c+2,
        mask, b+2,
        mask, a+2);
    return {value_a, value_b, value_c};
}

static __forceinline vec3 vec3_set_shuffle_hi(int d, int c, int b, int a)
{
    const auto value_a = _mm_set_epi8(
        mask, d,
        mask, c,
        mask, b,
        mask, a,
        mask, mask,
        mask, mask,
        mask, mask,
        mask, mask);

    const auto value_b = _mm_set_epi8(
        mask, d+1,
        mask, c+1,
        mask, b+1,
        mask, a+1,
        mask, mask,
        mask, mask,
        mask, mask,
        mask, mask);

    const auto value_c = _mm_set_epi8(
        mask, d+2,
        mask, c+2,
        mask, b+2,
        mask, a+2,
        mask, mask,
        mask, mask,
        mask, mask,
        mask, mask);
    return {value_a, value_b, value_c};
}

static __forceinline vec3 vec3_set_pack_vec2(int h, int g, int f, int e, int d, int c, int b, int a)
{
    const auto value_a = _mm_set_epi8(
        mask, mask, mask, mask, mask, mask, mask, mask,
        h, g, f, e, d, c, b, a);

    const auto value_b = _mm_set_epi8(
        h, g, f, e, d, c, b, a,
        mask, mask, mask, mask, mask, mask, mask, mask
    );

    return {value_a, value_b, value_a};
}

static __forceinline vec2 vec2_set_pack(int h, int g, int f, int e, int d, int c, int b, int a)
{
    const auto value_a = _mm_set_epi8(
        mask, mask, mask, mask, mask, mask, mask, mask,
        h, g, f, e, d, c, b, a);

    const auto value_b = _mm_set_epi8(
        h, g, f, e, d, c, b, a,
        mask, mask, mask, mask, mask, mask, mask, mask
    );

    return {value_a, value_b};
}

static __forceinline vec3 vec3_mullo(vec3 data, vec3 mul)
{
    const auto b = _mm_mullo_epi16(data.b, mul.b);
    const auto g = _mm_mullo_epi16(data.g, mul.g);
    const auto r = _mm_mullo_epi16(data.r, mul.r);
    return {b, g, r};
}

static __forceinline vec3 vec3_vsum(vec3 data0, vec3 data1, vec3 data2)
{
    const auto a = _mm_add_epi16(data0.b, _mm_add_epi16(data0.g, data0.r));
    const auto b = _mm_add_epi16(data1.b, _mm_add_epi16(data1.g, data1.r));
    const auto c = _mm_add_epi16(data2.b, _mm_add_epi16(data2.g, data2.r));
    return {a, b, c};
}

static __forceinline vec2 vec3_vsum_vec2(vec3 data0, vec3 data1)
{
    const auto a = _mm_add_epi16(data0.b, _mm_add_epi16(data0.g, data0.r));
    const auto b = _mm_add_epi16(data1.b, _mm_add_epi16(data1.g, data1.r));
    return {a, b};
}

static __forceinline vec3 vec3_add(vec3 data, __m128i value)
{
    const auto b = _mm_add_epi16(data.b, value);
    const auto g = _mm_add_epi16(data.g, value);
    const auto r = _mm_add_epi16(data.r, value);
    return {b, g, r};
}

static __forceinline vec2 vec2_add(vec2 data, __m128i value)
{
    const auto b = _mm_add_epi16(data.b, value);
    const auto g = _mm_add_epi16(data.g, value);
    return {b, g};
}

static __forceinline vec3 vec3_srai(vec3 data, int shift)
{
    const auto b = _mm_srai_epi16(data.b, shift);
    const auto g = _mm_srai_epi16(data.g, shift);
    const auto r = _mm_srai_epi16(data.r, shift);
    return {b, g, r};
}

static __forceinline vec2 vec2_srai(vec2 data, int shift)
{
    const auto b = _mm_srai_epi16(data.b, shift);
    const auto g = _mm_srai_epi16(data.g, shift);
    return {b, g};
}

static __forceinline __m128i vec3_pack_interleave(vec3 data)
{
    return _mm_or_si128(data.b, _mm_or_si128(data.g, data.r));
}

static __forceinline __m128i vec2_pack_interleave(vec2 data)
{
    return _mm_or_si128(data.b, data.g);
}

static __forceinline vec3 vec3_interleave(vec3 data0, vec3 data1)
{
    const auto b = _mm_or_si128(data0.b, data1.b);
    const auto g = _mm_or_si128(data0.g, data1.g);
    const auto r = _mm_or_si128(data0.r, data1.r);
    return {b, g, r};
}

static __forceinline vec3 vec3_shuffle(vec3 data, vec3 order)
{
    const auto b = _mm_shuffle_epi8(data.b, order.b);
    const auto g = _mm_shuffle_epi8(data.g, order.g);
    const auto r = _mm_shuffle_epi8(data.r, order.r);
    return {b, g, r};
}

static __forceinline vec3 vec3_unpack(__m128i packed_bgr, vec3 order)
{
    const auto b = _mm_shuffle_epi8(packed_bgr, order.b);
    const auto g = _mm_shuffle_epi8(packed_bgr, order.g);
    const auto r = _mm_shuffle_epi8(packed_bgr, order.r);
    return {b, g, r};
}

static __forceinline vec2 vec2_shuffle(vec2 data, vec2 order)
{
    auto b = _mm_shuffle_epi8(data.b, order.b);
    auto g = _mm_shuffle_epi8(data.g, order.g);
    return {b, g};
}

} // namespace simd
