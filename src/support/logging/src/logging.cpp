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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#include <logging/logging.h>
#include <spdlog/async.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <filesystem>

auto logging::get() -> logging &
{
    static auto global_logger = logging();
    return global_logger;
}

void logging::init(std::wstring log_filename)
{
    const auto filename = std::filesystem::path(log_filename);
    std::filesystem::create_directories(filename.parent_path());

    auto async_file = spdlog::basic_logger_mt<spdlog::async_factory>("main", filename.string());
    spdlog::set_default_logger(async_file);
}

auto logging::get_logger(std::string logger_name) -> std::shared_ptr<spdlog::logger>
{
    return spdlog::default_logger()->clone(std::move(logger_name));
}
