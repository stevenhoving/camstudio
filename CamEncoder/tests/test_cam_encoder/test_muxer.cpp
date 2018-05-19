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

#include <gtest/gtest.h>
#include <CamEncoder/av_muxer.h>
#include "test_utilities.h"
#include <thread>
#include <chrono>
#include <memory>

#if 0
av_video create_video(const av_video_meta &meta)
{
    av_video_codec video_codec_config;
    video_codec_config.id = AV_CODEC_ID_H264;

    return av_video(video_codec_config, meta);
}

TEST(test_muxer, test_create_muxer)
{
    av_video_meta meta;
    meta.bitrate = 4000;
    meta.bpp = 24;
    meta.height = 512;
    meta.width = 512;
    meta.fps = {25, 1};
    meta.preset = video::preset::ultrafast;  // configure 'almost' realtime video encoding
    meta.profile = video::profile::baseline; // lets try 264 baseline
    meta.tune = video::tune::zerolatency;
    auto video = create_video(meta);
    av_muxer muxer("test.mkv", av_muxer_type::mkv, meta, video);
}

#endif

av_video_meta create_video_config()
{
    av_video_meta meta;
    meta.bitrate = 4000;
    meta.bpp = 24;
    meta.height = 512;
    meta.width = 512;
    meta.fps = {25, 1};
    meta.preset = video::preset::ultrafast;  // configure 'almost' realtime video encoding
    meta.profile = video::profile::baseline; // lets try 264 baseline
    meta.tune = video::tune::zerolatency;
    return meta;
}

std::unique_ptr<av_video> create_video_codec(const av_video_meta &meta)
{
    av_video_codec video_codec_config;
    video_codec_config.id = AV_CODEC_ID_H264;

    return std::make_unique<av_video>(video_codec_config, meta);

    //return av_video(video_codec_config, meta);
}

TEST(test_muxer, test_create_mkv_muxer)
{
    auto config = create_video_config();

    av_muxer muxer("test.mkv", av_muxer_type::mkv);
    muxer.add_stream(
        create_video_codec(config)
    );
    muxer.open();

    auto frame = create_bmpinfo(512, 512);
    for (int i = 0; i < 100; ++i)
        muxer.encode_frame(i, frame);
}

TEST(test_muxer, test_create_mp4_muxer)
{
    auto config = create_video_config();

    av_muxer muxer("test.mp4", av_muxer_type::mp4);
    muxer.add_stream(
        create_video_codec(config)
    );
    muxer.open();

    auto frame = create_bmpinfo(512, 512);
    for (int i = 0; i < 100; ++i)
        muxer.encode_frame(i, frame);
}
