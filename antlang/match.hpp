#pragma once

#include "rules.hpp"

namespace ant
{

template <class Value, class Pattern>
struct match :
    public rule<
        match<Value, Pattern>,
        Value
    >
{
};

} // namespacea nt
