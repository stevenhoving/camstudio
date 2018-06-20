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

#include "CamEncoder/av_cam_codec/av_cam_codec.h"
#include <minilzo/minilzo.h>
#include <zlib.h>

int ff_alloc_packet2(AVCodecContext *avctx, AVPacket *avpkt, int64_t size, int64_t min_size)
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

int __cdecl cam_codec_init(AVCodecContext *avctx)
{
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

    //c->algorithm = 0; // we hardcode to lzo for now
    //c->autokeyframe = 1; // we force enable keyframe insertion
    //c->autokeyframe_rate = 25; // we force keyframe rate to 25
    c->currentFrame = 0; // the framecounter...

    c->previouse_frame = av_frame_alloc();
    if (c->previouse_frame == nullptr)
        return AVERROR(ENOMEM);

    c->delta_frame = av_frame_alloc();
    if (c->delta_frame == nullptr)
        return AVERROR(ENOMEM);

    c->previouse_frame->format = avctx->pix_fmt;
    c->previouse_frame->width = avctx->width;
    c->previouse_frame->height = avctx->height;

    c->delta_frame->format = avctx->pix_fmt;
    c->delta_frame->width = avctx->width;
    c->delta_frame->height = avctx->height;

    if (int ret = av_frame_get_buffer(c->previouse_frame, 1); ret < 0)
    {
        printf("unable to allocate prev video frame\n");
        return AVERROR(ENOMEM);
    }

    if (int ret = av_frame_get_buffer(c->delta_frame, 1); ret < 0)
    {
        printf("unable to allocate delta video frame\n");
        return AVERROR(ENOMEM);
    }

    return 0;
}

int gzip_compress(const Bytef * src, uLong src_len, Bytef *dst, uLongf *dst_len, const int level)
{
    return compress2(dst, dst_len, src, src_len, level);
}

/*   0                               1
 * |              byte 1           |               byte 2          |
 * | 7   6   5   4   3   2   1   0 | 7   6   5   4   3   2   1   0 |
 * +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
 * |     level     |   algo    |key|  reserved     | RGBbit| cmode |
 * +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
 *
 * Level: 4 bits
 *
 *   Level was initially used to store the gzip compression level. But this value is currently not
 *   used by the decoder.
 *
 * Algo: 3 bits
 *
 *   Algo stores the used compression algorithm.
 *   - 0 lzo
 *   - 1 gzip
 *   - 2 reserved
 *   - 3 reserved
 *   - 4 reserved
 *   - 5 reserved
 *   - 6 reserved
 *   - 7 reserved
 *
 * Key: 1 bit
 *
 *   Key stores the frame type.
 *   - 0 delta frame
 *   - 1 key frame.
 *
 * RGBbit: 2 bit.
 *
 *   RGBBit is intended to store the original bits per pixel.
 *   - 0 undefined
 *   - 1 16 bit rgb
 *   - 2 24 bit rgb
 *   - 3 32 bit rgba
 *
 * Cmode: 2 bit.
 *
 *   The original purpose of cmode a.k.a convertmodebit (colorspace) is not known to me. We can only
 *   guess what its purpose would have been. In the meantime these 2 bits are unused.
 */

#define CSCD_NON_KEYFRAME_BIT 0
#define CSCD_KEYFRAME_BIT 1

int __cdecl cam_codec_encode_picture(AVCodecContext *avctx, AVPacket *pkt, const AVFrame *frame, int *got_packet)
{
    CamStudioContext *c = (CamStudioContext *)avctx->priv_data;

    lzo_uint in_len = 0;

    const auto initial_packet_size = c->frame_size + 2; // +2 because we have 2 header bytes
    if (int ret = ff_alloc_packet2(avctx, pkt, initial_packet_size, 0); ret < 0)
        return ret;

    uint8_t *buf = pkt->data;

    bool insert_keyframe = (c->currentFrame % c->autokeyframe_rate) == 0;

    unsigned char keybit = insert_keyframe ? CSCD_KEYFRAME_BIT : CSCD_NON_KEYFRAME_BIT;
    unsigned char algo = static_cast<unsigned char>(c->algorithm) << 1;
    unsigned char gzip_level_bits = 0;

    /* why would you need to store the gzip compression level in your bytestream? */
    if (c->algorithm == 1)
        gzip_level_bits = static_cast<unsigned char>(c->gzip_level) << 4;

    unsigned char convertmodebit = 0;
    unsigned char originalRGBbit = ((c->bpp / 8) - 1) << 2;

    unsigned char byte1 = keybit | algo | gzip_level_bits;
    unsigned char byte2 = convertmodebit | originalRGBbit;

    buf[0] = byte1;
    buf[1] = byte2;

    /* when auto key frame is disabled, it means that we always insert a keyframe. */
    if (c->autokeyframe == 0)
        insert_keyframe = true;

    in_len = c->frame_size;
    if (insert_keyframe)
    {
        int ret = av_frame_copy(c->previouse_frame, frame);
        if (ret < 0)
        {
            // just return some error code... for now...
            return AVERROR(ENOMEM);
        }

        if (c->algorithm == 0)
        {
            lzo_uint out_len = 0;
            auto r = lzo1x_1_compress(frame->data[0], in_len, buf + 2, &out_len, c->comp_buf);
            pkt->flags |= AV_PKT_FLAG_KEY;

            av_shrink_packet(pkt, static_cast<int>(out_len + 2));
        }
        else if (c->algorithm == 1)
        {
            uLong out_len = initial_packet_size;
            auto r = gzip_compress(frame->data[0], static_cast<uLong>(in_len), buf + 2, &out_len, c->gzip_level);
            pkt->flags |= AV_PKT_FLAG_KEY;

            av_shrink_packet(pkt, static_cast<int>(out_len + 2));
        }
    }
    else
    {
        /* for now only support interleaved (planar needs a bit of extra work) */
        unsigned char *diffinputptr = c->delta_frame->data[0];
        unsigned char *inputptr = frame->data[0];
        unsigned char *prevFrameptr = c->previouse_frame->data[0];

        for (unsigned long i = 0; i != in_len; ++i)
        {
            *diffinputptr = *inputptr - *prevFrameptr;
            *prevFrameptr = *inputptr;

            prevFrameptr++;
            inputptr++;
            diffinputptr++;
        }

        if (c->algorithm == 0)
        {
            lzo_uint out_len = 0;
            auto r = lzo1x_1_compress(c->delta_frame->data[0], in_len, buf + 2, &out_len, c->comp_buf);
            pkt->flags &= ~AV_PKT_FLAG_KEY;

            av_shrink_packet(pkt, static_cast<int>(out_len + 2));
        }
        else if (c->algorithm == 1)
        {
            uLong out_len = initial_packet_size;
            auto r = gzip_compress(c->delta_frame->data[0], static_cast<uLong>(in_len), buf + 2,
                &out_len, c->gzip_level);
            pkt->flags &= ~AV_PKT_FLAG_KEY;

            av_shrink_packet(pkt, static_cast<int>(out_len + 2));
        }
    }

    c->currentFrame++;
    *got_packet = 1;
    return 0;
}

int __cdecl cam_codec_encode_end(AVCodecContext *avctx)
{
    CamStudioContext *c = (CamStudioContext *)avctx->priv_data;
    av_freep(&c->comp_buf);
    av_frame_free(&c->previouse_frame);
    av_frame_free(&c->delta_frame);
    return 0;
}
