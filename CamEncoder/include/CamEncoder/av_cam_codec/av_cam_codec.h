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

#include "CamEncoder/av_ffmpeg.h"

struct CamStudioContext
{
    AVClass *klass;

    /* encoder options */
    int algorithm;
    int gzip_level;
    int autokeyframe;
    int autokeyframe_rate;

    /* encoder members */
    AVFrame *previouse_frame;
    AVFrame *delta_frame;

    unsigned int comp_size;
    unsigned char *comp_buf;

    int linelen;
    int height;
    int bpp;
    int frame_size;

    // frame number counter.
    int currentFrame;
};

/* init video encoder */
int __cdecl cam_codec_init(AVCodecContext *avctx);
int __cdecl cam_codec_encode_picture(AVCodecContext *avctx, AVPacket *pkt, const AVFrame *frame, int *got_packet);
int __cdecl cam_codec_encode_end(AVCodecContext *avctx);

#define OFFSET(x) offsetof(CamStudioContext, x)
static const AVOption cam_codec_options[] = {
    { "algorithm", "42", OFFSET(algorithm), AV_OPT_TYPE_INT, {0}, 0, 1, AV_OPT_FLAG_ENCODING_PARAM, nullptr },
    { "gzip_level", "the gzip compression level 0-9", OFFSET(gzip_level), AV_OPT_TYPE_INT,{ 0 }, 0, 10, AV_OPT_FLAG_ENCODING_PARAM, nullptr },
    { "autokeyframe", "enable auto keyframe insertion, when disabled we are always inserting key frames", OFFSET(autokeyframe), AV_OPT_TYPE_INT,{ 1 }, 0, 1, AV_OPT_FLAG_ENCODING_PARAM, nullptr },
    { "autokeyframe_rate", "the rate of the keyframe insertion", OFFSET(autokeyframe_rate), AV_OPT_TYPE_INT,{ 25 }, 0, 1000 /* should be int max */, AV_OPT_FLAG_ENCODING_PARAM, nullptr },
    { nullptr },
};


static const AVClass cam_encoder_class = {
    "csce encoder",         // class_name
    av_default_item_name,   // const char* (*item_name)(void* ctx);
    cam_codec_options, // .option = 
    100, //LIBAVUTIL_VERSION_INT,  //.version = LIBAVUTIL_VERSION_INT,
};

/* technically the frame delta encoder doesn't care about the frame pixel format */
static const AVPixelFormat cam_codec_pxl_fmts[] = {
    // the original encoder supports:  1=YUY2, 2=UYVY, 3=RGB 24-bit, 4=RGB 32-bit
    AV_PIX_FMT_RGB555LE,
    AV_PIX_FMT_BGR24,
    //AV_PIX_FMT_RGB24, // bgr or rgb are transparently encoded.
    AV_PIX_FMT_BGR0,
    //AV_PIX_FMT_RGB0, // bgr or rgb are transparently encoded.
    //AV_PIX_FMT_YUV420P, // we can support yuv420...
    (AVPixelFormat)-1
};

static AVCodec cam_codec_encoder = {
    "cscd",                         // const char *name;
    "CamStudio",                    // const char *long_name;
    AVMEDIA_TYPE_VIDEO,             // enum AVMediaType type;
    AV_CODEC_ID_CSCD,               // enum AVCodecID id;
    0,                              // int capabilities;
    nullptr,                        // const AVRational *supported_framerates;
    cam_codec_pxl_fmts,             // const enum AVPixelFormat *pix_fmts;
    nullptr,                        // const int *supported_samplerates;
    nullptr,                        // const enum AVSampleFormat *sample_fmts;
    nullptr,                        // const uint64_t *channel_layouts;
    16,                             // uint8_t max_lowres;
    &cam_encoder_class,             // const AVClass *priv_class;
    nullptr,                        // const AVProfile *profiles;
    nullptr,                        // const char *wrapper_name;
    // private data fields
    sizeof(CamStudioContext),      // int priv_data_size;
    nullptr,                        // struct AVCodec *next;
    nullptr,                        // int(*init_thread_copy)(AVCodecContext *);
    nullptr,                        // int(*update_thread_context)(AVCodecContext *dst, const AVCodecContext *src);
    nullptr,                        // const AVCodecDefault *defaults; (\todo do we need this?)
    nullptr,                        // void(*init_static_data)(struct AVCodec *codec);
    cam_codec_init,             // int(*init)(AVCodecContext *);
    nullptr,                        // int(*encode_sub)(AVCodecContext *, uint8_t *buf, int buf_size, const struct AVSubtitle *sub);
    cam_codec_encode_picture,          // int(*encode2)(AVCodecContext *avctx, AVPacket *avpkt, const AVFrame *frame, int *got_packet_ptr);
    nullptr,                        // int(*decode)(AVCodecContext *, void *outdata, int *outdata_size, AVPacket *avpkt);
    cam_codec_encode_end,              // int(*close)(AVCodecContext *);
    nullptr,                        // int(*send_frame)(AVCodecContext *avctx, const AVFrame *frame);
    nullptr,                        // int(*receive_packet)(AVCodecContext *avctx, AVPacket *avpkt);
    nullptr,                        // int(*receive_frame)(AVCodecContext *avctx, AVFrame *frame);
    nullptr,                        // void(*flush)(AVCodecContext *);
    0,                              // int caps_internal;
    nullptr,                        // const char *bsfs;
    nullptr,                        // const struct AVCodecHWConfigInternal **hw_configs;
};

static int ff_alloc_packet2(AVCodecContext *avctx, AVPacket *avpkt, int64_t size, int64_t min_size)
{
    if (avpkt->size < 0)
    {
        av_log(avctx, AV_LOG_ERROR, "Invalid negative user packet size %d\n", avpkt->size);
        return AVERROR(EINVAL);
    }
    if (size < 0 || size > INT_MAX - AV_INPUT_BUFFER_PADDING_SIZE)
    {
        av_log(avctx, AV_LOG_ERROR, "Invalid minimum required packet size %" PRId64 " (max allowed is %d)\n", size,
               INT_MAX - AV_INPUT_BUFFER_PADDING_SIZE);
        return AVERROR(EINVAL);
    }

    if (avpkt->data)
    {
        AVBufferRef *buf = avpkt->buf;

        if (avpkt->size < size)
        {
            av_log(avctx, AV_LOG_ERROR, "User packet is too small (%d < %" PRId64 ")\n", avpkt->size, size);
            return AVERROR(EINVAL);
        }

        av_init_packet(avpkt);
        avpkt->buf = buf;
        avpkt->size = (int)size;
        return 0;
    }
    else
    {
        int ret = av_new_packet(avpkt, (int)size);
        if (ret < 0)
            av_log(avctx, AV_LOG_ERROR, "Failed to allocate packet of size %" PRId64 "\n", size);
        return ret;
    }
}
