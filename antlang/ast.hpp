#pragma once

#include "fundamental_types.hpp"

#include <string>
#include <variant>
#include <vector>

namespace ant
{
namespace ast
{

struct parameter
{
    std::string type;
    std::string name;
};

struct function
{
    std::string name;
    std::string return_type;
    std::vector<parameter> parameters;
};

struct structure
{
    std::string name;
    std::vector<parameter> fields;
};

template <typename T>
struct literal
{
    T value;
};

using i8  = literal<int8_t>;
using i16 = literal<int16_t>;
using i32 = literal<int32_t>;
using i64 = literal<int64_t>;

using u8  = literal<uint8_t>;
using u16 = literal<uint16_t>;
using u32 = literal<uint32_t>;
using u64 = literal<uint64_t>;

using f32 = literal<flt32_t>;
using f64 = literal<flt64_t>;

using literal_variant =
    std::variant
      < i8, i16, i32, i64
      , u8, u16, u32, u64
      , f32, f64 >;

struct evaluation;

using expression =
    std::variant
      < evaluation
      , std::string
      // , literal_variant
      >;

struct evaluation
{
    std::string function;
    std::vector<expression> arguments;
};

} // namespace ast
} // namespace ant
