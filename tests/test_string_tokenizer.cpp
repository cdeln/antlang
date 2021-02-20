#include <doctest/doctest.h>

#include "string_tokenizer.hpp"

using namespace ant;

TEST_CASE("string_tokenizer_tokenizes_simple_string_with_all_tokens")
{
    constexpr auto string = "() function structure i337 1337 13.37";
    const string_tokenizer tokenizer;
    const auto tokens = tokenizer.tokenize(string);

    REQUIRE(tokens.size() == 7);

    CHECK(std::holds_alternative<left_parenthesis_token>(tokens.at(0).variant));
    CHECK(std::holds_alternative<right_parenthesis_token>(tokens.at(1).variant));
    CHECK(std::holds_alternative<function_token>(tokens.at(2).variant));
    CHECK(std::holds_alternative<structure_token>(tokens.at(3).variant));

    CHECK(std::holds_alternative<identifier_token>(tokens.at(4).variant));
    CHECK(std::get<identifier_token>(tokens.at(4).variant).value == "i337");

    REQUIRE(std::holds_alternative<integer_literal_token>(tokens.at(5).variant));
    CHECK(std::get<integer_literal_token>(tokens.at(5).variant).value == "1337");

    REQUIRE(std::holds_alternative<floating_point_literal_token>(tokens.at(6).variant));
    CHECK(std::get<floating_point_literal_token>(tokens.at(6).variant).value == "13.37");
}
