#pragma once

#include "alternative.hpp"
#include "exceptions.hpp"
#include "formatting.hpp"
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
        return parser_failure{"Failed to parse alternative", pos};
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
            auto& [value, next] = get_success(result);
            return parser_success<attribute_type>{std::move(value), next};
        }
        else
        {
            auto sub_result = recursive_sub_parse(std::index_sequence<Is...>(), pos, end);
            if (is_success(sub_result))
            {
                return std::move(get_success(sub_result));
            }
            else
            {
                auto& sub_failure = get_failure(sub_result);
                sub_failure.children.push_back(std::move(get_failure(result)));
                return std::move(sub_failure);
            }
        }
    }

    result_type
    parse(std::vector<token>::const_iterator pos,
          std::vector<token>::const_iterator end) const
    {
        auto result = recursive_sub_parse(std::make_index_sequence<sizeof...(Ts)>(), pos, end);
        if (is_failure(result))
        {
            auto& failure = get_failure(result);
            std::stringstream message;
            message << "Exhausted all alternatives, tried parsing";
            std::vector<std::string> alternative_names = {ast::name_of_v<Ts>...};
            if (!alternative_names.empty())
            {
                for (size_t i = 0; i < alternative_names.size() - 1; ++i)
                {
                    message << " " << quote(alternative_names[i]) << ",";
                }
                message << " and " << quote(alternative_names.back());
            }
            // reverse the children, since they are pushed back int last to first rule order.
            std::vector<parser_failure> failures(
                failure.children.rbegin(), failure.children.rend());
            return parser_failure{
                message.str(),
                pos,
                std::move(failures)
            };
        }
        return result;
    }
};

} // namespace ant
