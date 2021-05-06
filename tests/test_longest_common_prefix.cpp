#include <doctest/doctest.h>

#include "longest_common_prefix.hpp"

#include "tokens.hpp"
#include "token_rules.hpp"
#include "ast_rules.hpp"

using namespace ant;

struct A : ant::rule_spec<A, A> {};
struct B : ant::rule_spec<B, B> {};
struct C : ant::rule_spec<C, C> {};

namespace ant
{

template <> struct rule_of<A> { using type = A; };
template <> struct rule_of<B> { using type = B; };
template <> struct rule_of<C> { using type = C; };

template <> struct attribute_of<A> { using type = none; };
template <> struct attribute_of<B> { using type = none; };
template <> struct attribute_of<C> { using type = none; };

};

TEST_CASE("longest common prefix between two non-equal terminals is 0")
{
    CHECK(longest_common_prefix_for<A, B>() == 0);
    CHECK(longest_common_prefix_for<B, A>() == 0);
}

TEST_CASE("longest common prefix between two equal terminals is 1")
{
    CHECK(longest_common_prefix_for<A, A>() == 1);
    CHECK(longest_common_prefix_for<B, B>() == 1);
}

TEST_CASE("longest common prefix between sequence and non-prefix terminal is 0")
{
    using sequence_rule = sequence<A, B>;
    CHECK(longest_common_prefix_for<sequence_rule, B>() == 0);
    CHECK(longest_common_prefix_for<B, sequence_rule>() == 0);
}

TEST_CASE("longest common prefix between sequence and terminal prefix is 1")
{
    using sequence_rule = sequence<A, B>;
    CHECK(longest_common_prefix_for<sequence_rule, A>() == 1);
    CHECK(longest_common_prefix_for<A, sequence_rule>() == 1);
}

TEST_CASE("longest common prefix between two sequences with terminals")
{
    CHECK(longest_common_prefix_for<sequence<A, B>, sequence<B, C>>() == 0);
    CHECK(longest_common_prefix_for<sequence<B, C>, sequence<A, B>>() == 0);
    CHECK(longest_common_prefix_for<sequence<A, B>, sequence<A, C>>() == 1);
    CHECK(longest_common_prefix_for<sequence<A, C>, sequence<A, B>>() == 1);
    CHECK(longest_common_prefix_for<sequence<A, B>, sequence<A, B>>() == 2);
}

TEST_CASE("longest common prefix between a sequence and a nested sequence")
{
    using sequence_rule = sequence<A, B, C>;
    using nested_rule = sequence<sequence_rule, sequence_rule>;
    CHECK(longest_common_prefix_for<sequence_rule, sequence_rule>() == 3);
    CHECK(longest_common_prefix_for<sequence_rule, nested_rule>()   == 3);
    CHECK(longest_common_prefix_for<nested_rule,   sequence_rule>() == 3);
    CHECK(longest_common_prefix_for<nested_rule,   nested_rule>()   == 6);
}

TEST_CASE("longest common prefix between repetition and non-prefix terminal is 0")
{
    using repetition_rule = repetition<A, B>;
    CHECK(longest_common_prefix_for<repetition_rule, B>() == 0);
    CHECK(longest_common_prefix_for<B, repetition_rule>() == 0);
}

TEST_CASE("longest common prefix between repetition and terminal prefix is 1")
{
    using repetition_rule = repetition<A, B>;
    CHECK(longest_common_prefix_for<repetition_rule, A>() == 1);
    CHECK(longest_common_prefix_for<A, repetition_rule>() == 1);
}

TEST_CASE("longest common prefix between repetition and sequence without common prefix is 0")
{
    using repetition_rule = repetition<A, B>;
    using sequence_rule   = sequence<B, C>;
    CHECK(longest_common_prefix_for<repetition_rule, sequence_rule>() == 0);
    CHECK(longest_common_prefix_for<sequence_rule, repetition_rule>() == 0);
}

TEST_CASE("longest common prefix between repetition and sequence with common prefix "
          "excluding end of repetition rule is length of common prefix")
{
    using repetition_rule = repetition<A, B>;
    using sequence_rule   = sequence<A, A, A, C>;
    CHECK(longest_common_prefix_for<repetition_rule, sequence_rule>() == 3);
    CHECK(longest_common_prefix_for<sequence_rule, repetition_rule>() == 3);
}

TEST_CASE("longest common prefix between repetition and sequence with common prefix "
          "including end of repetition rule is length of common prefix")
{
    using repetition_rule = repetition<A, B>;
    using sequence_rule   = sequence<A, A, A, B>;
    CHECK(longest_common_prefix_for<repetition_rule, sequence_rule>() == 4);
    CHECK(longest_common_prefix_for<sequence_rule, repetition_rule>() == 4);
}

