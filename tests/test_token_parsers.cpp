#include <doctest/doctest.h>

#include "token_parsers.hpp"

using namespace ant;

TEST_CASE("non-attributed token parser parses non attributed token")
{
    std::vector<token> tokens;
    tokens.push_back({left_parenthesis_token{}});
    parser<left_parenthesis_token> parser;
    auto position = parser.parse(tokens.cbegin(), tokens.cend()).position;
    CHECK(position == tokens.cend());
}

TEST_CASE("non-attributed token parser raises error on unexpected token")
{
    std::vector<token> tokens;
    tokens.push_back({right_parenthesis_token{}});
    parser<left_parenthesis_token> parser;
    CHECK_THROWS(parser.parse(tokens.cbegin(), tokens.cend()));
}

TEST_CASE("non-attributed token parser raises error on end of input")
{
    std::vector<token> tokens;
    parser<left_parenthesis_token> parser;
    CHECK_THROWS(parser.parse(tokens.cbegin(), tokens.cend()));
}

TEST_CASE("attributed token parser parses identifier token")
{
    std::vector<token> tokens;
    tokens.push_back({identifier_token{}});
    parser<identifier_token> parser;
    auto [value, position] = parser.parse(tokens.cbegin(), tokens.cend());
    CHECK((std::is_same_v<decltype(value), std::string>));
    CHECK(position == tokens.cend());
}

TEST_CASE("attributed token parser raises error on end of input")
{
    std::vector<token> tokens;
    parser<identifier_token> parser;
    CHECK_THROWS(parser.parse(tokens.cbegin(), tokens.cend()));
}
