#include <doctest/doctest.h>

#include "repetition_parser.hpp"

#include <sstream>

using namespace ant;

TEST_CASE("repetition parser parses zero non-attributed tokens")
{
    std::vector<token> tokens;
    parser<repetition<left_parenthesis_token>> parser;
    auto position = parser.parse(tokens.cbegin(), tokens.cend()).position;
    CHECK(position == tokens.cend());
}

TEST_CASE("repetition parser parses zero attributed tokens")
{
    std::vector<token> tokens;
    parser<repetition<identifier_token>> parser;
    auto [values, position] = parser.parse(tokens.cbegin(), tokens.cend());
    CHECK(position == tokens.cend());
    CHECK(values.size() == 0);
}

TEST_CASE("repetition parser parses multiple non-attributed tokens")
{
    parser<repetition<identifier_token>> parser;
    std::vector<token> tokens;
    tokens.reserve(100);
    for (int i = 0; i < 100; ++i)
    {
        std::stringstream name;
        name << i;
        tokens.push_back({identifier_token{name.str()}, {}});
    }
    auto [values, position] = parser.parse(tokens.cbegin(), tokens.cend());
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
    parser<repetition<identifier_token>> parser;
    std::vector<token> tokens;
    tokens.reserve(100);
    for (int i = 0; i < 100; ++i)
    {
        std::stringstream name;
        name << i;
        tokens.push_back({identifier_token{name.str()}, {}});
    }
    auto [values, position] = parser.parse(tokens.cbegin(), tokens.cend());
    CHECK(position == tokens.cend());
    REQUIRE(values.size() == 100);
    for (int i = 0; i < 100; ++i)
    {
        std::stringstream name;
        name << i;
        CHECK(values.at(i) == name.str());
    }
}
