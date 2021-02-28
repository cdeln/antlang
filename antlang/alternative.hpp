#pragma once

#include "rules.hpp"

#include <variant>

namespace ant
{

template <typename... Ts>
struct alternative
    : rule
        < alternative<Ts...>
        , std::variant<attribute_of_t<rule_of_t<Ts>>...>
        >
{
};

} // namespace ant
