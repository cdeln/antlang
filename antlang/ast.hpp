#pragma once

#include "fundamental_types.hpp"

#include <string>
#include <variant>
#include <vector>

namespace ant
{
namespace ast
{

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

using literal_variant =
    std::variant<
        i8, i16, i32, i64,
        u8, u16, u32, u64,
        f32, f64
    >;

struct parameter
{
    std::string type;
    std::string name;
};

struct evaluation;

using expression =
    std::variant<
        std::string,
        literal_variant,
        evaluation
    >;

struct evaluation
{
    std::string function;
    std::vector<expression> arguments;
};


struct function
{
    std::string name;
    std::string return_type;
    std::vector<parameter> parameters;
    expression body;
};

struct structure
{
    std::string name;
    std::vector<parameter> fields;
};

using statement =
    std::variant<
        function,
        structure
    >;

template <typename T>
struct name_of
{
    static constexpr auto const value = T::name;
};

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

template <> struct name_of<literal_variant> { static constexpr auto value = "literal";    };
template <> struct name_of<parameter>       { static constexpr auto value = "parameter";  };
template <> struct name_of<evaluation>      { static constexpr auto value = "evaluation"; };
template <> struct name_of<expression>      { static constexpr auto value = "expression"; };
template <> struct name_of<function>        { static constexpr auto value = "function";   };
template <> struct name_of<structure>       { static constexpr auto value = "structure";  };
template <> struct name_of<statement>       { static constexpr auto value = "statement";  };

template <typename T>
constexpr auto name_of_v = name_of<T>::value;

} // namespace ast
} // namespace ant
