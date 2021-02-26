#pragma once

#include "rules.hpp"
#include "type_filters.hpp"

#include <tuple>

namespace ant
{

template <typename... Rules>
struct sequence
    : rule
        < sequence<Rules...>
        , remove_none_t<std::tuple<attribute_of_t<rule_of_t<Rules>>...>>
        >
{
};

} // namespace