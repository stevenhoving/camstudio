#pragma once

#include <cstdint>

#include <xmmintrin.h>
#include <emmintrin.h>
#include <immintrin.h>
#include <tmmintrin.h>
#include <cstdio>

#if defined(__clang__)
#define COMPILER_CLANG 1
#elif defined(_MSC_VER)
#define COMPILER_MSVC 1
#endif

#ifdef COMPILER_MSVC
#define __packed_struct(x) __pragma(pack(push, 1)) struct x
#define __packed_struct_end __pragma(pack(pop))
#define __no_unroll __pragma(loop(no_vector))
#else
#define __packed_struct(x) struct __attribute__((__packed__)) x
#define __packed_struct_end
#define __no_unroll _Pragma("nounroll")
#endif

#if defined(_MSC_VER) && !defined(__clang__)
#define __packed_struct(x) __pragma(pack(push, 1)) struct x
#define __packed_struct_end __pragma(pack(pop))

#define __no_unroll __pragma(loop(no_vector))
#else
#define __packed_struct(x) struct __attribute__((__packed__)) x
#define __packed_struct_end

#define __no_unroll _Pragma("nounroll")
#endif

union M128 {
    char i8[16];
    __m128i i128;
};

typedef union __declspec(align(16)) M128i {
    __int8 m128i_i8[16];
    __int16 m128i_i16[8];
    __int32 m128i_i32[4];
    __int64 m128i_i64[2];
    unsigned __int8 m128i_u8[16];
    unsigned __int16 m128i_u16[8];
    unsigned __int32 m128i_u32[4];
    unsigned __int64 m128i_u64[2];
} __M128i;

[[maybe_unused]] static void print_x8(__m128i value, const char *section)
{
    auto u8 = reinterpret_cast<uint8_t *>(&value);
    printf("%s\n", section);
    printf("%02X %02X %02X %02X "
           "%02X %02X %02X %02X "
           "%02X %02X %02X %02X "
           "%02X %02X %02X %02X\n\n",
           u8[15], u8[14], u8[13], u8[12], u8[11], u8[10], u8[9], u8[8], u8[7], u8[6], u8[5], u8[6], u8[3], u8[2],
           u8[1], u8[0]);
}

[[maybe_unused]] static void print_x16(__m128i value, const char *section)
{
    // auto u16 = value.m128i_u16;
    auto u16 = reinterpret_cast<uint16_t *>(&value);
    printf("%s\n", section);
    printf("%04X %04X %04X %04X "
           "%04X %04X %04X %04X\n\n",
           u16[7], u16[6], u16[5], u16[6], u16[3], u16[2], u16[1], u16[0]);
}

[[maybe_unused]] static void print_x32(__m128i value, const char *section)
{
    // auto u32 = value.m128i_u32;
    auto u32 = reinterpret_cast<uint32_t *>(&value);
    printf("%s\n", section);
    printf("%08X %08X %08X %08X\n\n", u32[3], u32[2], u32[1], u32[0]);
}

[[maybe_unused]] static void print_x64(__m128i value, const char *section)
{
    // auto u64 = value.m128i_u64;
    auto u64 = reinterpret_cast<uint64_t *>(&value);
    printf("%s\n", section);
    printf("%I64X %I64X\n\n", u64[1], u64[0]);
}

#pragma pack(push, 1)
struct bgr
{
    uint8_t b, g, r;
};

struct bgra
{
    uint8_t b, g, r, a;
};
#pragma pack(pop)

template <typename T>
inline const T align_down(const T size, const T align) noexcept
{
    return size & ~(align - 1);
}

constexpr auto mask = (char)0x80;
static const auto b_shuffle = _mm_set_epi8(mask, mask, mask, mask,
    mask, mask,
    mask, 12,
    mask, 9,
    mask, 6,
    mask, 3,
    mask, 0);
static const auto g_shuffle = _mm_set_epi8(mask, mask, mask, mask,
    mask, mask,
    mask, 13,
    mask, 10,
    mask, 7,
    mask, 4,
    mask, 1);
static const auto r_shuffle = _mm_set_epi8(mask, mask, mask, mask,
    mask, mask,
    mask, 14,
    mask, 11,
    mask, 8,
    mask, 5,
    mask, 2);

