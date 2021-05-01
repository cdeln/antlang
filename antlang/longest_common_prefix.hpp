#pragma once

#include "fundamental_types.hpp"
#include "repetition.hpp"
#include "sequence.hpp"
#include "type_filters.hpp"

namespace ant
{

template <size_t Index, typename Rule, typename ParentIterator>
struct rule_iterator {};

struct rule_iterator_end_tag;

using rule_iterator_end = rule_iterator<0, void, rule_iterator_end_tag>;

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

template <typename T>
constexpr bool is_terminal_v = is_terminal<T>::value;

constexpr size_t unbounded_prefix_length = std::numeric_limits<size_t>::max();

// general case
template <size_t I, typename T, typename P, size_t J, typename U, typename Q>
constexpr size_t
longest_common_prefix(
        rule_iterator<I, T, P>,
        rule_iterator<J, U, Q>)
{
    static_assert(is_terminal_v<T>);
    if constexpr (!is_terminal_v<U>)
    {
        return longest_common_prefix(rule_iterator<J, U, Q>(), rule_iterator<I, T, P>());
    }
    else if constexpr (std::is_same_v<T, U>)
    {
        if constexpr (std::is_same_v<P, rule_iterator_end> || std::is_same_v<Q, rule_iterator_end>)
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

// helper method for getting the lcp for two rules (non-iterators)
template <typename R1, typename R2>
constexpr size_t longest_common_prefix_for()
{
    return longest_common_prefix(make_rule_iterator<R1>(), make_rule_iterator<R2>());
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
    using child_rule = type_at_t<I, Ts...>;
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
    return longest_common_prefix(make_rule_iterator<T>(next), i2);
}

template <typename T, typename E, typename P, size_t J, typename R, typename Q>
constexpr size_t
longest_common_prefix(
        rule_iterator<1, repetition<T, E>, P> i1,
        rule_iterator<J, R, Q> i2)
{
    return std::max(longest_common_prefix(make_rule_iterator<T>(i1),  i2),
                    longest_common_prefix(make_rule_iterator<E, P>(), i2));
}

template <typename T, typename E1, typename P, typename E2, typename Q>
constexpr size_t
longest_common_prefix(
        rule_iterator<0, repetition<T, E1>, P>,
        rule_iterator<0, repetition<T, E2>, Q>)
{
    return unbounded_prefix_length;
}

} // namespace ant
