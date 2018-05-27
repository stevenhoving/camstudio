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

#include "stdafx.h"
#include "capture_thread.h"
#include <CamEncoder/av_encoder.h>
#include <fmt/printf.h>
#include <algorithm>

av_video_meta cam_create_video_config(const int width, const int height, const int fps)
{
    av_video_meta meta;
    // \todo make all these parameters configurable through the UI.
    meta.quality = 26;
    meta.bpp = 24;
    meta.width = width;
    meta.height = height;
    meta.fps = {fps, 1};
    meta.preset = video::preset::ultrafast;
    meta.profile = video::profile::baseline;
    meta.tune = video::tune::zerolatency;
    return meta;
}

std::unique_ptr<av_video> cam_create_video_codec(const av_video_meta &meta)
{
    av_video_codec video_codec_config;
    video_codec_config.id = AV_CODEC_ID_H264;
    // \todo remove 'pixel_format'.
    video_codec_config.pixel_format = AV_PIX_FMT_BGR24;

    return std::make_unique<av_video>(video_codec_config, meta);
}

capture_thread::~capture_thread()
{
    stop();
}

void capture_thread::start(capture_settings settings)
{
    if (run_)
    {
        fmt::print("capture_thread: unable to start capturing 2x\n");
        return;
    }

    run_ = true;
    capture_settings_ = settings;
    capture_thread_ = std::thread([this](){run();});

    fmt::print("recording started, {}\n", settings.filename);
}

void capture_thread::stop()
{
    if (!run_)
        return;

    run_ = false;
    if (capture_thread_.joinable())
        capture_thread_.join();
}

bool capture_thread::capture_screen_frame(const rect<int> &capture_dst_rect)
{
    capture_source_->set_capture_dst_rect(capture_dst_rect);
    return capture_source_->capture_frame(capture_dst_rect);
}

void capture_thread::run()
{
    auto current_desktop = OpenInputDesktop(0, FALSE, GENERIC_ALL);
    if (!current_desktop)
    {
        fmt::print("capture_thread: unable to get current desktop\n");
        // we might retry getting the current desktop
        return;
    }

    // bind desktop to this thread
    bool desktop_attached = SetThreadDesktop(current_desktop) != 0;
    CloseDesktop(current_desktop);
    current_desktop = nullptr;
    if (!desktop_attached)
    {
        fmt::print("capture_thread: unable to bind desktop to current thread\n");
        // we might retry getting the current desktop
        return;
    }

    capture_source_ = std::make_unique<cam_capture_source>(capture_settings_.capture_hwnd_);

    const auto pre_frame = capture_screen_frame(capture_settings_.capture_rect_) ? capture_source_->get_frame() : nullptr;

    /* Setup ffmpeg video encoder */
    const auto config = cam_create_video_config(pre_frame->biWidth, pre_frame->biHeight, capture_settings_.fps);

    auto video_encoder = std::make_unique<av_muxer>(capture_settings_.filename.c_str(),
        av_muxer_type::mkv);
    video_encoder->add_stream(cam_create_video_codec(config));
    video_encoder->open();

    cam::stop_watch stopwatch;
    stopwatch.time_start();
    while (run_)
    {
        double time_capture_start = stopwatch.time_now();
        const auto frame = capture_screen_frame(capture_settings_.capture_rect_) ? capture_source_->get_frame() : nullptr;

        DWORD timestamp = static_cast<DWORD>(time_capture_start * 1000.0);
        video_encoder->encode_frame(timestamp, frame);

        double time_capture_end = stopwatch.time_now();
        double dt = time_capture_end - time_capture_start;

        fmt::print("fps: {}\n", 1.0/dt);

        //Sleep(0);
    }

    video_encoder.reset();
    fmt::print("capture_thread: completed recording");
}
