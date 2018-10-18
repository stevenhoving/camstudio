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

#pragma once

#include <atomic>
#include <thread>
#include <mutex>
#include <deque>
#include <cstdint>

namespace mouse_simulation
{

struct input_wrapper;
class simulator
{
    enum class mouse_button
    {
        none,
        left,
        middle,
        right
    };

    enum class mouse_click
    {
        none,
        single_click,
        double_click
    };

    struct click_data
    {
        int x;
        int y;
        mouse_button button;
        mouse_click click;
        uint64_t timeout; // in ms
    };
public:
    simulator();
    ~simulator();

    void add_click(int x, int y);

    void start();
    void flush();

    void mouse_thread();
private:
    void _send_input(const input_wrapper &input_event);
private:
    int old_x{0};
    int old_y{0};
    std::deque<click_data> mouse_clicks_{};
    std::atomic<bool> run_{true};
    std::thread thread_{};
};

} // namespace mouse_simulation
