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

struct literal_compiler
{
    compiler_environment const& env;

    template <typename T>
    compiler_expect<runtime::value_variant>
    operator()(ast::literal<T> literal)
    {
        constexpr auto type_name = ast::name_of_v<ast::literal<T>>;
        auto it = env.prototypes.find(type_name);
        if (it == env.prototypes.end())
        {
            std::stringstream message;
            message << "Missing prototype for literal " << quote(type_name);
            return compiler_failure{message.str(), literal.context};
        }
        return compiler_result<runtime::value_variant>{literal.value, type_name};
    }
};

compiler_expect<runtime::value_variant>
compile(compiler_environment const& env, ast::literal_variant const& literal)
{
    return visit(literal_compiler{env}, literal);
}

compiler_expect<runtime::value_variant>
compile(compiler_environment const& env, ast::parameter const& param)
{
    auto it = env.prototypes.find(param.type);
    if (it == env.prototypes.end())
    {
        std::stringstream message;
        message << "Undefined parameter type " << quote(param.type);
        return compiler_failure{message.str(), param.context};
    }
    auto const& prototype = it->second;
    return compiler_result<runtime::value_variant>{*prototype, param.type};
}

compiler_expect<runtime::value_variant*>
compile(compiler_scope const& scope, ast::reference const& ref)
{
    auto it = scope.parameters.find(ref.name);
    if (it == scope.parameters.end())
    {
        std::stringstream message;
        message << "Undefined reference to " << quote(ref.name);
        return compiler_failure{message.str(), ref.context};
    }
    return it->second;
}

compiler_expect<runtime::evaluation>
compile(compiler_environment const& env,
        compiler_scope const& scope,
        ast::evaluation const& eval)
{
    std::vector<compiler_result<runtime::expression>> arguments;
    arguments.reserve(eval.arguments.size());

    for (const auto& arg : eval.arguments)
    {
        auto expr = compile(env, scope, arg);
        if (is_success(expr))
        {
            arguments.push_back(std::move(get_success(expr)));
        }
        else
        {
            return std::move(get_failure(expr));
        }
    }

    std::vector<std::string> signature;
    signature.reserve(eval.arguments.size());
    std::transform(arguments.begin(), arguments.end(),
                   std::back_inserter(signature),
                   [](auto const& arg) { return arg.type; });
    auto func_query = find_function(env, scope, eval.function, signature);
    if (is_failure(func_query))
    {
        std::stringstream message;
        message << "Could not find function " << quote(eval.function)
                << " with parameter type signature (";
        if (!signature.empty())
        {
            for (size_t i = 0; i < signature.size() - 1; ++i)
            {
                message << signature.at(i) << " ";
            }
            message << signature.back();
        }
        message << ")";
        return compiler_failure{message.str(), eval.context};
    }
    auto& [return_type, func_ptr] = get_success(func_query);

    auto result = runtime::evaluation(func_ptr);
    std::transform(std::move_iterator(arguments.begin()),
                   std::move_iterator(arguments.end()),
                   result.arguments.begin(),
                   [](auto&& arg) { return std::move(arg.value); });

    return compiler_result<runtime::evaluation>{
        std::move(result),
        return_type
    };
}

