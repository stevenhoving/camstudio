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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#include "stdafx.h"
#include "capture_thread.h"
#include "buildinfo.h"
#include "logging/logging.h"
#include <CamEncoder/av_encoder.h>
#include <screen_capture/cam_stop_watch.h>
#include <screen_capture/annotations/cam_annotation_cursor.h>
#include <algorithm>
#include <fmt/format.h>

static auto logger = logging::get_logger("capture thread");

av_muxer_type cam_get_file_container(const video_container &container)
{
    switch (container.get_index())
    {
        case video_container::type::avi: return av_muxer_type::avi;
        case video_container::type::mp4: return av_muxer_type::mp4;
        case video_container::type::mkv: return av_muxer_type::mkv;
    }
    return {};
}

video::codec cam_create_codec(const video_codec &codec)
{
    switch (codec.get_index())
    {
        case video_codec::type::x264: return video::codec::x264;
        case video_codec::type::camstudio: return video::codec::camstudio;
    }
    return {};
}

std::optional<video::preset> cam_create_codec_preset(const video_codec_preset &preset)
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

std::optional<video::tune> cam_create_codec_tune(const video_codec_tune &tune)
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

std::optional<video::profile> cam_create_codec_profile(const video_codec_profile &profile)
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

std::optional<video::codec_level> cam_create_codec_level(video_codec_level level)
{
    switch (level.get_index())
    {
    case video_codec_level::type::none: return video::codec_level::none;
    case video_codec_level::type::level1_0: return video::codec_level::level1_0;
    case video_codec_level::type::level1_b: return video::codec_level::level1_b;
    case video_codec_level::type::level1_1: return video::codec_level::level1_1;
    case video_codec_level::type::level1_2: return video::codec_level::level1_2;
    case video_codec_level::type::level1_3: return video::codec_level::level1_3;
    case video_codec_level::type::level2_0: return video::codec_level::level2_0;
    case video_codec_level::type::level2_1: return video::codec_level::level2_1;
    case video_codec_level::type::level2_2: return video::codec_level::level2_2;
    case video_codec_level::type::level3_0: return video::codec_level::level3_0;
    case video_codec_level::type::level3_1: return video::codec_level::level3_1;
    case video_codec_level::type::level3_2: return video::codec_level::level3_2;
    case video_codec_level::type::level4_0: return video::codec_level::level4_0;
    case video_codec_level::type::level4_1: return video::codec_level::level4_1;
    case video_codec_level::type::level4_2: return video::codec_level::level4_2;
    case video_codec_level::type::level5_0: return video::codec_level::level5_0;
    case video_codec_level::type::level5_1: return video::codec_level::level5_1;
    case video_codec_level::type::level5_2: return video::codec_level::level5_2;
    }
    return {};
}

av_video_meta cam_create_video_config(const int width, const int height, const int fps, const video_settings_model &settings)
{
    av_video_meta meta;

    meta.bpp = 24;
    meta.width = width;
    meta.height = height;
    meta.fps = {fps, 1};

    switch(settings.video_container_.get_index())
    {
    case video_container::avi:  meta.container = video::container::avi; break;
    case video_container::mp4:  meta.container = video::container::mp4; break;
    case video_container::mkv:  meta.container = video::container::mkv; break;
    }

    if (settings.video_codec_quality_type_ == video_quality_type::constant_quality)
        meta.quality = settings.video_codec_quality_constant_;
    else
        meta.bitrate = settings.video_codec_quality_bitrate_;

    meta.codec = cam_create_codec(settings.video_codec_);
    meta.preset = cam_create_codec_preset(settings.video_codec_preset_);
    meta.profile = cam_create_codec_profile(settings.video_codec_profile_);
    meta.tune = cam_create_codec_tune(settings.video_codec_tune_);
    meta.level = cam_create_codec_level(settings.video_codec_level_);

    return meta;
}

std::unique_ptr<av_video> cam_create_video_codec(const av_video_meta &meta)
{
    av_video_codec video_codec_config;
    // \todo remove 'pixel_format'.
    video_codec_config.pixel_format = AV_PIX_FMT_BGRA;

    return std::make_unique<av_video>(video_codec_config, meta);
}

capture_thread::capture_thread(const std::function<void()> &on_recording_completed,
                               const std::function<void()> &on_recording_canceled)
    : on_recording_completed_(on_recording_completed)
    , on_recording_canceled_(on_recording_canceled)
{
}

capture_thread::~capture_thread()
{
    stop();
}

void capture_thread::start(capture_settings settings)
{
    if (run_ || capture_state_ != capture_state::stopped)
    {
        logger->error("capture_thread: unable to start capturing 2x");
        return;
    }

    run_ = true;
    capture_state_ = capture_state::capturing;
    capture_settings_ = std::move(settings);
    capture_thread_ = std::thread([this](){run();});

    logger->debug("capturing started, {}", settings.filename);
}

void capture_thread::stop()
{
    if (!run_)
        return;

    run_ = false;

    capture_state_ = capture_state::stopping;
    logger->debug("capturing stopping");

    if (capture_thread_.joinable())
        capture_thread_.join();

    capture_state_ = capture_state::stopped;
    logger->debug("capturing stopped");
}

void capture_thread::cancel()
{
    if (!run_)
        return;

    run_ = false;

    capture_state_ = capture_state::canceling;
    logger->debug("capturing canceling");

    if (capture_thread_.joinable())
        capture_thread_.join();

    capture_state_ = capture_state::canceled;
    logger->debug("capturing canceled");
}

