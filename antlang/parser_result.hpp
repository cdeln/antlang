#pragma once

#include "tokens.hpp"

#include <vector>

namespace ant
{

template <typename Attribute>
struct parser_result
{
    Attribute value;
    std::vector<token>::const_iterator position;
};

} // namespace ant
