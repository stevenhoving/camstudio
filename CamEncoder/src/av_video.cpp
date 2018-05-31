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
#include "CamEncoder/av_error.h"
#include "CamEncoder/av_rgb2yuv.h"
#include "CamEncoder/av_cam_codec/av_cam_codec.h"
#include "av_log.h"

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

/*!
 * truncate fps, only used for mpeg4.
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

/*!
 * calculate a approximate gob size.
 *
 * \note Currently generate a gop that is equal to fps. The practical problem I faced it that you
 * record a 5 second video having a gop of 250 is simply not good enough.
 * This used to be gop = ((num/den) + 0.5) * 10.
 */
int calculate_gop_size(const av_video_meta &meta)
{
    double gob_size = ((meta.fps.num / meta.fps.den) + 0.5);
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

    _log("Codec:\n");
    _log("    codec type: {}\n", params->codec_type);
    _log("      codec id: {}\n", params->codec_id);
    _log("        format: {}\n", static_cast<AVPixelFormat>(params->format));
    _log("      bit rate: {}\n", params->bit_rate);
    _log("           bcs: {}\n", params->bits_per_coded_sample);
    _log("           brs: {}\n", params->bits_per_raw_sample);

    _log("       profile: {}\n", params->profile);
    _log("         level: {}\n", params->level);
    _log("  aspect ratio: {}\n", params->sample_aspect_ratio);
    _log("    dimentions: {}x{}\n", params->width, params->height);

    avcodec_parameters_free(&params);
    params = nullptr;
}

AVFrame *create_video_frame(AVPixelFormat pix_fmt, int width, int height)
{
    AVFrame *video_frame = av_frame_alloc();
    if (!video_frame)
        return nullptr;

    video_frame->format = pix_fmt;
    video_frame->width = width;
    video_frame->height = height;

    /* allocate the buffers for the frame data */
    if (int ret = av_frame_get_buffer(video_frame, 32); ret < 0)
    {
        fprintf(stderr, "Could not allocate frame data.\n");
        throw std::runtime_error("blegh!!");
    }

    return video_frame;
}

