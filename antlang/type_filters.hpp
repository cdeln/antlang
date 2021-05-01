#pragma once

#include "rules.hpp"

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

template <typename T, class C>
struct remove;

template <typename T, class C>
using remove_t = typename remove<T, C>::type;

template <typename T, typename U, typename... Us>
struct remove<T, std::tuple<U, Us...>>
{
    using type = prepend_t<U, remove_t<T, std::tuple<Us...>>>;
};

template <typename T, typename... Us>
struct remove<T, std::tuple<T, Us...>>
{
    using type = remove_t<T, std::tuple<Us...>>;
};

template <typename T>
struct remove<T, std::tuple<>>
{
    using type = std::tuple<>;
};

template <typename T, typename U>
struct remove<T, std::vector<U>>
{
    using type = std::vector<U>;
};

template <typename T>
struct remove<T, std::vector<T>>
{
    using type = T;
};

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

template <int I, typename T, typename... Ts>
struct type_at : type_at<I - 1, Ts...> {};

template <typename T, typename... Ts>
struct type_at<0, T, Ts...>
{
    using type = T;
};

template <int I, typename... Ts>
using type_at_t = typename type_at<I, Ts...>::type;

template <typename T, typename U, typename... Us>
struct index_of :
    std::integral_constant<size_t,
        std::is_same_v<T, U>
        ? 0
        : 1 + index_of<T, Us...>::value
    >
{};

template <typename T, typename U>
struct index_of<T, U> :
    std::integral_constant<size_t,
        std::is_same_v<T, U>
        ? 0
        : 1
    >
{};

template <typename T, typename U, typename... Us>
constexpr size_t index_of_v = index_of<T, U, Us...>::value;

template <template <typename...> class Template, typename TypeList>
struct apply_type;

template <
    template <typename...> class Template,
    template <typename...> class TypeList,
    typename... Types
>
struct apply_type<Template, TypeList<Types...>>
{
    using type = Template<Types...>;
};

template <template <typename...> class Template, typename TypeList>
using apply_type_t = typename apply_type<Template, TypeList>::type;

struct remove_this_type
{
};

template <template <typename...> class Template, typename... Ts>
struct unique
{
    template <size_t... Is, size_t... Js>
    constexpr static std::integer_sequence<bool, (Is == Js)...>
    where_indices_matches(std::index_sequence<Is...>, std::index_sequence<Js...>);

    template <bool... Select>
    constexpr static
    remove_t<
        remove_this_type,
        std::tuple<
            std::conditional_t<
                Select,
                Ts,
                remove_this_type
            >...
        >
    >
    select_types(std::integer_sequence<bool, Select...>);

    using type =
        apply_type_t<
            Template,
            decltype(
                select_types(
                    where_indices_matches(
                        std::index_sequence<index_of_v<Ts, Ts...>...>(),
                        std::make_index_sequence<sizeof...(Ts)>()
                    )
                )
            )
        >;
};

template <template <typename...> class Template, typename... Ts>
using unique_t = typename unique<Template, Ts...>::type;

} // namespace ant
