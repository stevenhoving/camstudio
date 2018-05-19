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
#include "video_writer.h"

#include <fmt/format.h>
#include <fmt/printf.h>

#include <filesystem>
#include <string>

#define STREAM_DURATION   10
#define STREAM_FRAME_RATE 25 /* 25 images/s */
#define STREAM_PIX_FMT    AV_PIX_FMT_YUV420P /* default pix_fmt */

#define SCALE_FLAGS SWS_BICUBIC

std::string avx_ts2str(int64_t ts)
{
    char x[AV_TS_MAX_STRING_SIZE] = { 0 };
    av_ts_make_string(x, ts);
    return x;
}

std::string avx_ts2timestr(int64_t ts, AVRational *tb)
{
    char x[AV_TS_MAX_STRING_SIZE] = { 0 };
    av_ts_make_time_string(x, ts, tb);
    return x;
}

std::string avx_err2str(int errnum)
{
    char x[AV_TS_MAX_STRING_SIZE] = { 0 };
    av_make_error_string(x, AV_ERROR_MAX_STRING_SIZE, errnum);
    return x;
}

void log_packet(const AVFormatContext *fmt_ctx, const AVPacket *pkt)
{
    AVRational *time_base = &fmt_ctx->streams[pkt->stream_index]->time_base;

    fmt::printf("pts:%s pts_time:%s dts:%s dts_time:%s duration:%s duration_time:%s stream_index:%d\n",
        avx_ts2str(pkt->pts), avx_ts2timestr(pkt->pts, time_base),
        avx_ts2str(pkt->dts), avx_ts2timestr(pkt->dts, time_base),
        avx_ts2str(pkt->duration), avx_ts2timestr(pkt->duration, time_base),
        pkt->stream_index);
}

int write_frame(AVFormatContext *fmt_ctx, const AVRational *time_base, AVStream *st, AVPacket *pkt)
{
    /* rescale output packet timestamp values from codec to stream timebase */
    av_packet_rescale_ts(pkt, *time_base, st->time_base);
    pkt->stream_index = st->index;

    /* Write the compressed frame to the media file. */
    log_packet(fmt_ctx, pkt);
    return av_interleaved_write_frame(fmt_ctx, pkt);
}

/* Add an output stream. */
void add_stream(output_stream *ost, AVFormatContext *oc, AVCodec **codec, enum AVCodecID codec_id, video_meta meta)
{
    AVCodecContext *c;
    int i;

    /* find the encoder */
    *codec = avcodec_find_encoder(codec_id);
    if (!(*codec)) {
        fprintf(stderr, "Could not find encoder for '%s'\n", avcodec_get_name(codec_id));
        exit(1);
    }

    ost->st = avformat_new_stream(oc, NULL);
    if (!ost->st) {
        fprintf(stderr, "Could not allocate stream\n");
        exit(1);
    }
    ost->st->id = oc->nb_streams - 1;
    c = avcodec_alloc_context3(*codec);
    if (!c) {
        fprintf(stderr, "Could not alloc an encoding context\n");
        exit(1);
    }
    ost->enc = c;

    switch ((*codec)->type) {
    case AVMEDIA_TYPE_AUDIO:
        c->sample_fmt = (*codec)->sample_fmts ?
            (*codec)->sample_fmts[0] : AV_SAMPLE_FMT_FLTP;
        c->bit_rate = 64000;
        c->sample_rate = 44100;
        if ((*codec)->supported_samplerates) {
            c->sample_rate = (*codec)->supported_samplerates[0];
            for (i = 0; (*codec)->supported_samplerates[i]; i++) {
                if ((*codec)->supported_samplerates[i] == 44100)
                    c->sample_rate = 44100;
            }
        }
        c->channels = av_get_channel_layout_nb_channels(c->channel_layout);
        c->channel_layout = AV_CH_LAYOUT_STEREO;
        if ((*codec)->channel_layouts) {
            c->channel_layout = (*codec)->channel_layouts[0];
            for (i = 0; (*codec)->channel_layouts[i]; i++) {
                if ((*codec)->channel_layouts[i] == AV_CH_LAYOUT_STEREO)
                    c->channel_layout = AV_CH_LAYOUT_STEREO;
            }
        }
        c->channels = av_get_channel_layout_nb_channels(c->channel_layout);
        ost->st->time_base = { 1, c->sample_rate };
        break;

    case AVMEDIA_TYPE_VIDEO:
        c->codec_id = codec_id;

        c->bit_rate = 400000; // just for testing..
        /* Resolution must be a multiple of two. */
        c->width = meta.width;
        c->height = meta.height;

        /* timebase: This is the fundamental unit of time (in seconds) in terms
        * of which frame timestamps are represented. For fixed-fps content,
        * timebase should be 1/framerate and timestamp increments should be
        * identical to 1. */
        ost->st->time_base = { 1, STREAM_FRAME_RATE };
        c->time_base = ost->st->time_base;

        c->gop_size = 12; /* emit one intra frame every twelve frames at most */
        c->pix_fmt = STREAM_PIX_FMT;
        if (c->codec_id == AV_CODEC_ID_MPEG2VIDEO) {
            /* just for testing, we also add B-frames */
            c->max_b_frames = 2;
        }
        break;

    default:
        break;
    }

    /* Some formats want stream headers to be separate. */
    if (oc->oformat->flags & AVFMT_GLOBALHEADER)
        c->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
}

