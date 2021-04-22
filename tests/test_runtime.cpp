#include <doctest/doctest.h>

#include "runtime.hpp"

using namespace ant;
using namespace ant::runtime;

TEST_CASE("execute literal value variant expression works as expected")
{
    value_variant value = int32_t{1337};
    expression expr = value;
    const value_variant result = execute(expr);
    REQUIRE(holds<int32_t>(result));
    CHECK(get<int32_t>(result) == 1337);
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
    REQUIRE(holds<structure>(result));
    const structure instance = get<structure>(result);
    REQUIRE(instance.fields.size() == 2);
    REQUIRE(holds<int32_t>(instance.fields.at(0)));
    CHECK(get<int32_t>(instance.fields.at(0)) == 13);
    REQUIRE(holds<int64_t>(instance.fields.at(1)));
    CHECK(get<int64_t>(instance.fields.at(1)) == 37);
}

TEST_CASE("execute function with value expression type")
{
    function func;
    func.value = int32_t{1337};

    const value_variant result = execute(func.value);

    REQUIRE(holds<int32_t>(result));
    CHECK(get<int32_t>(result) == 1337);
}

TEST_CASE("execute function with parameter expression type")
{
    function func;
    func.parameters.resize(1);
    func.value = &func.parameters.at(0);

    func.parameters.at(0) = int32_t{1337};
    const value_variant result = execute(func.value);

    REQUIRE(holds<int32_t>(result));
    CHECK(get<int32_t>(result) == 1337);
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

    REQUIRE(holds<int32_t>(result));
    CHECK(get<int32_t>(result) == 1337);
}

TEST_CASE("execute construction with parameter expression type")
{
    function prototype;
    prototype.parameters.resize(1);

    construction ctor(&prototype);

    prototype.parameters.at(0) = int32_t{1337};
    const value_variant result = execute(ctor);

    REQUIRE(holds<structure>(result));
    structure instance = get<structure>(result);
    REQUIRE(instance.fields.size() == 1);
    REQUIRE(holds<int32_t>(instance.fields.at(0)));
    CHECK(get<int32_t>(instance.fields.at(0)) == 1337);
}

TEST_CASE("execute condition with literal check and value type")
{
    condition cond;

    SUBCASE("when first condition is true")
    {
        cond.branches.push_back({true,  int32_t{13}});
        cond.branches.push_back({false, int32_t{37}});
        auto value = execute(cond);
        REQUIRE(holds<int32_t>(value));
        CHECK(get<int32_t>(value) == 13);
    }

    SUBCASE("when second condition true")
    {
        cond.branches.push_back({false, int32_t{13}});
        cond.branches.push_back({true,  int32_t{37}});
        auto value = execute(cond);
        REQUIRE(holds<int32_t>(value));
        CHECK(get<int32_t>(value) == 37);
    }

    SUBCASE("fallback when no condition are true")
    {
        cond.branches.push_back({false, int32_t{13}});
        cond.branches.push_back({false,  int32_t{37}});
        cond.fallback = int32_t{1337};
        auto value = execute(cond);
        REQUIRE(holds<int32_t>(value));
        CHECK(get<int32_t>(value) == 1337);
    }
}

TEST_CASE("execute condition with literal check and structural value type")
{
    condition cond;
    cond.branches.push_back({true,  structure{{int32_t{1}, int64_t{3}}}});
    cond.branches.push_back({false, structure{{int32_t{3}, int64_t{7}}}});
    auto value = execute(cond);
    REQUIRE(holds<structure>(value));
    auto instance = get<structure>(value);
    REQUIRE(instance.fields.size() == 2);
    REQUIRE(holds<int32_t>(instance.fields.at(0)));
    REQUIRE(holds<int64_t>(instance.fields.at(1)));
    CHECK(get<int32_t>(instance.fields.at(0)) == 1);
    CHECK(get<int64_t>(instance.fields.at(1)) == 3);
}

TEST_CASE("execute condition with evaluation check and literal value type")
{
    condition cond;

    function identity;
    identity.parameters = {bool{}};
    identity.value = &identity.parameters.at(0);

    expression expr1 = evaluation(&identity);
    expression expr2 = evaluation(&identity);

    cond.branches.push_back({std::move(expr1), int32_t{13}});
    cond.branches.push_back({std::move(expr2), int32_t{37}});

    evaluation& eval1 = get<evaluation>(cond.branches.at(0).check);
    evaluation& eval2 = get<evaluation>(cond.branches.at(1).check);

    SUBCASE("when first condition evaluated to true")
    {
        eval1.arguments.at(0) = true;
        eval2.arguments.at(0) = false;
        auto value = execute(cond);
        REQUIRE(holds<int32_t>(value));
        CHECK(get<int32_t>(value) == 13);
    }

    SUBCASE("when second condition evaluated to true")
    {
        eval1.arguments.at(0) = false;
        eval2.arguments.at(0) = true;
        auto value = execute(cond);
        REQUIRE(holds<int32_t>(value));
        CHECK(get<int32_t>(value) == 37);
    }
}

TEST_CASE("execute plus operation works adds two integers")
{
    function blueprint;
    blueprint.parameters = {int32_t{}, int32_t{}};
    operation op = make_binary_operator<plus, int32_t>(&blueprint);
    blueprint.parameters.at(0) = 13;
    blueprint.parameters.at(1) = 37;
    value_variant result = execute(op);
    REQUIRE(holds<int32_t>(result));
    CHECK(get<int32_t>(result) == (13 + 37));
}

TEST_CASE("execute minus operation subtracts two integers")
{
    function blueprint;
    blueprint.parameters = {int32_t{}, int32_t{}};
    operation op = make_binary_operator<minus, int32_t>(&blueprint);
    blueprint.parameters.at(0) = 13;
    blueprint.parameters.at(1) = 37;
    value_variant result = execute(op);
    REQUIRE(holds<int32_t>(result));
    CHECK(get<int32_t>(result) == (13 - 37));
}

TEST_CASE("execute multiplication operation multiplies two integers")
{
    function blueprint;
    blueprint.parameters = {int32_t{}, int32_t{}};
    operation op = make_binary_operator<multiplies, int32_t>(&blueprint);
    blueprint.parameters.at(0) = 13;
    blueprint.parameters.at(1) = 37;
    value_variant result = execute(op);
    REQUIRE(holds<int32_t>(result));
    CHECK(get<int32_t>(result) == (13 * 37));
}

TEST_CASE("execute division operation divides two integers")
{
    function blueprint;
    blueprint.parameters = {int32_t{}, int32_t{}};
    operation op = make_binary_operator<divides, int32_t>(&blueprint);
    blueprint.parameters.at(0) = 37;
    blueprint.parameters.at(1) = 13;
    value_variant result = execute(op);
    REQUIRE(holds<int32_t>(result));
    CHECK(get<int32_t>(result) == (37 / 13));
}

TEST_CASE("execute division operation with zero divisor throws exception")
{
    function blueprint;
    blueprint.parameters = {int32_t{}, int32_t{}};
    operation op = make_binary_operator<divides, int32_t>(&blueprint);
    blueprint.parameters.at(0) = 37;
    blueprint.parameters.at(1) = 0;
    REQUIRE_THROWS(execute(op));
}
