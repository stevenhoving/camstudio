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
#pragma pack(pop)

template <typename T>
inline const T align_down(const T size, const T align) noexcept
{
    return size & ~(align - 1);
}



static
void bgr2yuv420p_v2(uint8_t *destination[8], const uint8_t *const src[3], const int width, const int height, const int src_stride[3])
{
    const bgr *bgr_pixels = (const bgr *)src[0];
    const auto stride = src_stride[0]/3;

    auto y = destination[0];
    auto u = destination[1];
    auto v = destination[2];

    const int sse_width = align_down(width, 15)/16;

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
                //*y++ = ((66 * r1 + 129 * g1 + 25 * b1) + 4096) >> 8;
                *u++ = ((-38 * r1 + -74 * g1 + 112 * b1) >> 8) + 128;
                *v++ = ((112 * r1 + -94 * g1 + -18 * b1) >> 8) + 128;

                const bgr &data2 = *bgr_pixels++;

                const auto r2 = data2.r;
                const auto g2 = data2.g;
                const auto b2 = data2.b;

                *y++ = ((66 * r2 + 129 * g2 + 25 * b2) >> 8) + 16;
                //*y++ = ((66 * r2 + 129 * g2 + 25 * b2) + 4096) >> 8;
            }
            bgr_pixels += stride * 2;
        }
        else
        {
            /* sse we can handle 16 / 3 = 5 bgr pixels at the same time
            *
            * b g r b g r b g r b g r b g r x
            * shuffle into 2 reg
            *   b   b   b   b   b   g   g   g
            *   g   g   r   r   r   r   r   x
            *
            * then do the multiply
            *
            * then shuffle into 3 regs
            *   b   b   b   b   b
            *   g   g   g   g   g
            *   r   r   r   r   r
            */
            constexpr auto mask = (char)0x80;
            static const auto b_shuffle =
                _mm_set_epi8(mask, mask, mask, mask, mask, mask,
                    mask, 12,
                    mask, 9,
                    mask, 6,
                    mask, 3,
                    mask, 0);
            static const auto g_shuffle =
                _mm_set_epi8(mask, mask, mask, mask, mask, mask,
                    mask, 13,
                    mask, 10,
                    mask, 7,
                    mask, 4,
                    mask, 1);
            static const auto r_shuffle =
                _mm_set_epi8(mask, mask, mask, mask, mask, mask,
                    mask, 14,
                    mask, 11,
                    mask, 8,
                    mask, 5,
                    mask, 2);

            static const auto b_mul = _mm_set_epi16(25, 25, 25, 25, 25, 25, 25, 25);
            static const auto g_mul = _mm_set_epi16(129, 129, 129, 129, 129, 129, 129, 129);
            static const auto r_mul = _mm_set_epi16(66, 66, 66, 66, 66, 66, 66, 66);
            static const auto y_add = _mm_set1_epi8(16);
            //static const auto store_mask = _mm_set_epi8(0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1);

            static const auto y_shuffle0 =
                _mm_set_epi8(mask,
                    mask, mask, mask, mask, mask,
                    mask, mask, mask, mask, mask,
                    9, 7, 5, 3, 1);

            static const auto y_shuffle1 =
                _mm_set_epi8(mask,
                    mask, mask, mask, mask, mask,
                    9, 7, 5, 3, 1,
                    mask, mask, mask, mask, mask);

            static const auto y_shuffle2 =
                _mm_set_epi8(mask,
                    9, 7, 5, 3, 1,
                    mask, mask, mask, mask, mask,
                    mask, mask, mask, mask, mask);

            //__no_unroll
            for (int x = 0; x < sse_width; ++x)
            {
                const auto pxl0 = _mm_lddqu_si128((__m128i *)bgr_pixels);
                bgr_pixels += 5;

                const auto pxl1 = _mm_lddqu_si128((__m128i *)bgr_pixels);
                bgr_pixels += 5;

                const auto pxl2 = _mm_lddqu_si128((__m128i *)bgr_pixels);
                bgr_pixels += 5;

                auto b_part0 = _mm_shuffle_epi8(pxl0, b_shuffle);
                auto g_part0 = _mm_shuffle_epi8(pxl0, g_shuffle);
                auto r_part0 = _mm_shuffle_epi8(pxl0, r_shuffle);

                auto b_part1 = _mm_shuffle_epi8(pxl1, b_shuffle);
                auto g_part1 = _mm_shuffle_epi8(pxl1, g_shuffle);
                auto r_part1 = _mm_shuffle_epi8(pxl1, r_shuffle);

                auto b_part2 = _mm_shuffle_epi8(pxl2, b_shuffle);
                auto g_part2 = _mm_shuffle_epi8(pxl2, g_shuffle);
                auto r_part2 = _mm_shuffle_epi8(pxl2, r_shuffle);

                b_part0 = _mm_mullo_epi16(b_part0, b_mul);
                g_part0 = _mm_mullo_epi16(g_part0, g_mul);
                r_part0 = _mm_mullo_epi16(r_part0, r_mul);

                b_part1 = _mm_mullo_epi16(b_part1, b_mul);
                g_part1 = _mm_mullo_epi16(g_part1, g_mul);
                r_part1 = _mm_mullo_epi16(r_part1, r_mul);

                b_part2 = _mm_mullo_epi16(b_part2, b_mul);
                g_part2 = _mm_mullo_epi16(g_part2, g_mul);
                r_part2 = _mm_mullo_epi16(r_part2, r_mul);

                auto y_part0 = _mm_add_epi16(b_part0, _mm_add_epi16(g_part0, r_part0));
                auto y_part1 = _mm_add_epi16(b_part1, _mm_add_epi16(g_part1, r_part1));
                auto y_part2 = _mm_add_epi16(b_part2, _mm_add_epi16(g_part2, r_part2));

                y_part0 = _mm_shuffle_epi8(y_part0, y_shuffle0);
                y_part1 = _mm_shuffle_epi8(y_part1, y_shuffle1);
                y_part2 = _mm_shuffle_epi8(y_part2, y_shuffle2);

                auto y_part = _mm_or_si128(y_part0, _mm_or_si128(y_part1, y_part2));
                y_part = _mm_add_epi8(y_part, y_add);

                _mm_store_si128((__m128i *)y, y_part); y += 15;
            }

            const int width_rest = width - (sse_width * 15);
            //__no_unroll
            for (int x = 0; x < width_rest; ++x)
            {
                const bgr &data = *bgr_pixels++;
                const auto r = data.r;
                const auto g = data.g;
                const auto b = data.b;

                *y++ = ((66 * r + 129 * g + 25 * b) >> 8) + 16;
            }
            bgr_pixels += stride * 2;
        }
    }
}

static
void bgr2yuv420p(uint8_t *destination[8], const uint8_t *rgb, const int width, const int height)
{
    const bgr *bgr_pixels = (const bgr *)rgb;

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
 