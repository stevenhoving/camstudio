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

std::optional<video::preset> cam_create_codec_preset(video_codec_preset preset)
{
    switch(preset.get_index())
    {
        case video_codec_preset::type::ultrafast: return video::preset::ultrafast;
        case video_codec_preset::type::superfast: return video::preset::superfast;
        case video_codec_preset::type::veryfast: return video::preset::veryfast;
        case video_codec_preset::type::faster: return video::preset::faster;
        case video_codec_preset::type::fast: return video::preset::fast;
        case video_codec_preset::type::medium: return video::preset::medium;
        case video_codec_preset::type::slow: return video::preset::slow;
        case video_codec_preset::type::slower: return video::preset::slower;
        case video_codec_preset::type::veryslow: return video::preset::veryslow;
    }
    return {};
}

std::optional<video::tune> cam_create_codec_tune(video_codec_tune tune)
{
    switch(tune.get_index())
    {
    case video_codec_tune::type::none: return {};
    case video_codec_tune::type::film: return video::tune::film;
    case video_codec_tune::type::animation: return video::tune::animation;
    case video_codec_tune::type::grain: return video::tune::grain;
    case video_codec_tune::type::stillimage: return video::tune::stillimage;
    case video_codec_tune::type::fastdecode: return video::tune::fastdecode;
    case video_codec_tune::type::zerolatency: return video::tune::zerolatency;
    }
    return {};
}

std::optional<video::profile> cam_create_codec_profile(video_codec_profile profile)
{
    switch(profile.get_index())
    {
    case video_codec_profile::type::none: return {};
    case video_codec_profile::type::baseline: return video::profile::baseline;
    case video_codec_profile::type::main: return video::profile::main;
    case video_codec_profile::type::high: return video::profile::high;
    //case video_codec_profile::type::high10: return video::profile::high10; /* disable for now */
    //case video_codec_profile::type::high422: return video::profile::high422;
    //case video_codec_profile::type::high444: return video::profile::high444;
    }
    return {};
}

av_video_meta cam_create_video_config(const int width, const int height, const int fps, video_settings_model &settings)
{
    av_video_meta meta;

    meta.bpp = 24;
    meta.width = width;
    meta.height = height;
    meta.fps = {fps, 1};

    if (settings.video_codec_quality_type_ == video_quality_type::constant_quality)
        meta.quality = settings.video_codec_quality_constant_;
    else
        meta.bitrate = settings.video_codec_quality_bitrate_;

    meta.preset = cam_create_codec_preset(settings.video_codec_preset_);
    meta.profile = cam_create_codec_profile(settings.video_codec_profile_);
    meta.tune = cam_create_codec_tune(settings.video_codec_tune_);

    return meta;
}

std::unique_ptr<av_video> cam_create_video_codec(const av_video_meta &meta)
{
    av_video_codec video_codec_config;
    video_codec_config.id = AV_CODEC_ID_H264;
    // \todo remove 'pixel_format'.
    video_codec_config.pixel_format = AV_PIX_FMT_BGRA;

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

    capture_source_ = std::make_unique<cam_capture_source>(capture_settings_.capture_hwnd_, capture_settings_.capture_rect_);

    const auto pre_frame = capture_screen_frame(capture_settings_.capture_rect_) ? capture_source_->get_frame() : nullptr;

    /* Setup ffmpeg video encoder */
    // \todo video encoder framerate is ignored atm..
    const auto config = cam_create_video_config(
        pre_frame->bitmap_info->bmiHeader.biWidth,
        pre_frame->bitmap_info->bmiHeader.biHeight,
        30,
        capture_settings_.video_settings);

    auto video_encoder = std::make_unique<av_muxer>(capture_settings_.filename.c_str(),
        av_muxer_type::mkv);
    video_encoder->add_stream(cam_create_video_codec(config));
    video_encoder->open();

    cam::stop_watch stopwatch;
    stopwatch.time_start();

    cam::stop_watch frame_limiter;
    frame_limiter.time_start();

    const double max_frame_time = 1.0/capture_settings_.video_settings.video_source_fps_;
    while (run_)
    {
        stopwatch.time_start();
        double time_capture_start = frame_limiter.time_now();
        const auto frame = capture_screen_frame(capture_settings_.capture_rect_) ? capture_source_->get_frame() : nullptr;

        DWORD timestamp = static_cast<DWORD>(time_capture_start * 1000.0);
        video_encoder->encode_frame(timestamp, frame->bitmap_info, frame->bitmap_data);

        double time_capture_end = frame_limiter.time_now();
        double dt = time_capture_end - time_capture_start;

        double sleep_for = max_frame_time - dt;
        sleep_for = std::max(sleep_for, 0.0);

        std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(ceil(sleep_for * 1000.0))));

        dt = stopwatch.time_since();
        //fmt::print("fps: {}\n", 1.0 / dt);
    }

    video_encoder.reset();
    fmt::print("capture_thread: completed recording");
}
