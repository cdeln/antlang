#include <doctest/doctest.h>

#include "ast_rules.hpp"
#include "ast_parser.hpp"

using namespace ant;

TEST_CASE("can parse parameter")
{
    const std::vector<token> tokens =
      { {identifier_token{"type"}}
      , {identifier_token{"name"}}
      };
    const auto parser = make_parser<ast::parameter>();
    const auto [parameter, pos] = parser.parse(tokens.cbegin(), tokens.cend());
    CHECK(parameter.type == "type");
    CHECK(parameter.name == "name");
    CHECK(pos == tokens.end());
}

TEST_CASE("can parse function")
{
    const std::vector<token> tokens =
      { {left_parenthesis_token{}}
      , {identifier_token{"function-name"}}
      , {identifier_token{"return-type"}}
      , {left_parenthesis_token{}}
      , {identifier_token{"parameter-type"}}
      , {identifier_token{"parameter-name"}}
      , {right_parenthesis_token{}}
      , {right_parenthesis_token{}}
      };
    const auto parser = make_parser<ast::function>();
    const auto [function, pos] = parser.parse(tokens.cbegin(), tokens.cend());
    CHECK(function.name == "function-name");
    CHECK(function.return_type == "return-type");
    REQUIRE(function.parameters.size() == 1);
    CHECK(function.parameters.at(0).type == "parameter-type");
    CHECK(function.parameters.at(0).name == "parameter-name");
    CHECK(pos == tokens.end());
}

TEST_CASE("can parse structure")
{
    const std::vector<token> tokens =
      { {left_parenthesis_token{}}
      , {identifier_token{"structure-name"}}
      , {left_parenthesis_token{}}
      , {identifier_token{"field-type"}}
      , {identifier_token{"field-name"}}
      , {right_parenthesis_token{}}
      , {right_parenthesis_token{}}
      };
    const auto parser = make_parser<ast::structure>();
    const auto [value, pos] = parser.parse(tokens.cbegin(), tokens.cend());
    CHECK(value.name == "structure-name");
    REQUIRE(value.fields.size() == 1);
    CHECK(value.fields.at(0).type == "field-type");
    CHECK(value.fields.at(0).name == "field-name");
    CHECK(pos == tokens.end());
}

TEST_CASE("can parse identifier expression")
{
    const std::vector<token> tokens = { {identifier_token{"name"}} };
    const auto parser = make_parser<ast::expression>();
    const auto [expr, pos] = parser.parse(tokens.cbegin(), tokens.cend());
    REQUIRE(std::holds_alternative<std::string>(expr));
    CHECK(std::get<std::string>(expr) == "name");
    CHECK(pos == tokens.end());
}

TEST_CASE("can parse simple evaluation")
{
    const std::vector<token> tokens = {
        {left_parenthesis_token{}},
        {identifier_token{"func"}},
        {identifier_token{"arg1"}},
        {identifier_token{"arg2"}},
        {right_parenthesis_token{}}
    };
    const auto parser = make_parser<ast::evaluation>();
    const auto [eval, pos] = parser.parse(tokens.cbegin(), tokens.cend());
    CHECK(pos == tokens.end());
    CHECK(eval.function == "func");
    CHECK(eval.arguments.size() == 2);
    REQUIRE(std::holds_alternative<std::string>(eval.arguments.at(0)));
    CHECK(std::get<std::string>(eval.arguments.at(0)) == "arg1");
    REQUIRE(std::holds_alternative<std::string>(eval.arguments.at(1)));
    CHECK(std::get<std::string>(eval.arguments.at(0)) == "arg1");
}

TEST_CASE("can parse evaluation expression")
{
    const std::vector<token> tokens = {
        {left_parenthesis_token{}},
        {identifier_token{"func"}},
        {identifier_token{"arg1"}},
        {identifier_token{"arg2"}},
        {right_parenthesis_token{}}
    };
    const auto parser = make_parser<ast::expression>();
    const auto [expr, pos] = parser.parse(tokens.cbegin(), tokens.cend());
    CHECK(pos == tokens.end());
    REQUIRE(std::holds_alternative<ast::evaluation>(expr));
    const auto eval = std::get<ast::evaluation>(expr);
    CHECK(eval.function == "func");
    CHECK(eval.arguments.size() == 2);
    REQUIRE(std::holds_alternative<std::string>(eval.arguments.at(0)));
    CHECK(std::get<std::string>(eval.arguments.at(0)) == "arg1");
    REQUIRE(std::holds_alternative<std::string>(eval.arguments.at(1)));
    CHECK(std::get<std::string>(eval.arguments.at(0)) == "arg1");
}

