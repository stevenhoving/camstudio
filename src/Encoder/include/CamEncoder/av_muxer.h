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

#include "av_config.h"
#include "av_audio.h"
#include "av_video.h"
#include "circular_frame_buffer.h"

#include <memory>
#include <string>
#include <array>

enum class av_track_type
{
    video,
    audio
};

enum class av_muxer_type
{
    none,
    mp4,
    mkv,
    avi
};

constexpr std::array<const char*, 4> av_muxer_type_names = {
    "invalid",
    "mp4",
    "matroska",
    "avi"
};

// a wrapper around a single output AVStream
struct av_track
{
    av_track_type type;
    AVStream *stream;
    AVCodecContext *codec_context;

    int sample_count;
};

struct av_metadata
{
    std::string encoding_tool;
};

struct muxer_settings
{
    bool use_circular_buffer{false};
    int circular_buffer_time{0};
};

class av_muxer
{
public:
    av_muxer(muxer_settings settings, std::string filename, const av_muxer_type muxer_type, av_metadata metadata);
    ~av_muxer();

    // open the muxer so its ready to encode stuff.
    void open();
    void flush();

    // Add a video codec as track/stream.
    void add_stream(std::unique_ptr<av_video> video_codec);

    // this sends a video frame to the video encoder and sends any pending results to the muxer.
    void encode_frame(timestamp_t timestamp, unsigned char *data, int width, int height, int stride);

    /* audio output */
    AVFrame *alloc_audio_frame(enum AVSampleFormat sample_fmt, uint64_t channel_layout,
        int sample_rate, int nb_samples);

    void open_audio(AVFormatContext *format_context, AVCodec *codec, av_track *track,
        AVDictionary *opt_arg);

    /*
     * encode one audio frame and send it to the muxer
     * return 1 when encoding is finished, 0 otherwise
     */
    int write_audio_frame(av_track *ost, AVFrame *frame);

    void close_stream(AVFormatContext *format_context, av_track *ost);

private:
    int write_frame(const AVRational &time_base, AVStream *st, AVPacket *pkt);
    void write_circular_frame_buffer();

private:
    AVFormatContext *format_context_{ nullptr };
    AVOutputFormat *output_format_{ nullptr };
    std::unique_ptr<av_video> video_codec_{};
    //std::unique_ptr<av_audio> audio_codec_;
    av_track video_track{};
    av_track audio_track{};
    std::string filename_{};
    av_metadata metadata_{};
    AVRational time_base_{1, 0};
    circular_frame_buffer buffer_;
    muxer_settings settings_;

    // Commented, because we do not handle audio yet.
    //int have_audio{0};
    //int encode_audio{0};
};
