#include <doctest/doctest.h>

#include "token_factory.hpp"

using namespace ant;

TEST_CASE("token factory creates one token")
{
    auto factory = token_factory_builder<left_parenthesis_token>::make();
    auto token = factory.create(0, "(");
    CHECK(holds<left_parenthesis_token>(token));
}

TEST_CASE("token factory creates two tokens")
{
    auto factory =
        token_factory_builder<
            left_parenthesis_token,
            right_parenthesis_token
        >::make();
    auto left_parenthesis = factory.create(0, "(");
    CHECK(holds<left_parenthesis_token>(left_parenthesis));
    auto right_parenthesis = factory.create(1, ")");
    CHECK(holds<right_parenthesis_token>(right_parenthesis));
}

TEST_CASE("token factory creates all tokens")
{
    auto factory = token_factory_builder<token_variant>::make();
    auto left_parenthesis = factory.create(0, "(");
    REQUIRE(holds<left_parenthesis_token>(left_parenthesis));
    auto right_parenthesis = factory.create(1, ")");
    REQUIRE(holds<right_parenthesis_token>(right_parenthesis));
    auto function = factory.create(2, "function");
    REQUIRE(holds<function_token>(function));
    auto structure = factory.create(3, "structure");
    REQUIRE(holds<structure_token>(structure));
    auto condition = factory.create(4, "condition");
    REQUIRE(holds<condition_token>(condition));
    auto floating = factory.create(5, "01234.56789");
    REQUIRE(holds<floating_point_literal_token>(floating));
    auto integer = factory.create(6, "0123456789");
    REQUIRE(holds<integer_literal_token>(integer));
    auto boolean = factory.create(7, "true");
    REQUIRE(holds<boolean_literal_token>(boolean));
    auto identifier = factory.create(
        8, "abcdefghijklmnopqrstuvxz-ABCDEFGHIJKLMNOPQRSTUVXYZ_!@#$%^&*+!9+=~_0123456789");
    REQUIRE(holds<identifier_token>(identifier));
}
