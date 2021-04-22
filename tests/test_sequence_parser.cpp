#include <doctest/doctest.h>

#include "sequence_parser.hpp"
#include "token_rules.hpp"

using namespace ant;

TEST_CASE("sequence parser parses an empty sequence")
{
    const std::vector<token> tokens;
    const auto parser = make_parser<sequence<>>();
    const auto result = parser.parse(tokens.cbegin(), tokens.cend());
    REQUIRE(is_success(result));
    const auto pos = get_success(result).position;
    CHECK(pos == tokens.cend());
}

TEST_CASE("sequence parser parses a sequence with one non-attributed token")
{
    const std::vector<token> tokens = { {left_parenthesis_token{}} };
    const auto parser = make_parser<sequence<left_parenthesis_token>>();
    const auto result = parser.parse(tokens.cbegin(), tokens.cend());
    REQUIRE(is_success(result));
    const auto pos = get_success(result).position;
    CHECK(pos == tokens.cend());
}

TEST_CASE("sequence parser parses a sequence with one attributed token")
{
    const std::vector<token> tokens = { {identifier_token{"test"}} };
    const auto parser = make_parser<sequence<identifier_token>>();
    const auto result = parser.parse(tokens.cbegin(), tokens.cend());
    REQUIRE(is_success(result));
    const auto [value, pos] = get_success(result);
    CHECK(pos == tokens.cend());
    CHECK(std::get<std::string>(value) == "test");
}

TEST_CASE("sequence parser throws exception on end of input for non-attributed token")
{
    const std::vector<token> tokens;
    const auto parser = make_parser<sequence<left_parenthesis_token>>();
    CHECK_THROWS(parser.parse(tokens.cbegin(), tokens.cend()));
}

TEST_CASE("sequence parser throws exception on end of input for attributed token")
{
    const std::vector<token> tokens;
    const auto parser = make_parser<sequence<identifier_token>>();
    CHECK_THROWS(parser.parse(tokens.cbegin(), tokens.cend()));
}

TEST_CASE("sequence parser parses a sequence of non-attributed tokens")
{
    const auto parser =
        make_parser<
            sequence<
                left_parenthesis_token,
                right_parenthesis_token
            >
        >();
    const std::vector<token> tokens = {
        {left_parenthesis_token{}},
        {right_parenthesis_token{}}
    };
    const auto result = parser.parse(tokens.cbegin(), tokens.cend());
    REQUIRE(is_success(result));
    const auto pos = get_success(result).position;
    CHECK(pos == tokens.cend());
}

TEST_CASE("sequence parser parses a sequence of attributed tokens")
{
    const auto parser =
        make_parser<
            sequence<
                identifier_token,
                integer_literal_token
            >
        >();
    const std::vector<token> tokens = {
        {identifier_token{"test"}},
        {integer_literal_token{"1337"}}
    };
    const auto result = parser.parse(tokens.cbegin(), tokens.cend());
    REQUIRE(is_success(result));
    const auto [values, pos] = get_success(result);
    CHECK(pos == tokens.cend());
    static_assert(std::tuple_size_v<decltype(values)> == 2);
    CHECK(std::get<0>(values) == "test");
    CHECK(std::get<1>(values) == "1337");
}

TEST_CASE("sequence parser parses a sequence of mixed non-attributed and attributed tokens")
{
    const auto parser =
        make_parser<
            sequence<
                left_parenthesis_token,
                identifier_token,
                integer_literal_token,
                right_parenthesis_token
            >
        >();
    const std::vector<token> tokens = {
        {left_parenthesis_token{}},
        {identifier_token{"test"}},
        {integer_literal_token{"1337"}},
        {right_parenthesis_token{}}
    };
    const auto result = parser.parse(tokens.cbegin(), tokens.cend());
    REQUIRE(is_success(result));
    const auto [values, pos] = get_success(result);
    CHECK(pos == tokens.cend());
    static_assert(std::tuple_size_v<decltype(values)> == 2);
    CHECK(std::get<0>(values) == "test");
    CHECK(std::get<1>(values) == "1337");
}

TEST_CASE("sequence parser returns failure with iterator to position of failing token")
{
    const auto parser =
        make_parser<
            sequence<
                left_parenthesis_token,
                identifier_token,
                right_parenthesis_token
            >
        >();
    const std::vector<token> tokens = {
        {left_parenthesis_token{}},
        {integer_literal_token{"1337"}},
        {right_parenthesis_token{}}
    };
    const auto result = parser.parse(tokens.cbegin(), tokens.cend());
    REQUIRE(is_failure(result));
    const auto& failure = get_failure(result);
    CHECK(std::distance(tokens.cbegin(), failure.position) == 1);
}
