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

#include "CamEncoder/av_dict.h"
#include "CamEncoder/av_ffmpeg.h"
#include <cassert>

av_dict & av_dict::operator=(const av_dict &dict)
{
    av_dict_copy(&dict_, dict.dict_, 0);
    return *this;
}

av_dict::av_dict(const av_dict &dict)
{
    av_dict_copy(&dict_, dict.dict_, 0);
}

av_dict::av_dict() noexcept = default;

av_dict::~av_dict() noexcept
{
    av_dict_free(&dict_);
}

int av_dict::size() const noexcept
{
    return av_dict_count(dict_);
}

bool av_dict::empty() const noexcept
{
    return size() == 0;
}

void av_dict::clear() noexcept
{
    av_dict_free(&dict_);
}

AVDictionaryEntry * av_dict::at(const std::string_view &key, const AVDictionaryEntry *prev /*= nullptr*/, int flags /*= 0*/)
{
    auto key_value = av_dict_get(dict_, key.data(), prev, flags);
    if (key_value == nullptr)
        throw std::out_of_range("unable to find key");
    return key_value;
}

av_dict::av_mapped_type av_dict::operator[](const std::string_view &key) noexcept
{
    return av_mapped_type(&dict_, key);
}

av_dict::operator AVDictionary **() noexcept
{
    return &dict_;
}

AVDictionary *av_dict::release() noexcept
{
    auto ptr = dict_;
    dict_ = nullptr;
    return ptr;
}

void av_dict::create_from_data(const creation_pair_type *items, int item_count)
{
    for (int i = 0; i < item_count; ++i)
    {
        const auto element = items[i];
        const auto value_index = element.second.index();
        if (value_index == 0) // std::string
        {
            const auto key = element.first;
            const auto value = std::get<std::string>(element.second);
            if (const auto ret = av_dict_set(&dict_, key.data(), value.data(), 0); ret < 0)
                throw std::runtime_error("key value insertion/assignment failed");
        }
        else if (value_index == 1) // int64_t
        {
            const auto key = element.first;
            const auto value = std::get<int64_t>(element.second);
            if (const auto ret = av_dict_set_int(&dict_, key.data(), value, 0); ret < 0)
                throw std::runtime_error("key value insertion/assignment failed");
        }
    }
}

av_dict::av_mapped_type::av_mapped_type(AVDictionary **dict, const std::string_view &key) noexcept
    : dict_(dict)
    , key_(key)
{
}

av_dict::av_mapped_type::operator const char *() const
{
    auto key_value = av_dict_get(*dict_, key_.data(), nullptr, 0);
    if (key_value == nullptr)
        throw std::out_of_range("unable to find key");

    return key_value->value;
}

av_dict::av_mapped_type& av_dict::av_mapped_type::operator=(const int64_t value)
{
    if (int ret = av_dict_set_int(dict_, key_.data(), value, 0); ret < 0)
        throw std::runtime_error("value insertion/assignment failed");
    return *this;
}

av_dict::av_mapped_type& av_dict::av_mapped_type::operator=(std::string_view value)
{
    if (int ret = av_dict_set(dict_, key_.data(), value.data(), 0); ret < 0)
        throw std::runtime_error("value insertion/assignment failed");
    return *this;
}

bool av_dict::av_dict_iterator::operator!=(const av_dict_iterator &other) const
{
    return !(*this == other);
}

bool av_dict::av_dict_iterator::operator==(const av_dict_iterator &other) const
{
    return it_ == other.it_;
}

av_dict::av_dict_iterator::av_dict_iterator(AVDictionary *dict) noexcept
    : dict_(dict)
    , it_(nullptr)
{
}

av_dict::av_dict_iterator::reference av_dict::av_dict_iterator::operator*()
{
    return *it_;
}

av_dict::av_dict_iterator::pointer av_dict::av_dict_iterator::operator->()
{
    return it_;
}

av_dict::av_dict_iterator av_dict::av_dict_iterator::operator++(int)
{
    av_dict_iterator rc(*this);
    operator++();
    return rc;
}

av_dict::av_dict::av_dict_iterator& av_dict::av_dict_iterator::operator++()
{
    it_ = av_dict_get(dict_, "", it_, AV_DICT_IGNORE_SUFFIX);
    return *this;
}
