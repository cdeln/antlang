#pragma once

#include "rules.hpp"

namespace ant
{

template <class Value, class Pattern>
struct match :
    rule_spec<
        match<Value, Pattern>,
        Value
    >
{
};

} // namespacea nt