compiler_expect<runtime::condition>
compile(compiler_environment const& env,
        compiler_scope const& scope,
        ast::condition const& cond)
{
    if (cond.branches.empty())
    {
        return compiler_failure{"Condition must have at least 1 branch", cond.context};
    }

    runtime::condition compiled_condition;
    compiled_condition.branches.reserve(cond.branches.size());

    auto compile_branch = [&env, &scope](auto const& branch)
    {
        auto check = compile(env, scope, branch.check);
        auto value = compile(env, scope, branch.value);
        return std::make_pair(std::move(check), std::move(value));
    };

    std::string result_type;

    for (size_t i = 0; i < cond.branches.size(); ++i)
    {
        auto const& branch = cond.branches.at(i);
        auto [check, value] = compile_branch(branch);

        if (is_failure(check))
        {
            return get_failure(check);
        }
        if (is_failure(value))
        {
            return get_failure(value);
        }

        auto& [check_expr, check_type] = get_success(check);

        if (check_type != ast::name_of_v<ast::literal<bool>>)
        {
            std::stringstream message;
            message << "Condition branch check expression must be of type "
                    << quote(ast::name_of_v<ast::literal<bool>>)
                    << ", but is of type " << quote(check_type);
            return compiler_failure{
                message.str(),
                ast::get_context(branch.check)
            };
        }

        auto& [value_expr, value_type] = get_success(value);

        if (i == 0)
        {
            result_type = value_type;
        }
        else if (value_type != result_type)
        {
            return compiler_failure{
                "Conflicting result type of conditional expression",
                 ast::get_context(branch.value)
            };
        }

        compiled_condition.branches.push_back({std::move(check_expr), std::move(value_expr)});
    }

    auto fallback = compile(env, scope, cond.fallback);

    if (is_failure(fallback))
    {
        return std::move(get_failure(fallback));
    }

    auto& [fallback_expr, fallback_type] = get_success(fallback);

    if (fallback_type != result_type)
    {
        return compiler_failure{
            "Conflicting result type of conditional expression",
             ast::get_context(cond.fallback)
        };
    }

    compiled_condition.fallback = std::move(fallback_expr);

    return compiler_result<runtime::condition>{
        std::move(compiled_condition),
        result_type
    };
}

struct expression_compiler
{
    compiler_environment const& env;
    compiler_scope const& scope;

    compiler_expect<runtime::expression>
    operator()(ast::reference const& ref)
    {
        compiler_expect<runtime::value_variant*> result = compile(scope, ref);
        if (is_success(result))
        {
            auto& [value, type] = get_success(result);
            return compiler_result<runtime::expression>{std::move(value), std::move(type)};
        }
        else
        {
            return std::move(get_failure(result));
        }
    }

    compiler_expect<runtime::expression>
    operator()(ast::literal_variant const& literal)
    {
        compiler_expect<runtime::value_variant> result = compile(env, literal);
        if (is_success(result))
        {
            auto& [value, type] = get_success(result);
            return compiler_result<runtime::expression>{std::move(value), std::move(type)};
        }
        else
        {
            return std::move(get_failure(result));
        }
    }

    compiler_expect<runtime::expression>
    operator()(ast::evaluation const& eval)
    {
        compiler_expect<runtime::evaluation> result = compile(env, scope, eval);
        if (is_success(result))
        {
            auto& [value, type] = get_success(result);
            return compiler_result<runtime::expression>{std::move(value), std::move(type)};
        }
        else
        {
            return std::move(get_failure(result));
        }
    }

    compiler_expect<runtime::expression>
    operator()(ast::condition const& cond)
    {
        compiler_expect<runtime::condition> result = compile(env, scope, cond);
        if (is_success(result))
        {
            auto& [value, type] = get_success(result);
            return compiler_result<runtime::expression>{std::move(value), std::move(type)};
        }
        else
        {
            return std::move(get_failure(result));
        }
    }
};

compiler_expect<runtime::expression>
compile(compiler_environment const& env,
        compiler_scope const& scope,
        ast::expression const& expr)
{
    return visit(expression_compiler{env, scope}, expr);
}

