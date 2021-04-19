#include "runtime.hpp"

namespace ant
{
namespace runtime
{

operation::operation(function* blueprint)
    : blueprint{blueprint}
{
    if (blueprint->parameters.size() != 2)
    {
        throw std::invalid_argument("operation blueprint invalid parameter size");
    }
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
        if (std::get<bool>(execute(check_expr)))
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

    value_variant operator()(std::unique_ptr<operation>& op) const
    {
        return op->execute();
    }

    value_variant operator()(std::unique_ptr<evaluation>& eval) const
    {
        return execute(*eval);
    }

    value_variant operator()(std::unique_ptr<construction>& ctor) const
    {
        return execute(*ctor);
    }

    value_variant operator()(std::unique_ptr<condition>& cond) const
    {
        return execute(*cond);
    }
};

value_variant execute(expression& expr)
{
    return std::visit(expression_executor(), static_cast<expression_base&>(expr));
}

}  // namespace runtime
}  // namespace ant
