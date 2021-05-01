#pragma once

#include "recursive_variant.hpp"
#include "rules.hpp"
#include "type_filters.hpp"

namespace ant
{

template <typename... Ts>
struct alternative :
    rule_spec<
        alternative<Ts...>,
        unique_t<
            recursive_variant,
            attribute_of_t<rule_of_t<Ts>>...
        >
    >
{
};

} // namespace ant
