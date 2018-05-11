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

#include "av_config.h"
#include "av_audio.h"
#include "av_video.h"

#include <fmt/format.h>
#include <string>
#include <vector>

enum class av_track_type
{
    video,
    audio
};

struct av_track
{
    av_track_type type;
    AVStream *stream;
    int64_t duration;
};

enum class av_muxer_type
{
    none,
    mp4,
    mkv
};

// also known as a ffmpeg formatter
class av_muxer
{
public:
    av_muxer(const char *filename, av_muxer_type muxer, bool mp4_optimize, const av_video_meta &meta, av_video &vcodec);
    ~av_muxer();
private:
    AVFormatContext *context_{ nullptr };
    AVOutputFormat *output_format_{ nullptr };

    std::string muxer_name_;
    av_muxer_type muxer_{ av_muxer_type ::none };
    AVRational time_base_;

    std::vector<av_track> tracks_;
};