/**************************************************************/
/* audio output */

AVFrame *alloc_audio_frame(enum AVSampleFormat sample_fmt, uint64_t channel_layout, int sample_rate, int nb_samples)
{
    AVFrame *frame = av_frame_alloc();
    int ret;

    if (!frame)
    {
        fprintf(stderr, "Error allocating an audio frame\n");
        exit(1);
    }

    frame->format = sample_fmt;
    frame->channel_layout = channel_layout;
    frame->sample_rate = sample_rate;
    frame->nb_samples = nb_samples;

    if (nb_samples) {
        ret = av_frame_get_buffer(frame, 0);
        if (ret < 0) {
            fprintf(stderr, "Error allocating an audio buffer\n");
            exit(1);
        }
    }

    return frame;
}

void open_audio(AVFormatContext *oc, AVCodec *codec, output_stream *ost, AVDictionary *opt_arg)
{
    AVCodecContext *c;
    int nb_samples;
    int ret;
    AVDictionary *opt = NULL;

    c = ost->enc;

    /* open it */
    av_dict_copy(&opt, opt_arg, 0);
    ret = avcodec_open2(c, codec, &opt);
    av_dict_free(&opt);
    if (ret < 0) {
        char err[AV_ERROR_MAX_STRING_SIZE] = { 0 };

        fprintf(stderr, "Could not open audio codec: %s\n", av_make_error_string(err, AV_ERROR_MAX_STRING_SIZE, ret));
        exit(1);
    }

    /* init signal generator */
    ost->t = 0;
    ost->tincr = static_cast<float>(2 * M_PI * 110.0 / c->sample_rate);
    /* increment frequency by 110 Hz per second */
    ost->tincr2 = static_cast<float>(2 * M_PI * 110.0 / c->sample_rate / c->sample_rate);

    if (c->codec->capabilities & AV_CODEC_CAP_VARIABLE_FRAME_SIZE)
        nb_samples = 10000;
    else
        nb_samples = c->frame_size;

    ost->frame = alloc_audio_frame(c->sample_fmt, c->channel_layout,
        c->sample_rate, nb_samples);
    ost->tmp_frame = alloc_audio_frame(AV_SAMPLE_FMT_S16, c->channel_layout,
        c->sample_rate, nb_samples);

    /* copy the stream parameters to the muxer */
    ret = avcodec_parameters_from_context(ost->st->codecpar, c);
    if (ret < 0) {
        fprintf(stderr, "Could not copy the stream parameters\n");
        exit(1);
    }

    /* create resampler context */
    ost->swr_ctx = swr_alloc();
    if (!ost->swr_ctx) {
        fprintf(stderr, "Could not allocate resampler context\n");
        exit(1);
    }

    /* set options */
    av_opt_set_int(ost->swr_ctx, "in_channel_count", c->channels, 0);
    av_opt_set_int(ost->swr_ctx, "in_sample_rate", c->sample_rate, 0);
    av_opt_set_sample_fmt(ost->swr_ctx, "in_sample_fmt", AV_SAMPLE_FMT_S16, 0);
    av_opt_set_int(ost->swr_ctx, "out_channel_count", c->channels, 0);
    av_opt_set_int(ost->swr_ctx, "out_sample_rate", c->sample_rate, 0);
    av_opt_set_sample_fmt(ost->swr_ctx, "out_sample_fmt", c->sample_fmt, 0);

    /* initialize the resampling context */
    if ((ret = swr_init(ost->swr_ctx)) < 0) {
        fprintf(stderr, "Failed to initialize the resampling context\n");
        exit(1);
    }
}

