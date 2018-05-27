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
#define NOMINMAX

#include <gtest/gtest.h>
#include <CamEncoder/av_rgb2yuv.h>
#include "test_utilities.h"
#include <CamCapture/cam_stop_watch.h>
#include <fmt/printf.h>
#include <vector>
#include <cstdint>

TEST(test_rgb2yuv, test_bgr2yuv)
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

    std::vector<uint8_t> rgb_buffer(width * height * 3);

    const uint8_t *const src[3] = { rgb_buffer.data() + (rgb_buffer.size() - (width * 3)), nullptr, nullptr};
    const int src_stride[3] = { width * -3, 0, 0 };

    cam::stop_watch stopwatch;
    stopwatch.time_start();

    for (int i = 0; i < 100; ++i)
        bgr2yuv420p_v2(destination, src, width, height, src_stride);

    double dt = stopwatch.time_since() * 1000.0;
    fmt::print("4000x4000 - 100x bgr2yuv took: {}ms\n", dt);
}
