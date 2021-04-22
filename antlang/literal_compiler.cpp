#include "compiler.hpp"

#include "formatting.hpp"

#include <sstream>

namespace ant
{

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

}  // namespace ant
