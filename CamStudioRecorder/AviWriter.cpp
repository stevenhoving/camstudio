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

#include "stdafx.h"
#include "AviWriter.h"
#include "Avi.h"

#define AVIIF_KEYFRAME 0x00000010L // this frame is a key frame.

avi_writer::avi_writer(const std::string video_filename, const int fps,
    const BITMAPINFOHEADER &frame_info, const sVideoOpts &video_options)
    : video_filename_(video_filename)
    , fps_(fps)
    , video_options_(video_options)
    , frame_info_(frame_info)
{
    ::AVIFileInit();

    HRESULT hr = ::AVIFileOpenA(&avi_file_, video_filename_.c_str(), OF_WRITE | OF_CREATE, nullptr);
    if (hr != AVIERR_OK)
    {
        TRACE("avi_multiplexer::avi_multiplexer: VideoAviFileOpen error\n");
        CAVI::OnError(hr);
        throw std::runtime_error("unable to initialize avi writer");
    }

    avi_stream_info_.fccType = streamtypeVIDEO;
    // avi_stream_info_.fccHandler = dwCompfccHandler;
    avi_stream_info_.fccHandler = 0;
    avi_stream_info_.dwScale = 1;
    avi_stream_info_.dwRate = (DWORD)fps_; // dwRate / dwScale == samples/second.
    avi_stream_info_.dwSuggestedBufferSize = frame_info_.biSizeImage;
    // rectangle for stream
    SetRect(&avi_stream_info_.rcFrame, 0, 0, (int)frame_info_.biWidth, (int)frame_info_.biHeight);

    // And create the stream;
    hr = ::AVIFileCreateStream(avi_file_, &avi_stream_, &avi_stream_info_);
    if (hr != AVIERR_OK)
    {
        TRACE("AviWriter: AVIFileCreateStream error\n");
        CAVI::OnError(hr);
        throw std::runtime_error("unable to initialize avi writer");
    }

    avi_compress_options_.fccType = streamtypeVIDEO;
    avi_compress_options_.fccHandler = video_options_.m_dwCompfccHandler;
    avi_compress_options_.dwKeyFrameEvery = video_options_.m_iKeyFramesEvery;    // keyframe rate
    avi_compress_options_.dwQuality = video_options_.m_iCompQuality;             // compress quality 0-10, 000
    avi_compress_options_.dwBytesPerSecond = 0;                                  // bytes per second
    avi_compress_options_.dwFlags = AVICOMPRESSF_VALID | AVICOMPRESSF_KEYFRAMES; // flags
    avi_compress_options_.lpFormat = 0x0;                                        // save format
    avi_compress_options_.cbFormat = 0;
    avi_compress_options_.dwInterleaveEvery = 0; // for non-video streams only

    // Ver 1.2
    if ((video_options_.m_dwCompfccHandler != 0) &&
        (video_options_.m_dwCompfccHandler == video_options_.m_dwCompressorStateIsFor))
    {
        // make a copy of the pVideoCompressParams just in case after compression,
        // this variable become messed up
        avi_compress_options_.lpParms = video_options_.State();
        avi_compress_options_.cbParms = video_options_.StateSize();
    }

    hr = AVIMakeCompressedStream(&avi_stream_compressed_, avi_stream_, &avi_compress_options_,
        nullptr);
    if (hr != AVIERR_OK)
    {
        TRACE("AviWriter: AVIMakeCompressedStream error\n");
        CAVI::OnError(hr);

        throw std::runtime_error("unable to initialize avi writer");
    }

    hr = ::AVIStreamSetFormat(avi_stream_compressed_, 0, &frame_info_,
        frame_info_.biSize + frame_info_.biClrUsed * sizeof(RGBQUAD));
    if (hr != AVIERR_OK)
    {
        CAVI::OnError(hr);
        throw std::runtime_error("unable to initialize avi writer");
    }
}

avi_writer::~avi_writer()
{
    if (!avi_file_)
        return;
    stop();
}

void avi_writer::stop()
{
    if (video_options_.m_dwCompfccHandler == video_options_.m_dwCompressorStateIsFor
        && video_options_.m_dwCompfccHandler != 0)
    {
        // Detach pParamsUse from AVICOMPRESSOPTIONS so AVISaveOptionsFree will not free it
        // (we will free it ourselves)
        avi_compress_options_.lpParms = 0;
        avi_compress_options_.cbParms = 0;
    }

    ::AVIStreamRelease(avi_stream_);
    ::AVIStreamRelease(avi_stream_compressed_);
    ::AVIFileRelease(avi_file_);
    avi_file_ = nullptr;
    ::AVIFileExit();
}

void avi_writer::write(DWORD frametime, BITMAPINFOHEADER *alpbi)
{
    LONG lSampWritten = 0L;
    LONG lBytesWritten = 0L;
    auto hr = ::AVIStreamWrite(avi_stream_compressed_, frametime, 1,
        (LPBYTE)alpbi + alpbi->biSize + alpbi->biClrUsed * sizeof(RGBQUAD),
        alpbi->biSizeImage, 0, &lSampWritten, &lBytesWritten);

    if (hr != AVIERR_OK)
    {
        TRACE("AviWriter: AVIStreamWrite error\n");
        CAVI::OnError(hr);
        throw std::runtime_error("unable to write avi frame");
    }

    total_bytes_written_ += lBytesWritten;
    total_samples_written_ += lSampWritten;
}

uint64_t avi_writer::total_bytes_written() const
{
    return total_bytes_written_;
}

uint64_t avi_writer::total_samples_written() const
{
    return total_samples_written_;
}
