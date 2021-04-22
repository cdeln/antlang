#include "compiler.hpp"

#include "formatting.hpp"

#include <sstream>

namespace ant
{

compiler_expect<runtime::condition>
compile(compiler_environment const& env,
        compiler_scope const& scope,
        ast::condition const& cond)
{
    if (cond.branches.empty())
    {
        return compiler_failure{"Condition must have at least 1 branch", cond.context};
    }

    runtime::condition compiled_condition;
    compiled_condition.branches.reserve(cond.branches.size());

    auto compile_branch = [&env, &scope](auto const& branch)
    {
        auto check = compile(env, scope, branch.check);
        auto value = compile(env, scope, branch.value);
        return std::make_pair(std::move(check), std::move(value));
    };

    std::string result_type;

    for (size_t i = 0; i < cond.branches.size(); ++i)
    {
        auto const& branch = cond.branches.at(i);
        auto [check, value] = compile_branch(branch);

        if (is_failure(check))
        {
            return get_failure(check);
        }
        if (is_failure(value))
        {
            return get_failure(value);
        }

        auto& [check_expr, check_type] = get_success(check);

        if (check_type != ast::name_of_v<ast::literal<bool>>)
        {
            std::stringstream message;
            message << "Condition branch check expression must be of type "
                    << quote(ast::name_of_v<ast::literal<bool>>)
                    << ", but is of type " << quote(check_type);
            return compiler_failure{
                message.str(),
                ast::get_context(branch.check)
            };
        }

        auto& [value_expr, value_type] = get_success(value);

        if (i == 0)
        {
            result_type = value_type;
        }
        else if (value_type != result_type)
        {
            return compiler_failure{
                "Conflicting result type of conditional expression",
                 ast::get_context(branch.value)
            };
        }

        compiled_condition.branches.push_back({std::move(check_expr), std::move(value_expr)});
    }

    auto fallback = compile(env, scope, cond.fallback);

    if (is_failure(fallback))
    {
        return std::move(get_failure(fallback));
    }

    auto& [fallback_expr, fallback_type] = get_success(fallback);

    if (fallback_type != result_type)
    {
        return compiler_failure{
            "Conflicting result type of conditional expression",
             ast::get_context(cond.fallback)
        };
    }

    compiled_condition.fallback = std::move(fallback_expr);

    return compiler_result<runtime::condition>{
        std::move(compiled_condition),
        result_type
    };
}

}  // namespace ant
