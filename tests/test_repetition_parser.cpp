#include <doctest/doctest.h>

#include "repetition_parser.hpp"
#include "token_rules.hpp"

#include <sstream>

using namespace ant;

TEST_CASE("repetition parser parses zero non-attributed tokens")
{
    const std::vector<token> tokens;
    const auto parser = make_parser<repetition<left_parenthesis_token>>();
    const auto result = parser.parse(tokens.cbegin(), tokens.cend());
    REQUIRE(is_success(result));
    const auto pos = get_success(result).position;
    CHECK(pos == tokens.cend());
}

TEST_CASE("repetition parser parses zero attributed tokens")
{
    const std::vector<token> tokens;
    const auto parser = make_parser<repetition<identifier_token>>();
    const auto result = parser.parse(tokens.cbegin(), tokens.cend());
    REQUIRE(is_success(result));
    const auto [values, position] = get_success(result);
    CHECK(values.size() == 0);
    CHECK(position == tokens.cend());
}

TEST_CASE("repetition parser parses multiple non-attributed tokens")
{
    std::vector<token> tokens;
    tokens.reserve(100);
    for (int i = 0; i < 100; ++i)
    {
        std::stringstream name;
        name << i;
        tokens.push_back({identifier_token{name.str()}, {}});
    }
    const auto parser = make_parser<repetition<identifier_token>>();
    const auto result = parser.parse(tokens.cbegin(), tokens.cend());
    REQUIRE(is_success(result));
    const auto [values, position] = get_success(result);
    CHECK(position == tokens.cend());
    REQUIRE(values.size() == 100);
    for (int i = 0; i < 100; ++i)
    {
        std::stringstream name;
        name << i;
        CHECK(values.at(i) == name.str());
    }
}

TEST_CASE("repetition parser parses multiple attributed tokens")
{
    std::vector<token> tokens;
    tokens.reserve(100);
    for (int i = 0; i < 100; ++i)
    {
        std::stringstream name;
        name << i;
        tokens.push_back({identifier_token{name.str()}, {}});
    }
    const auto parser = make_parser<repetition<identifier_token>>();
    const auto result = parser.parse(tokens.cbegin(), tokens.cend());
    REQUIRE(is_success(result));
    const auto [values, position] = get_success(result);
    REQUIRE(values.size() == 100);
    CHECK(position == tokens.cend());
    for (int i = 0; i < 100; ++i)
    {
        std::stringstream name;
        name << i;
        CHECK(values.at(i) == name.str());
    }
}
