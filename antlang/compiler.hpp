#pragma once

#include "exceptional.hpp"
#include "tokens.hpp"
#include "ast.hpp"
#include "runtime.hpp"

#include <map>
#include <string>

namespace ant
{

struct compiler_success
{
    std::string message;
};

struct compiler_failure
{
    std::string message;
    token_context context;
};

using compiler_status =
    exceptional<
        compiler_success,
        compiler_failure
    >;

template <typename T>
struct compiler_result
{
    T value;
    std::string type;
};

template <typename T>
using compiler_expect =
    exceptional<
        compiler_result<T>,
        compiler_failure
    >;

runtime::value_variant
get_evaluation_prototype(runtime::expression const& expr);

bool expression_type_matches(runtime::expression const& expr1,
                             runtime::expression const& expr2);

struct function_meta
{
    std::string return_type;
    std::vector<std::string> parameter_types;
};

struct compiled_function_result
{
    function_meta meta;
    std::unique_ptr<runtime::function> value;
};

struct compiled_function_meta
{
    function_meta meta;
    runtime::function* value;
};

struct compiler_environment
{
    std::map<std::string, std::vector<compiled_function_meta>> functions;
    std::map<std::string, std::unique_ptr<runtime::value_variant>> prototypes;
};

struct compiler_scope
{
    std::map<std::string, compiler_result<runtime::value_variant*>> parameters;
};

struct function_query_result
{
    std::string return_type;
    runtime::function* function;
};

exceptional<function_query_result, nullptr_t>
find_function(compiler_environment const& env,
              std::string const& name,
              std::vector<std::string> const& signature);

compiler_expect<runtime::value_variant>
compile(compiler_environment const& env,
        ast::literal_variant const& value);

compiler_expect<runtime::value_variant>
compile(compiler_environment const& env,
        ast::parameter const& param);

compiler_expect<runtime::value_variant*>
compile(compiler_scope const& scope,
        ast::reference const& ref);

compiler_expect<std::unique_ptr<runtime::evaluation>>
compile(compiler_environment const& env,
        compiler_scope const& scope,
        ast::evaluation const& eval);

compiler_expect<std::unique_ptr<runtime::condition>>
compile(compiler_environment const& env,
        compiler_scope const& scope,
        ast::condition const& cond);

compiler_expect<runtime::expression>
compile(compiler_environment const& env,
        compiler_scope const& scope,
        ast::expression const& eval);

exceptional<compiled_function_result, compiler_failure>
compile(compiler_environment const& env,
        ast::function const& function);

exceptional<std::unique_ptr<runtime::structure>, compiler_failure>
compile(compiler_environment const& env,
        ast::structure const& structure);

compiler_status
compile(runtime::program& prog,
        compiler_environment& env,
        ast::statement const& statement);

std::vector<compiler_status>
compile(runtime::program& result,
        compiler_environment& env,
        ast::program const& statements);

std::pair<compiler_environment, runtime::program>
setup_compiler();

}  // namespace ant
