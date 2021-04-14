#include "token_builder.hpp"

#include "exceptions.hpp"
#include "formatting.hpp"

#include <cassert>
#include <regex>

namespace ant
{

namespace detail
{

bool matches(std::string const& data, std::string const& pattern)
{
    return std::regex_match(data.begin(), data.end(), std::regex(pattern));
}

} // namespace detail

template <class Token>
token_variant
token_alternative_builder<Token>::build(std::string const& data) const
{
    if (!detail::matches(data, Token::pattern))
    {
        std::stringstream message;
        message << "Data "
                << quote(data)
                << " passed to build does not match builder token pattern "
                << quote(Token::pattern);
        throw token_pattern_mismatch_error(message.str());
    }
    return Token{};
}

template <>
token_variant
token_alternative_builder<identifier_token>::build(std::string const& name) const
{
    if (!detail::matches(name, identifier_token::pattern))
    {
        std::stringstream message;
        message << "Name "
                << quote(name)
                << " passed to build does not match identifier token pattern "
                << quote(identifier_token::pattern);
        throw token_pattern_mismatch_error(message.str());
    }
    return identifier_token{name};
}

template <>
token_variant
token_alternative_builder<boolean_literal_token>::build(std::string const& value) const
{
    if (!detail::matches(value, boolean_literal_token::pattern))
    {
        std::stringstream message;
        message << "Value "
                << quote(value)
                << " passed to build does not match boolean literal token pattern "
                << quote(boolean_literal_token::pattern);
        throw token_pattern_mismatch_error(message.str());
    }
    return boolean_literal_token{value};
}

template <>
token_variant
token_alternative_builder<integer_literal_token>::build(std::string const& value) const
{
    if (!detail::matches(value, integer_literal_token::pattern))
    {
        std::stringstream message;
        message << "Value "
                << quote(value)
                << " passed to build does not match integer literal token pattern "
                << quote(integer_literal_token::pattern);
        throw token_pattern_mismatch_error(message.str());
    }
    return integer_literal_token{value};
}

template <>
token_variant
token_alternative_builder<floating_point_literal_token>::build(std::string const& value) const
{
    if (!detail::matches(value, floating_point_literal_token::pattern))
    {
        std::stringstream message;
        message << "Value "
                << quote(value)
                << " passed to build does not match floating point literal token pattern "
                << quote(floating_point_literal_token::pattern);
        throw token_pattern_mismatch_error(message.str());
    }
    return floating_point_literal_token{value};
}

template <typename Token>
std::string
token_alternative_builder<Token>::pattern() const
{
    return Token::pattern;
}

template class token_alternative_builder<left_parenthesis_token>;
template class token_alternative_builder<right_parenthesis_token>;
template class token_alternative_builder<function_token>;
template class token_alternative_builder<structure_token>;
template class token_alternative_builder<condition_token>;
template class token_alternative_builder<floating_point_literal_token>;
template class token_alternative_builder<integer_literal_token>;
template class token_alternative_builder<boolean_literal_token>;
template class token_alternative_builder<identifier_token>;
template class token_alternative_builder<end_of_input_token>;

} // namespace ant
