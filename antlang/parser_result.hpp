#pragma once

#include "recursive_variant.hpp"
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
    recursive_variant<
        parser_success<Attribute>,
        parser_failure
    >;

template <typename Attribute>
bool is_success(const parser_result<Attribute>& result)
{
    return holds<parser_success<Attribute>>(result);
}

template <typename Attribute>
bool is_failure(const parser_result<Attribute>& result)
{
    return holds<parser_failure>(result);
}

template <typename Attribute>
parser_success<Attribute>&
get_success(parser_result<Attribute>& result)
{
    return get<parser_success<Attribute>>(result);
}

template <typename Attribute>
parser_success<Attribute> const&
get_success(parser_result<Attribute> const& result)
{
    return get<parser_success<Attribute>>(result);
}

template <typename Attribute>
parser_failure&
get_failure(parser_result<Attribute>& result)
{
    return get<parser_failure>(result);
}

template <typename Attribute>
parser_failure const&
get_failure(parser_result<Attribute> const& result)
{
    return get<parser_failure>(result);
}

} // namespace ant
