#include "compiler.hpp"

#include "formatting.hpp"

#include <sstream>

namespace ant
{

compiler_expect<runtime::evaluation>
compile(compiler_environment const& env,
        compiler_scope const& scope,
        ast::evaluation const& eval)
{
    std::vector<compiler_result<runtime::expression>> arguments;
    arguments.reserve(eval.arguments.size());

    for (const auto& arg : eval.arguments)
    {
        auto expr = compile(env, scope, arg);
        if (is_success(expr))
        {
            arguments.push_back(std::move(get_success(expr)));
        }
        else
        {
            return std::move(get_failure(expr));
        }
    }

    std::vector<std::string> signature;
    signature.reserve(eval.arguments.size());
    std::transform(arguments.begin(), arguments.end(),
                   std::back_inserter(signature),
                   [](auto const& arg) { return arg.type; });
    auto func_query = find_function(env, scope, eval.function, signature);
    if (is_failure(func_query))
    {
        std::stringstream message;
        message << "Could not find function " << quote(eval.function)
                << " with parameter type signature (";
        if (!signature.empty())
        {
            for (size_t i = 0; i < signature.size() - 1; ++i)
            {
                message << signature.at(i) << " ";
            }
            message << signature.back();
        }
        message << ")";
        return compiler_failure{message.str(), eval.context};
    }
    auto& [return_type, func_ptr] = get_success(func_query);

    auto result = runtime::evaluation(func_ptr);
    std::transform(std::move_iterator(arguments.begin()),
                   std::move_iterator(arguments.end()),
                   result.arguments.begin(),
                   [](auto&& arg) { return std::move(arg.value); });

    return compiler_result<runtime::evaluation>{
        std::move(result),
        return_type
    };
}

}  // namespace ant
