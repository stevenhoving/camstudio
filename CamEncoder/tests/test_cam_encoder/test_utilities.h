#pragma once

#include <CamEncoder/av_video.h>
#include <windows.h>

#pragma pack(1)
struct rgb
{
    uint8_t r, g, b;
};

// \note this function does not create a correct bitmapinfo object.
static BITMAPINFO *create_bmpinfo(int width, int height)
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
    frame->bmiHeader.biSizeImage = static_cast<DWORD>(image_size);

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