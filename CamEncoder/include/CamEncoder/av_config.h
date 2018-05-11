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

#include "av_ffmpeg.h"
#include <optional>
#include <string_view>
#include <string>
#include <array>

namespace video
{
    // h264 presets (maybe applicable for other video codecs)
    enum class preset
    {
        ultrafast,
        superfast,
        veryfast,
        faster,
        fast,
        medium,//default preset
        slow,
        slower,
        veryslow,
    };

    // \todo change when visual studio 2017 std::string or std::string_view gets its constexpr
    //       constructor.
    constexpr std::array<const char *, 9> preset_names = {
        "ultrafast",
        "superfast",
        "veryfast",
        "faster",
        "fast",
        "medium",
        "slow",
        "slower",
        "veryslow",
    };

    // h264 tune (might be applicable for other video codecs)
    enum class tune
    {
        film, // use for high quality movie content; lowers deblocking
        animation, // good for cartoons; uses higher deblocking and more reference frames
        grain, // preserves the grain structure in old, grainy film material
        stillimage, // good for slideshow-like content
        fastdecode, // allows faster decoding by disabling certain filters
        zerolatency, // good for fast encoding and low-latency streaming
    };

    constexpr std::array<const char *, 6> tune_names = {
        "film",
        "animation",
        "grain",
        "stillimage",
        "fastdecode",
        "zerolatency",
    };

    // h264 profile, which is its feature profile.
    // \see https://en.wikipedia.org/wiki/H.264/MPEG-4_AVC#Profiles
    enum class profile
    {
        baseline,
        main,
        high,
        high10,
        high422,
        high444
    };

    constexpr std::array<const char *, 6> profile_names = {
        "baseline",
        "main",
        "high",
        "high10",
        "high422",
        "high444"
    };

} // namespace video

struct frame_rate
{
    int den;
    int num;
};

// is a mix of video input config and user configurations. \todo split these.
struct av_video_meta
{
    int width = { 0 };
    int height = { 0 };
    int bpp = { 0 };
    frame_rate fps = { 0, 0 };
    std::optional<double> quality;
    std::optional<double> bitrate;
    std::optional<video::preset> preset;
    std::optional<video::tune> tune;
    std::optional<video::profile> profile; // for example h264
};

struct av_video_codec
{
    AVCodecID id;
};