#pragma once

#include "ast.hpp"
#include "formatting.hpp"
#include "literal_rule.hpp"
#include "parser.hpp"
#include "tokens.hpp"

#include <boost/lexical_cast.hpp>

#include <vector>

namespace ant
{

template <class Literal, class Token>
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
        try
        {
            return {boost::lexical_cast<typename Literal::value_type>(alternative.value), pos + 1};
        }
        catch (boost::bad_lexical_cast const& e)
        {
            std::stringstream message;
            message << "Literal " << quote(alternative.value)
                    << " does not fit into target type " << quote(ast::name_of_v<Literal>)
                    << " at line " << pos->context.line;
            throw std::runtime_error(message.str());
        }
    }
};

} // namespace ant
