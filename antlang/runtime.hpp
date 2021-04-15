#pragma once

#include "fundamental_types.hpp"

#include <algorithm>
#include <cstdint>
#include <memory>
#include <variant>
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
    std::variant<
        bool,
        int8_t,  int16_t,  int32_t,  int64_t,
        uint8_t, uint16_t, uint32_t, uint64_t,
        flt32_t, flt64_t,
        structure
    >;

struct value_variant : value_variant_base
{
    using value_variant_base::value_variant_base;
};

struct evaluation;
struct construction;
struct condition;

using expression_base =
    std::variant<
        value_variant,
        value_variant*,
        std::unique_ptr<evaluation>,
        std::unique_ptr<construction>,
        std::unique_ptr<condition>
    >;

struct expression : expression_base
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

struct construction
{
    function* prototype;

    construction(function* prototype)
        : prototype{prototype}
    {
    }
};

struct condition
{
    std::vector<std::pair<expression, expression>> branches;
};

struct program
{
    std::vector<std::unique_ptr<function>> functions;
    std::vector<std::unique_ptr<evaluation>> evaluations;
};

value_variant execute(evaluation& eval);

structure execute(construction& ctor);

value_variant execute(expression& expr);

value_variant execute(condition& expr);

}  // namespace runtime
}  // namespace ant
