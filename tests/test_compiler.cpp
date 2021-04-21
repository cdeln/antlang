#include <doctest/doctest.h>

#include "compiler.hpp"

using namespace ant;

struct fixture
{
    compiler_environment env;
    compiler_scope scope;
    runtime::program prog;

    fixture()
    {
        std::tie(env, prog) = setup_compiler();
    }
};

TEST_CASE_FIXTURE(fixture, "compile literal")
{
    const ast::literal_variant literal = ast::literal<int32_t>{1337};
    const auto result = compile(env, literal);
    REQUIRE(is_success(result));
    auto const& [value, type] = get_success(result);
    REQUIRE(holds<int32_t>(value));
    REQUIRE(type == ast::name_of_v<ast::literal<int32_t>>);
}

TEST_CASE_FIXTURE(fixture, "compile parameter with undefined type returns failure")
{
    const ast::parameter param = {"undefined-type", "parameter-name"};
    const compiler_expect<runtime::value_variant> result = compile(env, param);
    REQUIRE(is_failure(result));
}

TEST_CASE_FIXTURE(fixture, "compile parameter with defined type returns success")
{
    const ast::parameter param = {"i32", "parameter-name"};
    const compiler_expect<runtime::value_variant> result = compile(env, param);
    REQUIRE(is_success(result));
}

TEST_CASE_FIXTURE(fixture, "compile undefined reference returns failure")
{
    const ast::reference ref{"undefined-reference"};
    compiler_expect<runtime::value_variant*> result = compile(scope, ref);
    REQUIRE(is_failure(result));
}

TEST_CASE_FIXTURE(fixture, "compile undefined reference returns failure")
{
    const ast::reference ref{"undefined-reference"};
    compiler_expect<runtime::value_variant*> result = compile(scope, ref);
    REQUIRE(is_failure(result));
}

TEST_CASE_FIXTURE(fixture, "compile reference in scope returns pointer to value")
{
    const ast::reference ref{"defined-reference"};
    runtime::value_variant param;
    scope.parameters[ref.name] = {&param, "defined-type"};
    const compiler_expect<runtime::value_variant*> result = compile(scope, ref);
    REQUIRE(is_success(result));
    const auto& [value, type] = get_success(result);
    CHECK(value == &param);
    CHECK(type == "defined-type");
}

TEST_CASE_FIXTURE(fixture, "compile structure with no fields")
{
    const ast::structure structure = {"my-structure", {}};
    const auto result = compile(env, structure);
    REQUIRE(is_success(result));
    const auto& prototype = get_success(result);
    REQUIRE(prototype->fields.empty());
}

TEST_CASE_FIXTURE(fixture, "compile structure with one field of undefined type returns failure")
{
    const ast::structure structure = {"my-structure", {{"undefined-type", "field-name"}}};
    const auto result = compile(env, structure);
    REQUIRE(is_failure(result));
}

TEST_CASE_FIXTURE(fixture, "compile structure with one field of defined type")
{
    env.prototypes["i32"] = std::make_unique<runtime::value_variant>(int32_t{});
    const ast::structure structure = {"my-structure", {{"i32", "field-name"}}};
    const auto result = compile(env, structure);
    REQUIRE(is_success(result));
    const auto& prototype = get_success(result);
    REQUIRE(prototype->fields.size() == 1);
    const runtime::value_variant& field = prototype->fields.at(0);
    REQUIRE(holds<int32_t>(field));
}

TEST_CASE_FIXTURE(fixture, "compile evaluation of undefined function returns failure")
{
    const ast::evaluation eval = {"undefined-function", {}};
    compiler_expect<runtime::evaluation> result = compile(env, scope, eval);
    REQUIRE(is_failure(result));
}

TEST_CASE_FIXTURE(fixture, "compile evaluation of nullary function")
{
    runtime::function func;
    env.functions["defined-function"].push_back({function_meta{}, &func});
    const ast::evaluation eval = {"defined-function", {}};
    compiler_expect<runtime::evaluation> result = compile(env, scope, eval);
    REQUIRE(is_success(result));
    const auto& [value, type] = get_success(result);
    CHECK(value.blueprint == &func);
    CHECK(value.arguments.empty());
}

TEST_CASE_FIXTURE(fixture,
    "compile evaluation of function with non-matching argument-parameter types returns failure")
{
    runtime::function func;
    func.parameters.resize(1);
    func.parameters.at(0) = int32_t{0};
    function_meta meta;
    meta.parameter_types = {ast::name_of_v<ast::literal<int32_t>>};
    env.functions["defined-function"].push_back({meta, &func});

    const ast::evaluation eval = {"defined-function", {ast::literal<int64_t>{1337}}};
    const compiler_expect<runtime::evaluation> result = compile(env, scope, eval);

    REQUIRE(is_failure(result));
}

