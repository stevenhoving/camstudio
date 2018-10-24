#pragma once

#include "utility/filesystem.h"

#include <spdlog/spdlog.h>
#include <spdlog/async.h>
#include <spdlog/sinks/basic_file_sink.h>

class logging
{
public:
    auto create_logger(const std::string &logger_name)
    {
        return spdlog::default_logger()->clone(logger_name);
    }

    static auto &get()
    {
        static auto global_logger = logging();
        return global_logger;
    }

    static auto get_logger(const std::string &logger_name)
    {
        return get().create_logger(logger_name);
    }

protected:
    logging()
    {
        const auto app_data_path = utility::get_app_data_path();
        std::filesystem::create_directories(app_data_path);

        const auto log_file = app_data_path / "camstudio_log.txt";

        auto async_file = spdlog::basic_logger_mt<spdlog::async_factory>("recorder", log_file.string());
        spdlog::set_default_logger(async_file);
    }
};
