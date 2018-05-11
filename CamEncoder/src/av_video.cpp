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

#include "CamEncoder/av_video.h"
#include "CamEncoder/av_dict.h"
#include <fmt/printf.h>
#include <fmt/ostream.h>
#include <cassert>

std::ostream &operator<<(std::ostream &os, const AVMediaType d)
{
    return os << av_get_media_type_string(d);
}

std::ostream &operator<<(std::ostream &os, const AVCodecID d)
{
    return os << avcodec_get_name(d);
}

std::ostream &operator<<(std::ostream &os, const AVRational d)
{
    return os << d.den << '/' << d.num;
}

std::ostream &operator<<(std::ostream &os, const AVPixelFormat d)
{
    return os << av_get_pix_fmt_name(d);
}

std::string av_error_to_string(int errnum)
{
    char x[1024] = { 0 };
    int ret = av_strerror(errnum, x, 1023);
    if (ret < 0)
        throw std::runtime_error("av_strerror failed");
    return x;
}



/*!
 * truncate fps, only used for mpeg4
 */
AVRational truncate_fps(AVRational fps)
{
    while ((fps.num & ~0xFFFF) || (fps.den & ~0xFFFF))
    {
        fps.num >>= 1;
        fps.den >>= 1;
    }
    return fps;
}

// \note hack, helper function sets both framerate and time_base.
//       resulting in the video encoder to be set to Constant Frame Rate.
void set_fps(AVCodecContext *context, const AVRational &fps)
{
    context->time_base = { fps.den, fps.num };
    context->framerate = { fps.num, fps.den };
}

/*!
 * calculate a approximate gob size.
 */
int calculate_gop_size(const av_video_meta &meta)
{
    double gob_size = ((meta.fps.num / meta.fps.den) + 0.5) * 10.0;
    return static_cast<int>(gob_size);
}

void apply_preset(av_dict &av_opts, std::optional<video::preset> preset)
{
    const auto preset_idx = static_cast<int>(preset.value_or(video::preset::medium));
    const auto preset_name = video::preset_names.at(preset_idx);
    av_opts["preset"] = preset_name;
}

void apply_tune(av_dict &av_opts, std::optional<video::tune> preset)
{
    if (!preset)
        return;

    const auto tune_idx = static_cast<int>(preset.value());
    const auto tune_name = video::tune_names.at(tune_idx);
    av_opts["tune"] = tune_name;
}

void apply_profile(av_dict &av_opts, std::optional<video::profile> profile)
{
    if (!profile)
        return;

    const auto profile_idx = static_cast<int>(profile.value());
    const auto profile_name = video::profile_names.at(profile_idx);
    av_opts["profile"] = profile_name;
}

void dump_context(AVCodecContext *context)
{
    AVCodecParameters * params = avcodec_parameters_alloc();
    avcodec_parameters_from_context(params, context);

    fmt::print("Codec:\n");
    fmt::print("    codec type: {}\n", params->codec_type);
    fmt::print("      codec id: {}\n", params->codec_id);
    fmt::print("        format: {}\n", static_cast<AVPixelFormat>(params->format));
    fmt::print("      bit rate: {}\n", params->bit_rate);
    //fmt::print("           bpp: {}\n", params->bits_per_coded_sample);
    //fmt::print("           bpp: {}\n", params->bits_per_raw_sample);

    fmt::print("       profile: {}\n", params->profile);
    fmt::print("         level: {}\n", params->level);
    //fmt::print("  aspect ratio: {}:{}\n", params->sample_aspect_ratio.den, params->sample_aspect_ratio.num);
    fmt::print("  aspect ratio: {}\n", params->sample_aspect_ratio);
    fmt::print("    dimentions: {}x{}\n", params->width, params->height);

    avcodec_parameters_free(&params);
    params = nullptr;
}