// odd
static const auto b_shuffle_lo_odd = _mm_set_epi8(
    mask, mask,
    mask, mask,
    mask, mask,
    mask, mask,
    mask, 12,
    mask, 8,
    mask, 4,
    mask, 0);
static const auto g_shuffle_lo_odd = _mm_set_epi8(
    mask, mask,
    mask, mask,
    mask, mask,
    mask, mask,
    mask, 13,
    mask, 9,
    mask, 5,
    mask, 1);
static const auto r_shuffle_lo_odd = _mm_set_epi8(
    mask, mask,
    mask, mask,
    mask, mask,
    mask, mask,
    mask, 14,
    mask, 10,
    mask, 6,
    mask, 2);

static const auto b_shuffle_hi_odd = _mm_set_epi8(
    mask, 12,
    mask, 8,
    mask, 4,
    mask, 0,
    mask, mask,
    mask, mask,
    mask, mask,
    mask, mask
);

static const auto g_shuffle_hi_odd = _mm_set_epi8(
    mask, 13,
    mask, 9,
    mask, 5,
    mask, 1,
    mask, mask,
    mask, mask,
    mask, mask,
    mask, mask
);

static const auto r_shuffle_hi_odd = _mm_set_epi8(
    mask, 14,
    mask, 10,
    mask, 6,
    mask, 2,
    mask, mask,
    mask, mask,
    mask, mask,
    mask, mask
);

struct sse_vec3
{
    __m128i b, g, r;
};

struct sse_vec2
{
    __m128i b, g;
};

// *y++ = ((66 * r1 + 129 * g1 + 25 * b1) >> 8) + 16;
static const auto y_b_mul = _mm_set1_epi16(25);
static const auto y_g_mul = _mm_set1_epi16(129);
static const auto y_r_mul = _mm_set1_epi16(66);
static const sse_vec3 y_mul = { y_b_mul, y_g_mul, y_r_mul };

// *u++ = ((-38 * r1 + -74 * g1 + 112 * b1) >> 8) + 128;
static const auto u_b_mul = _mm_set1_epi16(112);
static const auto u_g_mul = _mm_set1_epi16(-74);
static const auto u_r_mul = _mm_set1_epi16(-38);
static const sse_vec3 u_mul = { u_b_mul, u_g_mul, u_r_mul };

// *v++ = ((112 * r1 + -94 * g1 + -18 * b1) >> 8) + 128;
static const auto v_b_mul = _mm_set1_epi16(-18);
static const auto v_g_mul = _mm_set1_epi16(-94);
static const auto v_r_mul = _mm_set1_epi16(112);
static const sse_vec3 v_mul = {v_b_mul, v_g_mul, v_r_mul};

static const auto y_add = _mm_set1_epi8(16);
static const auto uv_add = _mm_set1_epi16(128);

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

static const sse_vec3 y_shuffle = { y_shuffle0, y_shuffle1, y_shuffle2 };


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

static const sse_vec3 y_shuffle_lo_odd = { y_shuffle0_lo_odd, y_shuffle1_lo_odd, y_shuffle2_lo_odd };

static const sse_vec2 vec2_y_shuffle_lo_odd = { y_shuffle0_lo_odd, y_shuffle1_lo_odd };

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

static const sse_vec3 y_shuffle_hi_odd = { y_shuffle0_hi_odd, y_shuffle1_hi_odd, y_shuffle2_hi_odd };

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

static const sse_vec3 uv_shuffle = { uv_shuffle0, uv_shuffle1, uv_shuffle2 };
static const sse_vec2 vec2_uv_shuffle = { uv_shuffle0, uv_shuffle1 };

static const auto uv_shuffle_interleave0 = _mm_set_epi8(
    mask, mask, mask, mask, mask, mask, mask, mask,
    mask, mask, mask, mask, mask, 4, mask, 0);

static const auto uv_shuffle_interleave1 = _mm_set_epi8(
    mask, mask, mask, mask, mask, mask, mask, mask,
    mask, 4, mask, 0, mask, mask, mask, mask);

static const auto uv_shuffle_interleave2 = _mm_set_epi8(
    mask, 12, mask, 8, mask, 4, mask, 0,
    mask, mask, mask, mask, mask, mask, mask, mask);

