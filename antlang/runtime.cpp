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

template <template <typename> class Operator, typename Type>
fundamental_operation<Operator, Type>::fundamental_operation(function* blueprint)
    : operation(blueprint)
{
    for (size_t i = 0; i < 2; ++i)
    {
        if (!std::holds_alternative<Type>(blueprint->parameters.at(i)))
        {
            throw std::invalid_argument("operation blueprint invalid parameter type");
        }
    }
}

template <template <typename> class Operator, typename Type>
value_variant fundamental_operation<Operator, Type>::execute()
{
    Operator<Type> op;
    Type const& arg0 = std::get<Type>(blueprint->parameters.at(0));
    Type const& arg1 = std::get<Type>(blueprint->parameters.at(1));
    return op(arg0, arg1);
}

template class fundamental_operation<plus,       int8_t>;
template class fundamental_operation<plus,       int16_t>;
template class fundamental_operation<plus,       int32_t>;
template class fundamental_operation<plus,       int64_t>;
template class fundamental_operation<plus,       uint8_t>;
template class fundamental_operation<plus,       uint16_t>;
template class fundamental_operation<plus,       uint32_t>;
template class fundamental_operation<plus,       uint64_t>;
template class fundamental_operation<plus,       flt32_t>;
template class fundamental_operation<plus,       flt64_t>;

template class fundamental_operation<minus,       int8_t>;
template class fundamental_operation<minus,       int16_t>;
template class fundamental_operation<minus,       int32_t>;
template class fundamental_operation<minus,       int64_t>;
template class fundamental_operation<minus,       uint8_t>;
template class fundamental_operation<minus,       uint16_t>;
template class fundamental_operation<minus,       uint32_t>;
template class fundamental_operation<minus,       uint64_t>;
template class fundamental_operation<minus,       flt32_t>;
template class fundamental_operation<minus,       flt64_t>;

template class fundamental_operation<multiplies,  int8_t>;
template class fundamental_operation<multiplies,  int16_t>;
template class fundamental_operation<multiplies,  int32_t>;
template class fundamental_operation<multiplies,  int64_t>;
template class fundamental_operation<multiplies,  uint8_t>;
template class fundamental_operation<multiplies,  uint16_t>;
template class fundamental_operation<multiplies,  uint32_t>;
template class fundamental_operation<multiplies,  uint64_t>;
template class fundamental_operation<multiplies,  flt32_t>;
template class fundamental_operation<multiplies,  flt64_t>;

template class fundamental_operation<divides,     int8_t>;
template class fundamental_operation<divides,     int16_t>;
template class fundamental_operation<divides,     int32_t>;
template class fundamental_operation<divides,     int64_t>;
template class fundamental_operation<divides,     uint8_t>;
template class fundamental_operation<divides,     uint16_t>;
template class fundamental_operation<divides,     uint32_t>;
template class fundamental_operation<divides,     uint64_t>;
template class fundamental_operation<divides,     flt32_t>;
template class fundamental_operation<divides,     flt64_t>;

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
