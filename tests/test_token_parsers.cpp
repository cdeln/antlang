#include <doctest/doctest.h>

#include "token_parsers.hpp"
#include "token_rules.hpp"

using namespace ant;

TEST_CASE("non-attributed token parser parses non attributed token")
{
    std::vector<token> tokens;
    tokens.push_back({left_parenthesis_token{}});
    const auto parser = make_parser<left_parenthesis_token>();
    const auto result = parser.parse(tokens.cbegin(), tokens.cend());
    REQUIRE(is_success(result));
    CHECK(get_success(result).position == tokens.cend());
}

TEST_CASE("non-attributed token parser returns failure on unexpected token")
{
    std::vector<token> tokens;
    tokens.push_back({right_parenthesis_token{}});
    const auto parser = make_parser<left_parenthesis_token>();
    CHECK(is_failure(parser.parse(tokens.cbegin(), tokens.cend())));
}

TEST_CASE("non-attributed token parser raises error on end of input")
{
    std::vector<token> tokens;
    const auto parser = make_parser<left_parenthesis_token>();
    CHECK_THROWS(is_failure(parser.parse(tokens.cbegin(), tokens.cend())));
}

TEST_CASE("attributed token parser parses identifier token")
{
    std::vector<token> tokens;
    tokens.push_back({identifier_token{}});
    const auto parser = make_parser<identifier_token>();
    const auto result = parser.parse(tokens.cbegin(), tokens.cend());
    REQUIRE(is_success(result));
    const auto [value, position] = get_success(result);
    CHECK((std::is_same_v<decltype(value), const std::string>));
    CHECK(position == tokens.cend());
}

TEST_CASE("attributed token parser raises error on end of input")
{
    std::vector<token> tokens;
    const auto parser = make_parser<identifier_token>();
    CHECK_THROWS(parser.parse(tokens.cbegin(), tokens.cend()));
}