TEST_CASE_FIXTURE(fixture, "compile evaluation of unary function with matching parameter types")
{
    runtime::function func;
    func.parameters.resize(1);
    func.parameters.at(0) = int32_t{0};

    const function_meta meta = {
        ast::name_of_v<ast::literal<int32_t>>,
        {ast::name_of_v<ast::literal<int32_t>>}
    };
    env.functions["defined-function"].push_back({meta, &func});

    const ast::evaluation eval = {"defined-function", {ast::literal<int32_t>{1337}}};
    const compiler_expect<runtime::evaluation> result = compile(env, scope, eval);
    REQUIRE(is_success(result));
    const auto& [value, type] = get_success(result);
    CHECK(value.blueprint == &func);
    CHECK(value.arguments.size() == 1);
    CHECK(type == ast::name_of_v<ast::literal<int32_t>>);
}

TEST_CASE_FIXTURE(fixture, "compile literal value expression")
{
    const ast::expression expr = ast::literal<int32_t>{1337};
    const compiler_expect<runtime::expression> result = compile(env, scope, expr);
    REQUIRE(is_success(result));
    const auto& [compiled, type] = get_success(result);
    REQUIRE(holds<runtime::value_variant>(compiled));
    const auto value = get<runtime::value_variant>(compiled);
    REQUIRE(holds<int32_t>(value));
    CHECK(get<int32_t>(value) == 1337);
    CHECK(type == ast::name_of_v<ast::literal<int32_t>>);
}

TEST_CASE_FIXTURE(fixture, "compile parameter reference expression")
{
    const ast::reference ref{"defined-reference"};
    const ast::expression expr = ref;

    runtime::value_variant param;
    scope.parameters[ref.name] = {&param, "defined-type"};

    const compiler_expect<runtime::expression> result = compile(env, scope, expr);
    REQUIRE(is_success(result));
    const auto& [compiled, type ] = get_success(result);
    REQUIRE(holds<runtime::value_variant*>(compiled));
    const auto* ptr = get<runtime::value_variant*>(compiled);
    CHECK(ptr == &param);
    CHECK(type == "defined-type");
}

TEST_CASE_FIXTURE(fixture, "compile evaluation expression")
{
    const ast::evaluation eval = {"func", {}};
    const ast::expression expr = eval;

    runtime::function func;
    env.functions["func"].push_back({function_meta{"return-type"}, &func});

    const auto result = compile(env, scope, expr);
    REQUIRE(is_success(result));
    const auto& [expr_value, expr_type] = get_success(result);
    REQUIRE(holds<runtime::evaluation>(expr_value));
    const auto& compiled = get<runtime::evaluation>(expr_value);
    CHECK(compiled.blueprint == &func);
    CHECK(compiled.arguments.empty());
    CHECK(expr_type == "return-type");
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
    const auto result = compile(env, func);
    REQUIRE(is_failure(result));
}

TEST_CASE_FIXTURE(fixture, "compile function with return/value type mismatch returns failure")
{
    const ast::function func
    {
        "my-function",
        ast::reference{"i32"},
        {},
        ast::literal_variant{ast::literal<int64_t>{1337}}
    };
    const auto result = compile(env, func);
    REQUIRE(is_failure(result));
}

TEST_CASE_FIXTURE(fixture, "compile nullary value expression function")
{
    const ast::function func
    {
        "my-function",
        ast::reference{"i32"},
        {},
        ast::literal_variant{ast::literal<int32_t>{1337}}
    };
    const auto result = compile(env, func);
    REQUIRE(is_success(result));
    const auto& [meta, compiled] = get_success(result);
    CHECK(compiled->parameters.empty());
    CHECK(compiled->parameters.empty());
    REQUIRE(holds<runtime::value_variant>(compiled->value));
    const auto variant = get<runtime::value_variant>(compiled->value);
    REQUIRE(holds<int32_t>(variant));
    const auto value = get<int32_t>(variant);
    CHECK(value == 1337);
    CHECK(meta.return_type == "i32");
    CHECK(meta.parameter_types.empty());
}

TEST_CASE_FIXTURE(fixture,
    "compile function with expression referencing undefined parameter returns failure")
{
    const ast::function func
    {
        "my-function",
        ast::reference{"i32"},
        {
            ast::parameter{"i32", "param"}
        },
        ast::reference{"undefined"}
    };
    const auto result = compile(env, func);
    REQUIRE(is_failure(result));
}

