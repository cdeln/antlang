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

    std::vector<token>::const_iterator
    recursive_sub_parse(
            attribute_type& values,
            const std::vector<token>::const_iterator pos,
            const std::vector<token>::const_iterator end,
            const std::index_sequence<>,
            const std::index_sequence<>) const
    {
        return pos;
    }

    template<size_t RuleIdx, size_t... RuleInds>
    std::vector<token>::const_iterator
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
        auto next = sub_parser.parse(pos, end);
        return recursive_sub_parse(
                values,
                next.position, end,
                std::index_sequence<RuleInds...>(),
                std::index_sequence<>());
    }

    template
        < size_t RuleIdx, size_t... RuleInds
        , size_t AttrIdx, size_t... AttrInds
        >
    std::vector<token>::const_iterator
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
            std::get<AttrIdx>(values) = std::move(result.value);
            return recursive_sub_parse(
                    values,
                    result.position, end,
                    std::index_sequence<RuleInds...>(),
                    std::index_sequence<AttrInds...>());
        }
        else
        {
            return recursive_sub_parse(
                    values,
                    result.position, end,
                    std::index_sequence<RuleInds...>(),
                    std::index_sequence<AttrIdx, AttrInds...>());
        }
    }

    parser_result<attribute_type>
    parse(std::vector<token>::const_iterator pos,
          std::vector<token>::const_iterator end) const
    {
        parser_result<attribute_type> result;
        result.position =
            recursive_sub_parse(
                result.value,
                pos, end,
                std::make_index_sequence<sizeof...(Ts)>(),
                std::make_index_sequence<std::tuple_size_v<attribute_type>>());
        return result;
    }
};

} // namespacea ant
