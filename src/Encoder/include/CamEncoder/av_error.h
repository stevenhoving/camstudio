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

#pragma once


#include <string>

//typedef struct AVRational {
//    int num; ///< Numerator
//    int den; ///< Denominator
//} AVRational;

struct AVRational;

std::string av_error_to_string(int error_number);
std::string av_timestamp_to_string(int64_t timestamp);
std::string av_timestamp_to_timestring(int64_t timestamp, AVRational &tb);