TEST_CASE_FIXTURE(fixture, "compile unary function with valid parameter reference expression")
{
    const ast::function func
    {
        "my-function",
        ast::reference{"i32"},
        {
            ast::parameter{"i32", "param"}
        },
        ast::reference{"param"}
    };
    const auto result = compile(env, func);
    REQUIRE(is_success(result));
    const auto& [meta, compiled] = get_success(result);
    REQUIRE(holds<runtime::value_variant*>(compiled->value));
    const auto* ptr = get<runtime::value_variant*>(compiled->value);
    REQUIRE(compiled->parameters.size() == 1);
    CHECK(ptr == &compiled->parameters.at(0));
    CHECK(meta.return_type == "i32");
    REQUIRE(meta.parameter_types.size() == 1);
    CHECK(meta.parameter_types.at(0) == "i32");
}

TEST_CASE_FIXTURE(fixture,
    "compile function with mis-matching expression evaluation type returns failure")
{
    runtime::function i32;
    runtime::function i64;
    i32.value = int32_t{0};
    i64.value = int64_t{0};
    env.functions["i32"].push_back({function_meta{}, &i32});
    env.functions["i64"].push_back({function_meta{}, &i64});
    const ast::function func
    {
        "my-function",
        ast::reference{"i64"},
        {
            ast::parameter{"i32", "param"}
        },
        ast::evaluation{"i32", {ast::reference{"param"}}}
    };
    const auto result = compile(env, func);
    REQUIRE(is_failure(result));
}

TEST_CASE_FIXTURE(fixture, "compile unary function with valid evaluation expression")
{
    runtime::function i32;
    i32.value = int32_t{};
    i32.parameters = {int32_t{}};
    env.functions["i32"].push_back({function_meta{"i32", {"i32"}}, &i32});
    const ast::function func =
    {
        "my-function",
        ast::reference{"i32"},
        {
            {"i32", "param"}
        },
        ast::evaluation{"i32", {ast::reference{"param"}}}
    };
    const auto result = compile(env, func);
    REQUIRE(is_success(result));
    const auto& [meta, compiled] = get_success(result);
    REQUIRE(holds<runtime::evaluation>(compiled->value));
    const auto& eval = get<runtime::evaluation>(compiled->value);
    REQUIRE(compiled->parameters.size() == 1);
    REQUIRE(eval.blueprint == &i32);
    REQUIRE(eval.arguments.size() == 1);
    REQUIRE(holds<runtime::value_variant*>(eval.arguments.at(0)));
    const auto* argument = get<runtime::value_variant*>(eval.arguments.at(0));
    CHECK(argument == &compiled->parameters.at(0));
    CHECK(meta.return_type == "i32");
    REQUIRE(meta.parameter_types.size() == 1);
    CHECK(meta.parameter_types.at(0) == "i32");
}

TEST_CASE_FIXTURE(fixture, "compile condition with no branches returns failure")
{
    const ast::expression fallback = ast::literal<int32_t>{};
    const ast::condition cond = {{}, fallback};
    const auto result = compile(env, scope, cond);
    REQUIRE(is_failure(result));
}

TEST_CASE_FIXTURE(fixture, "compile condition without fallback returns failure")
{
    const ast::branch branch = {ast::literal<bool>{true},  ast::literal<int32_t>{}};
    const ast::condition cond = {{branch}};
    const auto result = compile(env, scope, cond);
    REQUIRE(is_failure(result));
}

TEST_CASE_FIXTURE(fixture, "compile with at least one branch and one fallback works")
{
    const ast::branch branch = {ast::literal<bool>{false}, ast::literal<int32_t>{}};
    const ast::expression fallback = ast::literal<int32_t>{};
    const ast::condition cond = {{branch}, fallback};
    const auto result = compile(env, scope, cond);
    REQUIRE(is_success(result));
}

TEST_CASE_FIXTURE(fixture, "compile condition with multiple branches works")
{
    const ast::branch first = {ast::literal<bool>{false}, ast::literal<int32_t>{}};
    const ast::branch second = {ast::literal<bool>{true},  ast::literal<int32_t>{}};
    const std::vector<ast::branch> branches = {first, second};
    const ast::expression fallback = ast::literal<int32_t>{};
    const ast::condition cond = {branches, fallback};
    const auto result = compile(env, scope, cond);
    REQUIRE(is_success(result));
}

