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

#include "stdafx.h"
#include "video_settings_model.h"
#include "utility/filesystem.h"
#include <cpptoml.h>

std::wstring video_settings_model::get_video_container_file_extention() const
{
    return video_container::names().at(video_container_.get_index());
}

void video_settings_model::save()
{
    auto root = cpptoml::make_table();
    // add a config file format version so we can handle conversions in the future.
    root->insert("version", 1);

    auto videosettings = cpptoml::make_table();

    /* video capture */
    auto capture = cpptoml::make_table();
    capture->insert("source", video_source_.get_index());
    capture->insert("fps", video_source_fps_);
    videosettings->insert("video-capture", capture);

    /* video codec */
    auto codec = cpptoml::make_table();
    codec->insert("codec", video_codec_.get_index());
    codec->insert("preset", video_codec_preset_.get_index());
    codec->insert("tune", video_codec_tune_.get_index());
    codec->insert("profile", video_codec_profile_.get_index());
    codec->insert("level", video_codec_level_.get_index());
    codec->insert("quality_bitrate", video_codec_quality_bitrate_);
    codec->insert("quality_constant", video_codec_quality_constant_);
    codec->insert("quality_type", static_cast<int>(video_codec_quality_type_));

    videosettings->insert("video-codec", codec);

    /* video container */
    videosettings->insert("video-container", video_container_.get_index());

    root->insert("video-settings", videosettings);

    std::ofstream stream(utility::create_config_path(L"video_options.toml"));
    cpptoml::toml_writer writer{stream, ""};
    root->accept(writer);
}

void video_settings_model::load()
{
    const auto config_filepath = utility::create_config_path(L"video_options.toml");

    // non existing config file is not an error.
    if (!std::filesystem::exists(config_filepath))
        return;

    const auto root = cpptoml::parse_file(config_filepath.string());
    const auto version = *root->get_as<int>("version");
    assert(version == 1);

    const auto videosettings = root->get_table("video-settings");

    /* video capture */
    const auto capture = videosettings->get_table("video-capture");
    video_source_.set_index(*capture->get_as<int>("source"));
    video_source_fps_ = *capture->get_as<int>("fps");

    /* video codec */
    const auto codec = videosettings->get_table("video-codec");
    video_codec_.set_index(*codec->get_as<int>("codec"));
    video_codec_preset_.set_index(*codec->get_as<int>("preset"));
    video_codec_tune_.set_index(*codec->get_as<int>("tune"));
    video_codec_profile_.set_index(*codec->get_as<int>("profile"));
    video_codec_level_.set_index(*codec->get_as<int>("level"));
    video_codec_quality_bitrate_ = *codec->get_as<int>("quality_bitrate");
    video_codec_quality_constant_ = *codec->get_as<int>("quality_constant");
    video_codec_quality_type_ = static_cast<video_quality_type>(*codec->get_as<int>("quality_type"));

    /* video container */
    video_container_.set_index(*videosettings->get_as<int>("video-container"));
}
