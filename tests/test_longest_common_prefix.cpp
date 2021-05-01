#include <doctest/doctest.h>

#include "longest_common_prefix.hpp"
#include "tokens.hpp"
#include "token_rules.hpp"

using namespace ant;

struct A : ant::rule_spec<A, A> {};
struct B : ant::rule_spec<B, B> {};
struct C : ant::rule_spec<C, C> {};

namespace ant
{

template <> struct rule_of<A> { using type = A; };
template <> struct rule_of<B> { using type = B; };
template <> struct rule_of<C> { using type = C; };

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
    using sequence_type = sequence<A, B>;
    CHECK(longest_common_prefix_for<sequence_type, B>() == 0);
    CHECK(longest_common_prefix_for<B, sequence_type>() == 0);
}

TEST_CASE("longest common prefix between sequence and terminal prefix is 1")
{
    using sequence_type = sequence<A, B>;
    CHECK(longest_common_prefix_for<sequence_type, A>() == 1);
    CHECK(longest_common_prefix_for<A, sequence_type>() == 1);
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
    using sequence_type = sequence<A, B, C>;
    using nested_type = sequence<sequence_type, sequence_type>;
    CHECK(longest_common_prefix_for<sequence_type, sequence_type>() == 3);
    CHECK(longest_common_prefix_for<sequence_type, nested_type>()   == 3);
    CHECK(longest_common_prefix_for<nested_type,   sequence_type>() == 3);
    CHECK(longest_common_prefix_for<nested_type,   nested_type>()   == 6);
}
