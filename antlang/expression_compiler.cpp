#include "compiler.hpp"

namespace ant
{

struct expression_compiler
{
    compiler_environment const& env;
    compiler_scope const& scope;

    compiler_expect<runtime::expression>
    operator()(ast::reference const& ref)
    {
        compiler_expect<runtime::value_variant*> result = compile(scope, ref);
        if (is_success(result))
        {
            auto& [value, type] = get_success(result);
            return compiler_result<runtime::expression>{std::move(value), std::move(type)};
        }
        else
        {
            return std::move(get_failure(result));
        }
    }

    compiler_expect<runtime::expression>
    operator()(ast::literal_variant const& literal)
    {
        compiler_expect<runtime::value_variant> result = compile(env, literal);
        if (is_success(result))
        {
            auto& [value, type] = get_success(result);
            return compiler_result<runtime::expression>{std::move(value), std::move(type)};
        }
        else
        {
            return std::move(get_failure(result));
        }
    }

    compiler_expect<runtime::expression>
    operator()(ast::evaluation const& eval)
    {
        compiler_expect<runtime::evaluation> result = compile(env, scope, eval);
        if (is_success(result))
        {
            auto& [value, type] = get_success(result);
            return compiler_result<runtime::expression>{std::move(value), std::move(type)};
        }
        else
        {
            return std::move(get_failure(result));
        }
    }

    compiler_expect<runtime::expression>
    operator()(ast::condition const& cond)
    {
        compiler_expect<runtime::condition> result = compile(env, scope, cond);
        if (is_success(result))
        {
            auto& [value, type] = get_success(result);
            return compiler_result<runtime::expression>{std::move(value), std::move(type)};
        }
        else
        {
            return std::move(get_failure(result));
        }
    }
};

compiler_expect<runtime::expression>
compile(compiler_environment const& env,
        compiler_scope const& scope,
        ast::expression const& expr)
{
    return visit(expression_compiler{env, scope}, expr);
}

}  // namespace ant
