#pragma once

#include "alternative.hpp"
#include "exceptions.hpp"
#include "formatting.hpp"
#include "longest_common_prefix.hpp"
#include "parser.hpp"
#include "type_filters.hpp"

#include <sstream>
#include <utility>
#include <vector>

namespace ant
{

ptrdiff_t
get_longest_failure_offset(std::vector<token>::const_iterator position,
                           parser_failure const& failure);

template <typename... Ts>
struct parser<alternative<Ts...>>
{
    using attribute_type = attribute_of_t<alternative<Ts...>>;
    using result_type = parser_result<attribute_type>;

    constexpr static ptrdiff_t lcp = alternative_longest_common_prefix(alternative<Ts...>());

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
            auto& failure = get_failure(result);
            if (get_longest_failure_offset(pos, failure) > lcp)
            {
                return std::move(failure);
            }
            auto sub_result = recursive_sub_parse(std::index_sequence<Is...>(), pos, end);
            if (is_success(sub_result))
            {
                return std::move(get_success(sub_result));
            }
            else
            {
                auto& sub_failure = get_failure(sub_result);
                if (get_longest_failure_offset(pos, sub_failure) > lcp)
                {
                    return std::move(sub_failure);
                }
                failure.children.push_back(std::move(sub_failure));
                return std::move(failure);
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
            if (get_longest_failure_offset(pos, failure) > lcp)
            {
                return std::move(failure);
            }
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
