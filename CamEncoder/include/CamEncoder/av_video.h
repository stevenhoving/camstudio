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
#include "av_ffmpeg.h"
#include <stdexcept>
#include <cstdint>

/*!
 * for BITMAPINFO, we temporary encode BITMAPINFO frames.
 */
#include <windows.h>

using timestamp_t = uint64_t;

enum class av_video_codec_type
{
    none,
    h264, // ffmpeg h264
    x264, // libx264 h264
};

class av_video
{
public:
    av_video(const av_video_codec &config, const av_video_meta &meta);
    ~av_video();

    void push_encode_frame(timestamp_t timestamp, BITMAPINFO *image);

    // this function will return false, if it was unable to read a encoded packet.
    bool pull_encoded_packet(AVPacket *pkt, bool *valid_packet);

    av_video_codec_type get_codec_type() const noexcept;

private:
    // hack function... currently contains both the send to video encoder and receiving encoded
    // video frames code.
    int write_video_frame(AVFrame *frame);

    // create a reusable video frame for feeding data into the video encoder.
    AVFrame *create_video_frame(AVPixelFormat pix_fmt, int width, int height);

    // create a video frame scaler/converter so we can convert our rgb24 to a.e. yuv420.
    SwsContext *create_software_scaler(AVPixelFormat pix_fmt, int width, int height);
private:
    AVCodecContext *context_{ nullptr };
    AVFrame *frame_{ nullptr };
    SwsContext *sws_context_{ nullptr };

    av_video_codec_type codec_type_{ av_video_codec_type::none };
};
