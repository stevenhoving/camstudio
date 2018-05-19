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

#include <memory>
#include <string>
#include <vector>
#include <array>

enum class av_track_type
{
    video,
    audio
};

struct av_track_x
{
    av_track_type type;
    AVStream *stream;
    //int64_t duration;
    AVCodecContext *codec_context;
    //AVFrame *frame;
};

enum class av_muxer_type
{
    none,
    mp4,
    mkv
};

constexpr std::array<const char*, 3> av_muxer_type_names = {
    "invalid",
    "mp4",
    "matroska"
};

// a wrapper around a single output AVStream
struct av_track
{
    av_track_type type;
    AVStream *stream;
    AVCodecContext *codec_context;

    /* pts of the next frame that will be generated */
    int64_t next_pts;
    int samples_count;
};

class av_muxer
{
public:
    av_muxer(const char *filename, av_muxer_type muxer_type);
    ~av_muxer();

    // open the muxer so its ready to encode stuff.
    void open();

    void flush();

    // Add a video codec as track/stream.
    void add_stream(std::unique_ptr<av_video> video_codec);

    // this encodes a video frame.
    void encode_frame(timestamp_t timestamp, BITMAPINFO *image);

    int write_frame(AVFormatContext *format_context, const AVRational &time_base, AVStream *st, AVPacket *pkt);

    /**************************************************************/
    /* audio output */

    AVFrame *alloc_audio_frame(enum AVSampleFormat sample_fmt, uint64_t channel_layout, int sample_rate, int nb_samples);

    void open_audio(AVFormatContext *format_context, AVCodec *codec, av_track *ost, AVDictionary *opt_arg);

    /* Prepare a 16 bit dummy audio frame of 'frame_size' samples and
    * 'nb_channels' channels. */
    //AVFrame *get_audio_frame(av_track *ost);

    /*
    * encode one audio frame and send it to the muxer
    * return 1 when encoding is finished, 0 otherwise
    */
    int write_audio_frame(AVFormatContext *format_context, av_track *ost);

    void close_stream(AVFormatContext *format_context, av_track *ost);

private:
    AVFormatContext *format_context_{nullptr};
    AVOutputFormat *output_format_{ nullptr };
    std::unique_ptr<av_video> video_codec_;
    av_track video_st{};
    av_track audio_st{};
    std::string filename_;
    int have_video = 0, have_audio = 0;
    int encode_video = 0, encode_audio = 0;
};
