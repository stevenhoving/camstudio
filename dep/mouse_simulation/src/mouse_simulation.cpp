/**
 * Copyright(C) 2018 - 2020  Steven Hoving
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

#include "mouse_simulation/mouse_simulation.h"
#include <fmt/printf.h>
#include <windows.h>
#include <array>

namespace mouse_simulation
{
void normalize_coordinates(int &x, int &y)
{
    static const auto max_x = ::GetSystemMetrics(SM_CXVIRTUALSCREEN);
    static const auto max_y = ::GetSystemMetrics(SM_CYVIRTUALSCREEN);
    static const auto min_x = ::GetSystemMetrics(SM_XVIRTUALSCREEN);
    static const auto min_y = ::GetSystemMetrics(SM_YVIRTUALSCREEN);
    static const auto width = max_x - min_x;
    static const auto height = max_y - min_y;
    static const auto virtual_width = 65535;
    static const auto virtual_height = 65535;

    x = static_cast<int>((static_cast<float>(x) / width) * virtual_width);
    y = static_cast<int>((static_cast<float>(y) / height) * virtual_height);
}

struct input_wrapper : INPUT
{
    input_wrapper(int x, int y, int mouse_event_type)
    {
        normalize_coordinates(x, y);
        type = INPUT_MOUSE;
        mi.dx = x;
        mi.dy = y;
        mi.mouseData = 0;
        mi.time = 0;
        mi.dwFlags = mouse_event_type;
    }
};

simulator::simulator()
{
    POINT pos;
    GetCursorPos(&pos);
    old_x = pos.x;
    old_y = pos.y;
}

simulator::~simulator()
{
    run_ = false;
    if (thread_.joinable())
        thread_.join();
}

void simulator::add_click(int x, int y)
{
    // linear interpolation between mouse clicks
    const auto dx = x - old_x;
    const auto x_stem = static_cast<float>(dx) / 100;

    const auto dy = y - old_y;
    const auto y_stem = static_cast<float>(dy) / 100;

    for (int i = 0; i < 100; ++i)
    {
        const auto new_x = static_cast<int>(old_x + (i * x_stem));
        const auto new_y = static_cast<int>(old_y + (i * y_stem));
        const auto data = click_data{ new_x, new_y, mouse_button::none, mouse_click::none, 10 };
        mouse_clicks_.emplace_back(data);
    }

    const auto data = click_data{ x, y, mouse_button::left, mouse_click::single_click, 1000 };
    mouse_clicks_.emplace_back(data);

    old_x = x;
    old_y = y;
}

void simulator::start()
{
    thread_ = std::thread([this](){mouse_thread();});
}

void simulator::flush()
{
    if (thread_.joinable())
        thread_.join();
}

void simulator::mouse_thread()
{
    while (run_)
    {
        fmt::print("process mouse event {}\n", mouse_clicks_.size());

        const auto click = mouse_clicks_.front();
        mouse_clicks_.pop_front();

        _send_input(input_wrapper(click.x, click.y,
            MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE |MOUSEEVENTF_VIRTUALDESK));

        if (click.button != mouse_button::none)
        {
            _send_input(input_wrapper(click.x, click.y, MOUSEEVENTF_LEFTDOWN));
            _send_input(input_wrapper(click.x, click.y, MOUSEEVENTF_LEFTUP));
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(click.timeout));

        if (mouse_clicks_.empty())
            run_ = false;
    }
}

void simulator::_send_input(const input_wrapper &input_event)
{
    const auto input_event_ptr = const_cast<input_wrapper *>(&input_event);
    SendInput(1, input_event_ptr, sizeof(INPUT));
}

} // namespace mouse_simulation
