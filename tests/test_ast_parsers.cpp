#include <doctest/doctest.h>

#include "ast_rules.hpp"
#include "ast_parser.hpp"

#include <iostream>

using namespace ant;

TEST_CASE("can parse parameter")
{
    const std::vector<token> tokens =
    {
        {identifier_token{"type"}},
        {identifier_token{"name"}}
    };
    const auto parser = make_parser<ast::parameter>();
    const auto result = parser.parse(tokens.cbegin(), tokens.cend());
    REQUIRE(is_success(result));
    const auto [parameter, pos] = get_success(result);
    CHECK(parameter.type == "type");
    CHECK(parameter.name == "name");
    CHECK(pos == tokens.end());
}

TEST_CASE("can parse function")
{
    const std::vector<token> tokens =
    {
        {left_parenthesis_token{}},
            {function_token{}},
            {identifier_token{"function-name"}},
            {identifier_token{"return-type"}},

            {left_parenthesis_token{}},
            {identifier_token{"parameter-type"}},
            {identifier_token{"parameter-name"}},
            {right_parenthesis_token{}},

            {identifier_token{"parameter-name"}},
        {right_parenthesis_token{}}
    };
    const auto parser = make_parser<ast::function>();
    const auto result = parser.parse(tokens.cbegin(), tokens.cend());
    if (!is_success(result))
    {
        FAIL(get_failure(result).message);
    }
    REQUIRE(is_success(result));
    const auto [function, pos] = get_success(result);
    CHECK(function.name == "function-name");
    CHECK(function.return_type.name == "return-type");
    REQUIRE(function.parameters.size() == 1);
    CHECK(function.parameters.at(0).type == "parameter-type");
    CHECK(function.parameters.at(0).name == "parameter-name");
    CHECK(pos == tokens.end());
}

TEST_CASE("can parse structure")
{
    const std::vector<token> tokens =
    {
        {left_parenthesis_token{}},
        {structure_token{}},
        {identifier_token{"structure-name"}},
        {identifier_token{"field-type"}},
        {identifier_token{"field-name"}},
        {right_parenthesis_token{}}
    };
    const auto parser = make_parser<ast::structure>();
    const auto result = parser.parse(tokens.cbegin(), tokens.cend());
    REQUIRE(is_success(result));
    const auto [value, pos] = get_success(result);
    CHECK(value.name == "structure-name");
    REQUIRE(value.fields.size() == 1);
    CHECK(value.fields.at(0).type == "field-type");
    CHECK(value.fields.at(0).name == "field-name");
    CHECK(pos == tokens.end());
}

TEST_CASE("can parse branch")
{
    const std::vector<token> tokens =
    {
        {left_bracket_token{}},
        {identifier_token{"boolean-condition"}},
        {identifier_token{"reference-name"}},
        {right_bracket_token{}}
    };
    const auto parser = make_parser<ast::branch>();
    const auto result = parser.parse(tokens.cbegin(), tokens.cend());
    REQUIRE(is_success(result));
}

TEST_CASE("can parse condition")
{
    const std::vector<token> tokens =
    {
        {left_parenthesis_token{}},
        {condition_token{}},

        // first branch
        {left_bracket_token{}},
            {identifier_token{"boolean-condition"}},
            {identifier_token{"reference-name"}},
        {right_bracket_token{}},

        // second branch
        {left_bracket_token{}},
            {boolean_literal_token{"true"}},
            {left_parenthesis_token{}},
                {identifier_token{"i32"}},
                {integer_literal_token{"1337"}},
            {right_parenthesis_token{}},
        {right_bracket_token{}},

        // fallback
        {identifier_token{"i32"}},

        {right_parenthesis_token{}}
    };
    const auto parser = make_parser<ast::condition>();
    const auto result = parser.parse(tokens.cbegin(), tokens.cend());
    REQUIRE(is_success(result));
    const auto [cond, pos] = get_success(result);
    CHECK(cond.branches.size() == 2);
    CHECK(pos == tokens.end());
}

