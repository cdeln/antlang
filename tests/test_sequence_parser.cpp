#include <doctest/doctest.h>

#include "sequence_parser.hpp"
#include "token_rules.hpp"

using namespace ant;

TEST_CASE("sequence parser parses an empty sequence")
{
    parser<sequence<>> parser;
    std::vector<token> tokens;
    auto position = parser.parse(tokens.cbegin(), tokens.cend()).position;
    CHECK(position == tokens.cend());
}

TEST_CASE("sequence parser parses a sequence with one non-attributed token")
{
    parser<sequence<left_parenthesis_token>> parser;
    std::vector<token> tokens;
    tokens.push_back({left_parenthesis_token{}});
    auto position = parser.parse(tokens.cbegin(), tokens.cend()).position;
    CHECK(position == tokens.cend());
}

TEST_CASE("sequence parser parses a sequence with one attributed token")
{
    parser<sequence<identifier_token>> parser;
    std::vector<token> tokens;
    tokens.push_back({identifier_token{"test"}});
    auto [value, position] = parser.parse(tokens.cbegin(), tokens.cend());
    CHECK(std::is_same_v<decltype(value), std::tuple<std::string>>);
    CHECK(position == tokens.cend());
    CHECK(std::get<std::string>(value) == "test");
}

TEST_CASE("sequence parser throws exception on end of input for non-attributed token")
{
    parser<sequence<left_parenthesis_token>> parser;
    std::vector<token> tokens;
    CHECK_THROWS(parser.parse(tokens.cbegin(), tokens.cend()));
}

TEST_CASE("sequence parser throws exception on end of input for attributed token")
{
    parser<sequence<identifier_token>> parser;
    std::vector<token> tokens;
    CHECK_THROWS(parser.parse(tokens.cbegin(), tokens.cend()));
}

TEST_CASE("sequence parser parses a sequence of non-attributed tokens")
{
    parser<
        sequence
          < left_parenthesis_token
          , right_parenthesis_token
          >
    > parser;
    std::vector<token> tokens;
    tokens.push_back({left_parenthesis_token{}});
    tokens.push_back({right_parenthesis_token{}});
    auto position = parser.parse(tokens.cbegin(), tokens.cend()).position;
    CHECK(position == tokens.cend());
}

TEST_CASE("sequence parser parses a sequence of attributed tokens")
{
    parser<
        sequence
          < identifier_token
          , integer_literal_token
          >
    > parser;
    std::vector<token> tokens;
    tokens.push_back({identifier_token{"test"}});
    tokens.push_back({integer_literal_token{"1337"}});
    auto [values, position] = parser.parse(tokens.cbegin(), tokens.cend());
    CHECK(position == tokens.cend());
    static_assert(std::tuple_size_v<decltype(values)> == 2);
    CHECK(std::get<0>(values) == "test");
    CHECK(std::get<1>(values) == "1337");
}

TEST_CASE("sequence parser parses a sequence of mixed non-attributed and attributed tokens")
{
    parser<
        sequence
          < left_parenthesis_token
          , identifier_token
          , integer_literal_token
          , right_parenthesis_token
          >
    > parser;
    std::vector<token> tokens;
    tokens.push_back({left_parenthesis_token{}});
    tokens.push_back({identifier_token{"test"}});
    tokens.push_back({integer_literal_token{"1337"}});
    tokens.push_back({right_parenthesis_token{}});
    auto [values, position] = parser.parse(tokens.cbegin(), tokens.cend());
    CHECK(position == tokens.cend());
    static_assert(std::tuple_size_v<decltype(values)> == 2);
    CHECK(std::get<0>(values) == "test");
    CHECK(std::get<1>(values) == "1337");
}
