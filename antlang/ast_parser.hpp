#pragma once

#include "ast_rules.hpp"
#include "formatting.hpp"
#include "parser.hpp"
#include "parser_result.hpp"
#include "tokens.hpp"

#include <functional>
#include <sstream>
#include <tuple>
#include <utility>
#include <variant>
#include <vector>

namespace ant
{

template <typename T>
T convert(T&& x)
{
    return std::move(x);
}

template <typename Struct, typename... Ts, size_t... Is>
Struct convert_impl(std::tuple<Ts...>&& x, std::index_sequence<Is...>)
{
    return std::invoke([](auto&&... xs) -> Struct { return {xs...}; }, std::get<Is>(x)...);
}

template <typename Struct, typename... Ts>
Struct convert(std::tuple<Ts...>&& x)
{
    return convert_impl<Struct>(std::move(x), std::make_index_sequence<sizeof...(Ts)>());
}

template <typename Attribute>
struct parser<ast_rule<Attribute>>
{
    using attribute_type = attribute_of_t<ast_rule<Attribute>>;

    parser_result<attribute_type>
    parse(std::vector<token>::const_iterator pos,
          std::vector<token>::const_iterator end) const
    {
        const auto parser = make_parser<ast_rule<Attribute>>();
        auto result = parser.parse(pos, end);
        if (is_success(result))
        {
            auto [value, next] = get_success(result);
            return parser_success<attribute_type>{convert<Attribute>(std::move(value)), next};
        }
        else
        {
            std::stringstream message;
            message << "Failed to parse " << quote(ast::name_of_v<Attribute>);
            return parser_failure{
                message.str(),
                pos->context,
                {std::move(get_failure(result))}
            };
        }
    }
};

} // namespace
