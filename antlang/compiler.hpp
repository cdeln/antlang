#pragma once

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
};

using compiler_status =
    std::variant<
        compiler_success,
        compiler_failure
    >;

template <typename T>
using compiler_result =
    std::variant<
        T,
        compiler_failure
    >;

template <typename T>
bool is_success(compiler_result<T> const& result);

template <typename T>
bool is_failure(compiler_result<T> const& result);

template <typename T>
T& get_success(compiler_result<T>& result);

template <typename T>
compiler_failure& get_failure(compiler_result<T>& result);

struct compiler_environment
{
    std::map<std::string, runtime::function*> functions;
    std::map<std::string, runtime::structure const*> structures;
};

struct compiler_scope
{
    std::map<std::string, runtime::value_variant*> parameters;
};

runtime::value_variant
compile(ast::literal_variant const& value);

compiler_result<runtime::value_variant>
compile(compiler_environment const& env,
        ast::parameter const& param);

compiler_result<runtime::value_variant*>
compile(compiler_scope const& scope,
        ast::reference const& ref);

compiler_result<std::unique_ptr<runtime::evaluation>>
compile(compiler_environment const& env,
        compiler_scope const& scope,
        ast::evaluation const& eval);

compiler_result<runtime::expression>
compile(compiler_environment const& env,
        compiler_scope const& scope,
        ast::expression const& eval);

compiler_result<std::unique_ptr<runtime::function>>
compile(compiler_environment const& env,
        ast::function const& function);

compiler_result<std::unique_ptr<runtime::function>>
compile(compiler_environment const& env,
        ast::structure const& structure);

std::vector<compiler_status>
compile(runtime::program& result,
        compiler_environment& env,
        ast::program const& statements);

runtime::value_variant
get_evaluation_prototype(runtime::expression const& expr);

bool expression_type_matches(
    runtime::expression const& expr1,
    runtime::expression const& expr2);

}  // namespace ant