av_video::av_video(const av_video_codec &config, const av_video_meta &meta)
{
    bool truncate_framerate = false;
    switch (config.id)
    {
    case AV_CODEC_ID_CSCD:
        codec_type_ = av_video_codec_type::cscd;
        input_pixel_format_ = config.pixel_format;
        output_pixel_format_ = AV_PIX_FMT_BGR24;
        codec_ = &cam_codec_encoder;
        break;
    case AV_CODEC_ID_H264:
        codec_type_ = av_video_codec_type::h264;
        input_pixel_format_ = config.pixel_format;
        output_pixel_format_ = AV_PIX_FMT_YUV420P;
        codec_ = avcodec_find_encoder(config.id);
        if (codec_ == nullptr)
            throw std::runtime_error("av_video: unable to find video encoder");
        break;
    default:
        throw std::runtime_error("av_video: unsupported encoder");
        break;
    }

    context_ = avcodec_alloc_context3(codec_);

    auto fps = AVRational{ meta.fps.num, meta.fps.den };

    // Check if the codec has a specific set of supported frame rates. If it has, find the nearest
    // matching framerate.
    if (codec_->supported_framerates)
    {
        const auto idx = av_find_nearest_q_idx(fps, codec_->supported_framerates);
        AVRational supported_fps = codec_->supported_framerates[idx];
        if (supported_fps != fps)
        {
            _log("av_video: framerate {} is not supported. Using {}.", fps, supported_fps);
            fps = supported_fps;
        }
    }

    // this allows for variable framerate with ms timestamps.
    context_->time_base = { 1, 1000 };

    if (codec_type_ != av_video_codec_type::cscd)
    {
        // calculate a approximate gob size.
        context_->gop_size = calculate_gop_size(meta);

        // either quality of bitrate must be set.
        assert(!!meta.quality || !!meta.bitrate);

        apply_preset(av_opts_, meta.preset);
        apply_tune(av_opts_, meta.tune);
        apply_profile(av_opts_, meta.profile);

        /*!
         * set variable framerate.
         * \see https://superuser.com/questions/908295/ffmpeg-libx264-how-to-specify-a-variable-frame-rate-but-with-a-maximum
         */
        //av_opts_["vsync"] = "vfr";

        // Now set the things in context that we don't want to allow
        // the user to override.
        if (meta.bitrate)
        {
            // Average bitrate
            context_->bit_rate = static_cast<int64_t>(1000.0 * meta.bitrate.value());

            // ffmpeg's mpeg2 encoder requires that the bit_rate_tolerance be >= bitrate * fps
            //context_->bit_rate_tolerance = static_cast<int>(context_->bit_rate * av_q2d(fps) + 1);
        }
        else
        {
            /* Constant quantizer */
            context_->flags |= AV_CODEC_FLAG_QSCALE;

            /* global_quality only seem to apply to mpeg 1, 2 and 4 */
            //context_->global_quality = static_cast<int>(FF_QP2LAMBDA * meta.quality.value() + 0.5);

            // x264 requires this.
            av_opts_["crf"] = static_cast<int64_t>(meta.quality.value());
        }
    }
    else
    {
        av_opts_["algorithm"] = 1; // select lzo compressor
        av_opts_["gzip_level"] = 9; // gzip compresion level is not used.
        av_opts_["autokeyframe"] = 1; // enable keyframe insertion every x frames.
        av_opts_["autokeyframe_rate"] = calculate_gop_size(meta) * 10;
    }

    context_->width = meta.width;
    context_->height = meta.height;
    context_->pix_fmt = output_pixel_format_;

    // \todo we have no grayscale settings for now
    //if (grayscale)
    //    context->flags |= AV_CODEC_FLAG_GRAY;

    context_->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;

    frame_ = create_video_frame(context_->pix_fmt, context_->width, context_->height);

    sws_context_ = create_software_scaler(
        input_pixel_format_, context_->width, context_->height,
        output_pixel_format_, context_->width, context_->height
    );
}

av_video::~av_video()
{
    avcodec_free_context(&context_);
    av_frame_free(&frame_);
}

void av_video::open(AVStream *stream, av_dict &dict)
{
    /*!
     * create a copy of our settings dict as avcodec_open2 clears it and fills it with the invalid
     * entries if it encounters them.
     */
    dict = av_opts_;
    auto av_opts = av_opts_;

    if (int ret = avcodec_open2(context_, codec_, av_opts); ret)
        throw std::runtime_error(fmt::format("av_video: unable to open video encoder: {}",
            av_error_to_string(ret)));

    /* avcodec_open populates the opts dictionary with the things it didn't recognize. */
    if (!av_opts.empty())
    {
        AVDictionaryEntry *t = nullptr;
        for (int i = 0; i < av_opts.size(); ++i)
        {
            t = av_opts.at("", t, AV_DICT_IGNORE_SUFFIX);
            _log("av_video: unknown avcodec option: {}", t->key);
        }
    }

    if (stream != nullptr)
    {
        if (int ret = avcodec_parameters_from_context(stream->codecpar, context_); ret)
            throw std::runtime_error(
                fmt::format("av_video: failed to copy avcodec parameters: {}", av_error_to_string(ret)));
    }

    dump_context(context_);
}

