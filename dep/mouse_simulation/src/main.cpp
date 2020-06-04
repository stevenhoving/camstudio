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

int main()
{
    mouse_simulation::simulator sim;
    sim.add_click(50, 50);
    sim.add_click(200, 20);
    sim.add_click(20, 1000); // click on start menu
    sim.add_click(1920, 1080);
    sim.start();
    sim.flush();

    return 0;
}