av_video::av_video(const av_video_codec &config, const av_video_meta &meta)
{
    bool truncate_framerate = false;
    switch (config.id)
    {
    // cam studio video codec is currently disabled because it only supports decoding.
#if 0
    case AV_CODEC_ID_CSCD:
        printf("av_video: CamStudio encoder\n");
        break;
#endif
    case AV_CODEC_ID_H264:
        codec_type_ = av_video_codec_type::h264;
        printf("av_video: H264 encoder\n");
        break;
    default:
        throw std::runtime_error("av_video: unsupported encoder");
        break;
    }

    AVCodec *codec = avcodec_find_encoder(config.id);
    if (codec == nullptr)
        throw std::runtime_error("av_video: unable to find video encoder");

    context_ = avcodec_alloc_context3(codec);

    auto fps = AVRational{ meta.fps.den, meta.fps.num };

    // Check that the framerate is supported.  If not, pick the closest.
    // The mpeg2 codec only supports a specific list of frame rates.
    if (codec->supported_framerates)
    {
        const auto idx = av_find_nearest_q_idx(fps, codec->supported_framerates);
        AVRational supported_fps = codec->supported_framerates[idx];
        if (supported_fps.num != fps.num || supported_fps.den != fps.den)
        {
            printf("av_video: framerate %d / %d is not supported. Using %d / %d.",
                fps.num, fps.den, supported_fps.num, supported_fps.den);
            fps = supported_fps;
        }
    }

    set_fps(context_, fps);

    // calculate a approximate gob size.
    context_->gop_size = calculate_gop_size(meta);

    // either quality of bitrate must be set.
    assert(!!meta.quality || !!meta.bitrate);

    if (meta.quality)
    {
        if (config.id == AV_CODEC_ID_VP8 ||
            config.id == AV_CODEC_ID_VP9)
        {
            // This value was chosen to make the bitrate high enough
            // for libvpx to "turn off" the maximum bitrate feature
            // that is normally applied to constant quality.
            // \todo read http://wiki.webmproject.org/ffmpeg/vp9-encoding-guide and adapt.
            context_->bit_rate = meta.width * meta.height * fps.num / fps.den;
        }
    }

    // Note: The output will be delayed by max_b_frames + 1 relative to the input.
    //       Is this related to mpeg2?
    //context_->max_b_frames = 1;

    av_dict av_opts;
    apply_preset(av_opts, meta.preset);
    apply_tune(av_opts, meta.tune);
    apply_profile(av_opts, meta.profile);

    /* iterate through lavc_opts and have avutil parse the options for us */
    //hb_dict_iter_t iter;
    //for (iter = hb_dict_iter_init(lavc_opts);
    //    iter != HB_DICT_ITER_DONE;
    //    iter = hb_dict_iter_next(lavc_opts, iter))
    //{
    //    const char *key = hb_dict_iter_key(iter);
    //    hb_value_t *value = hb_dict_iter_value(iter);
    //    char *str = hb_value_get_string_xform(value);
    //
    //    /* Here's where the strings are passed to avutil for parsing. */
    //    av_dict_set(&av_opts, key, str, 0);
    //    free(str);
    //}
    //hb_dict_free(&lavc_opts);

    // Now set the things in context that we don't want to allow
    // the user to override.
    if (meta.bitrate)
    {
        /* Average bitrate */
        context_->bit_rate = static_cast<int64_t>(1000.0 * meta.bitrate.value());
        // ffmpeg's mpeg2 encoder requires that the bit_rate_tolerance be >= bitrate * fps
        context_->bit_rate_tolerance = static_cast<int>(context_->bit_rate * av_q2d(fps) + 1);
    }
    else
    {
        /* Constant quantizer */
        // These settings produce better image quality than
        // what was previously used
        context_->flags |= AV_CODEC_FLAG_QSCALE;
        context_->global_quality = static_cast<int>(FF_QP2LAMBDA * meta.quality.value() + 0.5);
        //Set constant quality for libvpx
        if (config.id == AV_CODEC_ID_VP8 ||
            config.id == AV_CODEC_ID_VP9)
        {
            char quality[7];
            snprintf(quality, 7, "%.2f", meta.quality.value());
            av_opts["crf"] = quality;
            //This value was chosen to make the bitrate high enough
            //for libvpx to "turn off" the maximum bitrate feature
            //that is normally applied to constant quality.
            context_->bit_rate = meta.width * meta.height * fps.num / fps.den;
            printf("encavcodec: encoding at CQ %.2f", meta.quality.value());
        }
        else
        {
            printf("encavcodec: encoding at constant quantizer %d",
                context_->global_quality);
        }
    }
    context_->width = meta.width;
    context_->height = meta.height;
    context_->pix_fmt = AV_PIX_FMT_YUV420P;

    //context_->sample_aspect_ratio.num = job->par.num;
    //context_->sample_aspect_ratio.den = job->par.den;
    //if (config.id == HB_VCODEC_FFMPEG_MPEG4)
    //{
    //    // MPEG-4 Part 2 stores the PAR num/den as unsigned 8-bit fields,
    //    // and libavcodec's encoder fails to initialize if we don't
    //    // reduce it to fit 8-bits.
    //    hb_limit_rational(&context->sample_aspect_ratio.num,
    //        &context->sample_aspect_ratio.den,
    //        context->sample_aspect_ratio.num,
    //        context->sample_aspect_ratio.den, 255);
    //}

    //hb_log("encavcodec: encoding with stored aspect %d/%d", job->par.num, job->par.den);

    //if (job->mux & HB_MUX_MASK_MP4)
    //{
    //    context->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
    //}
    //

    // \todo we have no grayscale settings for now
    //if (job->grayscale)
    //{
    //    context->flags |= AV_CODEC_FLAG_GRAY;
    //}

    // we do not support two pass video encoding.

    //if (hb_avcodec_open(context, codec, &av_opts, HB_FFMPEG_THREADS_AUTO))
    //{
    //    hb_log("encavcodecInit: avcodec_open failed");
    //}

    int ret = avcodec_open2(context_, codec, av_opts);
    if (ret)
    {
        printf("unable to open video encoder\n");

        // \todo free mem
        return;
    }

    // avcodec_open populates the opts dictionary with the
    // things it didn't recognize.
    AVDictionaryEntry *t = nullptr;
    while ((t = av_opts.at("", t, AV_DICT_IGNORE_SUFFIX)))
    {
        printf("encavcodecInit: Unknown avcodec option %s", t->key);
    }

    dump_context(context_);

    frame_ = create_video_frame(context_->pix_fmt, context_->width, context_->height);
    sws_context_ = create_software_scaler(AV_PIX_FMT_BGR24, context_->width, context_->height);

    //job->areBframes = 0;
    //if (context->has_b_frames)
    //{
    //    job->areBframes = 1;
    //}

    //if ((job->mux & HB_MUX_MASK_MP4) && job->pass_id != HB_PASS_ENCODE_1ST)
    //{
    //    w->config->mpeg4.length = context->extradata_size;
    //    memcpy(w->config->mpeg4.bytes, context->extradata, context->extradata_size);
    //}
}

