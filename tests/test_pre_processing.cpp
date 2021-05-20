#include <doctest/doctest.h>

#include "pre_processing.hpp"

using namespace ant;

TEST_CASE("remove_comments removes source code right of and including comment")
{
    const std::string source = "(some source code ; this should be removed)";
    const std::string processed = remove_comments(source);
    CHECK(processed == "(some source code ");
}
