#include <doctest/doctest.h>

#include "compiler.hpp"

using namespace ant;

struct fixture
{
    compiler_environment env;
    compiler_scope scope;
};

TEST_CASE_FIXTURE(fixture, "compile literal")
{
    const ast::literal_variant literal = ast::literal<int32_t>{1337};
    runtime::value_variant result = compile(literal);
    REQUIRE(std::holds_alternative<int32_t>(result));
}

TEST_CASE_FIXTURE(fixture, "compile parameter with undefined type returns failure")
{
    const ast::parameter param = {"undefined-type", "parameter-name"};
    compiler_result<runtime::value_variant> result = compile(env, param);
    REQUIRE(is_failure(result));
}

TEST_CASE_FIXTURE(fixture, "compile parameter with defined type returns success")
{
    runtime::function type;
    env.functions["defined-type"] = &type;
    const ast::parameter param = {"defined-type", "parameter-name"};
    compiler_result<runtime::value_variant> result = compile(env, param);
    REQUIRE(is_success(result));
}

TEST_CASE_FIXTURE(fixture, "compile undefined reference returns failure")
{
    const ast::reference ref{"undefined-reference"};
    compiler_result<runtime::value_variant*> result = compile(scope, ref);
    REQUIRE(is_failure(result));
}

TEST_CASE_FIXTURE(fixture, "compile undefined reference returns failure")
{
    const ast::reference ref{"undefined-reference"};
    compiler_result<runtime::value_variant*> result = compile(scope, ref);
    REQUIRE(is_failure(result));
}

TEST_CASE_FIXTURE(fixture, "compile reference in scope returns pointer to value")
{
    const ast::reference ref{"defined-reference"};
    runtime::value_variant param;
    scope.parameters[ref.name] = &param;
    compiler_result<runtime::value_variant*> result = compile(scope, ref);
    REQUIRE(is_success(result));
    runtime::value_variant* ptr = std::get<runtime::value_variant*>(result);
    CHECK(ptr == &param);
}

TEST_CASE("get_evaluation_prototype returns the expected value variant for literal")
{
    runtime::expression expr = int32_t{0};
    runtime::value_variant proto = get_evaluation_prototype(expr);
    CHECK(std::holds_alternative<int32_t>(proto));
}

TEST_CASE("get_evaluation_prototype returns the expected value variant for structure")
{
    runtime::expression expr = runtime::structure{{int32_t{0}}};
    runtime::value_variant proto = get_evaluation_prototype(expr);
    REQUIRE(std::holds_alternative<runtime::structure>(proto));
    auto instance = std::get<runtime::structure>(proto);
    REQUIRE(instance.fields.size() == 1);
    CHECK(std::holds_alternative<int32_t>(instance.fields.at(0)));
}

TEST_CASE("get_evaluation_prototype returns the expected value variant for evaluation")
{
    runtime::function func;
    func.value = int32_t{};
    runtime::expression expr = std::make_unique<runtime::evaluation>(&func);
    runtime::value_variant proto = get_evaluation_prototype(expr);
    REQUIRE(std::holds_alternative<int32_t>(proto));
}

TEST_CASE("get_evaluation_prototype returns the expected value variant for construction")
{
    runtime::function func;
    func.parameters = {int32_t{}};
    func.value = std::make_unique<runtime::construction>(&func);
    runtime::value_variant proto = get_evaluation_prototype(func.value);
    REQUIRE(std::holds_alternative<runtime::structure>(proto));
    auto instance = std::get<runtime::structure>(proto);
    REQUIRE(instance.fields.size() == 1);
    CHECK(std::holds_alternative<int32_t>(instance.fields.at(0)));
}

TEST_CASE_FIXTURE(fixture, "compile structure with no fields")
{
    const ast::structure structure = {"my-structure", {}};
    compiler_result<std::unique_ptr<runtime::function>> result = compile(env, structure);
    REQUIRE(is_success(result));
    auto constructor = std::move(get_success(result));
    REQUIRE(constructor->parameters.empty());
}