exceptional<compiled_function_result, compiler_failure>
compile(compiler_environment const& env,
        ast::function const& function)
{
    if (env.prototypes.find(function.return_type.name) == env.prototypes.end())
    {
        std::stringstream message;
        message << "Unknown function return type " << quote(function.return_type.name);
        return compiler_failure{message.str(), function.return_type.context};
    }

    std::vector<std::string> signature;

    auto result = std::make_unique<runtime::function>();
    result->parameters.reserve(function.parameters.size());

    for (const auto& param : function.parameters)
    {
        auto compiled_param = compile(env, param);
        if (is_success(compiled_param))
        {
            auto& [value, type] = get_success(compiled_param);
            signature.push_back(std::move(type));
            result->parameters.push_back(std::move(value));
        }
        else
        {
            return std::move(get_failure(compiled_param));
        }
    }

    auto func_query = find_function(env, function.name, signature);

    if (is_success(func_query))
    {
        std::stringstream message;
        message << "Redefinition of function " << quote(function.name);
        return compiler_failure{message.str(), function.context};
    }

    compiler_scope scope;
    scope.function = {function.name, function.return_type.name, signature, result.get()};
    for (size_t i = 0; i < result->parameters.size(); ++i)
    {
        std::string param_name = function.parameters.at(i).name;
        scope.parameters[param_name] = {&result->parameters.at(i), signature.at(i)};
    }
    auto compiled_expr = compile(env, scope, function.body);
    if (is_success(compiled_expr))
    {
        auto& [value_expr, value_type] = get_success(compiled_expr);
        if (value_type != function.return_type.name)
        {
            std::stringstream message;
            message << "Function expression type " << quote(value_type)
                    << " does not match declared return type " << quote(function.return_type.name);
            return compiler_failure{
                message.str(),
                ast::get_context(function.body)
            };
        }
        result->value = std::move(value_expr);
    }
    else
    {
        return std::move(get_failure(compiled_expr));
    }

    return compiled_function_result{
        function_meta{
            function.return_type.name,
            std::move(signature)
        },
        std::move(result)
    };
}

std::unique_ptr<runtime::function>
make_constructor(runtime::structure const& prototype)
{
    auto constructor = std::make_unique<runtime::function>();
    constructor->parameters = prototype.fields;
    // bootstrap the type system!
    constructor->value = runtime::construction(constructor.get());
    return std::move(constructor);
}

function_meta
make_constructor_meta(ast::structure const& structure)
{
    function_meta meta;
    meta.return_type = structure.name;
    meta.parameter_types.reserve(structure.fields.size());
    std::transform(structure.fields.begin(), structure.fields.end(),
                   std::back_inserter(meta.parameter_types),
                   [](auto const& field) { return field.type; });
    return meta;
}

exceptional<std::unique_ptr<runtime::structure>, compiler_failure>
compile(compiler_environment const& env, ast::structure  const& structure)
{
    auto it = env.prototypes.find(structure.name);

    if (it != env.prototypes.end())
    {
        std::stringstream message;
        message << "Redefinition of structure " << structure.name;
        return compiler_failure{message.str(), structure.context};
    }

    auto prototype = std::make_unique<runtime::structure>();
    prototype->fields.reserve(structure.fields.size());

    for (const ast::parameter& field : structure.fields)
    {
        compiler_expect<runtime::value_variant> compiled = compile(env, field);
        if (is_success(compiled))
        {
            prototype->fields.push_back(std::move(get_success(compiled).value));
        }
        else
        {
            return std::move(get_failure(compiled));
        }
    }

    return std::move(prototype);
}

struct statement_compiler
{
    compiler_environment& env;
    runtime::program& program;

    compiler_status operator()(ast::function const& function)
    {
        auto result = compile(env, function);
        if (is_success(result))
        {
            auto [meta, blueprint] = std::move(get_success(result));
            env.functions[function.name].push_back({std::move(meta), blueprint.get()});
            program.functions.push_back(std::move(blueprint));
            return compiler_success{function.name};
        }
        else
        {
            return std::move(get_failure(result));
        }
    }