/* Prepare a 16 bit dummy audio frame of 'frame_size' samples and
* 'nb_channels' channels. */
AVFrame *get_audio_frame(output_stream *ost)
{
    AVFrame *frame = ost->tmp_frame;
    int j, i, v;
    int16_t *q = (int16_t*)frame->data[0];

    /* check if we want to generate more frames */
    if (av_compare_ts(ost->next_pts, ost->enc->time_base, STREAM_DURATION, { 1, 1 }) >= 0)
        return NULL;

    for (j = 0; j <frame->nb_samples; j++) {
        v = (int)(sin(ost->t) * 10000);
        for (i = 0; i < ost->enc->channels; i++)
            *q++ = v;
        ost->t += ost->tincr;
        ost->tincr += ost->tincr2;
    }

    frame->pts = ost->next_pts;
    ost->next_pts += frame->nb_samples;

    return frame;
}

/*
* encode one audio frame and send it to the muxer
* return 1 when encoding is finished, 0 otherwise
*/
static int write_audio_frame(AVFormatContext *oc, output_stream *ost)
{
    AVCodecContext *c;
    AVPacket pkt = { 0 }; // data and size must be 0;
    AVFrame *frame;
    int ret;
    int got_packet;
    int64_t dst_nb_samples;

    av_init_packet(&pkt);
    c = ost->enc;

    frame = get_audio_frame(ost);

    if (frame) {
        /* convert samples from native format to destination codec format, using the resampler */
        /* compute destination number of samples */
        dst_nb_samples = av_rescale_rnd(swr_get_delay(ost->swr_ctx, c->sample_rate) + frame->nb_samples,
            c->sample_rate, c->sample_rate, AV_ROUND_UP);
        av_assert0(dst_nb_samples == frame->nb_samples);

        /* when we pass a frame to the encoder, it may keep a reference to it
        * internally;
        * make sure we do not overwrite it here
        */
        ret = av_frame_make_writable(ost->frame);
        if (ret < 0)
            exit(1);

        /* convert to destination format */
        ret = swr_convert(ost->swr_ctx,
            ost->frame->data, (int)dst_nb_samples,
            (const uint8_t **)frame->data, frame->nb_samples);
        if (ret < 0) {
            fprintf(stderr, "Error while converting\n");
            exit(1);
        }
        frame = ost->frame;

        frame->pts = av_rescale_q(ost->samples_count, { 1, c->sample_rate }, c->time_base);
        ost->samples_count += (int)dst_nb_samples;
    }

    ret = avcodec_encode_audio2(c, &pkt, frame, &got_packet);
    if (ret < 0) {
        fmt::fprintf(stderr, "Error encoding audio frame: %s\n",
            avx_err2str(ret));
        exit(1);
    }

    if (got_packet) {
        ret = write_frame(oc, &c->time_base, ost->st, &pkt);
        if (ret < 0) {
            fmt::fprintf(stderr, "Error while writing audio frame: %s\n",
                avx_err2str(ret));
            exit(1);
        }
    }

    return (frame || got_packet) ? 0 : 1;
}

