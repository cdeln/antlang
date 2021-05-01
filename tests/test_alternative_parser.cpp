#include <doctest/doctest.h>

#include "alternative_parser.hpp"
#include "token_rules.hpp"

using namespace ant;

TEST_CASE("can parse alternative expression")
{
    const auto parser =
        make_parser<
            alternative<
                left_parenthesis_token,
                identifier_token
            >
        >();

    SUBCASE("when expression is left parenthesis")
    {
        const std::vector<token> tokens = {{left_parenthesis_token{}}};
        const auto result = parser.parse(tokens.cbegin(), tokens.cend());
        REQUIRE(is_success(result));
        const auto [value, pos] = get_success(result);
        CHECK(holds<none>(value));
        CHECK(pos == tokens.cend());
    }

    SUBCASE("when expression is identifier")
    {
        const std::vector<token> tokens = {{identifier_token{"name"}}};
        const auto result = parser.parse(tokens.cbegin(), tokens.cend());
        REQUIRE(is_success(result));
        const auto [value, pos] = get_success(result);
        REQUIRE(holds<std::string>(value));
        CHECK(get<std::string>(value) == "name");
        CHECK(pos == tokens.cend());
    }
}

TEST_CASE("alternative parser with prefix free alternatives returns failure "
          "with iterator to position of first failing token")
{
    const auto parser =
        make_parser<
            alternative<
                left_parenthesis_token,
                identifier_token
            >
        >();
    const std::vector<token> tokens = {
        {right_parenthesis_token{}}
    };
    const auto result = parser.parse(tokens.cbegin(), tokens.cend());
    CHECK(is_failure(result));
    const auto& failure = get_failure(result);
    CHECK(failure.position == tokens.cbegin());
}

TEST_CASE("alternative parser with shared-prefix alternatives returns failure "
          "with iterator to position of start of alternative when distance is less or equal to lcp")
{
    const auto parser =
        make_parser<
            alternative<
                sequence<
                    left_parenthesis_token,
                    identifier_token
                >,
                sequence<
                    left_parenthesis_token,
                    integer_literal_token
                >
            >
        >();
    const std::vector<token> tokens = {
        {left_parenthesis_token{}},
        {right_parenthesis_token{}}
    };
    const auto result = parser.parse(tokens.cbegin(), tokens.cend());
    CHECK(is_failure(result));
    const auto& failure = get_failure(result);
    CHECK(std::distance(tokens.cbegin(), failure.position) == 0);
}

TEST_CASE("alternative parser with shared-prefix alternatives returns failure "
          "with iterator to position of first failing token when distance exceed lcp")
{
    const auto parser =
        make_parser<
            alternative<
                sequence<
                    left_parenthesis_token,
                    identifier_token
                >,
                sequence<
                    left_parenthesis_token,
                    integer_literal_token,
                    right_parenthesis_token
                >
            >
        >();
    const std::vector<token> tokens = {
        {left_parenthesis_token{}},
        {integer_literal_token{}},
        {identifier_token{}}
    };
    const auto result = parser.parse(tokens.cbegin(), tokens.cend());
    CHECK(is_failure(result));
    const auto& failure = get_failure(result);
    CHECK(std::distance(tokens.cbegin(), failure.position) == 2);
}
