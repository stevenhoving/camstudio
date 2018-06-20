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

#include "CamEncoder/av_yuv/av_rgb2yuv_ssse3.h"
#include "CamEncoder/av_yuv/av_simd_debug.h"
#include "CamEncoder/av_yuv/av_yuv_pixel_type.h"
#include "CamEncoder/av_yuv/av_simd_vec.h"
#include "CamEncoder/av_yuv/av_simd_common.h"

#include <xmmintrin.h>
#include <emmintrin.h>
#include <immintrin.h>
#include <tmmintrin.h>
#include <cstdio>

namespace simd
{
static const auto shuffle_lo_odd = vec3_set_shuffle_lo(9, 6, 3, 0);
static const auto shuffle_hi_odd = vec3_set_shuffle_hi(9, 6, 3, 0);

static const auto y_shuffle0 =_mm_set_epi8(mask,
    mask, mask, mask, mask, mask,
    mask, mask, mask, mask, mask,
    9, 7, 5, 3, 1);

static const auto y_shuffle1 =_mm_set_epi8(mask,
    mask, mask, mask, mask, mask,
    9, 7, 5, 3, 1,
    mask, mask, mask, mask, mask);

static const auto y_shuffle2 =_mm_set_epi8(mask,
    9, 7, 5, 3, 1,
    mask, mask, mask, mask, mask,
    mask, mask, mask, mask, mask);

static const vec3 y_shuffle = { y_shuffle0, y_shuffle1, y_shuffle2 };


static const auto y_shuffle0_lo_odd = _mm_set_epi8(
    mask, mask, mask, mask, mask, mask, mask, mask,
    15, 13, 11, 9, 7, 5, 3, 1);

static const auto y_shuffle1_lo_odd = _mm_set_epi8(
    15, 13, 11, 9, 7, 5, 3, 1,
    mask, mask, mask, mask, mask, mask, mask, mask
);

static const auto y_shuffle2_lo_odd = _mm_set_epi8(
    mask, mask, mask, mask, mask, mask, mask, mask,
    15, 13, 11, 9, 7, 5, 3, 1);

static const vec3 y_shuffle_lo_odd = { y_shuffle0_lo_odd, y_shuffle1_lo_odd, y_shuffle2_lo_odd };

static const vec2 vec2_y_shuffle_lo_odd = { y_shuffle0_lo_odd, y_shuffle1_lo_odd };

static const auto y_shuffle0_hi_odd = _mm_set_epi8(
    mask, mask, mask, mask,
    mask, mask, mask, mask,
    mask, mask, mask, mask,
    15, 12, 11, 9);

static const auto y_shuffle1_hi_odd = _mm_set_epi8(
    mask, mask, mask, mask,
    mask, mask, mask, mask,
    15, 12, 11, 9,
    mask, mask, mask, mask);

static const auto y_shuffle2_hi_odd = _mm_set_epi8(
    mask, mask, mask, mask,
    15, 12, 11, 9,
    mask, mask, mask, mask,
    mask, mask, mask, mask);

static const vec3 y_shuffle_hi_odd = { y_shuffle0_hi_odd, y_shuffle1_hi_odd, y_shuffle2_hi_odd };

static const auto uv_shuffle0 = _mm_set_epi8(
    mask, mask, mask, mask,
    mask, mask, mask, mask,
    mask, mask, mask, mask,
      12,    8,    4,    0
);

static const auto uv_shuffle1 = _mm_set_epi8(
    mask, mask, mask, mask,
    mask, mask, mask, mask,
      12,    8,    4,    0,
    mask, mask, mask, mask
);

static const auto uv_shuffle2 = _mm_set_epi8(
    mask, mask, mask, mask,
      12,    8,    4,    0,
    mask, mask, mask, mask,
    mask, mask, mask, mask
);

static const vec3 uv_shuffle = { uv_shuffle0, uv_shuffle1, uv_shuffle2 };
static const vec2 vec2_uv_shuffle = { uv_shuffle0, uv_shuffle1 };

static const auto uv_shuffle_interleave0 = _mm_set_epi8(
    mask, mask, mask, mask, mask, mask, mask, mask,
    mask, mask, mask, mask, mask, 4, mask, 0);

static const auto uv_shuffle_interleave1 = _mm_set_epi8(
    mask, mask, mask, mask, mask, mask, mask, mask,
    mask, 4, mask, 0, mask, mask, mask, mask);

static const auto uv_shuffle_interleave2 = _mm_set_epi8(
    mask, 12, mask, 8, mask, 4, mask, 0,
    mask, mask, mask, mask, mask, mask, mask, mask);

static const vec3 uv_interleave = { uv_shuffle_interleave0, uv_shuffle_interleave1, uv_shuffle_interleave2 };


/*
 * - 4000x4000 - 100x bgr2yuv took: 1006.44ms
 * - 4000x4000 - 100x bgr2yuv took: 908.413ms
 */
void bgr2yuv420p_sse(uint8_t *destination[8], const int dst_stride[3], const uint8_t *const src[3],
                     const int width, const int height, const int src_stride[3])
{
    const bgr *bgr_pixels = (const bgr *)src[0];
    const auto stride = src_stride[0]/3;

    auto y = destination[0];
    auto u = destination[1];
    auto v = destination[2];

    const auto y_stride_delta = dst_stride[0] - width;
    const auto u_stride_delta = dst_stride[1] - (width >> 1);
    const auto v_stride_delta = dst_stride[2] - (width >> 1);

    const int sse_aligned_width = simd::align_down(width, 16);

    __no_unroll
    for (int line = 0; line < height; ++line)
    {
        if (!(line % 2))
        {
            int x = 0;
            __no_unroll
            for (x = 0; x < sse_aligned_width; x += 16)
            {
                const auto pxl0 = _mm_lddqu_si128((__m128i *)bgr_pixels); // load 4 pixels
                bgr_pixels += 4;

                const auto pxl1 = _mm_lddqu_si128((__m128i *)bgr_pixels); // load 4 pixels
                bgr_pixels += 4;

                const auto pxl2 = _mm_lddqu_si128((__m128i *)bgr_pixels); // load 4 pixels
                bgr_pixels += 4;

                const auto pxl3 = _mm_lddqu_si128((__m128i *)bgr_pixels); // load 4 pixels
                bgr_pixels += 4;

                // unpack so we end up with 4x 4 pixels
                auto vec_data0 = vec3_unpack(pxl0, shuffle_lo_odd);
                auto vec_data1 = vec3_unpack(pxl1, shuffle_hi_odd);
                auto vec_data2 = vec3_unpack(pxl2, shuffle_lo_odd);
                auto vec_data3 = vec3_unpack(pxl3, shuffle_hi_odd);

                // interleave so we end up with 2x 8 pixels
                auto vec_part0 = vec3_interleave(vec_data0, vec_data1);
                auto vec_part1 = vec3_interleave(vec_data2, vec_data3);

                // multiply the 2x 8 pixels
                auto vec_y_part0 = vec3_mullo(vec_part0, y_mul);
                auto vec_y_part1 = vec3_mullo(vec_part1, y_mul);

                // vertical sum the vec3 so we end up with 1 object that contains 2x 8 pixels.
                auto vec_y_part = vec3_vsum_vec2(vec_y_part0, vec_y_part1);

                auto vec_result = vec2_shuffle(vec_y_part, vec2_y_shuffle_lo_odd);

                auto y_result0 = _mm_or_si128(vec_result.b, vec_result.g);

                y_result0 = _mm_add_epi8(y_result0, y_add);

                // store 16 y pixels
                _mm_store_si128((__m128i *)y, y_result0); y += 16;

                if (x % 2 == 0)
                {
                    auto u_vec_part0 = vec3_mullo(vec_part0, u_mul); // contains 4 pixels (we skip every odd one)
                    auto u_vec_part1 = vec3_mullo(vec_part1, u_mul); // contains 4 pixels

                    auto v_vec_part0 = vec3_mullo(vec_part0, v_mul); // contains 4 pixels
                    auto v_vec_part1 = vec3_mullo(vec_part1, v_mul); // contains 4 pixels

                    auto u_part0 = vec3_vsum_vec2(u_vec_part0, u_vec_part1);
                    auto v_part0 = vec3_vsum_vec2(v_vec_part0, v_vec_part1);

                    u_part0 = vec2_srai(u_part0, 8);
                    v_part0 = vec2_srai(v_part0, 8);

                    u_part0 = vec2_add(u_part0, uv_add);
                    v_part0 = vec2_add(v_part0, uv_add);

                    u_part0 = vec2_shuffle(u_part0, vec2_uv_shuffle);
                    v_part0 = vec2_shuffle(v_part0, vec2_uv_shuffle);

                    auto u_0 = vec2_pack_interleave(u_part0);
                    auto v_0 = vec2_pack_interleave(v_part0);

                    _mm_storel_epi64((__m128i *)u, u_0); u += 8;
                    _mm_storel_epi64((__m128i *)v, v_0); v += 8;
                }
            }

            __no_unroll
            for (; x < width; x += 2)
            {
                const bgr &data1 = *bgr_pixels++;
                const auto r1 = data1.r;
                const auto g1 = data1.g;
                const auto b1 = data1.b;

                *y++ = ((66 * r1 + 129 * g1 + 25 * b1) >> 8) + 16;
                *u++ = ((-38 * r1 + -74 * g1 + 112 * b1) >> 8) + 128;
                *v++ = ((112 * r1 + -94 * g1 + -18 * b1) >> 8) + 128;

                const bgr &data2 = *bgr_pixels++;

                const auto r2 = data2.r;
                const auto g2 = data2.g;
                const auto b2 = data2.b;

                *y++ = ((66 * r2 + 129 * g2 + 25 * b2) >> 8) + 16;
            }
            y += y_stride_delta;
            u += u_stride_delta;
            v += v_stride_delta;
        }
        else
        {
            int x = 0;

            __no_unroll
            for (; x < sse_aligned_width; x += 16)
            {
                const auto pxl0 = _mm_lddqu_si128((__m128i *)bgr_pixels); // load 4 pixels
                bgr_pixels += 4;

                const auto pxl1 = _mm_lddqu_si128((__m128i *)bgr_pixels); // load 4 pixels
                bgr_pixels += 4;

                const auto pxl2 = _mm_lddqu_si128((__m128i *)bgr_pixels); // load 4 pixels
                bgr_pixels += 4;

                const auto pxl3 = _mm_lddqu_si128((__m128i *)bgr_pixels); // load 4 pixels
                bgr_pixels += 4;

                // unpack so we end up with 4x 4 pixels
                auto vec_data0 = vec3_unpack(pxl0, shuffle_lo_odd);
                auto vec_data1 = vec3_unpack(pxl1, shuffle_hi_odd);
                auto vec_data2 = vec3_unpack(pxl2, shuffle_lo_odd);
                auto vec_data3 = vec3_unpack(pxl3, shuffle_hi_odd);

                // interleave so we end up with 2x 8 pixels
                auto vec_part0 = vec3_interleave(vec_data0, vec_data1);
                auto vec_part1 = vec3_interleave(vec_data2, vec_data3);

                // multiply the 2x 8 pixels
                auto vec_y_part0 = vec3_mullo(vec_part0, y_mul);
                auto vec_y_part1 = vec3_mullo(vec_part1, y_mul);

                // vertical sum the vec3 so we end up with 1 object that contains 2x 8 pixels.
                auto vec_y_part = vec3_vsum_vec2(vec_y_part0, vec_y_part1);

                auto vec_result = vec2_shuffle(vec_y_part, vec2_y_shuffle_lo_odd);

                auto y_result0 = _mm_or_si128(vec_result.b, vec_result.g);

                y_result0 = _mm_add_epi8(y_result0, y_add);

                // store 16 y pixels
                _mm_store_si128((__m128i *)y, y_result0);
                y += 16;
            }

            __no_unroll
            for (; x < width; ++x)
            {
                const bgr &data = *bgr_pixels++;
                const auto r = data.r;
                const auto g = data.g;
                const auto b = data.b;

                *y++ = ((66 * r + 129 * g + 25 * b) >> 8) + 16;
            }
            y += y_stride_delta;
        }
        bgr_pixels += stride * 2;
    }
}

} // namespace simd