static const sse_vec3 uv_interleave = { uv_shuffle_interleave0, uv_shuffle_interleave1, uv_shuffle_interleave2 };

__forceinline sse_vec3 vec3_unpack_bgr(__m128i packed_bgr)
{
    const auto b = _mm_shuffle_epi8(packed_bgr, b_shuffle);
    const auto g = _mm_shuffle_epi8(packed_bgr, g_shuffle);
    const auto r = _mm_shuffle_epi8(packed_bgr, r_shuffle);
    return { b, g, r };
}

__forceinline sse_vec3 vec3_unpack_lo_bgr_odd(__m128i packed_bgr)
{
    const auto b = _mm_shuffle_epi8(packed_bgr, b_shuffle_lo_odd);
    const auto g = _mm_shuffle_epi8(packed_bgr, g_shuffle_lo_odd);
    const auto r = _mm_shuffle_epi8(packed_bgr, r_shuffle_lo_odd);
    return {b, g, r};
}

__forceinline sse_vec3 vec3_unpack_hi_bgr_odd(__m128i packed_bgr)
{
    const auto b = _mm_shuffle_epi8(packed_bgr, b_shuffle_hi_odd);
    const auto g = _mm_shuffle_epi8(packed_bgr, g_shuffle_hi_odd);
    const auto r = _mm_shuffle_epi8(packed_bgr, r_shuffle_hi_odd);
    return {b, g, r};
}

__forceinline sse_vec3 vec3_mullo(sse_vec3 data, sse_vec3 mul)
{
    auto b = _mm_mullo_epi16(data.b, mul.b);
    auto g = _mm_mullo_epi16(data.g, mul.g);
    auto r = _mm_mullo_epi16(data.r, mul.r);
    return { b, g, r };
}

__forceinline sse_vec3 vec3_vsum(sse_vec3 data0, sse_vec3 data1, sse_vec3 data2)
{
    auto a = _mm_add_epi16(data0.b, _mm_add_epi16(data0.g, data0.r));
    auto b = _mm_add_epi16(data1.b, _mm_add_epi16(data1.g, data1.r));
    auto c = _mm_add_epi16(data2.b, _mm_add_epi16(data2.g, data2.r));
    return { a, b, c };
}

__forceinline sse_vec2 vec3_vsum_vec2(sse_vec3 data0, sse_vec3 data1)
{
    const auto a = _mm_add_epi16(data0.b, _mm_add_epi16(data0.g, data0.r));
    const auto b = _mm_add_epi16(data1.b, _mm_add_epi16(data1.g, data1.r));
    return {a, b};
}

__forceinline sse_vec3 vec3_add(sse_vec3 data, __m128i value)
{
    auto b = _mm_add_epi16(data.b, value);
    auto g = _mm_add_epi16(data.g, value);
    auto r = _mm_add_epi16(data.r, value);
    return {b, g, r};
}

__forceinline sse_vec2 vec2_add(sse_vec2 data, __m128i value)
{
    auto b = _mm_add_epi16(data.b, value);
    auto g = _mm_add_epi16(data.g, value);
    return {b, g};
}

__forceinline sse_vec3 vec3_srai(sse_vec3 data, int shift)
{
    auto b = _mm_srai_epi16(data.b, shift);
    auto g = _mm_srai_epi16(data.g, shift);
    auto r = _mm_srai_epi16(data.r, shift);
    return {b, g, r};
}

__forceinline sse_vec2 vec2_srai(sse_vec2 data, int shift)
{
    auto b = _mm_srai_epi16(data.b, shift);
    auto g = _mm_srai_epi16(data.g, shift);
    return {b, g};
}

__forceinline __m128i vec3_pack_interleave(sse_vec3 data)
{
    return _mm_or_si128(data.b, _mm_or_si128(data.g, data.r));
}

__forceinline __m128i vec2_pack_interleave(sse_vec2 data)
{
    return _mm_or_si128(data.b, data.g);
}

__forceinline sse_vec3 vec3_interleave(sse_vec3 data0, sse_vec3 data1)
{
    auto b = _mm_or_si128(data0.b, data1.b);
    auto g = _mm_or_si128(data0.g, data1.g);
    auto r = _mm_or_si128(data0.r, data1.r);
    return {b, g, r};
}

