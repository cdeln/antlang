#pragma once

#include "recursive_variant.hpp"
#include "rules.hpp"

namespace ant
{

template <typename... Ts>
struct alternative :
    rule_spec<
        alternative<Ts...>,
        recursive_variant<
            attribute_of_t<rule_of_t<Ts>>...
        >
    >
{
};

} // namespace ant
