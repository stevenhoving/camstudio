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

#include "settings/settings_enum_type.h"

enum class video_quality_type
{
    constant_bitrate,
    constant_quality
};

struct video_source_type
{
    using enum_type = enum
    {
        gdi,
        //desktop_duplication /* disable for now */
    };
};

static const wchar_t* video_source_strings[] = {
    L"GDI"
    // L"Desktop Duplication" /* disabled for now */
};

using video_source = settings_enum_type<video_source_type,
    std::size(video_source_strings), video_source_strings>;

//////////////////////////////////////////////////////////////////////////

struct video_container_type
{
    using enum_type = enum
    {
        mkv,
        mp4,
        avi
    };
};

static const wchar_t* video_container_strings[] = {
    L"mkv",
    L"mp4",
    L"avi"
};

using video_container = settings_enum_type<video_container_type,
    std::size(video_container_strings), video_container_strings>;

//////////////////////////////////////////////////////////////////////////

struct video_codec_type
{
    using enum_type = enum
    {
        x264,
        camstudio
    };
};

static const wchar_t* video_codec_strings[] = {
    L"H.264 (x264)",
    L"CamStudio"
};

using video_codec = settings_enum_type<video_codec_type,
    std::size(video_codec_strings), video_codec_strings>;

//////////////////////////////////////////////////////////////////////////

struct video_codec_preset_type
{
    using enum_type = enum
    {
        ultrafast,
        superfast,
        veryfast,
        faster,
        fast,
        medium, // default preset
        slow,
        slower,
        veryslow,
    };
};

static const wchar_t* video_codec_preset_strings[] = {
    L"Ultrafast",
    L"Superfast",
    L"Veryfast",
    L"Faster",
    L"Fast",
    L"Medium",
    L"Slow",
    L"Slower",
    L"Veryslow"
};

using video_codec_preset = settings_enum_type<video_codec_preset_type,
    std::size(video_codec_preset_strings), video_codec_preset_strings>;

//////////////////////////////////////////////////////////////////////////

struct video_codec_tune_type
{
    using enum_type = enum
    {
        none,        // no tune specified
        film,        // use for high quality movie content; lowers deblocking
        animation,   // good for cartoons; uses higher deblocking and more reference frames
        grain,       // preserves the grain structure in old, grainy film material
        stillimage,  // good for slideshow-like content
        fastdecode,  // allows faster decoding by disabling certain filters
        zerolatency, // good for fast encoding and low-latency streaming
    };
};

static const wchar_t* video_codec_tune_strings[] = {
    L"None",
    L"Film",
    L"Animation",
    L"Grain",
    L"Still Image",
    L"Fast Decode",
    L"Zero Latency"
};

using video_codec_tune = settings_enum_type<video_codec_tune_type,
    std::size(video_codec_tune_strings), video_codec_tune_strings>;

//////////////////////////////////////////////////////////////////////////

struct video_codec_profile_type
{
    using enum_type = enum
    {
        none, // no profile specified
        baseline,
        main,
        high,
        //high10, /* disable for now */
        //high422,
        //high444
    };
};

static const wchar_t* video_codec_profile_strings[] = {
    L"Auto",
    L"Baseline",
    L"Main",
    L"High"
    //L"High10", /* disable for now */
    //L"High422",
    //L"High444"
};

using video_codec_profile = settings_enum_type<video_codec_profile_type,
    std::size(video_codec_profile_strings), video_codec_profile_strings>;

//////////////////////////////////////////////////////////////////////////

struct video_codec_level_type
{
    using enum_type = enum
    {
        none,
        level1_0,
        level1_b,
        level1_1,
        level1_2,
        level2_0,
        level2_1,
        level2_2,
        level3_0,
        level3_1,
        level3_2,
        level4_0,
        level4_1,
        level4_2,
        level5_0,
        level5_1,
        level5_2,
    };
};

static const wchar_t* video_codec_level_strings[] = {
    L"Auto",
    L"1.0",
    L"1b",
    L"1.1",
    L"1.2",
    L"2.0",
    L"2.1",
    L"2.2",
    L"3.0",
    L"3.1",
    L"3.2",
    L"4.0",
    L"4.1",
    L"4.2",
    L"5.0",
    L"5.1",
    L"5.2"
};

using video_codec_level = settings_enum_type<video_codec_level_type,
    std::size(video_codec_level_strings), video_codec_level_strings>;
