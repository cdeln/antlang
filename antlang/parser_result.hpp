#pragma once

#include "tokens.hpp"

#include <memory>
#include <variant>
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
    std::unique_ptr<parser_failure> previous;
};

template <typename Attribute>
using parser_result =
    std::variant<
        parser_success<Attribute>,
        parser_failure
    >;

template <typename Attribute>
bool is_success(const parser_result<Attribute>& result)
{
    return std::holds_alternative<parser_success<Attribute>>(result);
}

template <typename Attribute>
bool is_failure(const parser_result<Attribute>& result)
{
    return std::holds_alternative<parser_failure>(result);
}

template <typename Attribute>
parser_success<Attribute>&
get_success(parser_result<Attribute>& result)
{
    return std::get<parser_success<Attribute>>(result);
}

template <typename Attribute>
parser_success<Attribute> const&
get_success(parser_result<Attribute> const& result)
{
    return std::get<parser_success<Attribute>>(result);
}

template <typename Attribute>
parser_failure&
get_failure(parser_result<Attribute>& result)
{
    return std::get<parser_failure>(result);
}

template <typename Attribute>
parser_failure const&
get_failure(parser_result<Attribute> const& result)
{
    return std::get<parser_failure>(result);
}

} // namespace ant
