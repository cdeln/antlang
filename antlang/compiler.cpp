#include "compiler.hpp"

#include "formatting.hpp"

#include <cassert>
#include <sstream>

namespace ant
{

exceptional<function_query_result, nullptr_t>
find_function(compiler_environment const& env,
              std::string const& name,
              std::vector<std::string> const& signature)
{
    auto it = env.functions.find(name);
    if (it == env.functions.end())
    {
        return nullptr;
    }
    for (auto const& [meta, func] : it->second)
    {
        if (meta.parameter_types == signature)
        {
            return function_query_result{meta.return_type, func};
        }
    }
    return nullptr;
}

exceptional<function_query_result, nullptr_t>
find_function(compiler_environment const& env,
              compiler_scope const& scope,
              std::string const& name,
              std::vector<std::string> const& signature)
{
    if ((scope.function.name == name) && (scope.function.signature == signature))
    {
        return function_query_result{
            scope.function.return_type,
            scope.function.pointer
        };
    }
    return find_function(env, name, signature);
}

template <typename T>
void add_fundamental_type(compiler_environment& env)
{
    env.prototypes[ast::name_of_v<ast::literal<T>>] = std::make_unique<runtime::value_variant>(T{});
}

template <template <typename> class Operator, typename Type>
void add_fundamental_operation(
        compiler_environment& env,
        runtime::program& prog,
        std::string const& name)
{
    using ReturnType = decltype(Operator<Type>{}(std::declval<Type>(), std::declval<Type>()));

    auto op = std::make_unique<runtime::function>();
    op->parameters = {Type{}, Type{}};
    op->value = runtime::make_binary_operator<Operator, Type>(op.get());

    prog.functions.push_back(std::move(op));

    function_meta meta;
    meta.return_type = ast::name_of_v<ast::literal<ReturnType>>;
    const std::string operand_type = ast::name_of_v<ast::literal<Type>>;
    meta.parameter_types = {operand_type, operand_type};

    env.functions[name].push_back({std::move(meta), prog.functions.back().get()});
}

template <typename... Ts>
struct type_list
{
};

template <template <typename> class Operator, typename... Types>
constexpr void add_fundamental_operations(
    compiler_environment& env,
    runtime::program& prog,
    std::string const& name,
    type_list<Types...>)
{
    (add_fundamental_operation<Operator, Types>(env, prog, name), ...);
};

std::pair<compiler_environment, runtime::program>
setup_compiler()
{
    compiler_environment env;
    runtime::program prog;

    add_fundamental_type<bool>(env);
    add_fundamental_type<int8_t>(env);
    add_fundamental_type<int16_t>(env);
    add_fundamental_type<int32_t>(env);
    add_fundamental_type<int64_t>(env);
    add_fundamental_type<uint8_t>(env);
    add_fundamental_type<uint16_t>(env);
    add_fundamental_type<uint32_t>(env);
    add_fundamental_type<uint64_t>(env);
    add_fundamental_type<flt32_t>(env);
    add_fundamental_type<flt64_t>(env);

    constexpr auto arithmetic_types =
        type_list<
            int8_t,
            int16_t,
            int32_t,
            int64_t,
            uint8_t,
            uint16_t,
            uint32_t,
            uint64_t,
            flt32_t,
            flt64_t
        >();

    add_fundamental_operations<runtime::plus      >(env, prog, "+", arithmetic_types);
    add_fundamental_operations<runtime::minus     >(env, prog, "-", arithmetic_types);
    add_fundamental_operations<runtime::multiplies>(env, prog, "*", arithmetic_types);
    add_fundamental_operations<runtime::divides   >(env, prog, "/", arithmetic_types);

    add_fundamental_operations<runtime::equal_to     >(env, prog, "=", arithmetic_types);
    add_fundamental_operations<runtime::not_equal_to >(env, prog, "!=", arithmetic_types);
    add_fundamental_operations<runtime::greater      >(env, prog, ">", arithmetic_types);
    add_fundamental_operations<runtime::less         >(env, prog, "<", arithmetic_types);
    add_fundamental_operations<runtime::greater_equal>(env, prog, ">=", arithmetic_types);
    add_fundamental_operations<runtime::less_equal   >(env, prog, "<=", arithmetic_types);

    return {std::move(env), std::move(prog)};
};

}  // namespace ant