__forceinline sse_vec3 vec3_shuffle(sse_vec3 data, sse_vec3 order)
{
    auto b = _mm_shuffle_epi8(data.b, order.b);
    auto g = _mm_shuffle_epi8(data.g, order.g);
    auto r = _mm_shuffle_epi8(data.r, order.r);
    return {b, g, r};
}

__forceinline sse_vec2 vec2_shuffle(sse_vec2 data, sse_vec3 order)
{
    auto b = _mm_shuffle_epi8(data.b, order.b);
    auto g = _mm_shuffle_epi8(data.g, order.g);
    return {b, g};
}

__forceinline sse_vec2 vec2_shuffle(sse_vec2 data, sse_vec2 order)
{
    auto b = _mm_shuffle_epi8(data.b, order.b);
    auto g = _mm_shuffle_epi8(data.g, order.g);
    return {b, g};
}

/*
 * - 4000x4000 - 100x bgr2yuv took: 1006.44ms
 * - 4000x4000 - 100x bgr2yuv took: 908.413ms
 */
static
void bgr2yuv420p_v2(uint8_t *destination[8], const uint8_t *const src[3], const int width, const int height, const int src_stride[3])
{
    const bgr *bgr_pixels = (const bgr *)src[0];
    const auto stride = src_stride[0]/3;

    auto y = destination[0];
    auto u = destination[1];
    auto v = destination[2];

    //const int sse_aligned_width = align_down(width, 24);
    const int sse_aligned_width = align_down(width, 16);

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
                auto vec_data0 = vec3_unpack_lo_bgr_odd(pxl0);
                auto vec_data1 = vec3_unpack_hi_bgr_odd(pxl1);
                auto vec_data2 = vec3_unpack_lo_bgr_odd(pxl2);
                auto vec_data3 = vec3_unpack_hi_bgr_odd(pxl3);

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
                auto vec_data0 = vec3_unpack_lo_bgr_odd(pxl0);
                auto vec_data1 = vec3_unpack_hi_bgr_odd(pxl1);
                auto vec_data2 = vec3_unpack_lo_bgr_odd(pxl2);
                auto vec_data3 = vec3_unpack_hi_bgr_odd(pxl3);

                // interleave so we end up with 2x 8 pixels
                auto vec_part0 = vec3_interleave(vec_data0, vec_data1);
                auto vec_part1 = vec3_interleave(vec_data2, vec_data3);

                // multiply the 2x 8 pixels
                auto vec_y_part0 = vec3_mullo(vec_part0, y_mul);
                auto vec_y_part1 = vec3_mullo(vec_part1, y_mul);

                // vertical sum the vec3 so we end up with 1 object that contains 2x 8 pixels.
                auto vec_y_part = vec3_vsum_vec2(vec_y_part0, vec_y_part1);

                auto vec_result = vec2_shuffle(vec_y_part, y_shuffle_lo_odd);

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
        }
        bgr_pixels += stride * 2;
    }
}