TEST_CASE_FIXTURE(fixture, "compile structure with one field of undefind type returns failure")
{
    const ast::structure structure = {"my-structure", {{"undefined-type", "field-name"}}};
    compiler_result<std::unique_ptr<runtime::function>> result = compile(env, structure);
    REQUIRE(is_failure(result));
}

TEST_CASE_FIXTURE(fixture, "compile structure with one field of defined type")
{
    runtime::function type;
    type.value = int32_t{};
    env.functions["i32"] = &type;
    const ast::structure structure = {"my-structure", {{"i32", "field-name"}}};
    compiler_result<std::unique_ptr<runtime::function>> result = compile(env, structure);
    REQUIRE(is_success(result));
    auto constructor= std::move(get_success(result));
    REQUIRE(constructor->parameters.size() == 1);
    runtime::value_variant field_type = get_evaluation_prototype(constructor->parameters.at(0));
    REQUIRE(std::holds_alternative<int32_t>(field_type));
}

TEST_CASE_FIXTURE(fixture, "compile evaluation of undefined function returns failure")
{
    const ast::evaluation eval = {"undefined-function", {}};
    compiler_result<std::unique_ptr<runtime::evaluation>> result = compile(env, scope, eval);
    REQUIRE(is_failure(result));
}

TEST_CASE_FIXTURE(fixture, "compile evaluation of nullary function")
{
    runtime::function func;
    env.functions["defined-function"] = &func;
    const ast::evaluation eval = {"defined-function", {}};
    compiler_result<std::unique_ptr<runtime::evaluation>> result = compile(env, scope, eval);
    REQUIRE(is_success(result));
    auto& compiled = std::get<std::unique_ptr<runtime::evaluation>>(result);
    CHECK(compiled->blueprint == &func);
    CHECK(compiled->arguments.empty());
}

TEST_CASE("expression_type_matches returns false for different fundamental type")
{
    const runtime::value_variant val1 = int32_t{0};
    const runtime::value_variant val2 = int64_t{0};
    CHECK(!expression_type_matches(val1, val2));
}

TEST_CASE("expression_type_matches returns true for same fundamental types")
{
    const runtime::value_variant x1 = int32_t{13};
    const runtime::value_variant x2 = int32_t{37};
    CHECK(expression_type_matches(x1, x2));
}

TEST_CASE("expression_type_matches returns false for different structure types")
{
    const runtime::value_variant x1 = runtime::structure{{int32_t{13}}};
    const runtime::value_variant x2 = runtime::structure{{int64_t{37}}};
    REQUIRE(!expression_type_matches(x1, x2));
}

TEST_CASE("expression_type_matches returns true for simple isomorphic structure types")
{
    const runtime::value_variant x1 = runtime::structure{{int32_t{13}}};
    const runtime::value_variant x2 = runtime::structure{{int32_t{37}}};
    CHECK(expression_type_matches(x1, x2));
}

TEST_CASE("expression_type_matches returns false for different recursive structure types")
{

    const runtime::value_variant x1 = runtime::structure{{runtime::structure{{int32_t{13}}}}};
    const runtime::value_variant x2 = runtime::structure{{runtime::structure{{int64_t{37}}}}};
    CHECK(!expression_type_matches(x1, x2));
}

TEST_CASE("expression_type_matches returns true for isomorphic recursive structure types")
{
    const runtime::value_variant x1 = runtime::structure{{runtime::structure{{int32_t{13}}}}};
    const runtime::value_variant x2 = runtime::structure{{runtime::structure{{int32_t{37}}}}};
    CHECK(expression_type_matches(x1, x2));
}

TEST_CASE_FIXTURE(fixture,
    "compile evaluation of function with non-matching argument-parameter types returns failure")
{
    runtime::function func;
    func.parameters.resize(1);
    func.parameters.at(0) = int32_t{0};
    env.functions["defined-function"] = &func;
    const ast::evaluation eval = {"defined-function", {ast::literal<int64_t>{1337}}};
    compiler_result<std::unique_ptr<runtime::evaluation>> result = compile(env, scope, eval);
    REQUIRE(is_failure(result));
}

