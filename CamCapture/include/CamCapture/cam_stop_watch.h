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

#include <cstdint>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

namespace cam
{
class stop_watch
{
public:
    stop_watch() noexcept
    {
        int64_t freq;
        if( QueryPerformanceFrequency( reinterpret_cast<LARGE_INTEGER *>(&freq) ) )
        {
            resolution_ = 1.0 / static_cast<double>(freq);
            (void)QueryPerformanceCounter( reinterpret_cast<LARGE_INTEGER *>(&start_time_) );
        }
    }

    void time_start() noexcept
    {
        sampled_time_ = _get_time();
    }

    double time_now() noexcept
    {
        return _get_time();
    }

    double time_since() noexcept
    {
        return _get_time() - sampled_time_;
    }

protected:
    inline double _get_time() noexcept
    {
        int64_t t_64;
        (void)QueryPerformanceCounter( reinterpret_cast<LARGE_INTEGER *>(&t_64) );

        const auto t = static_cast<double>(t_64 - start_time_);
        return t * resolution_;
    }

private:
    double resolution_{0.0};
    double sampled_time_{0.0};
    int64_t start_time_{0};
};

} // namespace cam
