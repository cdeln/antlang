#include <doctest/doctest.h>

#include "ast_rules.hpp"
#include "ast_parser.hpp"

using namespace ant;

TEST_CASE("can parse parameter")
{
    std::vector<token> tokens;
    tokens.push_back({identifier_token{"type"}});
    tokens.push_back({identifier_token{"name"}});
    parser<ast::parameter> parser;
    auto [param, pos] = parser.parse(tokens.cbegin(), tokens.cend());
    CHECK(param.type == "type");
    CHECK(param.name == "name");
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
    parser<ast::function> parser;
    auto [param, pos] = parser.parse(tokens.cbegin(), tokens.cend());
    CHECK(param.name == "function-name");
    CHECK(param.return_type == "return-type");
    CHECK(pos == tokens.end());
}
