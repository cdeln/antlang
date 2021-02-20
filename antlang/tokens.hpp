#pragma once

#include <string>
#include <variant>

namespace ant
{

struct left_parenthesis_token
{
    static constexpr char name[] = "(";
    static constexpr char pattern[] = R"(\()";
};

struct right_parenthesis_token
{
    static constexpr char name[] = ")";
    static constexpr char pattern[] = R"(\))";
};

struct function_token
{
    static constexpr char name[] = "function";
    static constexpr char pattern[] = "function";
};

struct structure_token
{
    static constexpr char name[] = "structure";
    static constexpr char pattern[] = "structure";
};

struct floating_point_literal_token
{
    static constexpr char name[] = "floating-point-literal";
    static constexpr char pattern[] = R"([0-9]+[.][0-9]+)";
    std::string value;
};

struct integer_literal_token
{
    static constexpr char name[] = "integer-literal";
    static constexpr char pattern[] = R"([0-9]+)";
    std::string value;
};

struct identifier_token
{
    static constexpr char name[] = "identifier";
    static constexpr char pattern[] = R"([^() ]+)";
    std::string value;
};

using token_variant =
  std::variant
    < left_parenthesis_token
    , right_parenthesis_token
    , function_token
    , structure_token
    , floating_point_literal_token
    , integer_literal_token
    , identifier_token
    >;

struct token_context
{
    int line;
    int offset;
};

struct token
{
    token_variant variant;
    token_context context;
};

template <class TokenAlternative>
std::string token_name()
{
    return TokenAlternative::name;
}

std::string token_name(token_variant const& variant);


} // namespace ant
