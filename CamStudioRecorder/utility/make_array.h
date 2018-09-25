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

#include <array>

/* \see https://en.cppreference.com/w/cpp/experimental/make_array */
namespace details
{
    template <class>
    struct is_ref_wrapper : std::false_type
    {
    };
    template <class T>
    struct is_ref_wrapper<std::reference_wrapper<T>> : std::true_type
    {
    };

    template <class T>
    using not_ref_wrapper = std::negation<is_ref_wrapper<std::decay_t<T>>>;

    template <class D, class...>
    struct return_type_helper
    {
        using type = D;
    };
    template <class... Types>
    struct return_type_helper<void, Types...> : std::common_type<Types...>
    {
        static_assert(std::conjunction_v<not_ref_wrapper<Types>...>,
                      "Types cannot contain reference_wrappers when D is void");
    };

    template <class D, class... Types>
    using return_type = std::array<typename return_type_helper<D, Types...>::type, sizeof...(Types)>;
}

template <class D = void, class... Types>
constexpr details::return_type<D, Types...> make_array(Types &&... t)
{
    return {std::forward<Types>(t)...};
}