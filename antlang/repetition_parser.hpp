#pragma once

#include "exceptions.hpp"
#include "parser.hpp"
#include "repetition.hpp"

#include <vector>

namespace ant
{

template <class Rule>
struct rule_parser<repetition<Rule>>
{
    using attribute_type = attribute_of_t<repetition<Rule>>;
    using result_type = parser_result<attribute_type>;

    result_type
    parse(std::vector<token>::const_iterator position,
          std::vector<token>::const_iterator end)
    {
        result_type result;
        while (position != end)
        {
            try
            {
                parser<Rule> sub_parser;
                auto sub_result = sub_parser.parse(position, end);
                if constexpr (!std::is_same_v<attribute_type, none>)
                {
                    result.value.push_back(std::move(sub_result.value));
                }
                position = sub_result.position;
            }
            catch (unexpected_token_error const& e)
            {
                break;
            }
        }
        result.position = position;
        return result;
    }
};

} // namespace ant