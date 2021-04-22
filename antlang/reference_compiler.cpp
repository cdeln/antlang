#include "compiler.hpp"

#include "formatting.hpp"

#include <sstream>

namespace ant
{

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

}  // namespace ant
