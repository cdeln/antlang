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
    parser_result<Value>
    parse(std::vector<token>::const_iterator pos,
          std::vector<token>::const_iterator end) const
    {
        const auto parser = make_parser<Value>();
        const auto [value, next] = parser.parse(pos, end);
        if (value != Pattern::value)
        {
            std::stringstream message;
            message << "Value " << quote(value)
                    << " did not match the expected pattern " << quote(Pattern::value);
            throw literal_mismatch_error(message.str(), pos->context);
        }
        return {value, next};
    }
};

} // namespace ant