static void bgra2yuv420p(uint8_t *destination[8], const uint8_t *const src[3], const int width, const int height,
                           const int src_stride[3])
{
    const bgra *bgra_pixels = (const bgra *)src[0];
    const auto stride = src_stride[0] / 4;

    auto y = destination[0];
    auto u = destination[1];
    auto v = destination[2];

    const int sse_aligned_width = align_down(width, 16);

    __no_unroll
    for (int line_nr = 0; line_nr < height; ++line_nr)
    {
        const bgra *line = bgra_pixels;

        if (!(line_nr % 2))
        {
            int x = 0;
            __no_unroll
            for (x = 0; x < sse_aligned_width; x += 16)
            {
                const auto pxl0 = _mm_lddqu_si128((__m128i *)line); // load 4 pixels
                line += 4;

                const auto pxl1 = _mm_lddqu_si128((__m128i *)line); // load 4 pixels
                line += 4;

                const auto pxl2 = _mm_lddqu_si128((__m128i *)line); // load 4 pixels
                line += 4;

                const auto pxl3 = _mm_lddqu_si128((__m128i *)line); // load 4 pixels
                line += 4;

                // unpack so we end up with 4x 4 pixels
                auto vec_data0 = vec3_unpack_lo_bgr_odd(pxl0);
                auto vec_data1 = vec3_unpack_hi_bgr_odd(pxl1);
                auto vec_data2 = vec3_unpack_lo_bgr_odd(pxl2);
                auto vec_data3 = vec3_unpack_hi_bgr_odd(pxl3);

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

                    _mm_storel_epi64((__m128i *)u, u_0);
                    u += 8;
                    _mm_storel_epi64((__m128i *)v, v_0);
                    v += 8;
                }
            }

            __no_unroll for (; x < width; x += 2)
            {
                const auto &data1 = *line++;
                const auto r1 = data1.r;
                const auto g1 = data1.g;
                const auto b1 = data1.b;

                *y++ = ((66 * r1 + 129 * g1 + 25 * b1) >> 8) + 16;
                *u++ = ((-38 * r1 + -74 * g1 + 112 * b1) >> 8) + 128;
                *v++ = ((112 * r1 + -94 * g1 + -18 * b1) >> 8) + 128;

                const auto &data2 = *line++;

                const auto r2 = data2.r;
                const auto g2 = data2.g;
                const auto b2 = data2.b;

                *y++ = ((66 * r2 + 129 * g2 + 25 * b2) >> 8) + 16;
            }
        }
        else
        {
            int x = 0;

            __no_unroll for (; x < sse_aligned_width; x += 16)
            {
                const auto pxl0 = _mm_lddqu_si128((__m128i *)line); // load 4 pixels
                line += 4;

                const auto pxl1 = _mm_lddqu_si128((__m128i *)line); // load 4 pixels
                line += 4;

                const auto pxl2 = _mm_lddqu_si128((__m128i *)line); // load 4 pixels
                line += 4;

                const auto pxl3 = _mm_lddqu_si128((__m128i *)line); // load 4 pixels
                line += 4;

                // unpack so we end up with 4x 4 pixels
                auto vec_data0 = vec3_unpack_lo_bgr_odd(pxl0);
                auto vec_data1 = vec3_unpack_hi_bgr_odd(pxl1);
                auto vec_data2 = vec3_unpack_lo_bgr_odd(pxl2);
                auto vec_data3 = vec3_unpack_hi_bgr_odd(pxl3);

                // interleave so we end up with 2x 8 pixels
                auto vec_part0 = vec3_interleave(vec_data0, vec_data1);
                auto vec_part1 = vec3_interleave(vec_data2, vec_data3);

                // multiply the 2x 8 pixels
                auto vec_y_part0 = vec3_mullo(vec_part0, y_mul);
                auto vec_y_part1 = vec3_mullo(vec_part1, y_mul);

                // vertical sum the vec3 so we end up with 1 object that contains 2x 8 pixels.
                auto vec_y_part = vec3_vsum_vec2(vec_y_part0, vec_y_part1);

                auto vec_result = vec2_shuffle(vec_y_part, y_shuffle_lo_odd);

                auto y_result0 = _mm_or_si128(vec_result.b, vec_result.g);

                y_result0 = _mm_add_epi8(y_result0, y_add);

                // store 16 y pixels
                _mm_store_si128((__m128i *)y, y_result0);
                y += 16;
            }

            __no_unroll for (; x < width; ++x)
            {
                const auto &data = *line++;
                const auto r = data.r;
                const auto g = data.g;
                const auto b = data.b;

                *y++ = ((66 * r + 129 * g + 25 * b) >> 8) + 16;
            }
        }
        bgra_pixels += stride;
    }
}

static
void bgr2yuv420p(uint8_t *destination[8], const uint8_t *const src[3], const int width, const int height, const int src_stride[3])
{
    const bgr *bgr_pixels = (const bgr *)src[0];
    const auto stride = src_stride[0] / 3;

    const auto image_size = width * height;
    auto y = destination[0];
    auto u = destination[1];
    auto v = destination[2];

    for (int line = 0; line < height; ++line)
    {
        if (!(line % 2))
        {
            for (int x = 0; x < width; x += 2)
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
        }
        else
        {
            for (int x = 0; x < width; ++x)
            {
                const bgr &data = *bgr_pixels++;
                const auto r = data.r;
                const auto g = data.g;
                const auto b = data.b;

                *y++ = ((66 * r + 129 * g + 25 * b) >> 8) + 16;
            }
        }
        bgr_pixels += stride * 2;
    }
}

