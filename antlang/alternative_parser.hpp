#pragma once

#include "alternative.hpp"
#include "exceptions.hpp"
#include "parser.hpp"
#include "type_filters.hpp"

#include <sstream>
#include <utility>

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
        std::stringstream message;
        message << "Failed to parse alternative";
        throw alternative_parser_error(message.str(), pos->context);
    }

    template <size_t I, size_t... Is>
    result_type
    recursive_sub_parse(
            std::index_sequence<I, Is...>,
            std::vector<token>::const_iterator pos,
            std::vector<token>::const_iterator end) const
    {
        try
        {
            auto sub_parser = make_parser<type_at_t<I, Ts...>>();
            auto [value, next] = sub_parser.parse(pos, end);
            return {value, next};
        }
        catch (unexpected_token_error const&)
        {
            return recursive_sub_parse(std::index_sequence<Is...>(), pos, end);
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
