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

#include "CamEncoder/av_cam_codec/av_cam_codec.h"
#include <minilzo/minilzo.h>
#include <zlib.h>

int __cdecl cam_codec_init(AVCodecContext *avctx)
{
    printf("camcodec encoder init\n");

    switch(avctx->pix_fmt)
    {
    case AV_PIX_FMT_RGB555LE:
        avctx->bits_per_coded_sample = 16;
        break;
    case AV_PIX_FMT_BGR24:
        [[fallthrough]];
    case AV_PIX_FMT_RGB24:
        avctx->bits_per_coded_sample = 24;
        avctx->pix_fmt = AV_PIX_FMT_BGR24;
        break;
    case AV_PIX_FMT_BGR0:
        [[fallthrough]];
    case AV_PIX_FMT_RGB0:
        avctx->bits_per_coded_sample = 32;
        avctx->pix_fmt = AV_PIX_FMT_BGR0;
        break;
    default:
        av_log(avctx, AV_LOG_ERROR, "CamStudio codec error: invalid pixel format %i\n", avctx->pix_fmt);
        return AVERROR_INVALIDDATA;
    }

    CamStudioContext *c = (CamStudioContext *)avctx->priv_data;
    c->bpp = avctx->bits_per_coded_sample;
    c->linelen = avctx->width * avctx->bits_per_coded_sample / 8;
    c->height = avctx->height;

    int stride = FFALIGN(c->linelen, 4);
    c->frame_size = c->height * stride; // I hope that this is correct

    c->comp_size = LZO1X_1_MEM_COMPRESS * 8;
    c->comp_buf = (unsigned char *)av_malloc(c->comp_size + AV_LZO_OUTPUT_PADDING);

    c->algorithm = 0; // we hardcode to lzo for now
    c->autokeyframe = 0; // we force enable keyframe insertion
    c->autokeyframe_rate = 25; // we force keyframe rate to 25
    c->currentFrame = 0; // the framecounter...

    c->previouse_frame = av_frame_alloc();
    if (c->previouse_frame == nullptr)
        return AVERROR(ENOMEM);

    c->delta_frame = av_frame_alloc();
    if (c->delta_frame == nullptr)
        return AVERROR(ENOMEM);

    return 0;
}

int __cdecl cam_codec_encode_picture(AVCodecContext *avctx, AVPacket *pkt, const AVFrame *frame, int *got_packet)
{
    printf("camcodec encode picture\n");
    CamStudioContext *c = (CamStudioContext *)avctx->priv_data;

    lzo_uint in_len = 0;
    lzo_uint out_len = 0;

    auto packet_size = c->frame_size + 2; // +2 because we have 2 header bytes
    if (int ret = ff_alloc_packet2(avctx, pkt, packet_size, 0); ret < 0)
        return ret;

    uint8_t *buf = pkt->data;

//#define CONVERTCOLORYUY2_BIT 1
//#define NON_KEYFRAME_BIT 0
#define KEYFRAME_BIT 1       // 0 ==> DELTA FRAME, 1==>KEY FRAME
//#define ALGORITHM_GZIP_BIT 2 // 000=>LZO , 001 = >GZIP, 010==>RESERVED, 011 ==>RESERVED, etc.
    // byte 1 :0000  000  0
    // byte 1 :level algo frametype

    // byte 2 :0000          00           00
    // byte 2 :reserved      origRGBbit   convertmodebit (colorspace)

    unsigned char keybit = KEYFRAME_BIT;
    unsigned char algo = static_cast<unsigned char>(c->algorithm) << 1;
    unsigned char gzip_level_bits = 0;
    if (c->algorithm == 1)
        gzip_level_bits = static_cast<unsigned char>(c->gzip_level) << 4;

    unsigned char convertmodebit = 0;
    unsigned char originalRGBbit = ((c->bpp / 8) - 1) << 2;

    unsigned char byte1 = keybit | algo | gzip_level_bits;
    unsigned char byte2 = convertmodebit | originalRGBbit;

    buf[0] = byte1; // set the frame to keyframe
    buf[1] = byte2;

    bool insert_keyframe = (c->currentFrame % c->autokeyframe_rate) == 0;

    if (c->autokeyframe == 0)
        insert_keyframe = true;

    if (insert_keyframe || c->previouse_frame->format == -1)
    {
        av_frame_copy(c->previouse_frame, frame);

        in_len = c->frame_size;
        auto r = lzo1x_1_compress(frame->data[0], in_len, buf + 2, &out_len, c->comp_buf);
        pkt->flags |= AV_PKT_FLAG_KEY;

        av_shrink_packet(pkt, out_len + 2);
    }
    else
    {
        __debugbreak();
    }

    *got_packet = 1;
    return 0;
}

int __cdecl cam_codec_encode_end(AVCodecContext *avctx)
{
    printf("camcodec encoder end\n");
    CamStudioContext *c = (CamStudioContext *)avctx->priv_data;
    av_freep(&c->comp_buf);
    av_frame_free(&c->previouse_frame);
    av_frame_free(&c->delta_frame);
    return 0;
}
