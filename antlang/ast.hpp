#pragma once

#include "fundamental_types.hpp"
#include "tokens.hpp"
#include "recursive_variant.hpp"

#include <string>
#include <vector>

namespace ant
{
namespace ast
{

template <typename T>
token_context
get_context(T&& x)
{
    return x.context;
}

template <typename T>
token_context
get_context(recursive_wrapper<T> const& x)
{
    return get_context(static_cast<T const&>(x));
}

template <typename... Ts>
token_context
get_context(recursive_variant<Ts...> const& v)
{
    return visit([](auto const& x) { return get_context(x); }, v);
}

template <typename T>
struct literal
{
    using value_type = T;
    value_type value;
    token_context context;
};

using boolean = literal<bool>;

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
    recursive_variant<
        boolean,
        i8, i16, i32, i64,
        u8, u16, u32, u64,
        f32, f64
    >;

struct parameter
{
    std::string type;
    std::string name;
    token_context context;
};

struct reference
{
    std::string name;
    token_context context;
};

struct branch;
struct condition;
struct binding;
struct scope;
struct evaluation;

using expression =
    recursive_variant<
        reference,
        literal_variant,
        evaluation,
        condition,
        scope
    >;

struct evaluation
{
    std::string function;
    std::vector<expression> arguments;
    token_context context;
};

struct condition
{
    std::vector<branch> branches;
    recursive_wrapper<expression> fallback;
    token_context context;
};

struct scope
{
    std::vector<binding> bindings;
    recursive_wrapper<expression> value;
    token_context context;
};

struct branch
{
    expression check;
    expression value;
};

struct binding
{
    std::string name;
    expression value;
    token_context context;
};

struct function
{
    std::string name;
    reference return_type;
    std::vector<parameter> parameters;
    expression body;
    token_context context;
};

struct structure
{
    std::string name;
    std::vector<parameter> fields;
    token_context context;
};

using statement =
    recursive_variant<
        evaluation,
        function,
        structure
    >;

struct program
{
    std::vector<statement> statements;
};

template <typename T, typename = void>
struct has_name : std::false_type {};

template <typename T>
struct has_name<T, std::void_t<decltype(T::name)>> : std::true_type {};

template <typename T, bool HasName = has_name<T>::value>
struct name_of
{
    static constexpr auto const value = T::name;
};

template <typename T>
struct name_of<T, false>
{
    static constexpr auto const value = "anonymous";
};

template <> struct name_of<boolean>  { static constexpr auto const value = "bool"; };
template <> struct name_of<i8>       { static constexpr auto const value = "i8";   };
template <> struct name_of<i16>      { static constexpr auto const value = "i16";  };
template <> struct name_of<i32>      { static constexpr auto const value = "i32";  };
template <> struct name_of<i64>      { static constexpr auto const value = "i64";  };
template <> struct name_of<u8>       { static constexpr auto const value = "u8";   };
template <> struct name_of<u16>      { static constexpr auto const value = "u16";  };
template <> struct name_of<u32>      { static constexpr auto const value = "u32";  };
template <> struct name_of<u64>      { static constexpr auto const value = "u64";  };
template <> struct name_of<f32>      { static constexpr auto const value = "f32";  };
template <> struct name_of<f64>      { static constexpr auto const value = "f64";  };

template <> struct name_of<literal_variant> { static constexpr auto value = "literal";    };
template <> struct name_of<reference>       { static constexpr auto value = "reference";  };
template <> struct name_of<parameter>       { static constexpr auto value = "parameter";  };
template <> struct name_of<evaluation>      { static constexpr auto value = "evaluation"; };
template <> struct name_of<expression>      { static constexpr auto value = "expression"; };
template <> struct name_of<function>        { static constexpr auto value = "function";   };
template <> struct name_of<branch>          { static constexpr auto value = "branch";     };
template <> struct name_of<condition>       { static constexpr auto value = "condition";  };
template <> struct name_of<binding>         { static constexpr auto value = "binding";    };
template <> struct name_of<scope>           { static constexpr auto value = "scope";      };
template <> struct name_of<structure>       { static constexpr auto value = "structure";  };
template <> struct name_of<statement>       { static constexpr auto value = "statement";  };
template <> struct name_of<program>         { static constexpr auto value = "program";    };

template <typename T>
constexpr auto name_of_v = name_of<T>::value;

} // namespace ast
} // namespace ant
