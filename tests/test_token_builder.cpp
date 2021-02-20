#include <doctest/doctest.h>

#include "token_builder.hpp"

using namespace ant;

TEST_CASE("token_builder_builds_left_parenthesis")
{
    token_alternative_builder<left_parenthesis_token> builder;
    auto token = builder.build("(");
    CHECK(std::holds_alternative<left_parenthesis_token>(token));
}

TEST_CASE("token_builder_builds_right_parenthesis")
{
    token_alternative_builder<right_parenthesis_token> builder;
    auto token = builder.build(")");
    CHECK(std::holds_alternative<right_parenthesis_token>(token));
}

TEST_CASE("token_builder_builds_function")
{
    token_alternative_builder<function_token> builder;
    auto token = builder.build("function");
    CHECK(std::holds_alternative<function_token>(token));
}

TEST_CASE("token_builder_builds_structure")
{
    token_alternative_builder<structure_token> builder;
    auto token = builder.build("structure");
    CHECK(std::holds_alternative<structure_token>(token));
}

TEST_CASE("token_builder_builds_identifier")
{
    token_alternative_builder<identifier_token> builder;
    auto string = "abcdefghijklmnopqrstuvxz-ABCDEFGHIJKLMNOPQRSTUVXYZ_!@#$%^&*+!9+=~_0123456789";
    auto token = builder.build(string);
    CHECK(std::holds_alternative<identifier_token>(token));
    auto identifier = std::get<identifier_token>(token);
    CHECK(identifier.value == string);
}

TEST_CASE("token_builder_builds_integer_literal")
{
    token_alternative_builder<integer_literal_token> builder;
    auto string = "0123456789";
    auto token = builder.build(string);
    CHECK(std::holds_alternative<integer_literal_token>(token));
    auto identifier = std::get<integer_literal_token>(token);
    CHECK(identifier.value == string);
}

TEST_CASE("token_builder_builds_floating_point_literal")
{
    token_alternative_builder<floating_point_literal_token> builder;
    auto string = "01234.56789";
    auto token = builder.build(string);
    CHECK(std::holds_alternative<floating_point_literal_token>(token));
    auto identifier = std::get<floating_point_literal_token>(token);
    CHECK(identifier.value == string);
}
