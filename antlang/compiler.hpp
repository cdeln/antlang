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
bool is_success(compiler_result<T> const& result)
{
    return std::holds_alternative<T>(result);
}

template <typename T>
bool is_failure(compiler_result<T> const& result)
{
    return !is_success(result);
}

template <typename T>
T& get_success(compiler_result<T>& result)
{
    return std::get<T>(result);
}

template <typename T>
T const& get_success(compiler_result<T> const& result)
{
    return std::get<T>(result);
}

template <typename T>
compiler_failure& get_failure(compiler_result<T>& result)
{
    return std::get<compiler_failure>(result);
}

template <typename T>
compiler_failure const& get_success(compiler_result<T> const& result)
{
    return std::get<compiler_failure>(result);
}

struct compiler_environment
{
    std::map<std::string, runtime::function*> functions;
    std::map<std::string, runtime::structure*> structures;
};

compiler_result<std::unique_ptr<runtime::function>>
compile(compiler_environment const& env, ast::function const& function);

compiler_result<std::unique_ptr<runtime::function>>
compile(compiler_environment const& env, ast::structure  const& structure);

compiler_result<runtime::evaluation>
compile(compiler_environment const& env, ast::evaluation const& eval);

struct statement_compiler
{
    compiler_environment& env;
    runtime::program& program;

    compiler_status operator()(ast::function const& function)
    {
        compiler_result<std::unique_ptr<runtime::function>> result = compile(env, function);
        if (is_success(result))
        {
            std::unique_ptr<runtime::function> compiled = std::move(get_success(result));
            env.functions[function.name] = compiled.get();
            program.functions.push_back(std::move(compiled));
            return compiler_success{function.name};
        }
        else
        {
            return std::move(get_failure(result));
        }
    }

    compiler_status operator()(ast::structure const& structure)
    {
        compiler_result<std::unique_ptr<runtime::function>> result = compile(env, structure);
        if (is_success(result))
        {
            std::unique_ptr<runtime::function> constructor = std::move(get_success(result));
            env.functions[structure.name] = constructor.get();
            program.functions.push_back(std::move(constructor));
            return compiler_success{structure.name};
        }
        else
        {
            return std::move(get_failure(result));
        }
    }

    compiler_status operator()(ast::evaluation const& eval)
    {
        compiler_result<runtime::evaluation> result = compile(env, eval);
        if (is_success(result))
        {
            program.evaluations.push_back(std::move(get_success(result)));
            return compiler_success{};
        }
        else
        {
            return std::move(get_failure(result));
        }
    }
};

std::vector<compiler_status>
compile(runtime::program& result,
        compiler_environment& env,
        ast::program const& statements)
{
    std::vector<compiler_status> summary;
    summary.reserve(statements.size());
    for (auto& statement : statements)
    {
        compiler_status status =
            std::visit(
                statement_compiler{env, result},
                statement
            );
        summary.push_back(std::move(status));
        if (is_failure(summary.back()))
        {
            break;
        }
    }
    summary.shrink_to_fit();
    return summary;
}

}  // namespace ant