/**************************************************************/
/* video output */
AVFrame *alloc_picture(enum AVPixelFormat pix_fmt, int width, int height)
{
    AVFrame *picture;
    int ret;

    picture = av_frame_alloc();
    if (!picture)
        return NULL;

    picture->format = pix_fmt;
    picture->width = width;
    picture->height = height;

    /* allocate the buffers for the frame data */
    ret = av_frame_get_buffer(picture, 32);
    if (ret < 0) {
        fprintf(stderr, "Could not allocate frame data.\n");
        exit(1);
    }

    return picture;
}

void open_video(AVFormatContext *oc, AVCodec *codec, output_stream *ost, AVDictionary *opt_arg)
{
    int ret;
    AVCodecContext *c = ost->enc;
    AVDictionary *opt = NULL;

    av_dict_copy(&opt, opt_arg, 0);

    /* open the codec */
    ret = avcodec_open2(c, codec, &opt);
    av_dict_free(&opt);
    if (ret < 0) {
        fmt::fprintf(stderr, "Could not open video codec: %s\n", avx_err2str(ret));
        exit(1);
    }

    /* allocate and init a re-usable frame */
    ost->frame = alloc_picture(c->pix_fmt, c->width, c->height);
    if (!ost->frame) {
        fprintf(stderr, "Could not allocate video frame\n");
        exit(1);
    }

    /* If the output format is not YUV420P, then a temporary YUV420P
    * picture is needed too. It is then converted to the required
    * output format. */
    ost->tmp_frame = NULL;
    if (c->pix_fmt != AV_PIX_FMT_YUV420P) {
        ost->tmp_frame = alloc_picture(AV_PIX_FMT_YUV420P, c->width, c->height);
        if (!ost->tmp_frame) {
            fprintf(stderr, "Could not allocate temporary picture\n");
            exit(1);
        }
    }

    /* copy the stream parameters to the muxer */
    ret = avcodec_parameters_from_context(ost->st->codecpar, c);
    if (ret < 0) {
        fprintf(stderr, "Could not copy the stream parameters\n");
        exit(1);
    }
}

void close_stream(AVFormatContext *oc, output_stream *ost)
{
    avcodec_free_context(&ost->enc);
    av_frame_free(&ost->frame);
    av_frame_free(&ost->tmp_frame);
    sws_freeContext(ost->sws_ctx);
    swr_free(&ost->swr_ctx);
}

video_writer::video_writer(const char *filename, video_meta meta)
    : filename_(filename)
    , meta_(meta)
{
    av_register_all();

    //av_dict_set(&opt, "rotate", "90", 0);

    std::experimental::filesystem::path path(filename);
    path.replace_extension("mkv");

    std::string test = path.string();

    /* allocate the output media context */
    avformat_alloc_output_context2(&oc, NULL, NULL, test.c_str());
    if (!oc) {
        printf("Could not deduce output format from file extension: using MPEG.\n");
        avformat_alloc_output_context2(&oc, NULL, "mpeg", filename);
    }
    if (!oc)
        return;

    fmt = oc->oformat;

    /* Add the audio and video streams using the default format codecs
    * and initialize the codecs. */
    if (fmt->video_codec != AV_CODEC_ID_NONE) {
        add_stream(&video_st_, oc, &video_codec, fmt->video_codec, meta_);
        have_video = 1;
        encode_video = 1;
    }
    if (fmt->audio_codec != AV_CODEC_ID_NONE) {
        add_stream(&audio_st_, oc, &audio_codec, fmt->audio_codec, meta_);
        have_audio = 1;
        encode_audio = 1;
    }

    /* Now that all the parameters are set, we can open the audio and
    * video codecs and allocate the necessary encode buffers. */
    if (have_video)
        open_video(oc, video_codec, &video_st_, opt);

    if (have_audio)
        open_audio(oc, audio_codec, &audio_st_, opt);

    av_dump_format(oc, 0, filename, 1);

    int ret = 0;
    /* open the output file, if needed */
    if (!(fmt->flags & AVFMT_NOFILE)) {
        ret = avio_open(&oc->pb, filename, AVIO_FLAG_WRITE);
        if (ret < 0) {
            fmt::fprintf(stderr, "Could not open '%s': %s\n", filename,
                avx_err2str(ret));
            return;
        }
    }

    /* Write the stream header, if any. */
    ret = avformat_write_header(oc, &opt);
    if (ret < 0) {
        fmt::fprintf(stderr, "Error occurred when opening output file: %s\n",
            avx_err2str(ret));
        return;
    }
}

