#pragma once

#include "exceptions.hpp"
#include "parser.hpp"
#include "repetition.hpp"

#include <vector>

namespace ant
{

template <typename T, typename End>
struct parser<repetition<T, End>>
{
    using attribute_type = attribute_of_t<repetition<T>>;

    parser_result<attribute_type>
    parse(std::vector<token>::const_iterator pos,
          std::vector<token>::const_iterator end) const
    {
        parser_success<attribute_type> result;
        bool parsed_end = false;

        while (pos != end)
        {
            const auto end_parser = make_parser<End>();
            auto end_result = end_parser.parse(pos, end);
            if (is_success(end_result))
            {
                auto [value, next] = get_success(end_result);
                static_cast<void>(value);
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
                    result.value.push_back(std::move(value));
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

        result.position = pos;
        return result;
    }
};

} // namespace ant
