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

#include "CamEncoder/circular_frame_buffer.h"
#include "fmt/color.h"

circular_frame_buffer::circular_frame_buffer(int seconds)
    : buffer_size_seconds(seconds)
{
}

circular_frame_buffer::~circular_frame_buffer()
{
    for (auto &packet : buffer)
    {
        av_packet_unref(packet.get());
    }
}

void circular_frame_buffer::append_packet(std::unique_ptr<AVPacket> packet)
{
    buffer.emplace_back(std::move(packet));
    shrink_buffer();
}

void circular_frame_buffer::pop_gop()
{
    do
    {
        if (buffer.empty())
            return;

        av_packet_unref(buffer.front().get());
        buffer.pop_front();
    } while (!(buffer.front()->flags & AV_PKT_FLAG_KEY));
}

void circular_frame_buffer::shrink_buffer()
{
    while (buffer_exceeds_limit(buffer_size_seconds))
    {
        pop_gop();
    }
}

std::list<std::unique_ptr<AVPacket>>::iterator circular_frame_buffer::begin()
{
    return buffer.begin();
}

std::list<std::unique_ptr<AVPacket>>::iterator circular_frame_buffer::end()
{
    return buffer.end();
}

bool circular_frame_buffer::buffer_exceeds_limit(int seconds)
{
    if (buffer.begin() == buffer.end())
        return false;

    const auto begin_pts = buffer.front()->pts;
    const auto end_pts = buffer.back()->pts;

    const auto diff_pts = end_pts - begin_pts;
    const auto buffer_time = diff_pts / 1000;

    return buffer_time > seconds;
}