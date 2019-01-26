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
#include <variant>
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
        av_mapped_type(AVDictionary **dict, const std::string &key) noexcept;
        av_mapped_type& operator=(std::string value);
        av_mapped_type& operator=(const int64_t value);
        operator const char *() const;
    private:
        AVDictionary **dict_;
        std::string key_;
    };

    struct av_dict_iterator
    {
        using pointer = AVDictionaryEntry *;
        using value_type = typename std::iterator_traits<pointer>::value_type;
        using difference_type = typename std::iterator_traits<pointer>::difference_type;
        using reference = typename std::iterator_traits<pointer>::reference;

        av_dict_iterator(AVDictionary *dict) noexcept;

        bool operator== (const av_dict_iterator &other) const;
        bool operator!= (const av_dict_iterator &other) const;

        auto operator*() -> reference;
        auto operator->() -> pointer;

        // pre increment
        auto operator++() -> av_dict_iterator&;

        // post increment
        auto operator++(int) -> av_dict_iterator;
    private:
        pointer it_;
        AVDictionary *dict_;
    };

    using creation_value_type = std::variant<std::string, int64_t>;
    using creation_pair_type = std::pair<std::string, creation_value_type>;

    av_dict() noexcept;
    ~av_dict() noexcept;

    template<int Size>
    av_dict(std::array<creation_pair_type, Size> init)
    {
        create_from_data(init.data(), Size);
    }

    av_dict(const av_dict &dict);
    av_dict &operator=(const av_dict &dict);

    // disallow move.
    av_dict(av_dict &&) = delete;
    av_dict &operator=(const av_dict &&) = delete;

    int size() const noexcept;

    bool empty() const noexcept;

    void clear() noexcept;

    // \todo make this a bit mode std::map style...
    AVDictionaryEntry *at(const std::string &key, const AVDictionaryEntry *prev = nullptr,
        int flags = 0);

    auto begin()
    {
        auto itr = av_dict_iterator(dict_);
        itr++;
        return itr;
    }

    auto begin() const -> const av_dict_iterator
    {
        auto itr = av_dict_iterator(dict_);
        itr++;
        return itr;
    }

    auto end() -> av_dict_iterator
    {
        return av_dict_iterator(dict_);
    }

    auto end() const -> const av_dict_iterator
    {
        return av_dict_iterator(dict_);
    }

    av_mapped_type operator[](const std::string &key) noexcept;

    operator AVDictionary **() noexcept;

    [[nodiscard]]
    AVDictionary *release() noexcept;

private:
    void create_from_data(const av_dict::creation_pair_type *items, int item_count);
    AVDictionary *dict_{nullptr};
};

template <std::size_t N>
auto make_av_dict(const av_dict::creation_pair_type (&init)[N])
{
    std::array<av_dict::creation_pair_type, N> a;
    std::copy(init, init + N, a.begin());
    return  av_dict(a);
}
