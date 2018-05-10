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

av_muxer::av_muxer(const char *filename, muxer_type muxer, bool mp4_optimize)
    : muxer_(muxer)
{
    context_ = avformat_alloc_context();
    if (context_ == nullptr)
        throw std::runtime_error("unable to alloc avformat context");

    AVDictionary * av_opts = NULL;
    switch (muxer_)
    {
    case muxer_type::mp4:
        time_base_.num = 1;
        time_base_.den = 90000;
        muxer_name_ = "mp4";

        av_dict_set(&av_opts, "brand", "mp42", 0);
        if (mp4_optimize)
            av_dict_set(&av_opts, "movflags", "faststart+disable_chpl", 0);
        else
            av_dict_set(&av_opts, "movflags", "+disable_chpl", 0);
        break;

    case muxer_type::mkv:
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
}

av_muxer::~av_muxer()
{
    avformat_free_context(context_);
}
