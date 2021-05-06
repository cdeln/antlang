#pragma once

#include "exceptions.hpp"
#include "parser.hpp"
#include "repetition.hpp"

#include <vector>

namespace ant
{

template <typename T>
struct is_tuple : std::false_type {};

template <typename... Ts>
struct is_tuple<std::tuple<Ts...>> : std::true_type {};

template <typename T>
constexpr bool is_tuple_v = is_tuple<T>::value;

template <typename Attribute, typename T, typename End>
auto make_collapsed(std::vector<T>&& values, End end)
{
    if constexpr (is_tuple_v<Attribute>)
    {
        return std::make_tuple(std::move(values), std::move(end));
    }
    else
    {
        return std::move(values);
    }
}

template <typename T, typename End>
struct parser<repetition<T, End>>
{
    using rep_attr = attribute_of_t<rule_of_t<T>>;
    using end_attr = attribute_of_t<rule_of_t<End>>;
    using attribute_type = attribute_of_t<repetition<T, End>>;

    parser_result<attribute_type>
    parse(std::vector<token>::const_iterator pos,
          std::vector<token>::const_iterator end) const
    {
        std::vector<rep_attr> values;
        end_attr end_value;
        bool parsed_end = false;

        while (pos != end)
        {
            const auto end_parser = make_parser<End>();
            auto end_result = end_parser.parse(pos, end);
            if (is_success(end_result))
            {
                auto& [value, next] = get_success(end_result);
                end_value = std::move(value);
                pos = next;
                parsed_end = true;
                break;
            }

            const auto sub_parser = make_parser<T>();
            auto sub_result = sub_parser.parse(pos, end);
            if (is_success(sub_result))
            {
                auto [value, next] = get_success(sub_result);
                if constexpr (!std::is_same_v<attribute_type, none>)
                {
                    values.push_back(std::move(value));
                }
                else
                {
                    static_cast<void>(value);
                }
                pos = next;
            }
            else
            {
                return std::move(get_failure(sub_result));
            }
        }

        if (!parsed_end)
        {
            return parser_failure{
                "Unexpected end of input while parsing repetition"
            };
        }

        return parser_success<attribute_type>{
            make_collapsed<attribute_type>(
                std::move(values), std::move(end_value)
            ),
            pos
        };
    }
};

} // namespace ant
