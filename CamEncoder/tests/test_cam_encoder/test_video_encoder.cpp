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
#include <CamEncoder/av_video.h>
#include "test_utilities.h"

TEST(test_video_encoder, test_create_h264_encoder)
{
    av_video_codec video_codec_config;
    video_codec_config.id = AV_CODEC_ID_H264;

    av_video_meta meta;
    meta.bitrate = 4000;
    meta.bpp = 24;
    meta.height = 512;
    meta.width = 512;
    meta.fps = { 25, 1 };
    meta.preset = video::preset::ultrafast; // configure 'almost' realtime video encoding
    meta.profile = video::profile::baseline; // lets try 264 baseline
    meta.tune = video::tune::zerolatency;

    av_dict avargs;
    av_video test(video_codec_config, meta);
    test.open(nullptr, avargs);

    auto frame = create_bmpinfo(512, 512);
    test.push_encode_frame(0, (BITMAPINFOHEADER *)frame);
    free(frame);
}
