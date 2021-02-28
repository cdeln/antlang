#pragma once

#include "rules.hpp"
#include "type_filters.hpp"

#include <vector>

namespace ant
{

template <typename T>
struct repetition
    : rule
        < repetition<T>
        , remove_none_t<std::vector<attribute_of_t<rule_of_t<T>>>>
        >
{
};

} // namespace ant