TEST_CASE_FIXTURE(fixture, "compile condition with conflicting returns values fails")
{
    SUBCASE("when branch and fallback types differ")
    {
        const ast::branch branch = {ast::literal<bool>{false}, ast::literal<int32_t>{}};
        const ast::expression fallback = ast::literal<int64_t>{};
        const ast::condition cond = {{branch}, fallback};
        const auto result = compile(env, scope, cond);
        REQUIRE(is_failure(result));
    }

    SUBCASE("when branch types differ")
    {
        const ast::branch first = {ast::literal<bool>{false}, ast::literal<int32_t>{}};
        const ast::branch second = {ast::literal<bool>{true},  ast::literal<int64_t>{}};
        const std::vector<ast::branch> branches = {first, second};
        const ast::expression fallback = ast::literal<int32_t>{};
        const ast::condition cond = {branches, fallback};
        const auto result = compile(env, scope, cond);
        REQUIRE(is_failure(result));
    }
}

TEST_CASE_FIXTURE(fixture,
    "compile statement with function effects the runtime program and the compiler environment")
{
    const ast::function func = {
        "my-function",
        ast::reference{"i32"},
        {
            {"i32", "param"}
        },
        ast::reference{"param"}
    };
    const ast::statement statement = func;
    const size_t func_count = prog.functions.size();
    const compiler_status status = compile(prog, env, statement);
    REQUIRE(is_success(status));
    CHECK(prog.functions.size() == (func_count + 1));
    CHECK(is_success(find_function(env, "my-function", {"i32"})));
}

TEST_CASE_FIXTURE(fixture,
    "compile statement with structure effects the runtime program and the compiler environment")
{
    const ast::structure structure = {"my-structure", {{"i32", "field"}}};
    const ast::statement statement = structure;
    const size_t func_count = prog.functions.size();
    CHECK(env.functions.find(structure.name) == env.functions.end());
    CHECK(env.prototypes.find(structure.name) == env.prototypes.end());
    const compiler_status status = compile(prog, env, statement);
    REQUIRE(is_success(status));
    CHECK(prog.functions.size() == (func_count + 1));
    CHECK(env.functions.find(structure.name) != env.functions.end());
    CHECK(env.prototypes.find(structure.name) != env.prototypes.end());
}

TEST_CASE_FIXTURE(fixture, "function redefinition fails")
{
    const ast::function func = {
        "my-function",
        ast::reference{"i32"},
        {
            {"i32", "param"}
        },
        ast::reference{"param"}
    };
    const ast::statement statement = func;
    const size_t func_count = prog.functions.size();
    REQUIRE(is_success(compile(prog, env, statement)));
    REQUIRE(is_failure(compile(prog, env, statement)));
    CHECK(prog.functions.size() == (func_count + 1));
}

TEST_CASE_FIXTURE(fixture, "redefinition of structure fails")
{
    const ast::structure structure = {"my-structure", {{"i32", "field"}}};
    const ast::statement statement = structure;
    const size_t func_count = prog.functions.size();
    REQUIRE(is_success(compile(prog, env, statement)));
    REQUIRE(is_failure(compile(prog, env, statement)));
    CHECK(prog.functions.size() == (func_count + 1));
}

TEST_CASE_FIXTURE(fixture, "compiling function definitions with different signature")
{
    const ast::function func_i32 = {
        "my-function",
        ast::reference{"i32"},
        {
            {"i32", "param"}
        },
        ast::reference{"param"}
    };

    const ast::function func_i64 = {
        "my-function",
        ast::reference{"i64"},
        {
            {"i64", "param"}
        },
        ast::reference{"param"}
    };

    const ast::statement define_func_i32 = func_i32;
    const ast::statement define_func_i64 = func_i64;
    REQUIRE(is_success(compile(prog, env, define_func_i32)));
    REQUIRE(is_success(compile(prog, env, define_func_i64)));
}

