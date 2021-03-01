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
    using value_type = T;
    value_type value;
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

template <class Literal>
struct name_of;

template <> struct name_of<i8>  { static constexpr auto const value = "i8"; };
template <> struct name_of<i16> { static constexpr auto const value = "i16"; };
template <> struct name_of<i32> { static constexpr auto const value = "i32"; };
template <> struct name_of<i64> { static constexpr auto const value = "i64"; };
template <> struct name_of<u8>  { static constexpr auto const value = "u8"; };
template <> struct name_of<u16> { static constexpr auto const value = "u16"; };
template <> struct name_of<u32> { static constexpr auto const value = "u32"; };
template <> struct name_of<u64> { static constexpr auto const value = "u64"; };
template <> struct name_of<f32> { static constexpr auto const value = "f32"; };
template <> struct name_of<f64> { static constexpr auto const value = "f64"; };

template <class Literal>
constexpr auto const name_of_v = name_of<Literal>::value;

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