    compiler_status operator()(ast::structure const& structure)
    {
        auto result = compile(env, structure);
        if (is_success(result))
        {
            std::unique_ptr<runtime::structure> prototype = std::move(get_success(result));
            std::unique_ptr<runtime::function> constructor = make_constructor(*prototype);
            function_meta meta = make_constructor_meta(structure);
            env.prototypes[structure.name] = std::make_unique<runtime::value_variant>(*prototype);
            env.functions[structure.name].push_back({std::move(meta), constructor.get()});
            program.functions.push_back(std::move(constructor));
            return compiler_success{structure.name};
        }
        else
        {
            return std::move(get_failure(result));
        }
    }

    compiler_status operator()(ast::evaluation const& eval)
    {
        compiler_expect<runtime::evaluation> result = compile(env, {}, eval);
        if (is_success(result))
        {
            program.evaluations.push_back(std::move(get_success(result).value));
            return compiler_success{};
        }
        else
        {
            return std::move(get_failure(result));
        }
    }
};

compiler_status
compile(runtime::program& prog,
        compiler_environment& env,
        ast::statement const& statement)
{
    return visit(statement_compiler{env, prog}, statement);
}

std::vector<compiler_status>
compile(runtime::program& prog,
        compiler_environment& env,
        ast::program const& ast)
{
    std::vector<compiler_status> summary;
    summary.reserve(ast.statements.size());
    for (const auto& statement : ast.statements)
    {
        summary.push_back(compile(prog, env, statement));
        if (is_failure(summary.back()))
        {
            break;
        }
    }
    summary.shrink_to_fit();
    return summary;
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
    std::unique_ptr<runtime::operation> value =
        std::make_unique<runtime::fundamental_operation<Operator, Type>>(op.get());
    op->value = std::move(value);

    prog.functions.push_back(std::move(op));

    function_meta meta;
    meta.return_type = ast::name_of_v<ast::literal<ReturnType>>;
    const std::string operand_type = ast::name_of_v<ast::literal<Type>>;
    meta.parameter_types = {operand_type, operand_type};

    env.functions[name].push_back({std::move(meta), prog.functions.back().get()});
}

template <
    template <typename> class Operator,
    typename First,
    typename... Rest
>
struct operation_builder
{
    compiler_environment& env;
    runtime::program& prog;

    void build(std::string const& name)
    {
        add_fundamental_operation<Operator, First>(env, prog, name);
        operation_builder<Operator, Rest...>{env, prog}.build(name);
    }
};

template <template <typename> class Operator, typename Last>
struct operation_builder<Operator, Last>
{
    compiler_environment& env;
    runtime::program& prog;

    void build(std::string const& name)
    {
        add_fundamental_operation<Operator, Last>(env, prog, name);
    }
};


template <
    template <typename> class Operator,
    template <typename...> class TypeList,
    typename... Types
>
struct operation_builder<Operator, TypeList<Types...>>
{
    compiler_environment& env;
    runtime::program& prog;

    void build(std::string const& name)
    {
        operation_builder<Operator, Types...>{env, prog}.build(name);
    }
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

    using arithmetic_types =
        std::tuple<
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
        >;

    operation_builder<runtime::plus,          arithmetic_types>{env, prog}.build("+");
    operation_builder<runtime::minus,         arithmetic_types>{env, prog}.build("-");
    operation_builder<runtime::multiplies,    arithmetic_types>{env, prog}.build("*");
    operation_builder<runtime::divides,       arithmetic_types>{env, prog}.build("/");

    operation_builder<runtime::equal_to,      arithmetic_types>{env, prog}.build("=");
    operation_builder<runtime::not_equal_to,  arithmetic_types>{env, prog}.build("!=");
    operation_builder<runtime::greater,       arithmetic_types>{env, prog}.build(">");
    operation_builder<runtime::less,          arithmetic_types>{env, prog}.build("<");
    operation_builder<runtime::greater_equal, arithmetic_types>{env, prog}.build(">=");
    operation_builder<runtime::less_equal,    arithmetic_types>{env, prog}.build("<=");

    return {std::move(env), std::move(prog)};
};

}  // namespace ant
