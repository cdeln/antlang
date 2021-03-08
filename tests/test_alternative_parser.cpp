#include <doctest/doctest.h>

#include "alternative_parser.hpp"
#include "token_rules.hpp"

using namespace ant;

TEST_CASE("can parse alternative expression")
{
    const auto parser = make_parser<alternative<left_parenthesis_token, identifier_token>>();

    SUBCASE("when expression is left parenthesis")
    {
        const std::vector<token> tokens = {{left_parenthesis_token{}}};
        const auto result = parser.parse(tokens.cbegin(), tokens.cend());
        REQUIRE(is_success(result));
        const auto [value, pos] = get_success(result);
        CHECK(std::holds_alternative<none>(value));
        CHECK(pos == tokens.cend());
    }

    SUBCASE("when expression is identifier")
    {
        const std::vector<token> tokens = {{identifier_token{"name"}}};
        const auto result = parser.parse(tokens.cbegin(), tokens.cend());
        REQUIRE(is_success(result));
        const auto [value, pos] = get_success(result);
        REQUIRE(std::holds_alternative<std::string>(value));
        CHECK(std::get<std::string>(value) == "name");
        CHECK(pos == tokens.cend());
    }
}

TEST_CASE("alternative parser raises error on non-existing alternative")
{
    const auto parser = make_parser<alternative<left_parenthesis_token, identifier_token>>();
    const std::vector<token> tokens = {{right_parenthesis_token{}}};
    CHECK(is_failure(parser.parse(tokens.cbegin(), tokens.cend())));
}
