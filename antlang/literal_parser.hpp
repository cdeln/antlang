#pragma once

#include "formatting.hpp"
#include "literal_rule.hpp"
#include "parser.hpp"
#include "tokens.hpp"

#include <boost/lexical_cast.hpp>

#include <vector>

namespace ant
{

template <typename Literal, class Token>
struct parser<literal_rule<Literal, Token>>
{
    parser_result<Literal>
    parse(std::vector<token>::const_iterator pos,
          std::vector<token>::const_iterator end) const
    {
        if (pos == end)
        {
            std::stringstream message;
            message << "Unexpected end of input while parsing token";
            throw unexpected_end_of_input_error(message.str());
        }
        if (!std::holds_alternative<Token>(pos->variant))
        {
            std::stringstream message;
            message << "Expected token " << quote(Token::name)
                    << ", got " << quote(token_name(pos->variant))
                    << ", at line " << pos->context.line;
            throw unexpected_token_error(message.str(), pos->context);
        }
        const auto alternative = std::get<Token>(pos->variant);
        static_assert(std::is_same_v<Token, integer_literal_token> ||
                      std::is_same_v<Token, floating_point_literal_token>);
        return {boost::lexical_cast<Literal>(alternative.value), pos + 1};
    }
};

} // namespace ant