void av_video::push_encode_frame(timestamp_t timestamp, BITMAPINFOHEADER *image, unsigned char *data)
{
    // also handle encoder flush
    AVFrame *encode_frame = nullptr;
    if (image != nullptr)
    {
        /* when we pass a frame to the encoder, it may keep a reference to it
         * internally; make sure we do not overwrite it here
         */
        if (av_frame_make_writable(frame_) < 0)
            throw std::runtime_error("Unable to make temp video frame writable");

        const auto *src_data = data;
        const auto src_width = image->biWidth;
        const auto src_height = abs(image->biHeight);

        const auto dst_width = context_->width;
        const auto dst_height = context_->height;
        const auto dst_pixel_format = context_->pix_fmt;

        /* \todo fix hard coded input format */
        const auto src_data_size = src_width * src_height * 4;
        const uint8_t *const src[3] = {
            src_data + (src_data_size - (src_width * 4)),
            nullptr,
            nullptr
        };
        /* \todo fix hard coded input format stride */
        const int src_stride[3] = { src_width * -4, 0, 0 };

        int dst_stride[3] = {0, 0, 0};
        switch(output_pixel_format_)
        {
        case AV_PIX_FMT_RGB555LE:
            dst_stride[0] = src_width * 2;
            dst_stride[1] = 0;
            dst_stride[2] = 0;
            break;
        case AV_PIX_FMT_BGR24:
            dst_stride[0] = src_width * 3;
            dst_stride[1] = 0;
            dst_stride[2] = 0;
            break;
        case AV_PIX_FMT_BGR0:
            dst_stride[0] = src_width * 4;
            dst_stride[1] = 0;
            dst_stride[2] = 0;
            break;
        case AV_PIX_FMT_YUV420P:
            dst_stride[0] = dst_width;
            dst_stride[1] = dst_width / 2;
            dst_stride[2] = dst_width / 2;
            break;
        case AV_PIX_FMT_YUV444P:
            dst_stride[0] = dst_width;
            dst_stride[1] = dst_width;
            dst_stride[2] = dst_width;
            break;
        default:
            throw std::runtime_error("av_video: invalid encoder input format");
            break;
        }
        if (output_pixel_format_ != AV_PIX_FMT_YUV420P)
        {
            int ret = sws_scale(sws_context_, src, src_stride, 0, src_height, frame_->data, dst_stride);
            if (ret < 0)
            {
                fmt::print("scale failed: {}\n", av_error_to_string(ret));
            }
        }
        else if (input_pixel_format_ == AV_PIX_FMT_BGRA)
        {
            bgra2yuv420p(frame_->data, src, src_width, src_height, src_stride);
        }
        else
        {
            bgr2yuv420p(frame_->data, src, src_width, src_height, src_stride);
        }

        frame_->pts = timestamp;
        encode_frame = frame_;
    }
    else
    {
        _log("flush encoder\n");
    }

    if (int ret = avcodec_send_frame(context_, encode_frame); ret < 0)
        throw std::runtime_error(fmt::format("send video frame to encoder failed: {}",
            av_error_to_string(ret)));
}

bool av_video::pull_encoded_packet(AVPacket *pkt, bool *valid_packet)
{
    pkt->data = nullptr;
    pkt->size = 0;

    int ret = avcodec_receive_packet(context_, pkt);

    // only when ret == 0 we have a valid packet.
    *valid_packet = (ret == 0);

    if (ret == 0)
        return true;

    if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
        return true;

    return false;
}

av_video_codec_type av_video::get_codec_type() const noexcept
{
    return codec_type_;
}

AVCodecContext *av_video::get_codec_context() const noexcept
{
    return context_;
}

AVRational av_video::get_time_base() const noexcept
{
    return context_->time_base;
}

SwsContext *av_video::create_software_scaler(AVPixelFormat src_pixel_format, int src_width, int src_height,
                                             AVPixelFormat dst_pixel_format, int dst_width, int dst_height)
{
    SwsContext *software_scaler_context = sws_getContext(
        src_width, src_height, src_pixel_format,
        dst_width, dst_height, dst_pixel_format,
        SWS_BICUBIC, nullptr, nullptr, nullptr);

    if (!software_scaler_context)
        throw std::runtime_error("Could not initialize the conversion context");

    return software_scaler_context;
}
