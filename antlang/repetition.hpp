#pragma once

#include "rules.hpp"
#include "type_filters.hpp"

#include <vector>

namespace ant
{

template <class Rule>
struct repetition
    : rule
        < repetition<Rule>
        , remove_none_t<std::vector<attribute_of_t<rule_of_t<Rule>>>>
        >
{
};

} // namespace ant
