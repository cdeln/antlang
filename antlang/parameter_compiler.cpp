#include "compiler.hpp"

#include "formatting.hpp"

#include <sstream>

namespace ant
{

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

}  // namespace ant
