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

#include "CamEncoder/av_muxer.h"
#include "CamEncoder/av_dict.h"
#include "CamEncoder/av_error.h"
#include "av_log.h"

#include <fmt/format.h>

void av_log_packet(const AVFormatContext *fmt_ctx, const AVPacket *pkt)
{
    AVRational &time_base = fmt_ctx->streams[pkt->stream_index]->time_base;

    _log("pts:{} pts_time:{} dts:{} dts_time:{} duration:{} duration_time:{} stream_index:{}\n",
               av_timestamp_to_string(pkt->pts), av_timestamp_to_timestring(pkt->pts, time_base),
               av_timestamp_to_string(pkt->dts), av_timestamp_to_timestring(pkt->dts, time_base),
               av_timestamp_to_string(pkt->duration), av_timestamp_to_timestring(pkt->duration, time_base),
               pkt->stream_index);
}

av_muxer::av_muxer(const char *filename, const av_muxer_type muxer_type)
    : filename_(filename)
{
    const auto muxer_type_name = av_muxer_type_names.at(static_cast<int>(muxer_type));

    output_format_ = av_guess_format(muxer_type_name, nullptr, nullptr);
    if (output_format_ == nullptr)
        throw std::runtime_error(fmt::format("unable to guess output format: '{}'",
            muxer_type_name));

    /* allocate the output media context */
    if (const auto ret = avformat_alloc_output_context2(&format_context_, output_format_, nullptr, nullptr);  ret < 0)
        throw std::runtime_error(
            fmt::format("av_muxer: unable to create avformat output context: {}",
                av_error_to_string(ret)));

    switch(muxer_type)
    {
    case av_muxer_type::none:
        throw std::runtime_error(
            fmt::format("av_muxer: unable to create avformat output context: invalid muxer type")
        );
        break;
    case av_muxer_type::mp4:
        time_base_.num = 1;
        time_base_.den = 90000;
        break;
    case av_muxer_type::avi:
        [[fallthrough]];
    case av_muxer_type::mkv:
        time_base_.num = 1;
        time_base_.den = 1000;
        break;
    }
}

av_muxer::~av_muxer()
{
    flush();

    /* Write the trailer, if any. The trailer must be written before you
     * close the CodecContexts open when you wrote the header; otherwise
     * av_write_trailer() may try to use memory that was freed on
     * av_codec_close(). */
    av_write_trailer(format_context_);

    /* Close each codec. */
    video_codec_.reset();

    //if (have_audio)
    //    close_stream(format_context_, &audio_track);

    if (!(output_format_->flags & AVFMT_NOFILE))
        /* Close the output file. */
        avio_closep(&format_context_->pb);

    /* free the stream */
    avformat_free_context(format_context_);
}

void av_muxer::open()
{
    av_dict avargs;
    video_codec_->open(video_track.stream, avargs);

    // if (output_format_->audio_codec != AV_CODEC_ID_NONE)
    //{
    //    add_stream(&audio_st, format_context_, &audio_codec, output_format_->audio_codec);
    //    have_audio = 1;
    //    encode_audio = 1;
    //}


    /* Now that all the parameters are set, we can open the audio and
    * video codecs and allocate the necessary encode buffers. */

    //if (have_audio)
    //    open_audio(format_context_, audio_codec, &audio_st, opt);

    av_dump_format(format_context_, 0, filename_.c_str(), 1);

    /* open the output file, if needed */
    if (!(output_format_->flags & AVFMT_NOFILE))
    {
        if (int ret = avio_open(&format_context_->pb, filename_.c_str(), AVIO_FLAG_WRITE); ret < 0)
            throw std::runtime_error(fmt::format("Could not open '{}': {}", filename_,
                av_error_to_string(ret)));
    }

    /* Write the stream header, if any. */
    if (int ret = avformat_write_header(format_context_, avargs); ret < 0)
        throw std::runtime_error(fmt::format("Error occurred when opening output file: {}",
            av_error_to_string(ret)));
}

void av_muxer::flush()
{
    encode_frame(0, nullptr, 0, 0, 0);
}

void av_muxer::encode_frame(timestamp_t timestamp, unsigned char *data, int width, int height, int stride)
{
    video_codec_->push_encode_frame(timestamp, data, width, height, stride);

    AVPacket pkt = {};
    av_init_packet(&pkt);

    for(bool valid_packet = true; valid_packet;)
    {
        if (!video_codec_->pull_encoded_packet(&pkt, &valid_packet))
            throw std::runtime_error("pull encoded packet failed");

        if (!valid_packet)
            break;

        const auto time_base = video_codec_->get_time_base();
        write_frame(time_base, video_track.stream, &pkt);
        av_packet_unref(&pkt);
    }
}

void av_muxer::add_stream(std::unique_ptr<av_video> video_codec)
{
    video_codec_ = std::move(video_codec);
    auto codec_context = video_codec_->get_codec_context();

    av_track track;
    track.stream = avformat_new_stream(format_context_, codec_context->codec);
    if (!track.stream)
        throw std::runtime_error("Could not allocate stream");

    track.stream->id = format_context_->nb_streams - 1;
    track.stream->time_base = time_base_;

    /* Some formats want stream headers to be separate. */
    if (format_context_->oformat->flags & AVFMT_GLOBALHEADER)
        codec_context->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;

    video_track = track;
}