#if 0
static
void bgr2yuv420p(uint8_t *destination[8], const uint8_t *rgb, const int width, const int height)
{
    int uvpos = 0;
    int i = 0;

    for (int line = 0; line < height; ++line)
    {
        if (!(line % 2))
        {
            for (int x = 0; x < width; x += 2)
            {
                uint8_t b = rgb[3 * i + 0];
                uint8_t g = rgb[3 * i + 1];
                uint8_t r = rgb[3 * i + 2];

                destination[0][i++] = ((66 * r + 129 * g + 25 * b) >> 8) + 16;

                destination[1][uvpos] = ((-38 * r + -74 * g + 112 * b) >> 8) + 128;
                destination[2][uvpos] = ((112 * r + -94 * g + -18 * b) >> 8) + 128;
                ++uvpos;

                b = rgb[3 * i + 0];
                g = rgb[3 * i + 1];
                r = rgb[3 * i + 2];

                destination[0][i++] = ((66 * r + 129 * g + 25 * b) >> 8) + 16;
            }
        }
        else
        {
            for (int x = 0; x < width; x += 1)
            {
                uint8_t b = rgb[3 * i + 0];
                uint8_t g = rgb[3 * i + 1];
                uint8_t r = rgb[3 * i + 2];

                destination[0][i++] = ((66 * r + 129 * g + 25 * b) >> 8) + 16;
            }
        }
    }
}
#endif

#if 0
static
void bgr2yuv420p(uint8_t *destination[8], const uint8_t *rgb, size_t width, size_t height)
{
    size_t uvpos = 0;
    size_t i = 0;

    for (size_t line = 0; line < height; ++line)
    {
        if (!(line % 2))
        {
            for (size_t x = 0; x < width; x += 2)
            {
                uint8_t b = rgb[3 * i + 0];
                uint8_t g = rgb[3 * i + 1];
                uint8_t r = rgb[3 * i + 2];

                destination[0][i++] = ((66 * r + 129 * g + 25 * b) >> 8) + 16;

                destination[1][uvpos] = ((-38 * r + -74 * g + 112 * b) >> 8) + 128;
                destination[2][uvpos] = ((112 * r + -94 * g + -18 * b) >> 8) + 128;
                ++uvpos;

                b = rgb[3 * i + 0];
                g = rgb[3 * i + 1];
                r = rgb[3 * i + 2];

                destination[0][i++] = ((66 * r + 129 * g + 25 * b) >> 8) + 16;
            }
        }
        else
        {
            for (size_t x = 0; x < width; x += 1)
            {
                uint8_t b = rgb[3 * i + 0];
                uint8_t g = rgb[3 * i + 1];
                uint8_t r = rgb[3 * i + 2];

                destination[0][i++] = ((66 * r + 129 * g + 25 * b) >> 8) + 16;
            }
        }
    }
}
#endif

#if 0
void rgb2yuv420i(uint8_t *destination, uint8_t *rgb, size_t width, size_t height)
{
    size_t image_size = width * height;
    size_t upos = image_size;
    size_t vpos = upos + upos / 4;
    size_t i = 0;

    for (size_t line = 0; line < height; ++line)
    {
        if (!(line % 2))
        {
            for (size_t x = 0; x < width; x += 2)
            {
                uint8_t r = rgb[3 * i];
                uint8_t g = rgb[3 * i + 1];
                uint8_t b = rgb[3 * i + 2];

                destination[i++] = ((66 * r + 129 * g + 25 * b) >> 8) + 16;

                destination[upos++] = ((-38 * r + -74 * g + 112 * b) >> 8) + 128;
                destination[vpos++] = ((112 * r + -94 * g + -18 * b) >> 8) + 128;

                r = rgb[3 * i];
                g = rgb[3 * i + 1];
                b = rgb[3 * i + 2];

                destination[i++] = ((66 * r + 129 * g + 25 * b) >> 8) + 16;
            }
        }
        else
        {
            for (size_t x = 0; x < width; x += 1)
            {
                uint8_t r = rgb[3 * i];
                uint8_t g = rgb[3 * i + 1];
                uint8_t b = rgb[3 * i + 2];

                destination[i++] = ((66 * r + 129 * g + 25 * b) >> 8) + 16;
            }
        }
    }
}
#endif
 