#include <doctest/doctest.h>

#include "runtime.hpp"

using namespace ant::runtime;

TEST_CASE("execute literal value variant expression works as expected")
{
    value_variant value = int32_t{1337};
    expression expr = value;
    const value_variant result = execute(expr);
    REQUIRE(std::holds_alternative<int32_t>(result));
    CHECK(std::get<int32_t>(result) == 1337);
}

TEST_CASE("execute structural value variant expression works as expected")
{
    value_variant prototype =
        structure {{
            int32_t{13},
            int64_t{37}
        }};
    expression expr = prototype;
    const value_variant result = execute(expr);
    REQUIRE(std::holds_alternative<structure>(result));
    const structure instance = std::get<structure>(result);
    REQUIRE(instance.fields.size() == 2);
    REQUIRE(std::holds_alternative<int32_t>(instance.fields.at(0)));
    CHECK(std::get<int32_t>(instance.fields.at(0)) == 13);
    REQUIRE(std::holds_alternative<int64_t>(instance.fields.at(1)));
    CHECK(std::get<int64_t>(instance.fields.at(1)) == 37);
}

TEST_CASE("execute function with value expression type")
{
    function func;
    func.value = int32_t{1337};

    const value_variant result = execute(func.value);

    REQUIRE(std::holds_alternative<int32_t>(result));
    CHECK(std::get<int32_t>(result) == 1337);
}

TEST_CASE("execute function with parameter expression type")
{
    function func;
    func.parameters.resize(1);
    func.value = &func.parameters.at(0);

    func.parameters.at(0) = int32_t{1337};
    const value_variant result = execute(func.value);

    REQUIRE(std::holds_alternative<int32_t>(result));
    CHECK(std::get<int32_t>(result) == 1337);
}

TEST_CASE("execute evaluation of function with parameter expression type")
{
    function func;
    func.parameters.resize(1);
    func.value = &func.parameters.at(0);

    evaluation eval(&func);

    REQUIRE(eval.arguments.size() == func.parameters.size());

    eval.arguments.at(0) = int32_t{1337};
    const value_variant result = execute(eval);

    REQUIRE(std::holds_alternative<int32_t>(result));
    CHECK(std::get<int32_t>(result) == 1337);
}

TEST_CASE("execute construction with parameter expression type")
{
    function prototype;
    prototype.parameters.resize(1);

    construction ctor(&prototype);

    prototype.parameters.at(0) = int32_t{1337};
    const value_variant result = execute(ctor);

    REQUIRE(std::holds_alternative<structure>(result));
    structure instance = std::get<structure>(result);
    REQUIRE(instance.fields.size() == 1);
    REQUIRE(std::holds_alternative<int32_t>(instance.fields.at(0)));
    CHECK(std::get<int32_t>(instance.fields.at(0)) == 1337);
}