video_writer::~video_writer()
{
}

void video_writer::write(DWORD frametime, BITMAPINFOHEADER *alpbi)
{
    /* when we pass a frame to the encoder, it may keep a reference to it
     * internally; make sure we do not overwrite it here
     */
    if (av_frame_make_writable(video_st_.frame) < 0)
        exit(1);

    auto *ost = &video_st_;

    const auto *data = (LPBYTE)alpbi + alpbi->biSize + alpbi->biClrUsed * sizeof(RGBQUAD);
    const auto size = alpbi->biSizeImage;

    const int width = meta_.width;
    const int height = meta_.height;

    const auto pixel_format = AV_PIX_FMT_BGR24;

    if (pixel_format != AV_PIX_FMT_YUV420P)
    {
        /* as we only generate a YUV420P picture, we must convert it
         * to the codec pixel format if needed
         */
        if (!ost->sws_ctx)
        {
            ost->sws_ctx = sws_getContext(width, height,
                pixel_format,
                width, height,
                AV_PIX_FMT_YUV420P,
                SCALE_FLAGS, NULL, NULL, NULL);
            if (!ost->sws_ctx)
            {
                fprintf(stderr,
                    "Could not initialize the conversion context\n");
                exit(1);
            }
        }

        const uint8_t * const src[3] = {
            data + (size - width * 3),
            nullptr,
            nullptr
        };

        const int src_stride[3] = { -(width * 3), 0, 0 };
        const int dst_stride[3] = { width, width / 2, width / 2 };
        sws_scale(ost->sws_ctx, src,
            src_stride, 0,
            meta_.height, ost->frame->data,
            dst_stride);
    }

    //ost->frame->pts = frametime;

    ost->frame->pts = ost->next_pts++;

    int success = write_video_frame(oc, &video_st_, video_st_.frame);
    if (success)
        printf("frame written\n");
    else
        printf("frame not written\n");
}

int video_writer::write_video_frame(AVFormatContext *oc, output_stream *ost, AVFrame *frame)
{
    AVCodecContext *c;
    int got_packet = 0;
    AVPacket pkt = { 0 };

    c = ost->enc;

    av_init_packet(&pkt);

    /* encode the image */
    auto ret = avcodec_encode_video2(c, &pkt, frame, &got_packet);
    if (ret < 0) {
        fmt::fprintf(stderr, "Error encoding video frame: %s\n", avx_err2str(ret));
        exit(1);
    }

    if (got_packet)
    {
        ret = write_frame(oc, &c->time_base, ost->st, &pkt);
    }
    else
    {
        ret = 0;
    }

    if (ret < 0)
    {
        fmt::fprintf(stderr, "Error while writing video frame: %s\n", avx_err2str(ret));
        exit(1);
    }

    return (frame || got_packet) ? 0 : 1;
}

void video_writer::stop()
{
    /* Write the trailer, if any. The trailer must be written before you
    * close the CodecContexts open when you wrote the header; otherwise
    * av_write_trailer() may try to use memory that was freed on
    * av_codec_close(). */
    av_write_trailer(oc);

    /* Close each codec. */
    if (have_video)
        close_stream(oc, &video_st_);
    if (have_audio)
        close_stream(oc, &audio_st_);

    if (!(fmt->flags & AVFMT_NOFILE))
        /* Close the output file. */
        avio_closep(&oc->pb);

    /* free the stream */
    avformat_free_context(oc);
}

