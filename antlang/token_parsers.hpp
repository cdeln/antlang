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
                    << ", got " << quote(token_name(pos->variant));
            return parser_failure{message.str(), pos->context};
        }
        return parser_success<none>{{}, pos + 1};
    }
};

template <class Token, typename Attribute>
struct parser<attributed_token_rule<Token, Attribute>>
{
    parser_result<Attribute>
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
            return parser_failure{message.str(), pos->context};
        }
        return parser_success<Attribute>{std::get<Token>(pos->variant).value, pos + 1};
    }
};

template <class Token>
struct parser<attributed_token_rule<Token, none>>
{
    parser_result<none>
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
                    << ", got " << quote(token_name(pos->variant));
            return parser_success<none>{message.str(), pos->context};
        }
        return {none{}, pos + 1};
    }
};

} // namespace ant
