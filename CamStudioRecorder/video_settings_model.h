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

#include "settings/video_settings.h"





class video_settings_model
{
public:
    std::wstring get_video_container_file_extention() const;
    video_source video_source_{video_source::type::gdi};
    int video_source_fps_{30}; // this is heavily depending on the source and the OS.
    video_container video_container_{video_container::type::mkv};
    video_codec video_codec_{video_codec::type::x264};
    video_codec_preset video_codec_preset_{video_codec_preset::type::ultrafast};
    video_codec_tune video_codec_tune_{video_codec_tune::type::none};
    video_codec_profile video_codec_profile_{video_codec_profile::type::baseline};
    video_codec_level video_codec_level_{video_codec_level::type::none};
    int video_codec_quality_bitrate_{4000};
    int video_codec_quality_constant_{27};
    video_quality_type video_codec_quality_type_{video_quality_type::constant_quality};

    /* For now we will fall back to a simple save and load strategy.
     * The current implementation has a couple of problems. No input validation. No format version
     * validation in release mode. etc.
     */
    void save();
    void load();
};
