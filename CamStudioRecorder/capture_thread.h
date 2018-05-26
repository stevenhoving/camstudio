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

#include "CamCapture/cam_capture.h"
#include "CamCapture/cam_rect.h"
#include <atomic>
#include <thread>

enum class capture_type
{
    fixed,
    variable,
    fullscreen,
    window,
    allscreens
};

struct capture_settings
{
    HWND capture_hwnd_{0};
    rect<int> capture_rect_{0, 0, 0, 0};
    capture_type capture_type_{ capture_type::allscreens };

    int fps{30};
    std::string filename;
};

class capture_thread
{
public:
    capture_thread() = default;
    ~capture_thread();

    void start(capture_settings settings);
    void stop();

protected:
    void run();
    bool capture_screen_frame(const rect<int> &capture_dst_rect);

private:
    capture_settings capture_settings_;
    std::unique_ptr<cam_capture_source> capture_source_;
    std::thread capture_thread_;
    std::atomic<bool> run_{false};

    rect<int> capture_dst_rect_{0, 0, 0, 0};
};
