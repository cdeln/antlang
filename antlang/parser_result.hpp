#pragma once

#include "exceptional.hpp"
#include "tokens.hpp"

#include <vector>

namespace ant
{

template <typename Attribute>
struct parser_success
{
    Attribute value;
    std::vector<token>::const_iterator position;
};

struct parser_failure
{
    std::string message;
    std::vector<token>::const_iterator position;
    std::vector<parser_failure> children;
};

template <typename Attribute>
struct parser_result : public
    exceptional<
        parser_success<Attribute>,
        parser_failure
    >
{
    using exceptional<
        parser_success<Attribute>,
        parser_failure
    >::exceptional;
};

} // namespace ant
