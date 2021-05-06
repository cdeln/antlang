#include <doctest/doctest.h>

#include "type_filters.hpp"

using namespace ant;

TEST_CASE("remove removes types from tuple as expected")
{
    using type = remove_t<int, std::tuple<int, long, int, unsigned>>;
    CHECK(std::is_same_v<type, std::tuple<long, unsigned>>);
}


TEST_CASE("remove none removes types as expected")
{
    CHECK(std::is_same_v<std::tuple<int>, remove_none_t<std::tuple<int, none>>>);
    CHECK(std::is_same_v<std::tuple<>, remove_none_t<std::tuple<none, none>>>);
    CHECK(std::is_same_v<none, remove_none_t<std::vector<none>>>);
}

TEST_CASE("unique is no-op for duplicate free tuple")
{
    using type = unique_t<std::tuple, int, long, unsigned>;
    CHECK(std::is_same_v<type, std::tuple<int, long, unsigned>>);
}

TEST_CASE("unique remove duplicates from tuple")
{
    using type = unique_t<std::tuple, int, long, int, unsigned>;
    CHECK(std::is_same_v<type, std::tuple<int, long, unsigned>>);
}

TEST_CASE("collape is identity empty tuple")
{
    REQUIRE(std::is_same_v<collapse_t<std::tuple<>>, std::tuple<>>);
}

TEST_CASE("collape of singleton tuple is tuple element type")
{
    REQUIRE(std::is_same_v<collapse_t<std::tuple<int>>, int>);
}

TEST_CASE("collape collapses a tuple of empty tuple into a tuple")
{
    REQUIRE(std::is_same_v<collapse_t<std::tuple<std::tuple<>>>, std::tuple<>>);
}

TEST_CASE("collape collapses recursively")
{
    REQUIRE(std::is_same_v<collapse_t<std::tuple<std::tuple<int>>>, int>);
}
