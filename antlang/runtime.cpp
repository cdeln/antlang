#include "runtime.hpp"

namespace ant
{
namespace runtime
{

value_variant execute(evaluation& eval)
{
    auto* func = eval.blueprint;
    auto& params = func->parameters;
    auto& args = eval.arguments;
    const auto backup = params;
    auto exec_arg = [](auto& arg) { return execute(arg); };
    std::transform(args.begin(), args.end(), params.begin(), exec_arg);
    auto result = execute(func->value);
    params = backup;
    return result;
}

structure execute(construction& ctor)
{
    return structure{ctor.prototype->parameters};
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

    value_variant operator()(std::unique_ptr<evaluation>& eval) const
    {
        return execute(*eval);
    }

    value_variant operator()(std::unique_ptr<construction>& ctor) const
    {
        return execute(*ctor);
    }
};

value_variant execute(expression& expr)
{
    return std::visit(expression_executor(), static_cast<expression_base&>(expr));
}

}  // namespace runtime
}  // namespace ant
