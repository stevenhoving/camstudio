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

#include "CamEncoder/av_muxer.h"
#include "CamEncoder/av_dict.h"

av_muxer::av_muxer(const char *filename, av_muxer_type muxer, bool mp4_optimize,
    const av_video_meta &meta, av_video &vcodec)
    : muxer_(muxer)
{
    context_ = avformat_alloc_context();
    if (context_ == nullptr)
        throw std::runtime_error("alloc avformat context failed");

    av_dict av_opts;
    switch (muxer_)
    {
    case av_muxer_type::mp4:
        time_base_.num = 1;
        time_base_.den = 90000;
        muxer_name_ = "mp4";

        av_opts["brand"] = "mp42";
        if (mp4_optimize)
            av_opts["movflags"] = "faststart+disable_chpl";
        else
            av_opts["movflags"] = "+disable_chpl";
        break;

    case av_muxer_type::mkv:
        // libavformat is essentially hard coded such that it only
        // works with a timebase of 1/1000
        time_base_.num = 1;
        time_base_.den = 1000;
        muxer_name_ = "matroska";
        break;

    default:
        throw std::runtime_error("invalid muxer");
    }

    output_format_ = av_guess_format(muxer_name_.c_str(), nullptr, nullptr);
    if (output_format_ == nullptr)
        throw std::runtime_error(fmt::format("Could not guess output format {}", muxer_name_));
    
    int ret = avio_open2(&context_->pb, filename, AVIO_FLAG_WRITE, &context_->interrupt_callback,
        nullptr);
    if (ret < 0)
        throw std::runtime_error(fmt::format("avio_open2 failed, err: {}", ret));

    av_track video_track;
    video_track.type = av_track_type::video;
    video_track.stream = avformat_new_stream(context_, nullptr);
    if (video_track.stream == nullptr)
        throw std::runtime_error("Unable to create video stream");

    video_track.stream->codecpar->codec_type = AVMEDIA_TYPE_VIDEO;
    video_track.stream->time_base = time_base_;

    uint8_t *priv_data = NULL;
    int priv_size = 0;

    switch (vcodec.get_codec_type())
    {
    case av_video_codec_type::x264:
        video_track.stream->codecpar->codec_id = AV_CODEC_ID_H264;

        // adaptive streaming (SPS and PPS before IDR frames)
        //if (muxer_ == muxer_type::mp4 && job->inline_parameter_sets)
        //{
            //video_track.stream->codecpar->codec_tag = MKTAG('a', 'v', 'c', '3');
        //}
        //else
        //{
        video_track.stream->codecpar->codec_tag = MKTAG('a', 'v', 'c', '1');
        //}
#if 0
        /* Taken from x264 muxers.c */
        priv_size = 5 + 1 + 2 + job->config.h264.sps_length + 1 + 2 +
            job->config.h264.pps_length;

        priv_data = (uint8_t *)av_malloc(priv_size + AV_INPUT_BUFFER_PADDING_SIZE);
        if (priv_data == NULL)
        {
            //hb_error("H.264 extradata: malloc failure");
            throw std::runtime_error("H.264 extradata: malloc failure");
        }

        priv_data[0] = 1;
        priv_data[1] = job->config.h264.sps[1]; /* AVCProfileIndication */
        priv_data[2] = job->config.h264.sps[2]; /* profile_compat */
        priv_data[3] = job->config.h264.sps[3]; /* AVCLevelIndication */
        priv_data[4] = 0xff; // nalu size length is four bytes
        priv_data[5] = 0xe1; // one sps

        priv_data[6] = job->config.h264.sps_length >> 8;
        priv_data[7] = job->config.h264.sps_length;

        memcpy(priv_data + 8, job->config.h264.sps,
            job->config.h264.sps_length);

        priv_data[8 + job->config.h264.sps_length] = 1; // one pps
        priv_data[9 + job->config.h264.sps_length] =
            job->config.h264.pps_length >> 8;
        priv_data[10 + job->config.h264.sps_length] =
            job->config.h264.pps_length;

        memcpy(priv_data + 11 + job->config.h264.sps_length,
            job->config.h264.pps, job->config.h264.pps_length);
#endif
        break;
    }

    // we currently do not support codec extra data for now
    //video_track->st->codecpar->extradata = priv_data;
    //video_track->st->codecpar->extradata_size = priv_size;

    //video_track.stream->sample_aspect_ratio.num = job->par.num;
    //video_track.stream->sample_aspect_ratio.den = job->par.den;
    //video_track.stream->codecpar->sample_aspect_ratio.num = job->par.num;
    //video_track.stream->codecpar->sample_aspect_ratio.den = job->par.den;
    //video_track.stream->codecpar->width = job->width;
    //video_track.stream->codecpar->height = job->height;
    video_track.stream->codecpar->width = meta.width;
    video_track.stream->codecpar->height = meta.height;
    video_track.stream->disposition |= AV_DISPOSITION_DEFAULT;

    tracks_.emplace_back(video_track);

    // \todo add audio track

    // \todo add CamEncoding/CamStudio version information
    const char *encoding_tool = "CamStudio";

    av_dict_set(&context_->metadata, "encoding_tool", encoding_tool, 0);
    time_t now = time(NULL);
    struct tm * now_utc = gmtime(&now);
    char now_8601[24];
    strftime(now_8601, sizeof(now_8601), "%Y-%m-%dT%H:%M:%SZ", now_utc);
    av_dict_set(&context_->metadata, "creation_time", now_8601, 0);


    ret = avformat_write_header(context_, av_opts);
    if (ret < 0)
        throw std::runtime_error("av_muxer: avformat_write_header failed");

    AVDictionaryEntry *t = nullptr;
    while ((t = av_opts.at("", t, AV_DICT_IGNORE_SUFFIX)))
        fmt::print("av_muxer: unknown option {}\n", t->key);
}

av_muxer::~av_muxer()
{
    avformat_free_context(context_);
}
