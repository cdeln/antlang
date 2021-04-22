#include "compiler.hpp"

namespace ant
{

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

}  // namespace ant
