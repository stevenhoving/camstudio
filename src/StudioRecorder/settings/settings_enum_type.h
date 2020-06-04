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
#include <array>

namespace detail
{
template <class T, std::size_t N, std::size_t... I>
constexpr std::array<std::remove_cv_t<T>, N>
    to_array_impl(T *a, std::index_sequence<I...>)
{
    return { {a[I]...} };
}
}

template <class T, std::size_t N>
constexpr std::array<std::remove_cv_t<T>, N> to_array(T* a)
{
    return detail::to_array_impl<T, N>(a, std::make_index_sequence<N>{});
}

template<typename EnumType, int N, auto EnumStrings>
class settings_enum_type : public EnumType
{
    using string_type = std::remove_reference_t<decltype(EnumStrings[0])>;
public:
    using type = typename EnumType::enum_type;

    static constexpr auto names()
    {
        return to_array<string_type, N>(EnumStrings);
    }

    constexpr settings_enum_type(const type new_type) noexcept
        : type_(new_type)
    {
    }

    constexpr auto get_index() const noexcept
    {
        return static_cast<int>(type_);
    }

    constexpr void set_index(const type new_type) noexcept
    {
        type_ = new_type;
    }

    constexpr void set_index(const int new_type) noexcept
    {
        type_ = static_cast<type>(new_type);
    }

private:
    type type_;
};
