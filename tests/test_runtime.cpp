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

TEST_CASE("execute condition with literal check and value type")
{
    condition cond;

    SUBCASE("when first condition is true")
    {
        cond.branches.push_back({true,  int32_t{13}});
        cond.branches.push_back({false, int32_t{37}});
        auto value= execute(cond);
        REQUIRE(std::holds_alternative<int32_t>(value));
        CHECK(std::get<int32_t>(value) == 13);
        REQUIRE(std::holds_alternative<int32_t>(value));
    }

    SUBCASE("when second condition true")
    {
        cond.branches.push_back({false, int32_t{13}});
        cond.branches.push_back({true,  int32_t{37}});
        auto value= execute(cond);
        REQUIRE(std::holds_alternative<int32_t>(value));
        CHECK(std::get<int32_t>(value) == 37);
        REQUIRE(std::holds_alternative<int32_t>(value));
    }
}

TEST_CASE("execute condition with literal check and structural value type")
{
    condition cond;
    cond.branches.push_back({true,  structure{{int32_t{1}, int64_t{3}}}});
    cond.branches.push_back({false, structure{{int32_t{3}, int64_t{7}}}});
    auto value = execute(cond);
    REQUIRE(std::holds_alternative<structure>(value));
    auto instance = std::get<structure>(value);
    REQUIRE(instance.fields.size() == 2);
    REQUIRE(std::holds_alternative<int32_t>(instance.fields.at(0)));
    REQUIRE(std::holds_alternative<int64_t>(instance.fields.at(1)));
    CHECK(std::get<int32_t>(instance.fields.at(0)) == 1);
    CHECK(std::get<int64_t>(instance.fields.at(1)) == 3);
}

TEST_CASE("execute condition with evaluation check and literal value type")
{
    condition cond;

    function identity;
    identity.parameters = {bool{}};
    identity.value = &identity.parameters.at(0);

    auto eval_ptr_1 = std::make_unique<evaluation>(&identity);
    auto eval_ptr_2 = std::make_unique<evaluation>(&identity);
    evaluation& eval1 = *eval_ptr_1.get();
    evaluation& eval2 = *eval_ptr_2.get();
    expression expr1 = std::move(eval_ptr_1);
    expression expr2 = std::move(eval_ptr_2);

    cond.branches.push_back({std::move(expr1), int32_t{13}});
    cond.branches.push_back({std::move(expr2), int32_t{37}});

    SUBCASE("when first condition evaluated to true")
    {
        eval1.arguments.at(0) = true;
        eval2.arguments.at(0) = false;
        auto value = execute(cond);
        REQUIRE(std::holds_alternative<int32_t>(value));
        CHECK(std::get<int32_t>(value) == 13);
    }

    SUBCASE("when second condition evaluated to true")
    {
        eval1.arguments.at(0) = false;
        eval2.arguments.at(0) = true;
        auto value = execute(cond);
        REQUIRE(std::holds_alternative<int32_t>(value));
        CHECK(std::get<int32_t>(value) == 37);
    }
}
