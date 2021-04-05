#pragma once

#include "ast.hpp"
#include "alternative.hpp"
#include "literal_rule.hpp"
#include "match.hpp"
#include "repetition.hpp"
#include "sequence.hpp"
#include "token_rules.hpp"

namespace ant
{

namespace detail
{

template <typename Literal>
struct token_type
{
    using type = integer_literal_token;
};

template <>
struct token_type<ast::literal<float>>
{
    using type = floating_point_literal_token;
};

template <>
struct token_type<ast::literal<double>>
{
    using type = floating_point_literal_token;
};

template <typename Literal>
using token_type_t = typename token_type<Literal>::type;

} // namespace detail

template <class Attribute>
struct ast_rule;

template <>
struct ast_rule<ast::parameter> :
    rule_spec<
        sequence<identifier_token, identifier_token>,
        ast::parameter
    >
{};

template <>
struct rule_of<ast::parameter>
{
    using type = ast_rule<ast::parameter>;
};

template <>
struct ast_rule<ast::function> :
    rule_spec<
        sequence<
            left_parenthesis_token,
            function_token,
            identifier_token,
            identifier_token,
            left_parenthesis_token,
            repetition<
                ast::parameter,
                right_parenthesis_token
            >,
            ast::expression,
            right_parenthesis_token
        >,
        ast::function
    >
{};

template <>
struct rule_of<ast::function>
{
    using type = ast_rule<ast::function>;
};

template <>
struct ast_rule<ast::structure> :
    rule_spec<
        sequence<
            left_parenthesis_token,
            structure_token,
            identifier_token,
            repetition<
                ast::parameter,
                right_parenthesis_token
            >
        >,
        ast::structure
    >
{};

template <>
struct rule_of<ast::structure>
{
    using type = ast_rule<ast::structure>;
};

template <typename T>
struct ast_rule<ast::literal<T>> :
    rule_spec<
        sequence<
            left_parenthesis_token,
            discard<
                match<
                    identifier_token,
                    ast::name_of<ast::literal<T>>
                >
            >,
            literal_rule<
                ast::literal<T>,
                detail::token_type_t<ast::literal<T>>
            >,
            right_parenthesis_token
        >,
        ast::literal<T>
    >
{};

template <typename T>
struct rule_of<ast::literal<T>>
{
    using type = ast_rule<ast::literal<T>>;
};

template <>
struct ast_rule<ast::literal_variant> :
    rule_spec<
        alternative<
            ast::i8, ast::i16, ast::i32, ast::i64,
            ast::u8, ast::u16, ast::u32, ast::u64,
            ast::f32, ast::f64
        >,
        ast::literal_variant
    >
{};

template <>
struct rule_of<ast::literal_variant>
{
    using type = ast_rule<ast::literal_variant>;
};

template <>
struct ast_rule<ast::expression> :
    rule_spec<
        alternative<
            identifier_token,
            ast::literal_variant,
            ast::evaluation
        >,
        ast::expression
    >
{};

template <>
struct rule_of<ast::expression>
{
    using type = ast_rule<ast::expression>;
};

template <>
struct ast_rule<ast::evaluation> :
    rule_spec<
        sequence<
            left_parenthesis_token,
            identifier_token,
            repetition<
                ast::expression,
                right_parenthesis_token
            >
        >,
        ast::evaluation
    >
{};

template <>
struct rule_of<ast::evaluation>
{
    using type = ast_rule<ast::evaluation>;
};

template <>
struct ast_rule<ast::statement> :
    rule_spec<
        alternative<
            ast::function,
            ast::structure
        >,
        ast::statement
    >
{};

template <>
struct rule_of<ast::statement>
{
    using type = ast_rule<ast::statement>;
};

} // namespace ant