TEST_CASE("can parse identifier expression")
{
    const std::vector<token> tokens = { {identifier_token{"name"}} };
    const auto parser = make_parser<ast::expression>();
    const auto result = parser.parse(tokens.cbegin(), tokens.cend());
    REQUIRE(is_success(result));
    const auto [expr, pos] = get_success(result);
    REQUIRE(holds<ast::reference>(expr));
    CHECK(get<ast::reference>(expr).name == "name");
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
    const auto result = parser.parse(tokens.cbegin(), tokens.cend());
    REQUIRE(is_success(result));
    const auto [eval, pos] = get_success(result);
    CHECK(pos == tokens.end());
    CHECK(eval.function == "func");
    CHECK(eval.arguments.size() == 2);
    REQUIRE(holds<ast::reference>(eval.arguments.at(0)));
    CHECK(get<ast::reference>(eval.arguments.at(0)).name == "arg1");
    REQUIRE(holds<ast::reference>(eval.arguments.at(1)));
    CHECK(get<ast::reference>(eval.arguments.at(0)).name == "arg1");
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
    const auto result = parser.parse(tokens.cbegin(), tokens.cend());
    REQUIRE(is_success(result));
    const auto [expr, pos] = get_success(result);
    CHECK(pos == tokens.end());
    REQUIRE(holds<ast::evaluation>(expr));
    const auto eval = get<ast::evaluation>(expr);
    CHECK(eval.function == "func");
    CHECK(eval.arguments.size() == 2);
    REQUIRE(holds<ast::reference>(eval.arguments.at(0)));
    CHECK(get<ast::reference>(eval.arguments.at(0)).name == "arg1");
    REQUIRE(holds<ast::reference>(eval.arguments.at(1)));
    CHECK(get<ast::reference>(eval.arguments.at(0)).name == "arg1");
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
    const auto result = parser.parse(tokens.cbegin(), tokens.cend());
    REQUIRE(is_success(result));
    const auto [eval, pos] = get_success(result);

    CHECK(pos == tokens.end());
    CHECK(eval.function == "outer-func");
    CHECK(eval.arguments.size() == 2);

    REQUIRE(holds<ast::evaluation>(eval.arguments.at(0)));
    const auto inner_eval_1 = get<ast::evaluation>(eval.arguments.at(0));
    CHECK(inner_eval_1.function == "inner-func-1");
    REQUIRE(inner_eval_1.arguments.size() == 1);
    REQUIRE(holds<ast::reference>(inner_eval_1.arguments.at(0)));
    CHECK(get<ast::reference>(inner_eval_1.arguments.at(0)).name == "arg1");

    REQUIRE(holds<ast::evaluation>(eval.arguments.at(1)));
    const auto inner_eval_2 = get<ast::evaluation>(eval.arguments.at(1));
    CHECK(inner_eval_2.function == "inner-func-2");
    REQUIRE(inner_eval_2.arguments.size() == 1);
    REQUIRE(holds<ast::reference>(inner_eval_2.arguments.at(0)));
    CHECK(get<ast::reference>(inner_eval_2.arguments.at(0)).name == "arg2");
}

TEST_CASE("can parse boolean true literal")
{
    const std::vector<token> tokens = { {boolean_literal_token{"true"}} };
    const auto parser = make_parser<ast::boolean>();
    const auto result = parser.parse(tokens.cbegin(), tokens.cend());
    REQUIRE(is_success(result));
    const auto [boolean, pos] = get_success(result);
    CHECK(pos == tokens.cend());
    CHECK(boolean.value == true);
}

TEST_CASE("can parse boolean false literal")
{
    const std::vector<token> tokens = { {boolean_literal_token{"false"}} };
    const auto parser = make_parser<ast::boolean>();
    const auto result = parser.parse(tokens.cbegin(), tokens.cend());
    REQUIRE(is_success(result));
    const auto [boolean, pos] = get_success(result);
    CHECK(pos == tokens.cend());
    CHECK(boolean.value == false);
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
    const auto result = parser.parse(tokens.cbegin(), tokens.cend());
    REQUIRE(is_success(result));
    const auto [literal, pos] = get_success(result);
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
    CHECK(is_failure(parser.parse(tokens.cbegin(), tokens.cend())));
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
    CHECK(is_failure(parser.parse(tokens.cbegin(), tokens.cend())));
}

