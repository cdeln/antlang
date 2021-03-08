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
    using attribute_type = Literal;
    using value_type = typename Literal::value_type;

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
                    << ", got " << quote(token_name(pos->variant));
            return parser_failure{message.str(), pos->context};
        }
        const auto alternative = std::get<Token>(pos->variant);
        static_assert(std::is_same_v<Token, integer_literal_token> ||
                      std::is_same_v<Token, floating_point_literal_token>);
        try
        {
            return parser_success<Literal>{
                boost::lexical_cast<value_type>(alternative.value),
                pos + 1
            };
        }
        catch (boost::bad_lexical_cast const& e)
        {
            std::stringstream message;
            message << "Literal " << quote(alternative.value)
                    << " does not fit into target type " << quote(ast::name_of_v<Literal>);
            return parser_failure{message.str(), pos->context};
        }
    }
};

} // namespace ant
