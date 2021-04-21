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
    token_context context;
    std::vector<parser_failure> children;
};

template <typename Attribute>
using parser_result =
    exceptional<
        parser_success<Attribute>,
        parser_failure
    >;

} // namespace ant
