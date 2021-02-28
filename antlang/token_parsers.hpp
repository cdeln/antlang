#pragma once

#include "exceptions.hpp"
#include "formatting.hpp"
#include "parser.hpp"
#include "parser_result.hpp"

#include <sstream>

namespace ant
{

template <class Token>
struct parser<non_attributed_token_rule<Token>>
{
    parser_result<none>
    parse(std::vector<token>::const_iterator pos,
          std::vector<token>::const_iterator end)
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
        return {{}, pos + 1};
    }
};

template <class Token, typename Attribute>
struct parser<attributed_token_rule<Token, Attribute>>
{
    parser_result<Attribute>
    parse(std::vector<token>::const_iterator pos,
          std::vector<token>::const_iterator end)
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
        return {std::get<Token>(pos->variant).value, pos + 1};
    }
};

} // namespace ant
