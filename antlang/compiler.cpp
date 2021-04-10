#include "compiler.hpp"

#include "formatting.hpp"

#include <sstream>

namespace ant
{

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

template bool is_success(compiler_result<runtime::value_variant> const&);
template bool is_failure(compiler_result<runtime::value_variant> const&);

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
compiler_failure const& get_failure(compiler_result<T> const& result)
{
    return std::get<compiler_failure>(result);
}

compiler_result<runtime::value_variant>
compile(compiler_environment const& env, ast::parameter const& param)
{
    auto it = env.functions.find(param.type);
    if (it == env.functions.end())
    {
        std::stringstream message;
        message << "Undefined parameter type " << quote(param.type);
        return compiler_failure{message.str()};
    }
    return runtime::value_variant{};
}

compiler_result<std::unique_ptr<runtime::function>>
compile(compiler_environment const& env, ast::function const& function)
{
    auto it = env.functions.find(function.name);
    if (it != env.functions.end())
    {
        std::stringstream message;
        message << "Redefinition of function " << quote(function.name);
        return compiler_failure{message.str()};
    }
    it = env.functions.find(function.return_type);
    if (it != env.functions.end())
    {
        std::stringstream message;
        message << "Unknown function return type " << quote(function.return_type);
        return compiler_failure{message.str()};
    }
    auto result = std::make_unique<runtime::function>();
    result->parameters.reserve(function.parameters.size());
    for (const auto& param : function.parameters)
    {
        auto compiled_param = compile(env, param);
        if (is_success(compiled_param))
        {
            result->parameters.push_back(std::move(get_success(compiled_param)));
        }
        else
        {
            return std::move(get_failure(compiled_param));
        }
    }

    compiler_scope scope;
    for (size_t i = 0; i < result->parameters.size(); ++i)
    {
        scope.variables.at(function.parameters.at(i).name) = &result->parameters.at(i);
    }

    auto compiled_value = compile(env, scope, function.body);
    if (is_success(compiled_value))
    {
        result->value = std::move(get_success(compiled_value));
    }
    else
    {
        return std::move(get_failure(compiled_value));
    }
    return std::move(result);
}

compiler_result<std::unique_ptr<runtime::function>>
compile(compiler_environment const& env, ast::structure  const& structure)
{
    return compiler_failure{"structure"};
}

compiler_result<std::unique_ptr<runtime::evaluation>>
compile(compiler_environment const& env,
        compiler_scope const& scope,
        ast::evaluation const& eval)
{
    return compiler_failure{"evaluation"};
}

compiler_result<runtime::value_variant*>
compile(compiler_scope const& scope, ast::reference const& ref)
{
    auto var = scope.variables.find(ref);
    if (var == scope.variables.end())
    {
        std::stringstream message;
        message << "Undefined reference to " << quote(ref);
        return compiler_failure{message.str()};
    }
    return var->second;
}

struct literal_compiler
{
    template <typename T>
    runtime::value_variant operator()(ast::literal<T> x)
    {
        return x.value;
    }
};

runtime::value_variant
compile(ast::literal_variant const& lit)
{
    return std::visit(literal_compiler(), lit);
}

struct expression_compiler
{
    compiler_environment const& env;
    compiler_scope const& scope;

    compiler_result<runtime::expression>
    operator()(ast::reference const& ref)
    {
        compiler_result<runtime::value_variant*> result = compile(scope, ref);
        if (is_success(result))
        {
            return std::move(get_success(result));
        }
        else
        {
            return std::move(get_failure(result));
        }
    }

    compiler_result<runtime::expression>
    operator()(ast::literal_variant const& lit)
    {
        return compile(lit);
    }

    compiler_result<runtime::expression>
    operator()(ast::evaluation const& eval)
    {
        compiler_result<std::unique_ptr<runtime::evaluation>> result = compile(env, scope, eval);
        if (is_success(result))
        {
            return std::move(get_success(result));
        }
        else
        {
            return std::move(get_failure(result));
        }
    }
};

compiler_result<runtime::expression>
compile(compiler_environment const& env,
        compiler_scope const& scope,
        ast::expression const& expr)
{
    std::visit(expression_compiler{env, scope}, expr);
    return compiler_failure{"expression"};
}

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
        compiler_result<std::unique_ptr<runtime::evaluation>> result =
            compile(env, {}, eval);
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
