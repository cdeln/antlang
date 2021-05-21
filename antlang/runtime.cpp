#include "runtime.hpp"

namespace ant
{
namespace runtime
{

operation::operation(function* blueprint, binary_operator impl)
    : blueprint{blueprint}, impl{impl}
{
    if (blueprint->parameters.size() != 2)
    {
        throw std::invalid_argument("operation blueprint invalid parameter size");
    }
}

value_variant execute(operation& op)
{
    const value_variant& arg0 = op.blueprint->parameters.at(0);
    const value_variant& arg1 = op.blueprint->parameters.at(1);
    return op.impl(arg0, arg1);
}

value_variant execute(function& func)
{
    return execute(func.value);
}

value_variant execute(evaluation& eval)
{
    auto* func = eval.blueprint;
    auto& params = func->parameters;
    auto& args = eval.arguments;
    const auto backup = params;
    auto exec_arg = [](auto& arg) { return execute(arg); };
    std::transform(args.begin(), args.end(), params.begin(), exec_arg);
    auto result = execute(*func);
    params = backup;
    return result;
}

structure execute(construction& ctor)
{
    return structure{ctor.prototype->parameters};
}

value_variant execute(condition& cond)
{
    for (auto& [check_expr, value_expr] : cond.branches)
    {
        if (get<bool>(execute(check_expr)))
        {
            return execute(value_expr);
        }
    }
    return execute(cond.fallback);
}

struct expression_executor
{
    value_variant operator()(value_variant& value) const
    {
        return value;
    }

    value_variant operator()(value_variant* value) const
    {
        return *value;
    }

    value_variant operator()(operation& op) const
    {
        return execute(op);
    }

    value_variant operator()(evaluation& eval) const
    {
        return execute(eval);
    }

    value_variant operator()(construction& ctor) const
    {
        return execute(ctor);
    }

    value_variant operator()(condition& cond) const
    {
        return execute(cond);
    }

    value_variant operator()(std::unique_ptr<scope>& expr) const
    {
        return execute(*expr);
    }
};

value_variant execute(expression& expr)
{
    return visit(expression_executor(), expr);
}

void execute(binding& expr)
{
    expr.result = execute(expr.value);
}

value_variant execute(scope& expr)
{
    std::vector<value_variant> backup;
    backup.reserve(expr.bindings.size());
    std::transform(expr.bindings.cbegin(), expr.bindings.cend(),
                   std::back_inserter(backup),
                   [](auto const& e) { return e.result; });
    std::for_each(expr.bindings.begin(), expr.bindings.end(), [](auto& e) { execute(e); });
    const auto value = execute(expr.value);
    for (size_t i = 0; i < backup.size(); ++i)
    {
        expr.bindings.at(i).result = std::move(backup.at(i));
    }
    return value;
}

}  // namespace runtime
}  // namespace ant
