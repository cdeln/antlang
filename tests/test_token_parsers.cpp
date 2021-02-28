#include <doctest/doctest.h>

#include "token_parsers.hpp"
#include "token_rules.hpp"

using namespace ant;

TEST_CASE("non-attributed token parser parses non attributed token")
{
    std::vector<token> tokens;
    tokens.push_back({left_parenthesis_token{}});
    auto parser = make_parser<left_parenthesis_token>();
    auto position = parser.parse(tokens.cbegin(), tokens.cend()).position;
    CHECK(position == tokens.cend());
}

TEST_CASE("non-attributed token parser raises error on unexpected token")
{
    std::vector<token> tokens;
    tokens.push_back({right_parenthesis_token{}});
    auto parser = make_parser<left_parenthesis_token>();
    CHECK_THROWS(parser.parse(tokens.cbegin(), tokens.cend()));
}

TEST_CASE("non-attributed token parser raises error on end of input")
{
    std::vector<token> tokens;
    auto parser = make_parser<left_parenthesis_token>();
    CHECK_THROWS(parser.parse(tokens.cbegin(), tokens.cend()));
}

TEST_CASE("attributed token parser parses identifier token")
{
    std::vector<token> tokens;
    tokens.push_back({identifier_token{}});
    auto parser = make_parser<identifier_token>();
    auto [value, position] = parser.parse(tokens.cbegin(), tokens.cend());
    CHECK((std::is_same_v<decltype(value), std::string>));
    CHECK(position == tokens.cend());
}

TEST_CASE("attributed token parser raises error on end of input")
{
    std::vector<token> tokens;
    auto parser = make_parser<identifier_token>();
    CHECK_THROWS(parser.parse(tokens.cbegin(), tokens.cend()));
}
