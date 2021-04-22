#pragma once

#include "exceptions.hpp"
#include "match.hpp"
#include "parser.hpp"
#include "tokens.hpp"

#include <sstream>
#include <vector>

namespace ant
{

template <class Value, class Pattern>
struct parser<match<Value, Pattern>>
{
    using attribute_type = attribute_of_t<rule_of_t<Value>>;

    parser_result<attribute_type>
    parse(std::vector<token>::const_iterator pos,
          std::vector<token>::const_iterator end) const
    {
        const auto parser = make_parser<Value>();
        auto result = parser.parse(pos, end);
        if (is_success(result))
        {
            const auto [value, next] = get_success(result);
            if (value == Pattern::value)
            {
                return parser_success<attribute_type>{value, next};
            }
            else
            {
                std::stringstream message;
                message << "Value " << quote(value)
                        << " did not match the expected pattern " << quote(Pattern::value);
                return parser_failure{message.str(), pos};
            }
        }
        else
        {
            return std::move(get_failure(result));
        }
    }
};

} // namespace ant
