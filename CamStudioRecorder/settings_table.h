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

#pragma once

#include "string_convert.h"
#include <CamCapture/cam_color.h>
#include <CamCapture/cam_rect.h>
#include <CamCapture/cam_size.h>
#include <cpptoml.h>

 /* cpptoml table wrapper to make things easier */
class table
{
public:
    table(std::shared_ptr<cpptoml::table> table)
        : table_(table)
    {
    }

    template<typename T>
    void insert(const std::string &key, T value)
    {
        if constexpr(std::is_enum_v<T>)
            table_->insert<int64_t>(key, static_cast<int64_t>(value));
        else
            table_->insert<T>(key, std::move(value));
    }

    template<>
    void insert(const std::string &key, cam::color value)
    {
        table_->insert<uint32_t>(key, value);
    }

    template<>
    void insert(const std::string &key, rect<int> value)
    {
        auto array = cpptoml::make_array();
        array->push_back(value.left());
        array->push_back(value.top());
        array->push_back(value.right());
        array->push_back(value.bottom());
        table_->insert(key, array);
    }

    template<>
    void insert(const std::string &key, std::wstring value)
    {
        table_->insert<std::string>(key, wstring_to_utf8(value));
    }

    template<typename T>
    auto get(const std::string &key) const
    {
        return table_->get_as<T>(key);
    }

    template<>
    auto get<std::wstring>(const std::string &key) const
    {
        const auto value = *table_->get_as<std::string>(key);
        return cpptoml::option<std::wstring>(utf8_to_wstring(value));
    }

    bool contains(const std::string &key) const
    {
        return table_->contains(key);
    }

    template<typename T>
    auto get_optional(const std::string &key, const T default_value)
    {
        if (!table_)
            return default_value;

        if (!table_->contains(key))
            return default_value;

        if constexpr(std::is_enum_v<T>)
            return static_cast<T>(*table_->get_as<int64_t>(key));
        else
            return *table_->get_as<T>(key);
    }

    template<>
    auto get_optional<cam::color>(const std::string &key, const cam::color default_value)
    {
        const auto value = table_->get_as<uint32_t>(key);
        if (!value)
            return default_value;

        return cam::color(*value);
    }

    // cookie cutter of a cookie cutter is preferred.
    template<>
    auto get_optional<rect<int>>(const std::string &key, const rect<int> default_value)
    {
        const auto value = table_->get_array_of<int64_t>(key);
        if (!value)
            return default_value;

        assert(value->size() == 4);
        return rect<int>(
            static_cast<int>(value->at(0)),
            static_cast<int>(value->at(1)),
            static_cast<int>(value->at(2)),
            static_cast<int>(value->at(3))
        );
    }

    template<>
    auto get_optional<std::wstring>(const std::string &key, const std::wstring default_value)
    {
        const auto value = table_->get_as<std::string>(key);
        if (!value)
            return default_value;

        return utf8_to_wstring(*value);
    }

    auto get_table() const
    {
        return table_;
    }

private:
    std::shared_ptr<cpptoml::table> table_;
};