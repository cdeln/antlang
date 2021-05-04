#pragma once

#include "alternative.hpp"
#include "repetition.hpp"
#include "sequence.hpp"
#include "ast_rules.hpp"
#include "type_filters.hpp"

namespace ant
{

template <size_t Index, typename Rule, typename ParentIterator>
struct rule_iterator {};

struct rule_iterator_end_tag;

using rule_iterator_end = rule_iterator<0, void, rule_iterator_end_tag>;

template <size_t I, typename T, typename P>
constexpr bool is_end(rule_iterator<I, T, P>)
{
    return false;
}

constexpr bool is_end(rule_iterator_end)
{
    return true;
}

template <typename Rule, typename ParentIterator = rule_iterator_end>
constexpr auto make_rule_iterator() -> rule_iterator<0, Rule, ParentIterator> { return {}; };

template <typename Rule, typename ParentIterator = rule_iterator_end>
constexpr auto make_rule_iterator(ParentIterator)
{
    return make_rule_iterator<Rule, ParentIterator>();
};

template <typename T>
struct is_terminal : std::integral_constant<bool, true> {};

template <typename... Ts>
struct is_terminal<sequence<Ts...>> : std::integral_constant<bool, false> {};

template <typename T, typename E>
struct is_terminal<repetition<T, E>> : std::integral_constant<bool, false> {};

template <typename... Ts>
struct is_terminal<alternative<Ts...>> : std::integral_constant<bool, false> {};

template <typename T>
struct is_terminal<ast_rule<T>> : std::integral_constant<bool, false> {};

template <typename T>
constexpr bool is_terminal_v = is_terminal<T>::value;

constexpr size_t unbounded_prefix_length = std::numeric_limits<size_t>::max();

// general case
template <size_t I, typename T, typename P, size_t J, typename U, typename Q>
constexpr size_t
longest_common_prefix(
        rule_iterator<I, T, P> i1,
        rule_iterator<J, U, Q> i2)
{
    if constexpr (is_end(i1) || is_end(i2))
    {
        return 0;
    }
    else
    {
        using R1 = rule_of_t<T>;
        using R2 = rule_of_t<U>;
        static_assert(is_terminal_v<R1>);
        if constexpr (!is_terminal_v<R2>)
        {
            return longest_common_prefix(rule_iterator<J, R2, Q>(), rule_iterator<I, R1, P>());
        }
        else if constexpr (std::is_same_v<R1, R2>)
        {
            if constexpr (is_end(P()) || is_end(Q()))
            {
                return 1;
            }
            else
            {
                return 1 + longest_common_prefix(P(), Q());
            }
        }
        else
        {
            return 0;
        }
    }
}

// helper method for getting the lcp for two rules (non-iterators)
template <typename T1, typename T2>
constexpr size_t longest_common_prefix_for()
{
    return longest_common_prefix(make_rule_iterator<rule_of_t<T1>>(),
                                 make_rule_iterator<rule_of_t<T2>>());
}

// sequence

template <size_t I, typename... Ts, typename P>
constexpr auto next_iterator(rule_iterator<I, sequence<Ts...>, P>)
{
    if constexpr ((I+1) == sizeof...(Ts))
    {
        return P();
    }
    else
    {
        return rule_iterator<I+1, sequence<Ts...>, P>();
    }
}

template <size_t I, typename... Ts, typename P, size_t J, typename R, typename Q>
constexpr size_t
longest_common_prefix(
        rule_iterator<I, sequence<Ts...>, P> i1,
        rule_iterator<J, R, Q> i2)
{
    using child_rule = rule_of_t<type_at_t<I, Ts...>>;
    return longest_common_prefix(make_rule_iterator<child_rule>(next_iterator(i1)), i2);
}

// repetition

template <typename T, typename E, typename P, size_t J, typename R, typename Q>
constexpr size_t
longest_common_prefix(
        rule_iterator<0, repetition<T, E>, P> i1,
        rule_iterator<J, R, Q> i2)
{
    constexpr auto next = rule_iterator<1, repetition<T, E>, P>();
    return longest_common_prefix(make_rule_iterator<rule_of_t<T>>(next), i2);
}

template <typename T, typename E, typename P, size_t J, typename R, typename Q>
constexpr size_t
longest_common_prefix(
        rule_iterator<1, repetition<T, E>, P> i1,
        rule_iterator<J, R, Q> i2)
{
    return std::max(longest_common_prefix(make_rule_iterator<rule_of_t<T>>(i1),  i2),
                    longest_common_prefix(make_rule_iterator<rule_of_t<E>, P>(), i2));
}

template <typename T, typename E1, typename P, typename E2, typename Q>
constexpr size_t
longest_common_prefix(
        rule_iterator<0, repetition<T, E1>, P>,
        rule_iterator<0, repetition<T, E2>, Q>)
{
    return unbounded_prefix_length;
}

// alternative

template <size_t I, typename... Ts, typename P>
constexpr auto next_iterator(rule_iterator<I, alternative<Ts...>, P>)
{
    if constexpr ((I+1) == sizeof...(Ts))
    {
        return P();
    }
    else
    {
        return rule_iterator<I+1, alternative<Ts...>, P>();
    }
}

template <size_t I, typename... Ts, typename P, size_t J, typename R, typename Q>
constexpr size_t
longest_common_prefix(
        rule_iterator<I, alternative<Ts...>, P> i1,
        rule_iterator<J, R, Q> i2)
{
    using child_rule = rule_of_t<type_at_t<I, Ts...>>;
    return std::max(longest_common_prefix(make_rule_iterator<child_rule, P>(), i2),
                    longest_common_prefix(next_iterator(i1), i2));
}

template <typename T>
constexpr T max(T x)
{
    return x;
}

template <typename T, typename... Ts>
constexpr T max(T first, T second, Ts... rest)
{
    return max(std::max(first, second), rest...);
}

template <typename... Ts>
constexpr size_t
alternative_longest_common_prefix(alternative<Ts...>)
{
    return max(
        longest_common_prefix_for<
            apply_type_t<
                alternative,
                remove_t<Ts, std::tuple<Ts...>>
            >,
            Ts
        >()...
    );
}

// ast rules

template <size_t I, typename T, typename P, size_t J, typename R, typename Q>
constexpr size_t
longest_common_prefix(
        rule_iterator<I, ast_rule<T>, P> i1,
        rule_iterator<J, R, Q> i2)
{
    using rule = rule_of_t<ast_rule<T>>;
    return longest_common_prefix(make_rule_iterator<rule, P>(), i2);
}

} // namespace ant
