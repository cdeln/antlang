#include <doctest/doctest.h>

#include "recursive_variant.hpp"

using namespace ant;

struct recursive_struct;

using test_variant =
    recursive_variant<
        int,
        recursive_wrapper<recursive_struct>
    >;

struct recursive_struct
{
    test_variant value;
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
    CHECK(!is_recursive_v<int, test_variant>);
    CHECK( is_recursive_v<recursive_struct, test_variant>);
}

TEST_CASE("recursive wrapper in variant works as expected when used in a recursive context")
{
    test_variant x = 13;
    REQUIRE(holds<int>(x));
    REQUIRE(get<int>(x) == 13);
    x = recursive_struct{37};
    REQUIRE(holds<recursive_struct>(x));
    static_assert(is_recursive_v<recursive_struct, test_variant>);
    recursive_struct& y = get<recursive_struct>(x);
    REQUIRE(holds<int>(y.value));
    CHECK(get<int>(y.value) == 37);
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
    test_variant x = 13;
    CHECK(visit(visitor(), x) == 1);
    x = recursive_struct{37};
    CHECK(visit(visitor(), x) == 2);
}
