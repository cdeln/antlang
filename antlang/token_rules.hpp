#pragma once

#include "rules.hpp"
#include "tokens.hpp"

namespace ant
{

template <>
struct rule_of<left_parenthesis_token>
{
    using type =
        non_attributed_token_rule<
            left_parenthesis_token
        >;
};

template <>
struct rule_of<right_parenthesis_token>
{
    using type =
        non_attributed_token_rule<
            right_parenthesis_token
        >;
};

template <>
struct rule_of<function_token>
{
    using type =
        non_attributed_token_rule<
            function_token
        >;
};

template <>
struct rule_of<structure_token>
{
    using type =
        non_attributed_token_rule<
            structure_token
        >;
};

template <>
struct rule_of<floating_point_literal_token>
{
    using type =
        attributed_token_rule<
            floating_point_literal_token,
            std::string
        >;
};

template <>
struct rule_of<integer_literal_token>
{
    using type =
        attributed_token_rule<
            integer_literal_token,
            std::string
        >;
};

template <>
struct rule_of<identifier_token>
{
    using type =
        attributed_token_rule<
            identifier_token,
            std::string
        >;
};

template <>
struct rule_of<end_of_input_token>
{
    using type =
        non_attributed_token_rule<
            end_of_input_token
        >;
};

} // namespace ant