TEST_CASE("longest common prefix between repetition and repetition without common prefix is 0")
{
    using rule_1 = repetition<A, B>;
    using rule_2 = repetition<B, C>;
    CHECK(longest_common_prefix_for<rule_1, rule_2>() == 0);
    CHECK(longest_common_prefix_for<rule_2, rule_1>() == 0);
}

TEST_CASE("longest common prefix between repetition and repetition with common prefix is unbounded")
{
    using rule_1 = repetition<A, B>;
    using rule_2 = repetition<A, C>;
    CHECK(longest_common_prefix_for<rule_1, rule_2>() == unbounded_prefix_length);
    CHECK(longest_common_prefix_for<rule_2, rule_1>() == unbounded_prefix_length);
}

TEST_CASE("longest common prefix between terminal alternative and non-prefix terminal is 0")
{
    using alternative_rule = alternative<A, B>;
    CHECK(longest_common_prefix_for<alternative_rule, C>() == 0);
    CHECK(longest_common_prefix_for<C, alternative_rule>() == 0);
}

TEST_CASE("longest common prefix between terminal alternative and present terminal is 1")
{
    using alternative_rule = alternative<A, B>;
    CHECK(longest_common_prefix_for<alternative_rule, A>() == 1);
    CHECK(longest_common_prefix_for<A, alternative_rule>() == 1);
    CHECK(longest_common_prefix_for<alternative_rule, B>() == 1);
    CHECK(longest_common_prefix_for<B, alternative_rule>() == 1);
}

TEST_CASE("longest common prefix between terminal alternative "
          "and sequence without common prefix is 0")
{
    CHECK(longest_common_prefix_for<alternative<A, B>, sequence<C>>() == 0);
    CHECK(longest_common_prefix_for<sequence<C>, alternative<A, B>>() == 0);
}

TEST_CASE("longest common prefix between terminal alternative and sequence with common prefix is 1")
{
    CHECK(longest_common_prefix_for<alternative<A, B>, sequence<A, C>>() == 1);
    CHECK(longest_common_prefix_for<sequence<A, C>, alternative<A, B>>() == 1);
}

TEST_CASE("longest common prefix between sequence alternative and sequence "
          "with common prefix is length of sequence")
{
    using alternative_rule = alternative<sequence<A, B>, sequence<B, C>>;
    CHECK(longest_common_prefix_for<alternative_rule, sequence<A, B>>() == 2);
    CHECK(longest_common_prefix_for<alternative_rule, sequence<B, C>>() == 2);
    CHECK(longest_common_prefix_for<alternative_rule, sequence<A, C>>() == 1);
    CHECK(longest_common_prefix_for<alternative_rule, sequence<B, A>>() == 1);
    CHECK(longest_common_prefix_for<alternative_rule, sequence<C, B>>() == 0);
    CHECK(longest_common_prefix_for<sequence<A, B>, alternative_rule>() == 2);
    CHECK(longest_common_prefix_for<sequence<B, C>, alternative_rule>() == 2);
    CHECK(longest_common_prefix_for<sequence<A, C>, alternative_rule>() == 1);
    CHECK(longest_common_prefix_for<sequence<B, A>, alternative_rule>() == 1);
    CHECK(longest_common_prefix_for<sequence<C, B>, alternative_rule>() == 0);
}

TEST_CASE("longest common prefix between two alternatives works as expected")
{
    using alternative_rule =
        alternative<
            sequence<A,B>,
            sequence<A,C>,
            sequence<B,C>,
            sequence<A,B,C>
        >;
    CHECK(alternative_longest_common_prefix(alternative_rule()) == 2);
}

TEST_CASE("longest common prefix of two real rules works as expected")
{
    using rule =
        alternative<
            sequence<
                left_parenthesis_token,
                identifier_token
            >,
            sequence<
                left_parenthesis_token,
                integer_literal_token
            >
        >;
    CHECK(alternative_longest_common_prefix(rule()) == 1);
}

TEST_CASE("longest common prefix between ast element and sequence")
{
    using sequence_rule =
        sequence<
            left_parenthesis_token,
            identifier_token,
            right_parenthesis_token
        >;
    CHECK(longest_common_prefix_for<ast::function, sequence_rule>() == 1);
}

TEST_CASE("alternative longest common prefix of two non-pure (AST) rules works as expected")
{
    using rule =
        alternative<
            ast::function,
            ast::structure
        >;
    CHECK(alternative_longest_common_prefix(rule()) == 1);
}

TEST_CASE("longest common prefix of an AST element works as expected")
{
    CHECK(longest_common_prefix_for<ast::function, ast::structure>() == 1);
}