TEST_CASE("can parse complex evaluation")
{
    const std::vector<token> tokens = {
        {left_parenthesis_token{}},
        {identifier_token{"outer-func"}},
        {left_parenthesis_token{}},
        {identifier_token{"inner-func-1"}},
        {identifier_token{"arg1"}},
        {right_parenthesis_token{}},
        {left_parenthesis_token{}},
        {identifier_token{"inner-func-2"}},
        {identifier_token{"arg2"}},
        {right_parenthesis_token{}},
        {right_parenthesis_token{}}
    };
    const auto parser = make_parser<ast::evaluation>();
    const auto [eval, pos] = parser.parse(tokens.cbegin(), tokens.cend());

    CHECK(pos == tokens.end());
    CHECK(eval.function == "outer-func");
    CHECK(eval.arguments.size() == 2);

    REQUIRE(std::holds_alternative<ast::evaluation>(eval.arguments.at(0)));
    const auto inner_eval_1 = std::get<ast::evaluation>(eval.arguments.at(0));
    CHECK(inner_eval_1.function == "inner-func-1");
    REQUIRE(inner_eval_1.arguments.size() == 1);
    REQUIRE(std::holds_alternative<std::string>(inner_eval_1.arguments.at(0)));
    CHECK(std::get<std::string>(inner_eval_1.arguments.at(0)) == "arg1");

    REQUIRE(std::holds_alternative<ast::evaluation>(eval.arguments.at(1)));
    const auto inner_eval_2 = std::get<ast::evaluation>(eval.arguments.at(1));
    CHECK(inner_eval_2.function == "inner-func-2");
    REQUIRE(inner_eval_2.arguments.size() == 1);
    REQUIRE(std::holds_alternative<std::string>(inner_eval_2.arguments.at(0)));
    CHECK(std::get<std::string>(inner_eval_2.arguments.at(0)) == "arg2");
}

TEST_CASE("can parse 32 bit integer literal")
{
    const std::vector<token> tokens = {
        {left_parenthesis_token{}},
        {identifier_token{"i32"}},
        {integer_literal_token{"1337"}},
        {right_parenthesis_token{}}
    };
    const auto parser = make_parser<ast::i32>();
    const auto [literal, pos] = parser.parse(tokens.cbegin(), tokens.cend());
    CHECK(pos == tokens.cend());
    CHECK(literal.value == 1337);
}

TEST_CASE("parsing floating point literal as integer raises error")
{
    const std::vector<token> tokens = {
        {left_parenthesis_token{}},
        {identifier_token{"i32"}},
        {floating_point_literal_token{"13.37"}},
        {right_parenthesis_token{}}
    };
    const auto parser = make_parser<ast::i32>();
    CHECK_THROWS(parser.parse(tokens.cbegin(), tokens.cend()));
}

TEST_CASE("parsing narrowing integer conversion raises error")
{
    const std::vector<token> tokens = {
        {left_parenthesis_token{}},
        {identifier_token{"u8"}},
        {integer_literal_token{"256"}},
        {right_parenthesis_token{}}
    };
    const auto parser = make_parser<ast::u8>();
    CHECK_THROWS(parser.parse(tokens.cbegin(), tokens.cend()));
}


TEST_CASE("can parse 32 bit floating point literal")
{
    const std::vector<token> tokens = {
        {left_parenthesis_token{}},
        {identifier_token{"f32"}},
        {integer_literal_token{"13.37"}},
        {right_parenthesis_token{}}
    };
    const auto parser = make_parser<ast::f32>();
    const auto [literal, pos] = parser.parse(tokens.cbegin(), tokens.cend());
    CHECK(pos == tokens.cend());
    CHECK(literal.value == doctest::Approx(13.37f));
}

TEST_CASE("can parase an integer literal as floating point")
{
    const std::vector<token> tokens = {
        {left_parenthesis_token{}},
        {identifier_token{"f32"}},
        {integer_literal_token{"1337"}},
        {right_parenthesis_token{}}
    };
    const auto parser = make_parser<ast::f32>();
    const auto [literal, pos] = parser.parse(tokens.cbegin(), tokens.cend());
    CHECK(pos == tokens.cend());
    CHECK(literal.value == doctest::Approx(1337.0f));
}
