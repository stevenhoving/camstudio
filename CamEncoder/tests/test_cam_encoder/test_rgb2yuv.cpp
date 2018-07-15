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
#define NOMINMAX

#include <gtest/gtest.h>
#include <CamEncoder/av_yuv/av_rgb2yuv.h>
#include <CamEncoder/av_yuv/av_rgba2yuv.h>
#include <CamEncoder/av_yuv/av_rgb2yuv_ssse3.h>
#include <CamEncoder/av_yuv/av_rgba2yuv_ssse3.h>

#include "test_utilities.h"
#include <CamCapture/cam_stop_watch.h>
#include <fmt/printf.h>
#include <vector>
#include <cstdint>

TEST(test_rgb2yuv, test_bgr2yuv)
{
    const int width = 3999;
    const int height = 3999;

    int total = width * height;

    std::vector<uint8_t> yuv420_sse(total * 3);
    std::vector<uint8_t> yuv420_c(total * 3);

    auto sse_y = yuv420_sse.data();
    auto sse_u = sse_y + total;
    auto sse_v = sse_u + (total >> 1);

    auto c_y = yuv420_c.data();
    auto c_u = c_y + total;
    auto c_v = c_u + (total >> 1);

    uint8_t *destination_sse[8] = {sse_y, sse_u, sse_v};
    uint8_t *destination_c[8] = { c_y, c_u, c_v };
    const int dst_stride[3] = {width, width>>1, width>>1};

    int color = 0;
    std::vector<uint8_t> rgb_buffer(width * height * 3);
    for (auto &itr : rgb_buffer)
    {
        itr = color++ % 255;
    }

    const uint8_t *const src[3] = {rgb_buffer.data() + (rgb_buffer.size() - (width * 3)), nullptr, nullptr};
    const int src_stride[3] = {width * -3, 0, 0};

    bgr2yuv420p(destination_c, dst_stride, src, width, height, src_stride);
    bgr2yuv420p_sse(destination_sse, dst_stride, src, width, height, src_stride);

    for (int i = 0; i < yuv420_sse.size(); ++i)
    {
        if (yuv420_sse[i] != yuv420_c[i])
        {
            fmt::print("{} is not the same - sse: {}, c: {}\n", i, yuv420_sse[i], yuv420_c[i]);
        }
    }
}

TEST(test_rgb2yuv, test_bgra2yuv)
{
    const int width = 3999;
    const int height = 3999;

    int total = width * height;

    std::vector<uint8_t> yuv420_sse(total * 3);
    std::vector<uint8_t> yuv420_c(total * 3);

    auto sse_y = yuv420_sse.data();
    auto sse_u = sse_y + total;
    auto sse_v = sse_u + (total >> 1);

    auto c_y = yuv420_c.data();
    auto c_u = c_y + total;
    auto c_v = c_u + (total >> 1);

    uint8_t *destination_sse[8] = {sse_y, sse_u, sse_v};
    uint8_t *destination_c[8] = {c_y, c_u, c_v};
    const int dst_stride[3] = {width, width >> 1, width >> 1};

    int color = 0;
    std::vector<uint8_t> rgb_buffer(width * height * 4);
    for (auto &itr : rgb_buffer)
    {
        itr = color++ % 255;
    }

    const uint8_t *const src[3] = {rgb_buffer.data() + (rgb_buffer.size() - (width * 4)), nullptr, nullptr};
    const int src_stride[3] = {width * -4, 0, 0};

    bgra2yuv420p(destination_c, dst_stride, src, width, height, src_stride);
    bgra2yuv420p_sse(destination_sse, dst_stride, src, width, height, src_stride);

    for (int i = 0; i < yuv420_sse.size(); ++i)
    {
        if (yuv420_sse[i] != yuv420_c[i])
        {
            fmt::print("{} is not the same - sse: {}, c: {}\n", i, yuv420_sse[i], yuv420_c[i]);
        }
    }
}

TEST(test_rgb2yuv, test_bgr2yuv_perf)
{
    const int width = 4000;
    const int height = 4000;

    int total = width * height;

    std::vector<uint8_t> yuv420(total * 3);

    auto y = yuv420.data();
    auto u = y + total;
    auto v = u + (total >> 1);

    uint8_t *destination[8] = {
        y, u, v
    };
    const int dst_stride[3] = {width, width>>1, width>>1};

    std::vector<uint8_t> rgb_buffer(width * height * 3);

    const uint8_t *const src[3] = { rgb_buffer.data() + (rgb_buffer.size() - (width * 3)), nullptr, nullptr};
    const int src_stride[3] = { width * -3, 0, 0 };

    cam::stop_watch stopwatch;
    stopwatch.time_start();

    for (int i = 0; i < 100; ++i)
        bgr2yuv420p_sse(destination, dst_stride, src, width, height, src_stride);

    double dt = stopwatch.time_since() * 1000.0;
    fmt::print("4000x4000 - 100x bgr2yuv took: {}ms\n", dt);
}

TEST(test_rgb2yuv, test_bgra2yuv_perf)
{
    const int width = 4000;
    const int height = 4000;

    int total = width * height;

    std::vector<uint8_t> yuv420(total * 3);

    auto y = yuv420.data();
    auto u = y + total;
    auto v = u + (total >> 1);

    uint8_t *destination[8] = {y, u, v};
    const int dst_stride[3] = {width, width>>1, width>>1};

    std::vector<uint8_t> rgb_buffer(width * height * 4);

    const uint8_t *const src[3] = {rgb_buffer.data(), nullptr, nullptr};
    const int src_stride[3] = {width * 4, 0, 0};

    cam::stop_watch stopwatch;
    stopwatch.time_start();

    for (int i = 0; i < 100; ++i)
        bgra2yuv420p_sse(destination, dst_stride, src, width, height, src_stride);

    double dt = stopwatch.time_since() * 1000.0;
    fmt::print("4000x4000 - 100x bgra2yuv took: {}ms\n", dt);
}
