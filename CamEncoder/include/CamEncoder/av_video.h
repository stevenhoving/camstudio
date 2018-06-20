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

#pragma once

#include "av_config.h"
#include "av_icodec.h"
#include "av_dict.h"
#include "av_ffmpeg.h"
#include <stdexcept>
#include <cstdint>

using timestamp_t = uint64_t;

enum class av_video_codec_type
{
    none,
    h264,
    cscd, // cam codec encoder
};

class av_video : public av_icodec
{
public:
    av_video(const av_video_codec &config, const av_video_meta &meta);
    ~av_video();

    void open(AVStream *stream, av_dict &dict) override;

    void push_encode_frame(timestamp_t timestamp, unsigned char *data, int width, int height, int stride);

    // this function will return false, if it was unable to read a encoded packet.
    bool pull_encoded_packet(AVPacket *pkt, bool *valid_packet) override;

    av_video_codec_type get_codec_type() const noexcept;
    AVCodecContext *get_codec_context() const noexcept override;
    AVRational get_time_base() const noexcept override;

private:
    // create a video frame scaler/converter so we can convert our rgb24 to a.e. yuv420.
    SwsContext *create_software_scaler(AVPixelFormat src_pixel_format, int src_width, int src_height,
                                       AVPixelFormat dst_pixel_format, int dst_width, int dst_height);

private:
    AVCodec *codec_{ nullptr };
    AVCodecContext *context_{ nullptr };
    AVFrame *frame_{ nullptr };

    AVPixelFormat input_pixel_format_{ AV_PIX_FMT_NONE };
    AVPixelFormat output_pixel_format_{ AV_PIX_FMT_NONE };
    SwsContext *sws_context_{ nullptr };

    av_video_codec_type codec_type_{ av_video_codec_type::none };
    av_dict av_opts_{};
};
