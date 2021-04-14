#include "compiler.hpp"

#include "formatting.hpp"

#include <cassert>
#include <sstream>

namespace ant
{

template <typename T>
bool is_success(compiler_result<T> const& result)
{
    return std::holds_alternative<T>(result);
}

template <typename T>
bool is_failure(compiler_result<T> const& result)
{
    return !is_success(result);
}

template bool is_success(compiler_result<runtime::value_variant> const&);

template bool is_failure(compiler_result<runtime::value_variant> const&);
template bool is_failure(compiler_result<runtime::value_variant*> const&);
template bool is_failure(compiler_result<std::unique_ptr<runtime::function>> const&);
template bool is_failure(compiler_result<std::unique_ptr<runtime::structure>> const&);
template bool is_failure(compiler_result<std::unique_ptr<runtime::evaluation>> const&);

template <typename T>
T& get_success(compiler_result<T>& result)
{
    return std::get<T>(result);
}

template <typename T>
T const& get_success(compiler_result<T> const& result)
{
    return std::get<T>(result);
}

template <typename T>
compiler_failure& get_failure(compiler_result<T>& result)
{
    return std::get<compiler_failure>(result);
}

template <typename T>
compiler_failure const& get_failure(compiler_result<T> const& result)
{
    return std::get<compiler_failure>(result);
}

template compiler_failure const& get_failure(compiler_status const& status);

struct literal_compiler
{
    template <typename T>
    runtime::value_variant operator()(ast::literal<T> literal)
    {
        return literal.value;
    }
};

runtime::value_variant
compile(ast::literal_variant const& literal)
{
    return ast::visit(literal_compiler(), literal);
}

compiler_result<runtime::value_variant>
compile(compiler_environment const& env, ast::parameter const& param)
{
    auto it = env.functions.find(param.type);
    if (it == env.functions.end())
    {
        std::stringstream message;
        message << "Undefined parameter type " << quote(param.type);
        return compiler_failure{message.str(), param.context};
    }
    runtime::function* type = it->second;
    return get_evaluation_prototype(type->value);
}

compiler_result<runtime::value_variant*>
compile(compiler_scope const& scope, ast::reference const& ref)
{
    auto param = scope.parameters.find(ref.name);
    if (param == scope.parameters.end())
    {
        std::stringstream message;
        message << "Undefined reference to " << quote(ref.name);
        return compiler_failure{message.str(), ref.context};
    }
    return param->second;
}

struct expression_prototype_getter
{
    runtime::value_variant operator()(runtime::value_variant const& value) const
    {
        return value;
    }

    runtime::value_variant operator()(runtime::value_variant const* param) const
    {
        return *param;
    }

    runtime::value_variant operator()(std::unique_ptr<runtime::evaluation> const& eval) const
    {
        return get_evaluation_prototype(eval->blueprint->value);
    }

    runtime::value_variant operator()(std::unique_ptr<runtime::construction> const& ctor) const
    {
        return runtime::execute(*ctor);
    }

