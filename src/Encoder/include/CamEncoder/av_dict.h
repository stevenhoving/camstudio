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


#include <string_view>
#include <cstdint>

struct AVDictionary;
struct AVDictionaryEntry;

// this is a wrapper around AVDictionary. The interface is somewhat modeled after std::map.
class av_dict
{
public:
    class av_mapped_type
    {
    public:
        av_mapped_type(AVDictionary **dict, const std::string_view &key) noexcept;
        av_mapped_type& operator=(std::string_view value);
        av_mapped_type& operator=(const int64_t value);
        operator const char *() const;
    private:
        AVDictionary **dict_;
        std::string_view key_;
    };

    av_dict() noexcept;
    ~av_dict() noexcept;

    av_dict(const av_dict &dict);

    av_dict &operator=(const av_dict &dict);

    // disallow move.
    av_dict(av_dict &&) = delete;
    av_dict &operator=(const av_dict &&) = delete;

    int size() const noexcept;

    bool empty() const noexcept;

    void clear() noexcept;

    // \todo make this a bit mode std::map style...
    AVDictionaryEntry *at(const std::string_view &key, const AVDictionaryEntry *prev = nullptr,
        int flags = 0);

    av_mapped_type operator[](const std::string_view &key) noexcept;

    operator AVDictionary **() noexcept;

private:
    AVDictionary *dict_{ nullptr };
};
