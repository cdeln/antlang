#pragma once

#include <vector>
#include <tuple>

namespace ant
{

template <typename T, class C>
struct prepend;

template <typename T, typename... Ts>
struct prepend<T, std::tuple<Ts...>>
{
    using type = std::tuple<T, Ts...>;
};

template <typename T, class C>
using prepend_t = typename prepend<T, C>::type;

template <class C>
struct remove_none;

template <class C>
using remove_none_t = typename remove_none<C>::type;

template <typename T, typename... Ts>
struct remove_none<std::tuple<T, Ts...>>
{
    using type = prepend_t<T, remove_none_t<std::tuple<Ts...>>>;
};

template <typename... Ts>
struct remove_none<std::tuple<none, Ts...>>
{
    using type = remove_none_t<std::tuple<Ts...>>;
};

template <>
struct remove_none<std::tuple<>>
{
    using type = std::tuple<>;
};

template <typename T>
struct remove_none<std::vector<T>>
{
    using type = std::vector<T>;
};

template <>
struct remove_none<std::vector<none>>
{
    using type = none;
};

} // namespace ant
