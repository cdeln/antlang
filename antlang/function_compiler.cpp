#include "compiler.hpp"

#include "formatting.hpp"

#include <sstream>

namespace ant
{

exceptional<compiled_function_result, compiler_failure>
compile(compiler_environment const& env,
        ast::function const& function)
{
    if (env.prototypes.find(function.return_type.name) == env.prototypes.end())
    {
        std::stringstream message;
        message << "Unknown function return type " << quote(function.return_type.name);
        return compiler_failure{message.str(), function.return_type.context};
    }

    std::vector<std::string> signature;

    auto result = std::make_unique<runtime::function>();
    result->parameters.reserve(function.parameters.size());

    for (const auto& param : function.parameters)
    {
        auto compiled_param = compile(env, param);
        if (is_success(compiled_param))
        {
            auto& [value, type] = get_success(compiled_param);
            signature.push_back(std::move(type));
            result->parameters.push_back(std::move(value));
        }
        else
        {
            return std::move(get_failure(compiled_param));
        }
    }

    auto func_query = find_function(env, function.name, signature);

    if (is_success(func_query))
    {
        std::stringstream message;
        message << "Redefinition of function " << quote(function.name);
        return compiler_failure{message.str(), function.context};
    }

    compiler_scope scope;
    scope.function = {function.name, function.return_type.name, signature, result.get()};
    for (size_t i = 0; i < result->parameters.size(); ++i)
    {
        std::string param_name = function.parameters.at(i).name;
        scope.parameters[param_name] = {&result->parameters.at(i), signature.at(i)};
    }
    auto compiled_expr = compile(env, scope, function.body);
    if (is_success(compiled_expr))
    {
        auto& [value_expr, value_type] = get_success(compiled_expr);
        if (value_type != function.return_type.name)
        {
            std::stringstream message;
            message << "Function expression type " << quote(value_type)
                    << " does not match declared return type " << quote(function.return_type.name);
            return compiler_failure{
                message.str(),
                ast::get_context(function.body)
            };
        }
        result->value = std::move(value_expr);
    }
    else
    {
        return std::move(get_failure(compiled_expr));
    }

    return compiled_function_result{
        function_meta{
            function.return_type.name,
            std::move(signature)
        },
        std::move(result)
    };
}

}  // namespace ant
