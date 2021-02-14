#include "test.hpp"

#include "token_factory.hpp"

using namespace ant;

BOOST_AUTO_TEST_CASE(token_factory_creates_one_token)
{
    auto factory = token_factory_builder<left_parenthesis_token>::make();
    auto token = factory.create(0, "(");
    BOOST_CHECK(std::holds_alternative<left_parenthesis_token>(token));
}

BOOST_AUTO_TEST_CASE(token_factory_creates_two_tokens)
{
    auto factory =
        token_factory_builder
          < left_parenthesis_token
          , right_parenthesis_token
          >::make();
    auto left_parenthesis = factory.create(0, "(");
    BOOST_CHECK(std::holds_alternative<left_parenthesis_token>(left_parenthesis));
    auto right_parenthesis = factory.create(1, ")");
    BOOST_CHECK(std::holds_alternative<right_parenthesis_token>(right_parenthesis));
}

BOOST_AUTO_TEST_CASE(token_factory_creates_all_tokens)
{
    auto factory = token_factory_builder<token_variant>::make();
    auto left_parenthesis = factory.create(0, "(");
    BOOST_REQUIRE(std::holds_alternative<left_parenthesis_token>(left_parenthesis));
    auto right_parenthesis = factory.create(1, ")");
    BOOST_REQUIRE(std::holds_alternative<right_parenthesis_token>(right_parenthesis));
    auto function = factory.create(2, "function");
    BOOST_REQUIRE(std::holds_alternative<function_token>(function));
    auto structure = factory.create(3, "structure");
    BOOST_REQUIRE(std::holds_alternative<structure_token>(structure));
    auto protocol = factory.create(4, "protocol");
    BOOST_REQUIRE(std::holds_alternative<protocol_token>(protocol));
    auto module = factory.create(5, "module");
    BOOST_REQUIRE(std::holds_alternative<module_token>(module));
    auto import = factory.create(6, "import");
    BOOST_REQUIRE(std::holds_alternative<import_token>(import));
    auto exported = factory.create(7, "export");
    BOOST_REQUIRE(std::holds_alternative<export_token>(exported));
    auto floating = factory.create(8, "01234.56789");
    BOOST_REQUIRE(std::holds_alternative<floating_point_literal_token>(floating));
    auto integer = factory.create(9, "0123456789");
    BOOST_REQUIRE(std::holds_alternative<integer_literal_token>(integer));
    auto identifier = factory.create(
        10, "abcdefghijklmnopqrstuvxz-ABCDEFGHIJKLMNOPQRSTUVXYZ_!@#$%^&*+!9+=~_0123456789");
    BOOST_REQUIRE(std::holds_alternative<identifier_token>(identifier));
}
