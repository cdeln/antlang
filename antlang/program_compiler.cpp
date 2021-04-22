#include "compiler.hpp"

#include "formatting.hpp"

#include <sstream>

namespace ant
{

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

}  // namespace ant
