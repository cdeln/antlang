#include <doctest/doctest.h>

#include "ast_rules.hpp"
#include "ast_parser.hpp"

using namespace ant;

TEST_CASE("can parse parameter")
{
    std::vector<token> tokens;
    tokens.push_back({identifier_token{"type"}});
    tokens.push_back({identifier_token{"name"}});
    auto parser = make_parser<ast::parameter>();
    auto [parameter, pos] = parser.parse(tokens.cbegin(), tokens.cend());
    CHECK(parameter.type == "type");
    CHECK(parameter.name == "name");
    CHECK(pos == tokens.end());
}

TEST_CASE("can parse function")
{
    std::vector<token> tokens =
      { {left_parenthesis_token{}}
      , {identifier_token{"function-name"}}
      , {identifier_token{"return-type"}}
      , {left_parenthesis_token{}}
      , {identifier_token{"parameter-type"}}
      , {identifier_token{"parameter-name"}}
      , {right_parenthesis_token{}}
      , {right_parenthesis_token{}}
      };
    auto parser = make_parser<ast::function>();
    auto [function, pos] = parser.parse(tokens.cbegin(), tokens.cend());
    CHECK(function.name == "function-name");
    CHECK(function.return_type == "return-type");
    REQUIRE(function.parameters.size() == 1);
    CHECK(function.parameters.at(0).type == "parameter-type");
    CHECK(function.parameters.at(0).name == "parameter-name");
    CHECK(pos == tokens.end());
}

TEST_CASE("can parse structure")
{
    std::vector<token> tokens =
      { {left_parenthesis_token{}}
      , {identifier_token{"structure-name"}}
      , {left_parenthesis_token{}}
      , {identifier_token{"field-type"}}
      , {identifier_token{"field-name"}}
      , {right_parenthesis_token{}}
      , {right_parenthesis_token{}}
      };
    auto parser = make_parser<ast::structure>();
    auto [value, pos] = parser.parse(tokens.cbegin(), tokens.cend());
    CHECK(value.name == "structure-name");
    REQUIRE(value.fields.size() == 1);
    CHECK(value.fields.at(0).type == "field-type");
    CHECK(value.fields.at(0).name == "field-name");
    CHECK(pos == tokens.end());
}
