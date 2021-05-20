#include "compiler.hpp"

#include <sstream>

namespace ant
{

compiler_expect<runtime::scope>
compile(compiler_environment const& env,
        compiler_scope& scope,
        ast::scope const& expr)
{
    runtime::scope compiled_let;
    compiled_let.bindings.reserve(expr.bindings.size());

    std::string result_type;

    for (size_t i = 0; i < expr.bindings.size(); ++i)
    {
        auto const& binding = expr.bindings.at(i);
        auto it = scope.parameters.find(binding.name);
        if (it != scope.parameters.end())
        {
            std::stringstream message;
            message << "Redefinition of parameter " << binding.name;
            return compiler_failure{message.str(), binding.context};
        }

        auto binding_value_result = compile(env, scope, binding.value);

        if (is_failure(binding_value_result))
        {
            return std::move(get_failure(binding_value_result));
        }

        auto& [binding_value, binding_value_type] = get_success(binding_value_result);

        auto binding_result_prototype = runtime::execute(binding_value);
        runtime::binding compiled_binding = {std::move(binding_result_prototype), std::move(binding_value)};
        compiled_let.bindings.push_back(std::move(compiled_binding));
        compiler_result<runtime::value_variant*> result = {
            &compiled_let.bindings.back().result,
            std::move(binding_value_type)
        };
        scope.parameters.insert(it, std::make_pair(binding.name, std::move(result)));
    }

    auto value_result = compile(env, scope, expr.value);

    if (is_failure(value_result))
    {
        return std::move(get_failure(value_result));
    }

    auto& [value_expr, value_type] = get_success(value_result);

    compiled_let.value = std::move(value_expr);

    return compiler_result<runtime::scope>{
        std::move(compiled_let),
        value_type
    };
}

}  // namespace ant
