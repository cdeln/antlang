#pragma once

#include "ast.hpp"
#include "alternative.hpp"
#include "repetition.hpp"
#include "sequence.hpp"
#include "token_rules.hpp"

namespace ant
{

template <class Attribute>
struct ast_rule;

template <>
struct ast_rule<ast::parameter>
    : rule
        < sequence<identifier_token, identifier_token>
        , ast::parameter
        > {};

template <>
struct rule_of<ast::parameter>
{
    using type = ast_rule<ast::parameter>;
};

template <>
struct ast_rule<ast::function>
    : rule
        < sequence
            < left_parenthesis_token,
              identifier_token
            , identifier_token
            , left_parenthesis_token
            , repetition<ast::parameter>
            , right_parenthesis_token
            , right_parenthesis_token
            >
        , ast::function
        > {};

template <>
struct rule_of<ast::function>
{
    using type = ast_rule<ast::function>;
};

template <>
struct ast_rule<ast::structure>
    : rule
        < sequence
            < left_parenthesis_token,
              identifier_token
            , left_parenthesis_token
            , repetition<ast::parameter>
            , right_parenthesis_token
            , right_parenthesis_token
            >
        , ast::structure
        > {};

template <>
struct rule_of<ast::structure>
{
    using type = ast_rule<ast::structure>;
};

template <>
struct ast_rule<ast::expression>
    : rule
        < alternative
            < ast::evaluation
            , identifier_token
            >
        , ast::expression
        > {};

template <>
struct rule_of<ast::expression>
{
    using type = ast_rule<ast::expression>;
};

template <>
struct ast_rule<ast::evaluation>
    : rule
        < sequence
            < left_parenthesis_token
            , identifier_token
            , repetition<ast::expression>
            , right_parenthesis_token
            >
        , ast::evaluation
        > {};

template <>
struct rule_of<ast::evaluation>
{
    using type = ast_rule<ast::evaluation>;
};

} // namespace ant
