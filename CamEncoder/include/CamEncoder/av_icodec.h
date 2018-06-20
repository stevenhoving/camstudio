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
 * along with this program.If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once

class av_dict;

// interface for video codec.
class av_icodec
{
public:
    virtual void open(AVStream *stream, av_dict &dict) = 0;
    virtual bool pull_encoded_packet(AVPacket *pkt, bool *valid_packet) = 0;
    virtual AVCodecContext *get_codec_context() const noexcept = 0;
    virtual AVRational get_time_base() const noexcept = 0;
};