TEST_CASE("mismatching identifier token value and literal type raises error")
{
    const std::vector<token> tokens = {
        {left_parenthesis_token{}},
        {identifier_token{"i32"}},
        {integer_literal_token{"123"}},
        {right_parenthesis_token{}}
    };
    const auto parser = make_parser<ast::i64>();
    CHECK(is_failure(parser.parse(tokens.cbegin(), tokens.cend())));
}

TEST_CASE("can parse 32 bit floating point literal")
{
    const std::vector<token> tokens = {
        {left_parenthesis_token{}},
        {identifier_token{"f32"}},
        {floating_point_literal_token{"13.37"}},
        {right_parenthesis_token{}}
    };
    const auto parser = make_parser<ast::f32>();
    const auto result = parser.parse(tokens.cbegin(), tokens.cend());
    REQUIRE(is_success(result));
    const auto [literal, pos] = get_success(result);
    CHECK(pos == tokens.cend());
    CHECK(literal.value == doctest::Approx(13.37f));
}

TEST_CASE("can parse literal variant with integer alternative")
{
    const std::vector<token> tokens = {
        {left_parenthesis_token{}},
        {identifier_token{"i32"}},
        {integer_literal_token{"1337"}},
        {right_parenthesis_token{}}
    };
    const auto parser = make_parser<ast::literal_variant>();
    const auto result = parser.parse(tokens.cbegin(), tokens.cend());
    REQUIRE(is_success(result));
    const auto [literal, pos] = get_success(result);
    CHECK(pos == tokens.cend());
    REQUIRE(holds<ast::i32>(literal));
    const auto i32 = get<ast::i32>(literal);
    CHECK(i32.value == 1337);
}

TEST_CASE("can parse identifier expression")
{
    const std::vector<token> tokens = {
        {identifier_token{"name"}},
    };
    const auto parser = make_parser<ast::expression>();
    const auto result = parser.parse(tokens.cbegin(), tokens.cend());
    REQUIRE(is_success(result));
    const auto [expr, pos] = get_success(result);
    CHECK(pos == tokens.cend());
    REQUIRE(holds<ast::reference>(expr));
    const auto ref = get<ast::reference>(expr);
    CHECK(ref.name == "name");
}

TEST_CASE("can parse literal expression")
{
    const std::vector<token> tokens = {
        {left_parenthesis_token{}},
        {identifier_token{"i32"}},
        {integer_literal_token{"1337"}},
        {right_parenthesis_token{}}
    };
    const auto parser = make_parser<ast::expression>();
    const auto result = parser.parse(tokens.cbegin(), tokens.cend());
    REQUIRE(is_success(result));
    const auto [expr, pos] = get_success(result);
    CHECK(pos == tokens.cend());
    REQUIRE(holds<ast::literal_variant>(expr));
    const auto variant = get<ast::literal_variant>(expr);
    REQUIRE(holds<ast::i32>(variant));
    const auto literal = get<ast::i32>(variant);
    CHECK(literal.value == 1337);
}