    runtime::value_variant operator()(std::unique_ptr<runtime::condition> const& cond) const
    {
        assert(!cond->branches.empty());
        return get_evaluation_prototype(cond->branches.front().second);
    }
};

runtime::value_variant
get_evaluation_prototype(runtime::expression const& expr)
{
    return std::visit(expression_prototype_getter(),
                      static_cast<runtime::expression_base const&>(expr));
}

bool expression_type_matches(
    runtime::expression const& expr1,
    runtime::expression const& expr2)
{
    const runtime::value_variant proto1 = get_evaluation_prototype(expr1);
    const runtime::value_variant proto2 = get_evaluation_prototype(expr2);
    return proto1.index() == proto2.index();
}

compiler_result<std::unique_ptr<runtime::evaluation>>
compile(compiler_environment const& env,
        compiler_scope const& scope,
        ast::evaluation const& eval)
{
    auto it = env.functions.find(eval.function);

    if (it == env.functions.end())
    {
        std::stringstream message;
        message << "Call to undefined function " << quote(eval.function);
        return compiler_failure{message.str(), eval.context};
    }

    runtime::function* func = it->second;

    if (eval.arguments.size() != func->parameters.size())
    {
        std::stringstream message;
        message << "Function call argument length does not match function parameter length";
        return compiler_failure{message.str(), eval.context};
    }

    auto result = std::make_unique<runtime::evaluation>(func);

    for (size_t i = 0; i < eval.arguments.size(); ++i)
    {
        const ast::expression& arg = eval.arguments.at(i);
        compiler_result<runtime::expression> expr = compile(env, scope, arg);
        if (is_success(expr))
        {
            runtime::expression& argument = get_success(expr);
            runtime::expression const& parameter = func->parameters.at(i);
            if (expression_type_matches(argument, parameter))
            {
                result->arguments.at(i) = std::move(argument);
            }
            else
            {
                return compiler_failure{
                    "Function call argument/parameter type mismatch",
                     ast::get_context(arg)
                };
            }
        }
        else
        {
            return std::move(get_failure(expr));
        }
    }

    return std::move(result);
}

compiler_result<std::unique_ptr<runtime::condition>>
compile(compiler_environment const& env,
        compiler_scope const& scope,
        ast::condition const& cond)
{
    if (cond.branches.size() < 2)
    {
        return compiler_failure{"Condition have less than 2 branches", cond.context};
    }

    auto compiled_condition = std::make_unique<runtime::condition>();
    compiled_condition->branches.reserve(cond.branches.size());

    auto compile_branch = [&env, &scope](auto const& branch)
    {
        auto check = compile(env, scope, branch.check);
        auto value = compile(env, scope, branch.value);
        return std::make_pair(std::move(check), std::move(value));
    };

    runtime::value_variant result_type;

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

        runtime::expression& check_expr = get_success(check);

        if (!expression_type_matches(check_expr, bool{}))
        {
            return compiler_failure{
                "Condition branch check expression must be of boolean type",
                 ast::get_context(branch.check)
            };
        }

        runtime::expression& value_expr = get_success(value);

        if (i == 0)
        {
            result_type = get_evaluation_prototype(value_expr);
        }
        else if (!expression_type_matches(value_expr, result_type))
        {
            return compiler_failure{
                "Conflicting result type of conditional expression",
                 ast::get_context(branch.value)
            };
        }

        compiled_condition->branches.emplace_back(std::move(check_expr), std::move(value_expr));
    }

    return std::move(compiled_condition);
}

struct expression_compiler
{
    compiler_environment const& env;
    compiler_scope const& scope;

    compiler_result<runtime::expression>
    operator()(ast::reference const& ref)
    {
        compiler_result<runtime::value_variant*> result = compile(scope, ref);
        if (is_success(result))
        {
            return std::move(get_success(result));
        }
        else
        {
            return std::move(get_failure(result));
        }
    }

    compiler_result<runtime::expression>
    operator()(ast::literal_variant const& lit)
    {
        return compile(lit);
    }

    compiler_result<runtime::expression>
    operator()(ast::evaluation const& eval)
    {
        compiler_result<std::unique_ptr<runtime::evaluation>> result = compile(env, scope, eval);
        if (is_success(result))
        {
            return std::move(get_success(result));
        }
        else
        {
            return std::move(get_failure(result));
        }
    }

    compiler_result<runtime::expression>
    operator()(ast::condition const& cond)
    {
        compiler_result<std::unique_ptr<runtime::condition>> result = compile(env, scope, cond);
        if (is_success(result))
        {
            return std::move(get_success(result));
        }
        else
        {
            return std::move(get_failure(result));
        }
    }
};

compiler_result<runtime::expression>
compile(compiler_environment const& env,
        compiler_scope const& scope,
        ast::expression const& expr)
{
    return ast::visit(expression_compiler{env, scope}, expr);
}

