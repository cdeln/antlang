#include "compiler.hpp"

#include "formatting.hpp"

#include <sstream>

namespace ant
{

exceptional<std::unique_ptr<runtime::structure>, compiler_failure>
compile(compiler_environment const& env, ast::structure  const& structure)
{
    auto it = env.prototypes.find(structure.name);

    if (it != env.prototypes.end())
    {
        std::stringstream message;
        message << "Redefinition of structure " << quote(structure.name);
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

}  // namespace ant
