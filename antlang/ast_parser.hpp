#pragma once

#include "parser.hpp"
#include "parser_result.hpp"
#include "tokens.hpp"

#include <functional>
#include <tuple>
#include <utility>
#include <vector>

namespace ant
{

template <typename Struct, typename... Ts, size_t... Is>
Struct convert_impl(std::tuple<Ts...> const& x, std::index_sequence<Is...>)
{
    return std::invoke([](auto&&... xs) -> Struct { return {xs...}; }, std::get<Is>(x)...);
}

template <typename Struct, typename... Ts>
Struct convert(std::tuple<Ts...> const& x)
{
    return convert_impl<Struct>(x, std::make_index_sequence<sizeof...(Ts)>());
}

template <class Element>
struct rule_parser<ast_rule<Element>>
{
    using rule_type = rule_of_t<Element>;
    using attribute_type = attribute_of_t<rule_type>;

    parser_result<attribute_type>
    parse(std::vector<token>::const_iterator pos,
          std::vector<token>::const_iterator end) const
    {
        parser<rule_type> parser;
        auto [value, next] = parser.parse(pos, end);
        return {convert<attribute_type>(value), next};
    }
};

} // namespace
