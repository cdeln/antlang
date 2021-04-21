#include "formatting.hpp"

#include <sstream>

namespace ant
{

namespace
{

struct token_alternative_string
{
    template <class Token>
    std::string operator()(Token const&)
    {
        return Token::name;
    }

    std::string operator()(floating_point_literal_token const& x)
    {
        std::stringstream builder;
        builder << floating_point_literal_token::name << " " << x.value;
        return builder.str();
    }

    std::string operator()(integer_literal_token const& x)
    {
        std::stringstream builder;
        builder << integer_literal_token::name << " " << x.value;
        return builder.str();
    }

    std::string operator()(identifier_token const& x)
    {
        std::stringstream builder;
        builder << identifier_token::name << " " << x.value;
        return builder.str();
    }
};

} // namespace

std::string quote(std::string const& x)
{
    std::stringstream result;
    result << "'" << x << "'";
    return result.str();
}

std::string token_string(token_variant const& x)
{
    return visit(token_alternative_string(), x);
}

} // namespace
