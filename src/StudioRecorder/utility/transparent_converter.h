#pragma once

#include <stdexcept>
#include <utility>
#include <tuple>
#include <set>

#define OVERLOAD 1

template<class... Ts> struct transparent_compare_overload : Ts... { using Ts::operator()...; using is_transparent = int; };
template<class... Ts> transparent_compare_overload(Ts...)->transparent_compare_overload<Ts...>;

template<typename LeftType, typename RightType>
class transparent_converter
{
private:
    using entry_type = std::tuple<LeftType, RightType>;
public:
    static_assert(!std::is_same_v<LeftType, RightType>, "transparent_converter only support different types");

    template<typename LeftTypeCon, typename RightTypeCon, int N>
    transparent_converter(const std::tuple<LeftTypeCon, RightTypeCon>(&init)[N])
#if OVERLOAD
        : m_set(std::begin(init), std::end(init), transparent_converter::CompareOverload)
#else
        : m_set(std::begin(init), std::end(init))
#endif
    {
    }

    LeftType Convert(const RightType& right) const
    {
        const auto found = m_set.find(right);
        if (found == std::end(m_set))
            throw std::out_of_range("Convertable entry is not found");
        return std::get<0>(*found);
    }

    RightType Convert(const LeftType& left) const
    {
        const auto found = m_set.find(left);
        if (found == std::end(m_set))
            throw std::out_of_range("Convertable entry is not found");
        return std::get<1>(*found);
    }

private:
#if OVERLOAD
    inline static auto CompareOverload = transparent_compare_overload{
        [](const entry_type& lhs, const LeftType& rhs) noexcept {return std::get<0>(lhs) < rhs; },
        [](const LeftType& rhs, const entry_type& lhs) noexcept {return rhs < std::get<0>(lhs); },
        [](const entry_type& lhs, const RightType& rhs) noexcept {return std::get<1>(lhs) < rhs; },
        [](const RightType& rhs, const entry_type& lhs) noexcept {return rhs < std::get<1>(lhs); },
        [](const entry_type& lhs, const entry_type& rhs) noexcept {return rhs < lhs; },
    };
    using Compare = decltype(CompareOverload);
#else
    struct Compare
    {
        using is_transparent = int;

        constexpr bool operator() (const entry_type& lhs, const LeftType& rhs) const noexcept
        {
            return std::get<0>(lhs) < rhs;
        }

        constexpr bool operator() (const LeftType& rhs, const entry_type& lhs) const noexcept
        {
            return rhs < std::get<0>(lhs);
        }

        constexpr bool operator() (const entry_type& lhs, const RightType& rhs) const noexcept
        {
            return std::get<1>(lhs) < rhs;
        }

        constexpr bool operator() (const RightType& rhs, const entry_type& lhs) const noexcept
        {
            return rhs < std::get<1>(lhs);
        }

        constexpr bool operator() (const entry_type& lhs, const entry_type& rhs) const noexcept
        {
            return lhs < rhs;
        }
    };
#endif

    std::set<entry_type, Compare> m_set;
};

template <typename T, typename U, std::size_t N>
auto MakeDynamicConverter(const std::tuple<T, U>(&items)[N])
{
    return transparent_converter<T, U>{items};
}


//static const auto henk = MakeDynamicConverter<int, float>({
//    {1, 1.0},
//    {2, 2.0},
//    });
//
//int main()
//{
//    return henk.Convert(0);
//}