av_video::~av_video()
{
    avcodec_free_context(&context_);
    av_frame_free(&frame_);
}

void av_video::push_encode_frame(timestamp_t timestamp, BITMAPINFO *image)
{
    /* when we pass a frame to the encoder, it may keep a reference to it
    * internally; make sure we do not overwrite it here
    */
    if (av_frame_make_writable(frame_) < 0)
        throw std::runtime_error("Unable to make temp video frame writable");

    const auto &header = image->bmiHeader;

    //image->bmiColors
    const auto *src_data = ((LPBYTE)image) + header.biSize + (header.biClrUsed * sizeof(RGBQUAD));
    const auto src_data_size = header.biSizeImage;
    const auto src_width = header.biWidth;
    const auto src_height = header.biHeight;
    const auto src_pixel_format = AV_PIX_FMT_BGR24;

    const auto dst_width = context_->width;
    const auto dst_height = context_->height;
    const auto dst_pixel_format = context_->pix_fmt;

    // convert from rgb to yuv420p
    const uint8_t * const src[3] = {
        src_data + (src_data_size - (src_width * 3)),
        nullptr,
        nullptr
    };

    const int src_stride[3] = { src_width * -3, 0, 0 };
    const int dst_stride[3] = { dst_width, dst_width / 2, dst_width / 2 }; // for YUV420
    //const int dst_stride[3] = { dst_width, dst_width, dst_width }; // for YUV444
    sws_scale(sws_context_, src,
        src_stride, 0,
        src_height, frame_->data,
        dst_stride);

    frame_->pts = timestamp;

    write_video_frame(frame_);
}

bool av_video::pull_encoded_packet(AVPacket *pkt, bool *valid_packet)
{
    int ret = avcodec_receive_packet(context_, pkt);

    // only when ret == 0 we have a valid packet.
    *valid_packet = (ret == 0);

    if (ret == 0)
        return true;

    // do not treat eagain as error.
    if (ret == AVERROR(EAGAIN))
        return true;

    throw std::runtime_error("avcodec_receive_packet failed");
    return false;
}

av_video_codec_type av_video::get_codec_type() const noexcept
{
    return codec_type_;
}

int av_video::write_video_frame(AVFrame *frame)
{
    //AVPacket pkt;
    //av_init_packet(&pkt);
    //pkt.data = nullptr;
    //pkt.size = 0;

    int ret = avcodec_send_frame(context_, frame);
    if (ret < 0)
    {
        //fmt::fprintf(stderr, "Error code: %s\n", avx_err2str(ret));
        const auto err = av_error_to_string(ret);
        const auto msg = fmt::format("send video frame to encoder failed: {}", err);
        throw std::runtime_error(msg);
    }

#if 0
    // this needs to be handled differently, as we can not be the sender and receiving side of the
    // video encoder.
    while (1)
    {
        ret = avcodec_receive_packet(context_, &pkt);
        if (ret)
            break;

        //ret = write_frame(oc, &c->time_base, ost->st, &pkt);
        printf("received encoded video frame\n");
        av_packet_unref(&pkt);
    }
#endif

    return ((ret == AVERROR(EAGAIN)) ? 0 : -1);
}

AVFrame *av_video::create_video_frame(enum AVPixelFormat pix_fmt, int width, int height)
{
    int ret;

    AVFrame *video_frame = av_frame_alloc();
    if (!video_frame)
        return nullptr;

    video_frame->format = pix_fmt;
    video_frame->width = width;
    video_frame->height = height;

    /* allocate the buffers for the frame data */
    ret = av_frame_get_buffer(video_frame, 32);
    if (ret < 0)
    {
        fprintf(stderr, "Could not allocate frame data.\n");
        throw std::runtime_error("blegh!!");
    }

    return video_frame;
}

// is this wrong?
#define SCALE_FLAGS SWS_BICUBIC

// \todo handle output pixel format, for when we want to encode yuv444 video.
SwsContext *av_video::create_software_scaler(enum AVPixelFormat src_pixel_format, int width,
    int height)
{
    SwsContext *software_scaler_context = sws_getContext(width, height,
        src_pixel_format,
        width, height,
        AV_PIX_FMT_YUV420P,
        //AV_PIX_FMT_YUV444P,
        SCALE_FLAGS, NULL, NULL, NULL);
    if (!software_scaler_context)
    {
        throw std::runtime_error("Could not initialize the conversion context");
    }

    return software_scaler_context;
}