TEST_CASE_FIXTURE(fixture, "compile evaluation of unary function with matching parameter types")
{
    runtime::function func;
    func.parameters.resize(1);
    func.parameters.at(0) = int32_t{0};
    env.functions["defined-function"] = &func;
    const ast::evaluation eval = {"defined-function", {ast::literal<int32_t>{1337}}};
    compiler_result<std::unique_ptr<runtime::evaluation>> result = compile(env, scope, eval);
    REQUIRE(is_success(result));
    auto& compiled = std::get<std::unique_ptr<runtime::evaluation>>(result);
    CHECK(compiled->blueprint == &func);
    CHECK(compiled->arguments.size() == 1);
}

TEST_CASE_FIXTURE(fixture, "compile literal value expression")
{
    const ast::expression expr = ast::literal<int32_t>{1337};
    compiler_result<runtime::expression> result = compile(env, scope, expr);
    REQUIRE(is_success(result));
    runtime::expression compiled = std::move(get_success(result));
    REQUIRE(std::holds_alternative<runtime::value_variant>(compiled));
    auto value = std::get<runtime::value_variant>(compiled);
    REQUIRE(std::holds_alternative<int32_t>(value));
    CHECK(std::get<int32_t>(value) == 1337);
}

TEST_CASE_FIXTURE(fixture, "compile parameter reference expression")
{
    const ast::reference ref{"defined-reference"};
    const ast::expression expr = ref;

    runtime::value_variant param;
    scope.parameters[ref.name] = &param;

    compiler_result<runtime::expression> result = compile(env, scope, expr);
    REQUIRE(is_success(result));
    runtime::expression compiled = std::move(get_success(result));
    REQUIRE(std::holds_alternative<runtime::value_variant*>(compiled));
    auto* ptr = std::get<runtime::value_variant*>(compiled);
    CHECK(ptr == &param);
}

TEST_CASE_FIXTURE(fixture, "compile evaluation expression")
{
    const ast::evaluation eval = {"func", {}};
    const ast::expression expr = eval;

    runtime::function func;
    env.functions[eval.function] = &func;

    compiler_result<runtime::expression> result = compile(env, scope, expr);
    REQUIRE(is_success(result));
    REQUIRE(std::holds_alternative<std::unique_ptr<runtime::evaluation>>(get_success(result)));
    auto compiled = std::move(std::get<std::unique_ptr<runtime::evaluation>>(get_success(result)));
    CHECK(compiled->blueprint == &func);
    CHECK(compiled->arguments.empty());
}

TEST_CASE_FIXTURE(fixture, "compile function with undefined return type returns failure")
{
    const ast::function func
    {
        "my-function",
        ast::reference{"undefined-type"},
        {},
        ast::literal_variant{ast::literal<int32_t>{1337}}
    };
    auto result = compile(env, func);
    REQUIRE(is_failure(result));
}

TEST_CASE_FIXTURE(fixture, "compile function with return/value type mis-match returns failure")
{
    runtime::function i32;
    i32.value = int32_t{0};
    env.functions["i32"] = &i32;
    const ast::function func
    {
        "my-function",
        ast::reference{"i32"},
        {},
        ast::literal_variant{ast::literal<int64_t>{1337}}
    };
    auto result = compile(env, func);
    REQUIRE(is_failure(result));
}

TEST_CASE_FIXTURE(fixture, "compile nullary value expression function")
{
    runtime::function i32;
    i32.value = int32_t{0};
    env.functions["i32"] = &i32;
    const ast::function func
    {
        "my-function",
        ast::reference{"i32"},
        {},
        ast::literal_variant{ast::literal<int32_t>{1337}}
    };
    auto result = compile(env, func);
    REQUIRE(is_success(result));
    std::unique_ptr<runtime::function> compiled = std::move(get_success(result));
    CHECK(compiled->parameters.empty());
    CHECK(compiled->parameters.empty());
    REQUIRE(std::holds_alternative<runtime::value_variant>(compiled->value));
    auto variant = std::get<runtime::value_variant>(compiled->value);
    REQUIRE(std::holds_alternative<int32_t>(variant));
    auto value = std::get<int32_t>(variant);
    CHECK(value == 1337);
}

