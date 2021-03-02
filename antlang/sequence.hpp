#pragma once

#include "rules.hpp"
#include "type_filters.hpp"

#include <tuple>

namespace ant
{

template <typename... Ts>
struct sequence :
    rule_spec<
        sequence<Ts...>,
        remove_none_t<
            std::tuple<
                attribute_of_t<rule_of_t<Ts>>...
            >
        >
    >
{
};

} // namespace
