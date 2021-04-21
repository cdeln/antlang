#include "tokens.hpp"

namespace ant
{

namespace
{

struct token_name_visitor
{
    template <typename TokenAlternative>
    std::string operator()(TokenAlternative const&)
    {
        return token_name<TokenAlternative>();
    }
};

} // namespace

std::string token_name(token_variant const& variant)
{
    return visit(token_name_visitor(), variant);
}

} // ant