TEST_CASE_FIXTURE(fixture,
    "compile function with expression referencing undefined parameter returns failure")
{
    runtime::function i32;
    i32.value = int32_t{0};
    env.functions["i32"] = &i32;
    const ast::function func
    {
        "my-function",
        ast::reference{"i32"},
        {
            ast::parameter{"i32", "param"}
        },
        ast::reference{"undefined"}
    };
    auto result = compile(env, func);
    REQUIRE(is_failure(result));
}

TEST_CASE_FIXTURE(fixture, "compile unary function with valid parameter reference expression")
{
    runtime::function i32;
    i32.value = int32_t{0};
    env.functions["i32"] = &i32;
    const ast::function func
    {
        "my-function",
        ast::reference{"i32"},
        {
            ast::parameter{"i32", "param"}
        },
        ast::reference{"param"}
    };
    auto result = compile(env, func);
    REQUIRE(is_success(result));
    std::unique_ptr<runtime::function> compiled = std::move(get_success(result));
    REQUIRE(std::holds_alternative<runtime::value_variant*>(compiled->value));
    auto* ptr = std::get<runtime::value_variant*>(compiled->value);
    REQUIRE(compiled->parameters.size() == 1);
    CHECK(ptr == &compiled->parameters.at(0));
}

TEST_CASE_FIXTURE(fixture,
    "compile function with mis-matching expression evaluation type returns failure")
{
    runtime::function i32;
    runtime::function i64;
    i32.value = int32_t{0};
    i64.value = int64_t{0};
    env.functions["i32"] = &i32;
    env.functions["i64"] = &i64;
    const ast::function func
    {
        "my-function",
        ast::reference{"i64"},
        {
            ast::parameter{"i32", "param"}
        },
        ast::evaluation{"i32", {ast::reference{"param"}}}
    };
    auto result = compile(env, func);
    REQUIRE(is_failure(result));
}

TEST_CASE_FIXTURE(fixture, "compile unary function with valid evaluation expression")
{
    runtime::function i32;
    i32.value = int32_t{};
    i32.parameters = {int32_t{}};
    env.functions["i32"] = &i32;
    const ast::function func =
    {
        "my-function",
        ast::reference{"i32"},
        {
            {"i32", "param"}
        },
        ast::evaluation{"i32", {ast::reference{"param"}}}
    };
    auto result = compile(env, func);
    REQUIRE(is_success(result));
    std::unique_ptr<runtime::function> compiled = std::move(get_success(result));
    REQUIRE(std::holds_alternative<std::unique_ptr<runtime::evaluation>>(compiled->value));
    auto& eval = std::get<std::unique_ptr<runtime::evaluation>>(compiled->value);
    REQUIRE(compiled->parameters.size() == 1);
    REQUIRE(eval->blueprint == &i32);
    REQUIRE(eval->arguments.size() == 1);
    REQUIRE(std::holds_alternative<runtime::value_variant*>(eval->arguments.at(0)));
    auto* argument = std::get<runtime::value_variant*>(eval->arguments.at(0));
    CHECK(argument == &compiled->parameters.at(0));
}

TEST_CASE_FIXTURE(fixture, "compile condition with no branches returns failure")
{
    ast::expression fallback = ast::literal<int32_t>{};
    ast::condition cond = {{}, fallback};
    auto result = compile(env, scope, cond);
    REQUIRE(is_failure(result));
}

TEST_CASE_FIXTURE(fixture, "compile condition without fallback returns failure")
{
    ast::branch branch = {ast::literal<bool>{true},  ast::literal<int32_t>{}};
    ast::condition cond = {{branch}};
    auto result = compile(env, scope, cond);
    REQUIRE(is_failure(result));
}