compiler_result<std::unique_ptr<runtime::function>>
compile(compiler_environment const& env, ast::function const& function)
{
    auto it = env.functions.find(function.name);

    if (it != env.functions.end())
    {
        std::stringstream message;
        message << "Redefinition of function " << quote(function.name);
        return compiler_failure{message.str(), function.context};
    }

    it = env.functions.find(function.return_type.name);

    if (it == env.functions.end())
    {
        std::stringstream message;
        message << "Unknown function return type " << quote(function.return_type.name);
        return compiler_failure{message.str(), function.return_type.context};
    }

    runtime::function const* return_type = it->second;

    auto result = std::make_unique<runtime::function>();
    result->parameters.reserve(function.parameters.size());

    for (const auto& param : function.parameters)
    {
        auto compiled_param = compile(env, param);
        if (is_success(compiled_param))
        {
            result->parameters.push_back(std::move(get_success(compiled_param)));
        }
        else
        {
            return std::move(get_failure(compiled_param));
        }
    }

    compiler_scope scope;

    for (size_t i = 0; i < result->parameters.size(); ++i)
    {
        std::string param_name = function.parameters.at(i).name;
        scope.parameters[param_name] = &result->parameters.at(i);
    }

    auto compiled_value = compile(env, scope, function.body);

    if (is_success(compiled_value))
    {
        result->value = std::move(get_success(compiled_value));
    }
    else
    {
        return std::move(get_failure(compiled_value));
    }

    if (!expression_type_matches(result->value, return_type->value))
    {
        return compiler_failure{
            "Function expression type does not match declared return type",
             ast::get_context(function.body)
        };
    }

    return std::move(result);
}

compiler_result<std::unique_ptr<runtime::function>>
compile(compiler_environment const& env, ast::structure  const& structure)
{
    auto it = env.functions.find(structure.name);

    if (it != env.functions.end())
    {
        std::stringstream message;
        message << "Redefinition of structure " << structure.name;
        return compiler_failure{message.str(), structure.context};
    }

    auto constructor = std::make_unique<runtime::function>();
    constructor->parameters.reserve(structure.fields.size());

    for (const ast::parameter& field : structure.fields)
    {
        compiler_result<runtime::value_variant> compiled = compile(env, field);
        if (is_success(compiled))
        {
            constructor->parameters.push_back(std::move(get_success(compiled)));
        }
        else
        {
            return std::move(get_failure(compiled));
        }
    }

    // bootstrap the type system!
    constructor->value = std::make_unique<runtime::construction>(constructor.get());

    return std::move(constructor);
}

struct statement_compiler
{
    compiler_environment& env;
    runtime::program& program;

    compiler_status operator()(ast::function const& function)
    {
        compiler_result<std::unique_ptr<runtime::function>> result = compile(env, function);
        if (is_success(result))
        {
            std::unique_ptr<runtime::function> blueprint = std::move(get_success(result));
            env.functions[function.name] = blueprint.get();
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
        compiler_result<std::unique_ptr<runtime::function>> result = compile(env, structure);
        if (is_success(result))
        {
            std::unique_ptr<runtime::function> constructor = std::move(get_success(result));
            env.functions[structure.name] = constructor.get();
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
        compiler_result<std::unique_ptr<runtime::evaluation>> result = compile(env, {}, eval);
        if (is_success(result))
        {
            program.evaluations.push_back(std::move(get_success(result)));
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
    return std::visit(statement_compiler{env, prog}, statement);
}

std::vector<compiler_status>
compile(runtime::program& prog,
        compiler_environment& env,
        ast::program const& statements)
{
    std::vector<compiler_status> summary;
    summary.reserve(statements.size());
    for (const auto& statement : statements)
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
std::unique_ptr<runtime::function>
make_fundamental_type()
{
    auto type = std::make_unique<runtime::function>();
    type->value = T{};
    return std::move(type);
};

template <typename T>
void add_fundamental_type(compiler_environment& env, runtime::program& prog)
{
    prog.functions.push_back(make_fundamental_type<T>());
    env.functions[ast::name_of<ast::literal<T>>::value] = prog.functions.back().get();
}

std::pair<compiler_environment, runtime::program>
setup_compiler()
{
    compiler_environment env;
    runtime::program prog;
    add_fundamental_type<int8_t>(env, prog);
    add_fundamental_type<int16_t>(env, prog);
    add_fundamental_type<int32_t>(env, prog);
    add_fundamental_type<int64_t>(env, prog);
    add_fundamental_type<uint8_t>(env, prog);
    add_fundamental_type<uint16_t>(env, prog);
    add_fundamental_type<uint32_t>(env, prog);
    add_fundamental_type<uint64_t>(env, prog);
    add_fundamental_type<flt32_t>(env, prog);
    add_fundamental_type<flt64_t>(env, prog);
    return {std::move(env), std::move(prog)};
};

}  // namespace ant