void capture_thread::pause()
{
    if (capture_state_ == capture_state::capturing)
        capture_state_ = capture_state::paused;
}

void capture_thread::unpause()
{
    if (capture_state_ == capture_state::paused)
        capture_state_ = capture_state::capturing;
}

capture_state capture_thread::get_capture_state() const noexcept
{
    return capture_state_;
}

const cam_frame *capture_thread::capture_screen_frame(const cam::rect<int> &capture_dst_rect)
{
    if (!capture_source_->capture_frame(capture_dst_rect))
        return nullptr;
    return capture_source_->get_frame();
}

void capture_thread::run()
{
    auto current_desktop = ::OpenInputDesktop(0, FALSE, GENERIC_ALL);
    if (!current_desktop)
    {
        logger->error("capture_thread: unable to get current desktop");
        // \todo we might retry getting the current desktop.
        return;
    }

    // bind current desktop to this thread.
    bool desktop_attached = ::SetThreadDesktop(current_desktop) != 0;
    ::CloseDesktop(current_desktop);
    current_desktop = nullptr;
    if (!desktop_attached)
    {
        logger->error("capture_thread: unable to bind desktop to current thread");
        // \todo we might retry getting the current desktop.
        return;
    }

    capture_source_ = std::make_unique<cam_capture_source>(capture_settings_.capture_hwnd_,
        capture_settings_.capture_rect_);
    capture_source_->enable_annotations();

    const auto &settings = capture_settings_.settings;

    const auto halo_size = cam::size(
        settings.get_cursor_halo_size(),
        settings.get_cursor_halo_size()
    );

    const auto ring_size = cam::size(
        settings.get_cursor_ring_size(),
        settings.get_cursor_ring_size()
    );

    const auto show_cursor_enabled = settings.get_cursor_enabled();
    const auto show_cursor_ring_enabled = settings.get_cursor_ring_enabled();
    const auto show_cursor_halo_enabled = settings.get_cursor_halo_enabled();
    const auto show_cursor_halo_clicks_enabled = settings.get_cursor_click_enabled();

    capture_source_->add_annotation(
        std::make_unique<cam_annotation_cursor>(
            show_cursor_enabled,
            show_cursor_ring_enabled,
            mouse_action_config{ show_cursor_ring_enabled, ring_size, settings.get_cursor_click_left_color() },
            mouse_action_config{ show_cursor_ring_enabled, ring_size, settings.get_cursor_click_right_color() },
            mouse_action_config{ show_cursor_ring_enabled, ring_size, settings.get_cursor_click_middle_color() },

            static_cast<cam_halo_type>(capture_settings_.settings.get_cursor_halo_type()),
            mouse_action_config{ show_cursor_halo_enabled, halo_size, settings.get_cursor_halo_color() },
            mouse_action_config{ show_cursor_halo_clicks_enabled, halo_size, settings.get_cursor_click_left_color() },
            mouse_action_config{ show_cursor_halo_clicks_enabled, halo_size, settings.get_cursor_click_right_color() },
            mouse_action_config{ show_cursor_halo_clicks_enabled, halo_size, settings.get_cursor_click_middle_color() }
    ));

    const auto pre_frame = capture_screen_frame(capture_settings_.capture_rect_);
    if (pre_frame == nullptr)
    {
        logger->error("capture_thread: unable to capture the pre frame");
        return;
    }

    /* Setup ffmpeg video encoder */
    // \todo video encoder framerate is ignored atm.
    const auto config = cam_create_video_config(
        pre_frame->width,
        pre_frame->height,
        30,
        capture_settings_.video_settings);

    const av_metadata metadata = {fmt::format("CamStudio {}", buildinfo::full_version)};

    const bool limit_capture_time = (capture_settings_.video_settings.max_capture_time_ > 0);
    const muxer_settings muxer_settings = {limit_capture_time, capture_settings_.video_settings.max_capture_time_};

    auto video_encoder =
        std::make_unique<av_muxer>(muxer_settings,
        capture_settings_.filename,
        cam_get_file_container(capture_settings_.video_settings.video_container_),
        metadata);

    video_encoder->add_stream(cam_create_video_codec(config));
    video_encoder->open();

    cam::stop_watch frame_limiter;
    frame_limiter.time_start();

    const auto max_frame_time = 1.0/capture_settings_.video_settings.video_source_fps_;
    while (run_)
    {
        const auto timestamp_capture_start = frame_limiter.time_now();
        const auto frame = capture_screen_frame(capture_settings_.capture_rect_);

        if (frame != nullptr)
        {
            const auto timestamp = static_cast<timestamp_t>(timestamp_capture_start * 1000.0);
            video_encoder->encode_frame(timestamp, frame->bitmap_data, frame->width, frame->height,
                frame->stride);
        }

        const auto timestamp_capture_end = frame_limiter.time_now();
        
        if (const auto sleep_for = max_frame_time - (timestamp_capture_end - timestamp_capture_start); sleep_for > 0)
            std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(std::ceil(sleep_for * 1000.0))));

        while (capture_state_ == capture_state::paused && run_ == true)
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    video_encoder.reset();
    logger->debug("capture_thread: completed capturing");

    if (capture_state_ == capture_state::stopping)
        on_recording_completed_();
    else /* if (capture_state == capture_state::canceling) */
        on_recording_canceled_();
}
