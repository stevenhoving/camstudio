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
#include "CamEncoder/av_yuv/av_rgb2yuv.h"

void bgr2yuv420p(uint8_t *destination[8], const int dst_stride[3], const uint8_t *const src[3],
    const int width, const int height, const int src_stride[3])
{
    const uint8_t *bgr_pixels = src[0];
    const auto stride = src_stride[0];

    const auto image_size = width * height;
    auto y = destination[0];
    auto u = destination[1];
    auto v = destination[2];

    const auto y_stride_delta = dst_stride[0] - width;
    const auto u_stride_delta = dst_stride[1] - (width >> 1);
    const auto v_stride_delta = dst_stride[2] - (width >> 1);

    for (int line_nr = 0; line_nr < height; ++line_nr)
    {
        const bgr *line = (const bgr *)bgr_pixels;

        if (!(line_nr % 2))
        {
            for (int x = 0; x < width; x += 2)
            {
                const bgr &data1 = *line++;
                const auto r1 = data1.r;
                const auto g1 = data1.g;
                const auto b1 = data1.b;

                *y++ = ((66 * r1 + 129 * g1 + 25 * b1) >> 8) + 16;
                *u++ = ((-38 * r1 + -74 * g1 + 112 * b1) >> 8) + 128;
                *v++ = ((112 * r1 + -94 * g1 + -18 * b1) >> 8) + 128;

                const bgr &data2 = *line++;

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
            for (int x = 0; x < width; ++x)
            {
                const bgr &data = *line++;
                const auto r = data.r;
                const auto g = data.g;
                const auto b = data.b;

                *y++ = ((66 * r + 129 * g + 25 * b) >> 8) + 16;
            }
            y += y_stride_delta;
        }
        bgr_pixels += stride;
    }
}
