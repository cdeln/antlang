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

template <typename Target, typename Source>
Target convert(Source&& x)
{
    return Target{std::forward<Source>(x)};
}

template <typename Struct, typename... Ts, size_t... Is>
Struct convert_impl(std::tuple<Ts...>&& x, std::index_sequence<Is...>)
{
    const auto f = [](auto&&... xs) -> Struct { return {std::forward<decltype(xs)>(xs)...}; };
    return std::invoke(f, std::get<Is>(x)...);
}

template <typename Struct, typename... Ts>
Struct convert(std::tuple<Ts...>&& x)
{
    return convert_impl<Struct>(std::move(x), std::make_index_sequence<sizeof...(Ts)>());
}

template <typename Struct, typename T>
Struct convert(std::tuple<T>&& x)
{
    return convert<Struct>(std::move(std::get<0>(x)));
}

template <typename Attribute, typename = void>
struct has_context : std::false_type {};

template <typename Attribute>
struct has_context<Attribute, std::void_t<decltype(Attribute::context)>> : std::true_type {};

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
            attribute_type converted = convert<attribute_type>(std::move(value));
            if constexpr (has_context<Attribute>())
            {
                converted.context = pos->context;
            }
            return parser_success<attribute_type>{std::move(converted), next};
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
