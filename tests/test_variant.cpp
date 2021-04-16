#include <doctest/doctest.h>

#include "recursive_variant.hpp"

using namespace ant;

struct recursive_struct;

using recursive_variant =
    std::variant<
        int,
        recursive_wrapper<recursive_struct>
    >;

struct recursive_struct
{
    recursive_variant value;
};

TEST_CASE("recursive wrapper works works like the type it wraps")
{
    recursive_wrapper<int> x;

    x = 13;
    int& y = x;
    CHECK(y == 13);

    x = 37;
    CHECK(y == 37);

    int z = x;
    CHECK(z == 37);
}

TEST_CASE("is_recursive reports recursive structs as recursive")
{
    CHECK(!is_recursive_v<int, recursive_variant>);
    CHECK( is_recursive_v<recursive_struct, recursive_variant>);
}

TEST_CASE("recursive wrapper in variant works as expected when used in a recursive context")
{
    recursive_variant x = 13;
    REQUIRE(holds<int>(x));
    REQUIRE(get_recursive<int>(x) == 13);
    x = recursive_struct{37};
    REQUIRE(holds<recursive_struct>(x));
    static_assert(is_recursive_v<recursive_struct, recursive_variant>);
    auto y = get_recursive<recursive_struct>(x);
    REQUIRE(holds<int>(y.value));
    CHECK(get_recursive<int>(y.value) == 37);
}

struct visitor
{
    int operator()(int) const
    {
        return 1;
    }
    int operator()(recursive_struct) const
    {
        return 2;
    }
};

TEST_CASE("visit recursive wrapper works")
{
    recursive_variant x = 13;
    CHECK(visit(visitor(), x) == 1);
    x = recursive_struct{37};
    CHECK(visit(visitor(), x) == 2);
}