TEST_CASE_FIXTURE(fixture, "compile with at least one branch and one fallback works")
{
    ast::branch branch = {ast::literal<bool>{false}, ast::literal<int32_t>{}};
    ast::expression fallback = ast::literal<int32_t>{};
    ast::condition cond = {{branch}, fallback};
    auto result = compile(env, scope, cond);
    REQUIRE(is_success(result));
}

TEST_CASE_FIXTURE(fixture, "compile condition with multiple branches works")
{
    ast::branch first = {ast::literal<bool>{false}, ast::literal<int32_t>{}};
    ast::branch second = {ast::literal<bool>{true},  ast::literal<int32_t>{}};
    std::vector<ast::branch> branches = {first, second};
    ast::expression fallback = ast::literal<int32_t>{};
    ast::condition cond = {branches, fallback};
    auto result = compile(env, scope, cond);
    REQUIRE(is_success(result));
}

TEST_CASE_FIXTURE(fixture, "compile condition with conflicting returns values fails")
{
    SUBCASE("when branch and fallback types differ")
    {
        ast::branch branch = {ast::literal<bool>{false}, ast::literal<int32_t>{}};
        ast::expression fallback = ast::literal<int64_t>{};
        ast::condition cond = {{branch}, fallback};
        auto result = compile(env, scope, cond);
        REQUIRE(is_failure(result));
    }

    SUBCASE("when branch types differ")
    {
        ast::branch first = {ast::literal<bool>{false}, ast::literal<int32_t>{}};
        ast::branch second = {ast::literal<bool>{true},  ast::literal<int64_t>{}};
        std::vector<ast::branch> branches = {first, second};
        ast::expression fallback = ast::literal<int32_t>{};
        ast::condition cond = {branches, fallback};
        auto result = compile(env, scope, cond);
        REQUIRE(is_failure(result));
    }
}

struct prog_fixture : fixture
{
    std::unique_ptr<runtime::function> i32;
    runtime::program prog;

    prog_fixture()
    {
        i32 = std::make_unique<runtime::function>();
        i32->value = int32_t{};
        i32->parameters = {int32_t{}};
        env.functions["i32"] = i32.get();
        prog.functions.push_back(std::move(i32));
    }
};

TEST_CASE_FIXTURE(prog_fixture,
    "compile statement with function populates the runtime program "
    "and compiler environment functions")
{
    const ast::function func = {
        "my-function",
        ast::reference{"i32"},
        {
            {"i32", "param"}
        },
        ast::evaluation{"i32", {ast::reference{"param"}}}
    };
    ast::statement statement = func;
    REQUIRE(env.functions.size() == 1);
    REQUIRE(prog.functions.size() == 1);
    compiler_status status = compile(prog, env, statement);
    REQUIRE(is_success(status));
    REQUIRE(env.functions.size() == 2);
    REQUIRE(prog.functions.size() == 2);
    auto it = env.functions.find(func.name);
    REQUIRE(it != env.functions.end());
}

TEST_CASE_FIXTURE(prog_fixture,
    "compile statement with structure populates the runtime program and the compiler environment")
{
    const ast::structure structure = {"my-structure", {{"i32", "field"}}};
    ast::statement statement = structure;
    CHECK(env.functions.size() == 1);
    CHECK(prog.functions.size() == 1);
    CHECK(env.functions.find(structure.name) == env.functions.end());
    compiler_status status = compile(prog, env, statement);
    REQUIRE(is_success(status));
    CHECK(env.functions.size() == 2);
    CHECK(prog.functions.size() == 2);
    CHECK(env.functions.find(structure.name) != env.functions.end());
}

TEST_CASE_FIXTURE(prog_fixture,
    "compile statement with evaluation populates the program evaluations")
{
    const ast::evaluation eval = {"i32", {ast::literal<int32_t>{1337}}};
    ast::statement statement = eval;
    REQUIRE(prog.evaluations.empty());
    const compiler_status status = compile(prog, env, statement);
    REQUIRE(is_success(status));
    REQUIRE(prog.evaluations.size() == 1);
}
