#pragma once

#include <algorithm>
#include <cstdint>
#include <variant>
#include <vector>

namespace ant
{
namespace runtime
{

using std::int32_t;
using std::int64_t;

using value_variant =
    std::variant<
        int32_t,
        int64_t
    >;

struct evaluation;

using expression =
    std::variant<
        value_variant,
        value_variant*,
        evaluation*
    >;

struct function
{
    std::vector<value_variant> parameters;
    expression value;
};

struct evaluation
{
    function* blueprint;
    std::vector<expression> arguments;

    evaluation(function& func)
        : blueprint{&func}
        , arguments(func.parameters.size())
    {
    }
};

value_variant execute(evaluation& eval);

value_variant execute(expression& expr);

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

    value_variant operator()(evaluation* eval) const
    {
        return execute(*eval);
    }
};

value_variant execute(expression& expr)
{
    return std::visit(expression_executor(), expr);
}

}  // namespace runtime
}  // namespace ant
