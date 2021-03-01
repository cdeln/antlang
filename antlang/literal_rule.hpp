#pragma once

#include "rules.hpp"

namespace ant
{

template <typename Literal, class Token>
struct literal_rule :
    public rule<
        literal_rule<Literal, Token>,
        Literal
    >
{
};

} // namespace ant
