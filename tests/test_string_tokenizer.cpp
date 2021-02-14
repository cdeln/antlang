#include "test.hpp"

#include "string_tokenizer.hpp"

using namespace ant;

BOOST_AUTO_TEST_CASE(string_tokenizer_tokenizes_simple_string_with_all_tokens)
{
    constexpr auto string = "() function structure protocol module import export i337 1337 13.37";
    const string_tokenizer tokenizer;
    const auto tokens = tokenizer.tokenize(string);
    BOOST_REQUIRE_EQUAL(tokens.size(), 11);
    BOOST_CHECK(std::holds_alternative<left_parenthesis_token>(tokens.at(0).variant));
    BOOST_CHECK(std::holds_alternative<right_parenthesis_token>(tokens.at(1).variant));
    BOOST_CHECK(std::holds_alternative<function_token>(tokens.at(2).variant));
    BOOST_CHECK(std::holds_alternative<structure_token>(tokens.at(3).variant));
    BOOST_CHECK(std::holds_alternative<protocol_token>(tokens.at(4).variant));
    BOOST_CHECK(std::holds_alternative<module_token>(tokens.at(5).variant));
    BOOST_CHECK(std::holds_alternative<import_token>(tokens.at(6).variant));
    BOOST_CHECK(std::holds_alternative<export_token>(tokens.at(7).variant));
    BOOST_CHECK(std::holds_alternative<identifier_token>(tokens.at(8).variant));
    BOOST_CHECK_EQUAL(std::get<identifier_token>(tokens.at(8).variant).name, "i337");
    BOOST_REQUIRE(std::holds_alternative<integer_literal_token>(tokens.at(9).variant));
    BOOST_CHECK_EQUAL(std::get<integer_literal_token>(tokens.at(9).variant).value, "1337");
    BOOST_REQUIRE(std::holds_alternative<floating_point_literal_token>(tokens.at(10).variant));
    BOOST_CHECK_EQUAL(std::get<floating_point_literal_token>(tokens.at(10).variant).value, "13.37");
}
