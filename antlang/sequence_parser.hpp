#pragma once

#include "parser.hpp"
#include "sequence.hpp"
#include "fundamental_types.hpp"

#include <utility>
#include <vector>

namespace ant
{

template <class... Ts>
struct parser<sequence<Ts...>>
{
    using attribute_type = attribute_of_t<sequence<Ts...>>;
    using result_type = parser_result<attribute_type>;

    result_type
    recursive_sub_parse(
            attribute_type& values,
            const std::vector<token>::const_iterator pos,
            const std::vector<token>::const_iterator end,
            const std::index_sequence<>,
            const std::index_sequence<>) const
    {
        return parser_success<attribute_type>{values, pos};
    }

    template<size_t RuleIdx, size_t... RuleInds>
    result_type
    recursive_sub_parse(
            attribute_type& values,
            const std::vector<token>::const_iterator pos,
            const std::vector<token>::const_iterator end,
            const std::index_sequence<RuleIdx, RuleInds...>,
            const std::index_sequence<>) const
    {
        using sub_rule = rule_of_t<type_at_t<RuleIdx, Ts...>>;
        using sub_attr = attribute_of_t<sub_rule>;
        static_assert(std::is_same_v<sub_attr, none>);
        parser<sub_rule> sub_parser;
        auto result = sub_parser.parse(pos, end);
        if (is_success(result))
        {
            auto [value, next] = get_success(result);
            static_assert(std::is_same_v<decltype(value), none>);
            return recursive_sub_parse(
                    values,
                    next, end,
                    std::index_sequence<RuleInds...>(),
                    std::index_sequence<>());
        }
        else
        {
            return std::move(get_failure(result));
        }
    }

    template
        < size_t RuleIdx, size_t... RuleInds
        , size_t AttrIdx, size_t... AttrInds
        >
    result_type
    recursive_sub_parse(
            attribute_type& values,
            const std::vector<token>::const_iterator pos,
            const std::vector<token>::const_iterator end,
            std::index_sequence<RuleIdx, RuleInds...> rule_inds,
            std::index_sequence<AttrIdx, AttrInds...> attr_inds) const
    {
        using sub_rule = rule_of_t<type_at_t<RuleIdx, Ts...>>;
        parser<sub_rule> sub_parser;
        auto result = sub_parser.parse(pos, end);
        if constexpr (!std::is_same_v<attribute_of_t<sub_rule>, none>)
        {
            if (is_success(result))
            {
                auto [value, next] = get_success(result);
                std::get<AttrIdx>(values) = std::move(value);
                return recursive_sub_parse(
                        values,
                        next, end,
                        std::index_sequence<RuleInds...>(),
                        std::index_sequence<AttrInds...>());
            }
            else
            {
                return std::move(get_failure(result));
            }
        }
        else
        {
            if (is_success(result))
            {
                auto [value, next] = get_success(result);
                static_cast<void>(value);
                return recursive_sub_parse(
                        values,
                        next, end,
                        std::index_sequence<RuleInds...>(),
                        std::index_sequence<AttrIdx, AttrInds...>());
            }
            else
            {
                return std::move(get_failure(result));
            }
        }
    }

    result_type
    parse(std::vector<token>::const_iterator pos,
          std::vector<token>::const_iterator end) const
    {
        parser_success<attribute_type> success;
        auto result =
            recursive_sub_parse(
                success.value,
                pos, end,
                std::make_index_sequence<sizeof...(Ts)>(),
                std::make_index_sequence<std::tuple_size_v<attribute_type>>());
        if (is_success(result))
        {
            return result;
        }
        else
        {
            auto& sub_failure = get_failure(result);
            sub_failure.previous =
                std::make_unique<parser_failure>(parser_failure{
                    "While parsing sequence",
                    pos->context,
                });
            return std::move(sub_failure);
        }
    }
};

} // namespacea ant
