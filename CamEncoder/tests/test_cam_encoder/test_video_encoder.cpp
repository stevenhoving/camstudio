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

#if 0
#pragma pack(1)
struct rgb
{
    uint8_t r, g, b;
};

// \note this function does not create a correct bitmapinfo object.
BITMAPINFO *create_bmpinfo(int width, int height)
{
    // create a bmp
    const auto image_size = width * height * sizeof(rgb);
    const auto frame_size = sizeof(BITMAPINFOHEADER) + image_size;

    BITMAPINFO *frame = (BITMAPINFO *)malloc(frame_size + 1);
    memset(frame, 0, frame_size);
    frame->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    frame->bmiHeader.biClrUsed = 0;
    frame->bmiHeader.biWidth = width;
    frame->bmiHeader.biHeight = height;
    frame->bmiHeader.biSizeImage = image_size;

    auto *frame_data = ((uint8_t *)frame) + frame->bmiHeader.biSize + (frame->bmiHeader.biClrUsed * sizeof(RGBQUAD));
    rgb *data = reinterpret_cast<rgb *>(frame_data);
    uint8_t value = 0;
    for (int i = 0; i < width * height; ++i)
    {
        auto &pxl = data[i];
        pxl.r = pxl.g = pxl.b = value++;
    }
    return frame;
}

TEST(test_video_encoder, test_create_h264_encoder)
{
    video_codec video_codec_config;
    video_codec_config.id = AV_CODEC_ID_H264;

    video_meta meta;
    meta.bitrate = 4000;
    meta.bpp = 24;
    meta.height = 512;
    meta.width = 512;
    meta.fps = { 25, 1 };
    meta.preset = video::preset::ultrafast; // configure 'almost' realtime video encoding
    meta.profile = video::profile::baseline; // lets try 264 baseline
    meta.tune = video::tune::zerolatency;

    av_video test(video_codec_config, meta);

    auto frame = create_bmpinfo(512, 512);

    //for (int i = 0; i < 25; ++i)
    for (int i = 0; i < 5; ++i)
        test.push_encode_frame(i, frame);


    free(frame);
}

#endif
