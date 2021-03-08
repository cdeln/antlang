#pragma once

#include "alternative.hpp"
#include "exceptions.hpp"
#include "parser.hpp"
#include "type_filters.hpp"

#include <sstream>
#include <utility>
#include <vector>

namespace ant
{

template <typename... Ts>
struct parser<alternative<Ts...>>
{
    using attribute_type = attribute_of_t<alternative<Ts...>>;
    using result_type = parser_result<attribute_type>;

    result_type
    recursive_sub_parse(
            std::index_sequence<>,
            std::vector<token>::const_iterator pos,
            std::vector<token>::const_iterator end) const
    {
        return parser_failure{"Failed to parse alternative", pos->context};
    }

    template <size_t I, size_t... Is>
    result_type
    recursive_sub_parse(
            std::index_sequence<I, Is...>,
            std::vector<token>::const_iterator pos,
            std::vector<token>::const_iterator end) const
    {
        const auto parser = make_parser<type_at_t<I, Ts...>>();
        auto result = parser.parse(pos, end);
        if (is_success(result))
        {
            auto [value, next] = get_success(result);
            return parser_success<attribute_type>{std::move(value), next};
        }
        else
        {
            auto sub_result = recursive_sub_parse(std::index_sequence<Is...>(), pos, end);
            if (is_success(sub_result))
            {
                return get_success(sub_result);
            }
            else
            {
                auto& failure = get_failure(result);
                auto& sub_failure = get_failure(sub_result);
                sub_failure.previous = std::make_unique<parser_failure>(std::move(failure));
                return std::move(sub_failure);
            }
        }
    }

    result_type
    parse(std::vector<token>::const_iterator pos,
          std::vector<token>::const_iterator end) const
   {
        return recursive_sub_parse(std::make_index_sequence<sizeof...(Ts)>(), pos, end);
    }
};

} // namespace ant
