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

#include <string>
#include <array>

class CWnd;
class CStatic;
class CDC;


namespace utility
{
auto get_window_text(const CWnd &window) -> std::wstring;
auto get_window_title(HWND hwnd) -> std::wstring;

// function to automatically resize a static label based on its window text.
void label_auto_size(CStatic *label);

void draw_bitmap(CDC *pDC, HBITMAP hbitmap, CRect size);

// returns true if point is within the bounds of rect
auto is_in_rect(const CRect &rect, const CPoint &point) -> bool;

// returns true if rect is empty
auto rect_empty(const CRect &rect) -> bool;

auto is_top_most(HWND hwnd) -> bool;

auto get_root_parent(HWND hwnd) -> HWND;

auto get_process_name(unsigned int process_id) -> std::tuple<std::wstring, std::wstring>;

} // namespace utility