AVFrame *av_muxer::alloc_audio_frame(enum AVSampleFormat sample_fmt, uint64_t channel_layout, int sample_rate,
                                     int nb_samples)
{
    AVFrame *frame = av_frame_alloc();
    if (frame == nullptr)
    {
        _log("Error allocating an audio frame\n");
        exit(1);
    }

    frame->format = sample_fmt;
    frame->channel_layout = channel_layout;
    frame->sample_rate = sample_rate;
    frame->nb_samples = nb_samples;

    if (nb_samples)
    {
        if (int ret = av_frame_get_buffer(frame, 0); ret < 0)
        {
            _log("Error allocating an audio buffer\n");
            exit(1);
        }
    }

    return frame;
}

void av_muxer::open_audio(AVFormatContext *format_context, AVCodec *codec, av_track *track, AVDictionary *opt_arg)
{
    AVCodecContext *c;
    int nb_samples;
    int ret;
    AVDictionary *opt = nullptr;

    c = track->codec_context;

    /* open it */
    av_dict_copy(&opt, opt_arg, 0);
    ret = avcodec_open2(c, codec, &opt);
    av_dict_free(&opt);
    if (ret < 0)
    {
        _log("Could not open audio codec: {}\n", av_error_to_string(ret));
        exit(1);
    }

    /* init signal generator */
    // track->t = 0;
    // track->tincr = 2 * M_PI * 110.0 / c->sample_rate;
    /* increment frequency by 110 Hz per second */
    // track->tincr2 = 2 * M_PI * 110.0 / c->sample_rate / c->sample_rate;

    if (c->codec->capabilities & AV_CODEC_CAP_VARIABLE_FRAME_SIZE)
        nb_samples = 10000;
    else
        nb_samples = c->frame_size;

    // track->frame = alloc_audio_frame(c->sample_fmt, c->channel_layout, c->sample_rate, nb_samples);
    // track->tmp_frame = alloc_audio_frame(AV_SAMPLE_FMT_S16, c->channel_layout, c->sample_rate, nb_samples);

    /* copy the stream parameters to the muxer */
    ret = avcodec_parameters_from_context(track->stream->codecpar, c);
    if (ret < 0)
    {
        _log("Could not copy the stream parameters\n");
        exit(1);
    }

    /* create resampler context */
    // track->swr_ctx = swr_alloc();
    // if (!track->swr_ctx) {
    //    _log("Could not allocate resampler context\n");
    //    exit(1);
    //}

    /* set options */
    // av_opt_set_int(track->swr_ctx, "in_channel_count", c->channels, 0);
    // av_opt_set_int(track->swr_ctx, "in_sample_rate", c->sample_rate, 0);
    // av_opt_set_sample_fmt(track->swr_ctx, "in_sample_fmt", AV_SAMPLE_FMT_S16, 0);
    // av_opt_set_int(track->swr_ctx, "out_channel_count", c->channels, 0);
    // av_opt_set_int(track->swr_ctx, "out_sample_rate", c->sample_rate, 0);
    // av_opt_set_sample_fmt(track->swr_ctx, "out_sample_fmt", c->sample_fmt, 0);

    /* initialize the resampling context */
    // if ((ret = swr_init(track->swr_ctx)) < 0) {
    //    _log("Failed to initialize the resampling context\n");
    //    exit(1);
    //}
}

int av_muxer::write_audio_frame(av_track *track, AVFrame *frame)
{
    AVCodecContext *c = nullptr;
    AVPacket pkt = {}; // data and size must be 0;

    int ret = 0;
    int got_packet = 0;
    int dst_nb_samples = 0;

    av_init_packet(&pkt);
    c = track->codec_context;

    if (frame)
    {
        /* convert samples from native format to destination codec format, using the resampler */
        /* compute destination number of samples */
        // dst_nb_samples = av_rescale_rnd(swr_get_delay(track->swr_ctx, c->sample_rate) + frame->nb_samples,
        // c->sample_rate, c->sample_rate, AV_ROUND_UP);
        av_assert0(dst_nb_samples == frame->nb_samples);

        /* when we pass a frame to the encoder, it may keep a reference to it
         * internally;
         * make sure we do not overwrite it here
         */
        // ret = av_frame_make_writable(track->frame);
        if (ret < 0)
            exit(1);

        /* convert to destination format */
        // ret = swr_convert(track->swr_ctx,
        //    track->frame->data, dst_nb_samples,
        //    (const uint8_t **)frame->data, frame->nb_samples);
        // if (ret < 0) {
        //    _log("Error while converting\n");
        //    exit(1);
        //}
        // frame = track->frame;
        //
        // frame->pts = av_rescale_q(track->samples_count, { 1, c->sample_rate }, c->time_base);
        // track->samples_count += dst_nb_samples;
    }

    ret = avcodec_encode_audio2(c, &pkt, frame, &got_packet);
    if (ret < 0)
    {
        _log("Error encoding audio frame: {}\n", av_error_to_string(ret));
        exit(1);
    }

    if (got_packet)
    {
        ret = write_frame(c->time_base, track->stream, &pkt);
        if (ret < 0)
        {
            _log("Error while writing audio frame: {}\n", av_error_to_string(ret));
            exit(1);
        }
    }

    return (frame || got_packet) ? 0 : 1;
}

void av_muxer::close_stream(AVFormatContext *format_context, av_track *ost)
{
    avcodec_free_context(&ost->codec_context);
}

int av_muxer::write_frame(const AVRational &time_base, AVStream *stream, AVPacket *pkt)
{
    /* rescale output packet timestamp values from codec to stream timebase */
    av_packet_rescale_ts(pkt, time_base, stream->time_base);
    pkt->stream_index = stream->index;

    /* Write the compressed frame to the media file. */
    //av_log_packet(format_context_, pkt);
    return av_interleaved_write_frame(format_context_, pkt);
}