TEST_CASE_FIXTURE(fixture, "arithmetic operations are defined as expected")
{
    SUBCASE("for + i32 i32")
    {
        auto func_query = find_function(env, "+", {"i32", "i32"});
        REQUIRE(is_success(func_query));
        auto& [return_type, plus] = get_success(func_query);
        REQUIRE(plus->parameters.size() == 2);
        CHECK(holds<int32_t>(plus->parameters.at(0)));
        CHECK(holds<int32_t>(plus->parameters.at(1)));
        CHECK(holds<int32_t>(runtime::execute(*plus)));
        CHECK(return_type == "i32");
    }

    SUBCASE("for + i64 i64")
    {
        auto func_query = find_function(env, "+", {"i64", "i64"});
        REQUIRE(is_success(func_query));
        auto& [return_type, plus] = get_success(func_query);
        REQUIRE(plus->parameters.size() == 2);
        CHECK(holds<int64_t>(plus->parameters.at(0)));
        CHECK(holds<int64_t>(plus->parameters.at(1)));
        CHECK(holds<int64_t>(runtime::execute(*plus)));
        CHECK(return_type == "i64");
    }

    SUBCASE("for - u8 u8")
    {
        auto func_query = find_function(env, "-", {"u8", "u8"});
        REQUIRE(is_success(func_query));
        auto& [return_type, minus] = get_success(func_query);
        REQUIRE(minus->parameters.size() == 2);
        CHECK(holds<uint8_t>(minus->parameters.at(0)));
        CHECK(holds<uint8_t>(minus->parameters.at(1)));
        CHECK(holds<uint8_t>(runtime::execute(*minus)));
        CHECK(return_type == "u8");
    }

    SUBCASE("for * u16 u16")
    {
        auto func_query = find_function(env, "*", {"u16", "u16"});
        REQUIRE(is_success(func_query));
        auto& [return_type, mult] = get_success(func_query);
        REQUIRE(mult->parameters.size() == 2);
        CHECK(holds<uint16_t>(mult->parameters.at(0)));
        CHECK(holds<uint16_t>(mult->parameters.at(1)));
        CHECK(holds<uint16_t>(runtime::execute(*mult)));
        CHECK(return_type == "u16");
    }

    SUBCASE("for / f32 f32")
    {
        auto func_query = find_function(env, "/", {"f32", "f32"});
        REQUIRE(is_success(func_query));
        auto& [return_type, div] = get_success(func_query);
        REQUIRE(div->parameters.size() == 2);
        CHECK(holds<flt32_t>(div->parameters.at(0)));
        CHECK(holds<flt32_t>(div->parameters.at(1)));
        div->parameters.at(1) = flt32_t{1};
        CHECK(holds<flt32_t>(runtime::execute(*div)));
        CHECK(return_type == "f32");
    }
}

TEST_CASE_FIXTURE(fixture, "comparison operations are defined as expected")
{
    SUBCASE("for = u32 u32")
    {
        auto func_query = find_function(env, "=", {"u32", "u32"});
        REQUIRE(is_success(func_query));
        auto& [return_type, equals] = get_success(func_query);
        REQUIRE(equals->parameters.size() == 2);
        CHECK(holds<uint32_t>(equals->parameters.at(0)));
        CHECK(holds<uint32_t>(equals->parameters.at(1)));
        CHECK(holds<bool>(runtime::execute(*equals)));
        CHECK(return_type == "bool");
    }

    SUBCASE("for < f64 f64")
    {
        auto func_query = find_function(env, "<", {"f64", "f64"});
        REQUIRE(is_success(func_query));
        auto& [return_type, less] = get_success(func_query);
        REQUIRE(less->parameters.size() == 2);
        CHECK(holds<flt64_t>(less->parameters.at(0)));
        CHECK(holds<flt64_t>(less->parameters.at(1)));
        CHECK(holds<bool>(runtime::execute(*less)));
        CHECK(return_type == "bool");
    }
}

TEST_CASE_FIXTURE(fixture, "compile statement with evaluation populates the program evaluations")
{
    const ast::evaluation eval = {"+", {ast::literal<int32_t>{13}, ast::literal<int32_t>{37}}};
    const ast::statement statement = eval;
    REQUIRE(prog.evaluations.empty());
    const compiler_status status = compile(prog, env, statement);
    REQUIRE(is_success(status));
    REQUIRE(prog.evaluations.size() == 1);
}

TEST_CASE_FIXTURE(fixture, "compile recursive function")
{
    const ast::function func =
    {
        "my-function",
        ast::reference{"i32"},
        {
            {"i32", "param"}
        },
        ast::evaluation{"my-function", {ast::reference{"param"}}}
    };
    const auto result = compile(env, func);
    if (is_failure(result))
    {
        MESSAGE(get_failure(result).message);
    }
    REQUIRE(is_success(result));
    const auto& [meta, compiled] = get_success(result);
    REQUIRE(holds<runtime::evaluation>(compiled->value));
    const auto& eval = get<runtime::evaluation>(compiled->value);
    REQUIRE(compiled->parameters.size() == 1);
    REQUIRE(eval.blueprint == compiled.get());
    REQUIRE(eval.arguments.size() == 1);
    REQUIRE(holds<runtime::value_variant*>(eval.arguments.at(0)));
    const auto* argument = get<runtime::value_variant*>(eval.arguments.at(0));
    CHECK(argument == &compiled->parameters.at(0));
    CHECK(meta.return_type == "i32");
    REQUIRE(meta.parameter_types.size() == 1);
    CHECK(meta.parameter_types.at(0) == "i32");
}
