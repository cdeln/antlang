#include <doctest/doctest.h>

#include "runtime.hpp"

using namespace ant::runtime;

TEST_CASE("execute value variant expression works as expected")
{
    value_variant value = static_cast<int32_t>(1337);
    expression expr = value;
    const value_variant result = execute(expr);
    REQUIRE(std::holds_alternative<int32_t>(result));
    CHECK(std::get<int32_t>(result) == 1337);
}

TEST_CASE("execute function with value expression type")
{
    function func;
    value_variant value = static_cast<int32_t>(1337);
    expression expr = value;
    func.value = expr;

    const value_variant result = execute(func.value);

    REQUIRE(std::holds_alternative<int32_t>(result));
    CHECK(std::get<int32_t>(result) == 1337);
}

TEST_CASE("execute function with parameter expression type")
{
    function func;
    func.parameters.resize(1);
    value_variant* param = &func.parameters.at(0);
    expression expr = param;
    func.value = expr;

    func.parameters.at(0) = static_cast<int32_t>(1337);
    const value_variant result = execute(func.value);

    REQUIRE(std::holds_alternative<int32_t>(result));
    CHECK(std::get<int32_t>(result) == 1337);
}

TEST_CASE("execute evaluation of function with parameter expression type")
{
    function func;
    func.parameters.resize(1);
    value_variant* param = &func.parameters.at(0);
    expression param_expr = param;
    func.value = param_expr;

    evaluation eval(func);

    REQUIRE(eval.arguments.size() == func.parameters.size());

    value_variant value = static_cast<int32_t>(1337);
    expression arg_expr = value;
    eval.arguments.at(0) = arg_expr;
    const value_variant result = execute(eval);

    REQUIRE(std::holds_alternative<int32_t>(result));
    CHECK(std::get<int32_t>(result) == 1337);
}