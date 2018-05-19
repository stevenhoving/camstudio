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

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <string.h>
#include <string>

extern "C"
{
#include <libavcodec/avcodec.h>

#include <libavutil/opt.h>
#include <libavutil/imgutils.h>

#include <libavutil/avassert.h>
#include <libavutil/channel_layout.h>
#include <libavutil/opt.h>
#include <libavutil/mathematics.h>
#include <libavutil/timestamp.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>
}

struct video_meta
{
    int width;
    int height;
    int bpp;
    int fps;
};

// a wrapper around a single output AVStream
struct output_stream
{
    AVStream *st;
    AVCodecContext *enc;

    /* pts of the next frame that will be generated */
    int64_t next_pts;
    int samples_count;

    AVFrame *frame;
    AVFrame *tmp_frame;

    float t, tincr, tincr2;

    struct SwsContext *sws_ctx;
    struct SwrContext *swr_ctx;
};

class video_writer
{
public:
    video_writer(const char *filename, video_meta meta);
    ~video_writer();

    void write(DWORD frametime, BITMAPINFOHEADER *alpbi);

    /*
     * encode one video frame and send it to the muxer
     * return 1 when encoding is finished, 0 otherwise
     */
    int write_video_frame(AVFormatContext *oc, output_stream *ost, AVFrame *frame);

    void stop();

    int total_bytes_written();

    int write_frame(AVFormatContext *fmt_ctx, const AVRational *time_base, AVStream *st, AVPacket *pkt);

    /* Add an output stream. */
    void add_stream(output_stream *ost, AVFormatContext *oc, AVCodec **codec, enum AVCodecID codec_id, video_meta meta);

    /**************************************************************/
    /* audio output */
    AVFrame *alloc_audio_frame(enum AVSampleFormat sample_fmt, uint64_t channel_layout, int sample_rate, int nb_samples);

    void open_audio(AVFormatContext *oc, AVCodec *codec, output_stream *ost, AVDictionary *opt_arg);

    /* Prepare a 16 bit dummy audio frame of 'frame_size' samples and
    * 'nb_channels' channels. */
    AVFrame *get_audio_frame(output_stream *ost);

    /*
    * encode one audio frame and send it to the muxer
    * return 1 when encoding is finished, 0 otherwise
    */
    int write_audio_frame(AVFormatContext *oc, output_stream *ost);

    /**************************************************************/
    /* video output */
    AVFrame *alloc_picture(enum AVPixelFormat pix_fmt, int width, int height);

    void open_video(AVFormatContext *oc, AVCodec *codec, output_stream *ost, AVDictionary *opt_arg);

    void close_stream(AVFormatContext *oc, output_stream *ost);

private:
    std::string filename_;
    video_meta meta_;
    output_stream video_st_ = { 0 };
    output_stream audio_st_ = { 0 };

    AVOutputFormat *output_format_ = { nullptr };
    AVFormatContext *format_context_ = { nullptr };
    AVCodec *audio_codec_ = { nullptr };
    AVCodec *video_codec_ = { nullptr };

    int have_video_ = 0, have_audio_ = 0;
    int encode_video = 0, encode_audio_ = 0;
    AVDictionary *opt_ = { nullptr };
};