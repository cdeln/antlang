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
        const auto [expr, pos] = parser.parse(tokens.cbegin(), tokens.cend());
        CHECK(std::holds_alternative<none>(expr));
        CHECK(pos == tokens.cend());
    }

    SUBCASE("when expression is identifier")
    {
        const std::vector<token> tokens = {{identifier_token{"name"}}};
        const auto [expr, pos] = parser.parse(tokens.cbegin(), tokens.cend());
        REQUIRE(std::holds_alternative<std::string>(expr));
        CHECK(std::get<std::string>(expr) == "name");
        CHECK(pos == tokens.cend());
    }
}

TEST_CASE("alternative parser raises error on non-existing alternative")
{
    const auto parser = make_parser<alternative<left_parenthesis_token, identifier_token>>();
    const std::vector<token> tokens = {{right_parenthesis_token{}}};
    CHECK_THROWS(parser.parse(tokens.cbegin(), tokens.cend()));
}
