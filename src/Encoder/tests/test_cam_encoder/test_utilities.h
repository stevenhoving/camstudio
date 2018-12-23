#pragma once

#include <CamEncoder/av_video.h>
#include <windows.h>

#pragma pack(1)
struct rgb555
{
    uint8_t r : 5;
    uint8_t g : 5;
    uint8_t b : 5;
    uint8_t a : 1;
};

#pragma pack(1)
struct rgb
{
    uint8_t r, g, b;
};

#pragma pack(1)
struct rgba
{
    uint8_t r, g, b, a;
};

static int get_pixel_size(AVPixelFormat pixel_format)
{
    switch(pixel_format)
    {
    case AV_PIX_FMT_RGB555LE: return 2;
    case AV_PIX_FMT_BGR24: return 3;
    case AV_PIX_FMT_BGRA:
        [[fallthrough]];
    case AV_PIX_FMT_BGR0: return 4;
    default:
        throw std::runtime_error("unable to get pixel size, invalid pixel format");
    }
}

static void fill_bmpinfo(BITMAPINFO *frame, int index, AVPixelFormat pixel_format = AV_PIX_FMT_BGR24)
{
    const auto width = frame->bmiHeader.biWidth;
    const auto height = frame->bmiHeader.biHeight;
    auto *frame_data = ((uint8_t *)frame) + frame->bmiHeader.biSize + (frame->bmiHeader.biClrUsed * sizeof(RGBQUAD));

    uint8_t value = index % 255;
    switch(pixel_format)
    {
    case AV_PIX_FMT_RGB555LE: {
        rgb555 *data = reinterpret_cast<rgb555 *>(frame_data);
        for (int i = 0; i < width * height; ++i)
        {
            auto &pxl = data[i];
            pxl.r = pxl.g = pxl.b = value++;
        }
    } break;
    case AV_PIX_FMT_BGR24: {
        rgb *data = reinterpret_cast<rgb *>(frame_data);
        for (int i = 0; i < width * height; ++i)
        {
            auto &pxl = data[i];
            pxl.r = pxl.g = pxl.b = value++;
        }
    } break;

    case AV_PIX_FMT_BGRA:
        [[fallthrough]];
    case AV_PIX_FMT_BGR0: {
        rgba *data = reinterpret_cast<rgba *>(frame_data);
        for (int i = 0; i < width * height; ++i)
        {
            auto &pxl = data[i];
            pxl.r = pxl.g = pxl.b = value++;
            pxl.a = 0;
        }
    }
    break;
    }
}

// \note this function does not create a correct bitmapinfo object.
static BITMAPINFO *create_bmpinfo(int width, int height, AVPixelFormat pixel_format = AV_PIX_FMT_BGRA)
{
    // create a bmp
    const auto image_size = width * height * get_pixel_size(pixel_format);
    const auto frame_size = sizeof(BITMAPINFOHEADER) + image_size;

    BITMAPINFO *frame = (BITMAPINFO *)malloc(frame_size + 1);
    if (frame == nullptr)
        return nullptr;

    memset(frame, 0, frame_size);
    frame->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    frame->bmiHeader.biClrUsed = 0;
    frame->bmiHeader.biWidth = width;
    frame->bmiHeader.biHeight = height;
    frame->bmiHeader.biSizeImage = static_cast<DWORD>(image_size);

    fill_bmpinfo(frame, 0, pixel_format);
    return frame;
}