TEST_CASE("can parse evaluation expression")
{
    const std::vector<token> tokens = {
        {left_parenthesis_token{}},
            {identifier_token{"my-function"}},

            {left_parenthesis_token{}},
                {identifier_token{"i32"}},
                {integer_literal_token{"1337"}},
            {right_parenthesis_token{}},

            {identifier_token{"my-argument"}},
        {right_parenthesis_token{}}
    };
    const auto parser = make_parser<ast::expression>();
    const auto result = parser.parse(tokens.cbegin(), tokens.cend());
    REQUIRE(is_success(result));
    const auto [expr, pos] = get_success(result);
    CHECK(pos == tokens.cend());
    REQUIRE(holds<ast::evaluation>(expr));
    const auto eval = get<ast::evaluation>(expr);
    CHECK(eval.function == "my-function");
    REQUIRE(eval.arguments.size() == 2);
    REQUIRE(holds<ast::literal_variant>(eval.arguments.at(0)));
    const auto variant = get<ast::literal_variant>(eval.arguments.at(0));
    REQUIRE(holds<ast::i32>(variant));
    const auto literal = get<ast::i32>(variant);
    CHECK(literal.value == 1337);
    REQUIRE(holds<ast::reference>(eval.arguments.at(1)));
    const auto ref = get<ast::reference>(eval.arguments.at(1));
    CHECK(ref.name == "my-argument");
}

TEST_CASE("can parse nested evaluation expression")
{
    const std::vector<token> tokens = {
        {left_parenthesis_token{}},
            {identifier_token{"my-function"}},

            {left_parenthesis_token{}},
                {identifier_token{"i32"}},
                {integer_literal_token{"1337"}},
            {right_parenthesis_token{}},

            {left_parenthesis_token{}},
                {identifier_token{"my-nested-function"}},
            {right_parenthesis_token{}},
        {right_parenthesis_token{}}
    };
    const auto parser = make_parser<ast::expression>();
    const auto result = parser.parse(tokens.cbegin(), tokens.cend());
    REQUIRE(is_success(result));
    const auto [expr, pos] = get_success(result);
    CHECK(pos == tokens.cend());
    REQUIRE(holds<ast::evaluation>(expr));
    const auto eval = get<ast::evaluation>(expr);
    CHECK(eval.function == "my-function");
    REQUIRE(eval.arguments.size() == 2);
    REQUIRE(holds<ast::literal_variant>(eval.arguments.at(0)));
    const auto variant = get<ast::literal_variant>(eval.arguments.at(0));
    REQUIRE(holds<ast::i32>(variant));
    const auto literal = get<ast::i32>(variant);
    CHECK(literal.value == 1337);
    REQUIRE(holds<ast::evaluation>(eval.arguments.at(1)));
    const auto nested = get<ast::evaluation>(eval.arguments.at(1));
    CHECK(nested.function == "my-nested-function");
    CHECK(nested.arguments.empty());
}

TEST_CASE("can parse let expression")
{
    const std::vector<token> tokens =
    {
        {left_parenthesis_token{}},
        {scope_token{}},

        // first bind
        {left_bracket_token{}},
            {identifier_token{"x"}},
            {identifier_token{"y"}},
        {right_bracket_token{}},

        // second bind
        {left_bracket_token{}},
            {identifier_token{"z"}},
            {left_parenthesis_token{}},
                {identifier_token{"i32"}},
                {integer_literal_token{"1337"}},
            {right_parenthesis_token{}},
        {right_bracket_token{}},

        // expression
        {left_parenthesis_token{}},
            {identifier_token{"+"}},
            {identifier_token{"x"}},
            {identifier_token{"y"}},
        {right_parenthesis_token{}},

        {right_parenthesis_token{}}
    };
    const auto parser = make_parser<ast::scope>();
    const auto result = parser.parse(tokens.cbegin(), tokens.cend());
    REQUIRE(is_success(result));
}

TEST_CASE("parser returns failure with position to first failing token with offset exceeding LCP")
{
    const auto parser = make_parser<ast::statement>();
    const std::vector<token> tokens = {
        {left_parenthesis_token{}},
        {structure_token{}},
        {integer_literal_token{}}
    };
    const auto result = parser.parse(tokens.cbegin(), tokens.cend());
    CHECK(alternative_longest_common_prefix(rule_of_t<rule_of_t<ast::statement>>()) == 1);
    REQUIRE(is_failure(result));
    const auto& failure = get_failure(result);
    CHECK(get_longest_failure_offset(tokens.cbegin(), failure) == 2);
}
