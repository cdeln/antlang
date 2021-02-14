#include "test.hpp"

#include "token_builder.hpp"

using namespace ant;

BOOST_AUTO_TEST_CASE(token_builder_builds_left_parenthesis)
{
    token_alternative_builder<left_parenthesis_token> builder;
    auto token = builder.build("(");
    BOOST_CHECK(std::holds_alternative<left_parenthesis_token>(token));
}

BOOST_AUTO_TEST_CASE(token_builder_builds_right_parenthesis)
{
    token_alternative_builder<right_parenthesis_token> builder;
    auto token = builder.build(")");
    BOOST_CHECK(std::holds_alternative<right_parenthesis_token>(token));
}

BOOST_AUTO_TEST_CASE(token_builder_builds_function)
{
    token_alternative_builder<function_token> builder;
    auto token = builder.build("function");
    BOOST_CHECK(std::holds_alternative<function_token>(token));
}

BOOST_AUTO_TEST_CASE(token_builder_builds_structure)
{
    token_alternative_builder<structure_token> builder;
    auto token = builder.build("structure");
    BOOST_CHECK(std::holds_alternative<structure_token>(token));
}

BOOST_AUTO_TEST_CASE(token_builder_builds_protocol)
{
    token_alternative_builder<protocol_token> builder;
    auto token = builder.build("protocol");
    BOOST_CHECK(std::holds_alternative<protocol_token>(token));
}

BOOST_AUTO_TEST_CASE(token_builder_builds_module)
{
    token_alternative_builder<module_token> builder;
    auto token = builder.build("module");
    BOOST_CHECK(std::holds_alternative<module_token>(token));
}

BOOST_AUTO_TEST_CASE(token_builder_builds_import)
{
    token_alternative_builder<import_token> builder;
    auto token = builder.build("import");
    BOOST_CHECK(std::holds_alternative<import_token>(token));
}

BOOST_AUTO_TEST_CASE(token_builder_builds_export)
{
    token_alternative_builder<export_token> builder;
    auto token = builder.build("export");
    BOOST_CHECK(std::holds_alternative<export_token>(token));
}

BOOST_AUTO_TEST_CASE(token_builder_builds_identifier)
{
    token_alternative_builder<identifier_token> builder;
    auto string = "abcdefghijklmnopqrstuvxz-ABCDEFGHIJKLMNOPQRSTUVXYZ_!@#$%^&*+!9+=~_0123456789";
    auto token = builder.build(string);
    BOOST_CHECK(std::holds_alternative<identifier_token>(token));
    auto identifier = std::get<identifier_token>(token);
    BOOST_CHECK(identifier.name == string);
}

BOOST_AUTO_TEST_CASE(token_builder_builds_integer_literal)
{
    token_alternative_builder<integer_literal_token> builder;
    auto string = "0123456789";
    auto token = builder.build(string);
    BOOST_CHECK(std::holds_alternative<integer_literal_token>(token));
    auto identifier = std::get<integer_literal_token>(token);
    BOOST_CHECK_EQUAL(identifier.value, string);
}

BOOST_AUTO_TEST_CASE(token_builder_builds_floating_point_literal)
{
    token_alternative_builder<floating_point_literal_token> builder;
    auto string = "01234.56789";
    auto token = builder.build(string);
    BOOST_CHECK(std::holds_alternative<floating_point_literal_token>(token));
    auto identifier = std::get<floating_point_literal_token>(token);
    BOOST_CHECK(identifier.value == string);
}
