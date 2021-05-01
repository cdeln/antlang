#include <doctest/doctest.h>

#include "type_filters.hpp"

using namespace ant;

TEST_CASE("remove removes types from tuple as expected")
{
    using type = remove_t<int, std::tuple<int, long, int, unsigned>>;
    CHECK(std::is_same_v<type, std::tuple<long, unsigned>>);
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
