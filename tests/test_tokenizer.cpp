#include <doctest/doctest.h>

#include "tokenizer.hpp"

using namespace ant;

TEST_CASE("tokenizer tokenizes simple string with some tokens")
{
    constexpr auto string = "() function structure i337 1337 +1337 -1337 13.37 +13.37 -13.37";
    const tokenizer tokenizer;
    const auto tokens = tokenizer.tokenize(string);

    REQUIRE(tokens.size() == 11);

    CHECK(holds<left_parenthesis_token>(tokens.at(0).variant));
    CHECK(holds<right_parenthesis_token>(tokens.at(1).variant));
    CHECK(holds<function_token>(tokens.at(2).variant));
    CHECK(holds<structure_token>(tokens.at(3).variant));

    CHECK(holds<identifier_token>(tokens.at(4).variant));
    CHECK(get<identifier_token>(tokens.at(4).variant).value == "i337");

    REQUIRE(holds<integer_literal_token>(tokens.at(5).variant));
    CHECK(get<integer_literal_token>(tokens.at(5).variant).value == "1337");

    REQUIRE(holds<integer_literal_token>(tokens.at(6).variant));
    CHECK(get<integer_literal_token>(tokens.at(6).variant).value == "+1337");

    REQUIRE(holds<integer_literal_token>(tokens.at(7).variant));
    CHECK(get<integer_literal_token>(tokens.at(7).variant).value == "-1337");

    REQUIRE(holds<floating_point_literal_token>(tokens.at(8).variant));
    CHECK(get<floating_point_literal_token>(tokens.at(8).variant).value == "13.37");

    REQUIRE(holds<floating_point_literal_token>(tokens.at(9).variant));
    CHECK(get<floating_point_literal_token>(tokens.at(9).variant).value == "+13.37");

    REQUIRE(holds<floating_point_literal_token>(tokens.at(10).variant));
    CHECK(get<floating_point_literal_token>(tokens.at(10).variant).value == "-13.37");
}
