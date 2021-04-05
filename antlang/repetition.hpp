#pragma once

#include "rules.hpp"
#include "tokens.hpp"
#include "type_filters.hpp"

#include <vector>

namespace ant
{

template <typename T, typename End = right_parenthesis_token>
struct repetition :
    rule_spec<
        repetition<T, End>,
        remove_none_t<
            std::vector<
                attribute_of_t<rule_of_t<T>>
            >
        >
    >
{
};

} // namespace ant
