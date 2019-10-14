/**
 * Copyright(C) 2019  Steven Hoving
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

#pragma once

#include "av_ffmpeg.h"
#include <list>
#include <memory>

class circular_frame_buffer
{
public:
    explicit circular_frame_buffer(int seconds);
    ~circular_frame_buffer();
    void append_packet(std::unique_ptr<AVPacket> packet);

    std::list<std::unique_ptr<AVPacket>>::iterator begin();
    std::list<std::unique_ptr<AVPacket>>::iterator end();

private:
    void shrink_buffer();
    bool buffer_exceeds_limit(int seconds);
    void pop_gop();

private:
    std::list<std::unique_ptr<AVPacket>> buffer;
    int buffer_size_seconds;
};