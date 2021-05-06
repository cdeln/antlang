#include <doctest/doctest.h>

#include "repetition_parser.hpp"
#include "token_rules.hpp"

#include <sstream>

using namespace ant;

TEST_CASE("repetition parser parses zero non-attributed tokens plus end rule")
{
    const std::vector<token> tokens = {{right_parenthesis_token{}}};
    const auto parser = make_parser<repetition<left_parenthesis_token>>();
    const auto result = parser.parse(tokens.cbegin(), tokens.cend());
    REQUIRE(is_success(result));
    const auto pos = get_success(result).position;
    CHECK(pos == tokens.cend());
}

TEST_CASE("repetition parser parses zero attributed tokens plus end rule")
{
    const std::vector<token> tokens = {{right_parenthesis_token{}}};
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
    tokens.push_back({right_parenthesis_token{}});
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
    tokens.push_back({right_parenthesis_token{}});
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

TEST_CASE("repetition parser parses attributed end token")
{
    const auto parser =
        make_parser<
            repetition<
                identifier_token,
                integer_literal_token
            >
        >();
    const std::vector<token> tokens =
    {
        {identifier_token{"first"}, {}},
        {identifier_token{"second"}, {}},
        {identifier_token{"third"}, {}},
        {integer_literal_token{"1337"}, {}}
    };
    const auto result = parser.parse(tokens.cbegin(), tokens.cend());
    REQUIRE(is_success(result));
    const auto& [value, position] = get_success(result);
    REQUIRE(position == tokens.cend());
    const auto& [values, last] = value;
    REQUIRE(values.size() == 3);
    CHECK(values.at(0) == "first");
    CHECK(values.at(1) == "second");
    CHECK(values.at(2) == "third");
    CHECK(last == "1337");
}

TEST_CASE("repetition parser returns failure if terminal pattern is not found before end of input")
{
    const auto parser =
        make_parser<
            repetition<
                left_parenthesis_token,
                right_parenthesis_token
            >
        >();
    const std::vector<token> tokens =
    {
        {left_parenthesis_token{}}
    };
    const auto result = parser.parse(tokens.cbegin(), tokens.cend());
    REQUIRE(is_failure(result));
}

TEST_CASE("repetition parser returns failure with iterator to position of failing token")
{
    const auto parser =
        make_parser<
            repetition<
                identifier_token,
                right_parenthesis_token
            >
        >();
    const std::vector<token> tokens = {
        {identifier_token{"a"}},
        {identifier_token{"b"}},
        {identifier_token{"c"}},
        {integer_literal_token{"1337"}},
        {right_parenthesis_token{}}
    };
    const auto result = parser.parse(tokens.cbegin(), tokens.cend());
    REQUIRE(is_failure(result));
    const auto& failure = get_failure(result);
    CHECK(std::distance(tokens.cbegin(), failure.position) == 3);
}
