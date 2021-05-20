#include <doctest/doctest.h>

#include "token_builder.hpp"

using namespace ant;

TEST_CASE("token builder builds left_parenthesis")
{
    token_alternative_builder<left_parenthesis_token> builder;
    auto token = builder.build("(");
    CHECK(holds<left_parenthesis_token>(token));
}

TEST_CASE("token builder builds right_parenthesis")
{
    token_alternative_builder<right_parenthesis_token> builder;
    auto token = builder.build(")");
    CHECK(holds<right_parenthesis_token>(token));
}

TEST_CASE("token builder builds left_parenthesis")
{
    token_alternative_builder<left_bracket_token> builder;
    auto token = builder.build("[");
    CHECK(holds<left_bracket_token>(token));
}

TEST_CASE("token builder builds right_parenthesis")
{
    token_alternative_builder<right_bracket_token> builder;
    auto token = builder.build("]");
    CHECK(holds<right_bracket_token>(token));
}

TEST_CASE("token builder builds function")
{
    token_alternative_builder<function_token> builder;
    auto token = builder.build("function");
    CHECK(holds<function_token>(token));
}

TEST_CASE("token builder builds structure")
{
    token_alternative_builder<structure_token> builder;
    auto token = builder.build("structure");
    CHECK(holds<structure_token>(token));
}

TEST_CASE("token builder builds condition")
{
    token_alternative_builder<condition_token> builder;
    auto token = builder.build("condition");
    CHECK(holds<condition_token>(token));
}

TEST_CASE("token builder builds identifier")
{
    token_alternative_builder<identifier_token> builder;
    auto string = "abcdefghijklmnopqrstuvxz-ABCDEFGHIJKLMNOPQRSTUVXYZ_!@#$%^&*+!9+=~_0123456789";
    auto token = builder.build(string);
    CHECK(holds<identifier_token>(token));
    auto identifier = get<identifier_token>(token);
    CHECK(identifier.value == string);
}

TEST_CASE("token builder builds boolean literal for true")
{
    token_alternative_builder<boolean_literal_token> builder;
    auto token = builder.build("true");
    CHECK(holds<boolean_literal_token>(token));
    auto true_token = get<boolean_literal_token>(token);
    CHECK(true_token.value == "true");
}

TEST_CASE("token builder builds boolean literal for false")
{
    token_alternative_builder<boolean_literal_token> builder;
    auto token = builder.build("false");
    CHECK(holds<boolean_literal_token>(token));
    auto false_token = get<boolean_literal_token>(token);
    CHECK(false_token.value == "false");
}

TEST_CASE("token builder builds integer literal")
{
    token_alternative_builder<integer_literal_token> builder;
    auto string = "0123456789";
    auto token = builder.build(string);
    CHECK(holds<integer_literal_token>(token));
    auto identifier = get<integer_literal_token>(token);
    CHECK(identifier.value == string);
}

TEST_CASE("token builder builds floating point literal")
{
    token_alternative_builder<floating_point_literal_token> builder;
    auto string = "01234.56789";
    auto token = builder.build(string);
    CHECK(holds<floating_point_literal_token>(token));
    auto identifier = get<floating_point_literal_token>(token);
    CHECK(identifier.value == string);
}
