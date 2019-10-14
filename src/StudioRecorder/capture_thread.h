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

#pragma once

#include <screen_capture/cam_capture.h>
#include <screen_capture/cam_rect.h>

#include "video_settings_ui.h"
#include "settings_model.h"

#include <atomic>
#include <thread>
#include <functional>

struct capture_settings
{
    HWND capture_hwnd_{0};
    cam::rect<int> capture_rect_{0, 0, 0, 0};
    capture_type capture_type_{ capture_type::allscreens };

    std::string filename;
    video_settings_model video_settings;
    settings_model settings;
};

enum class capture_state
{
    stopping,
    stopped,
    capturing,
    paused,
    canceling,
    canceled
};

class capture_thread
{
public:
    capture_thread(
        const std::function<void()> &on_recording_completed,
        const std::function<void()> &on_recording_canceled
    );
    ~capture_thread();
    capture_thread(const capture_thread &) = delete;
    capture_thread &operator = (const capture_thread &) = delete;

    void start(capture_settings settings);
    /* stop the recording and finish */
    void stop();
    /* cancel the recording and cleanup */
    void cancel();
    /* pause the current recording */
    void pause();
    /* unpause a paused recording */
    void unpause();
    /* returns the capture state */
    capture_state get_capture_state() const noexcept;

protected:
    void run();
    const cam_frame *capture_screen_frame(const cam::rect<int> &capture_dst_rect);

private:
    capture_settings capture_settings_;
    std::unique_ptr<cam_capture_source> capture_source_;
    std::thread capture_thread_;
    std::atomic<bool> run_{false};
    std::atomic<capture_state> capture_state_{capture_state::stopped};

    cam::rect<int> capture_dst_rect_{0, 0, 0, 0};

    std::function<void()> on_recording_completed_;
    std::function<void()> on_recording_canceled_;
};
