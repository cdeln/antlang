#pragma once

#include <string>
#include <variant>

namespace ant
{

struct left_parenthesis_token
{
    constexpr static char pattern[] = R"(\()";
};

struct right_parenthesis_token
{
    constexpr static char pattern[] = R"(\))";
};

struct function_token
{
    constexpr static char pattern[] = "function";
};

struct structure_token
{
    constexpr static char pattern[] = "structure";
};

struct protocol_token
{
    constexpr static char pattern[] = "protocol";
};

struct module_token
{
    constexpr static char pattern[] = "module";
};

struct import_token
{
    constexpr static char pattern[] = "import";
};

struct export_token
{
    constexpr static char pattern[] = "export";
};

struct floating_point_literal_token
{
    constexpr static char pattern[] = R"([0-9]+[.][0-9]+)";
    std::string value;
};

struct integer_literal_token
{
    constexpr static char pattern[] = R"([0-9]+)";
    std::string value;
};


struct identifier_token
{
    constexpr static char pattern[] = R"([^() ]+)";
    std::string name;
};

using token_variant =
  std::variant
    < left_parenthesis_token
    , right_parenthesis_token
    , function_token
    , structure_token
    , protocol_token
    , module_token
    , import_token
    , export_token
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

} // namespace ant
