#pragma once

#include "fundamental_types.hpp"
#include "recursive_variant.hpp"

#include <algorithm>
#include <functional>
#include <memory>
#include <stdexcept>
#include <vector>

namespace ant
{
namespace runtime
{

struct value_variant;

struct structure
{
    std::vector<value_variant> fields;
};

using value_variant_base =
    recursive_variant<
        bool,
        int8_t,  int16_t,  int32_t,  int64_t,
        uint8_t, uint16_t, uint32_t, uint64_t,
        flt32_t, flt64_t,
        structure
    >;

struct value_variant : public value_variant_base
{
    using value_variant_base::value_variant_base;
};

struct function;

struct construction
{
    function* prototype;

    construction(function* prototype)
        : prototype{prototype}
    {
    }
};

struct evaluation;
struct condition;
struct operation;

using expression_base =
    recursive_variant<
        value_variant,
        value_variant*,
        construction,
        recursive_wrapper<evaluation>,
        recursive_wrapper<condition>,
        std::unique_ptr<operation>
    >;

struct expression : public expression_base
{
    using expression_base::expression_base;
};

struct function
{
    std::vector<value_variant> parameters;
    expression value;
};

struct evaluation
{
    function* blueprint;
    std::vector<expression> arguments;

    evaluation(function* func)
        : blueprint{func}
        , arguments(func->parameters.size())
    {
    }
};

struct branch
{
    expression check;
    expression value;
};

struct condition
{
    std::vector<branch> branches;
    expression fallback;
};

struct operation
{
    function* blueprint;

    operation(function* blueprint);

    virtual ~operation() = default;

    virtual value_variant execute() = 0;
};

template <template <typename> class Operator, typename Type>
struct fundamental_operation final : public operation
{
    fundamental_operation(function* blueprint)
        : operation(blueprint)
    {
        for (size_t i = 0; i < 2; ++i)
        {
            if (!holds<Type>(blueprint->parameters.at(i)))
            {
                throw std::invalid_argument("operation blueprint invalid parameter type");
            }
        }
    }

    value_variant execute()
    {
        Operator<Type> op;
        Type const& arg0 = get<Type>(blueprint->parameters.at(0));
        Type const& arg1 = get<Type>(blueprint->parameters.at(1));
        return op(arg0, arg1);
    }
};

struct arithmetic_error : public std::runtime_error
{
    using runtime_error::runtime_error;
};

using std::plus;
using std::minus;
using std::multiplies;

template <typename T>
struct divides
{
    T operator()(T numerator, T denominator)
    {
        if (denominator == T{0})
        {
            throw arithmetic_error("division by zero");
        }
        return numerator / denominator;
    }
};

using std::equal_to;
using std::not_equal_to;
using std::greater;
using std::less;
using std::greater_equal;
using std::less_equal;

struct program
{
    std::vector<std::unique_ptr<function>> functions;
    std::vector<evaluation> evaluations;
};

value_variant execute(function& func);

value_variant execute(evaluation& eval);

structure execute(construction& ctor);

value_variant execute(expression& expr);

value_variant execute(condition& expr);

}  // namespace runtime
}  // namespace ant
