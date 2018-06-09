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

#pragma once

#include "utility/make_array.h"
#include "utility/filesystem.h"
#include <fmt/format.h>
#include <cpptoml.h>
#include <string_view>
#include <optional>
#include <iostream>

class video_source
{
public:
    enum type
    {
        gdi,
        //desktop_duplication /* disable for now */
    };

    static constexpr auto names()
    {
        return make_array(
            L"GDI"
            // L"Desktop Duplication" /* disabled for now */
        );
    }

    video_source(type new_type)
        : type_(new_type)
    {
    }
    int get_index() const
    {
        return type_;
    }
    void set_index(const type new_type)
    {
        type_ = new_type;
    }
    void set_index(const int new_type)
    {
        type_ = static_cast<type>(new_type);
    }

private:
    type type_;
};

class video_container
{
public:
    enum type
    {
        mkv,
        mp4,
        avi
    };

    static constexpr auto names()
    {
        return make_array(
            L"MKV",
            L"MP4",
            L"AVI");
    }

    video_container(type new_type)
        : type_(new_type)
    {
    }
    int get_index() const
    {
        return type_;
    }
    void set_index(const type new_type)
    {
        type_ = new_type;
    }
    void set_index(const int new_type)
    {
        type_ = static_cast<type>(new_type);
    }

private:
    type type_;
};

class video_codec
{
public:
    enum type
    {
        x264,
        camstudio
    };
    static constexpr auto names()
    {
        return make_array(
            L"H.264 (x264)",
            L"CamStudio"
        );
    }

    video_codec(type new_type)
        : type_(new_type)
    {
    }
    int get_index() const
    {
        return type_;
    }
    void set_index(const type new_type)
    {
        type_ = new_type;
    }
    void set_index(const int new_type)
    {
        type_ = static_cast<type>(new_type);
    }

private:
    type type_;
};

class video_codec_preset
{
public:
    enum type
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
    static constexpr auto names()
    {
        return make_array(
            L"Ultrafast",
            L"Superfast",
            L"Veryfast",
            L"Faster",
            L"Fast",
            L"Medium",
            L"Slow",
            L"Slower",
            L"Veryslow");
    }

    video_codec_preset(type new_type)
        : type_(new_type)
    {
    }
    int get_index() const
    {
        return type_;
    }
    void set_index(const type new_type)
    {
        type_ = new_type;
    }
    void set_index(const int new_type)
    {
        type_ = static_cast<type>(new_type);
    }

private:
    type type_;
};

class video_codec_tune
{
public:
    enum type
    {
        none,        // no tune specified
        film,        // use for high quality movie content; lowers deblocking
        animation,   // good for cartoons; uses higher deblocking and more reference frames
        grain,       // preserves the grain structure in old, grainy film material
        stillimage,  // good for slideshow-like content
        fastdecode,  // allows faster decoding by disabling certain filters
        zerolatency, // good for fast encoding and low-latency streaming
    };
    static constexpr auto names()
    {
        return make_array(
            L"None",
            L"Film",
            L"Animation",
            L"Grain",
            L"Still Image",
            L"Fast Decode",
            L"Zero Latency");
    }

    video_codec_tune(type new_type)
        : type_(new_type)
    {
    }
    int get_index() const
    {
        return type_;
    }
    void set_index(const type new_type)
    {
        type_ = new_type;
    }
    void set_index(const int new_type)
    {
        type_ = static_cast<type>(new_type);
    }

private:
    type type_;
};

class video_codec_profile
{
public:
    enum type
    {
        none, // no profile specified
        baseline,
        main,
        high,
        //high10, /* disable for now */
        //high422,
        //high444
    };
    static constexpr auto names()
    {
        return make_array(
            L"Auto",
            L"Baseline",
            L"Main",
            L"High"
            //L"High10", /* disable for now */
            //L"High422",
            //L"High444"
            );
    }

    video_codec_profile(type new_type)
        : type_(new_type)
    {
    }
    int get_index() const
    {
        return type_;
    }
    void set_index(const type new_type)
    {
        type_ = new_type;
    }
    void set_index(const int new_type)
    {
        type_ = static_cast<type>(new_type);
    }

private:
    type type_;
};

class video_codec_level
{
public:
    enum type
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
    static constexpr auto names()
    {
        return make_array(
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
            L"5.2");
    }

    video_codec_level(type new_type)
        : type_(new_type)
    {
    }
    int get_index() const
    {
        return type_;
    }
    void set_index(const type new_type)
    {
        type_ = new_type;
    }
    void set_index(const int new_type)
    {
        type_ = static_cast<type>(new_type);
    }

private:
    type type_;
};

enum class video_quality_type
{
    constant_bitrate,
    constant_quality
};

class video_settings_model
{
public:
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
