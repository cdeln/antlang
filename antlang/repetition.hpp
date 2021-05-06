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
        collapse_t<
            remove_none_t<
                std::tuple<
                    std::vector<
                        attribute_of_t<rule_of_t<T>>
                    >,
                    attribute_of_t<rule_of_t<End>>
                >
            >
        >
    >
{
};

} // namespace ant
