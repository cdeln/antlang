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
    auto parser = make_parser<ast::function>();
    auto [func, pos] = parser.parse(tokens.cbegin(), tokens.cend());
    CHECK(func.name == "function-name");
    CHECK(func.return_type == "return-type");
    CHECK(pos == tokens.end());
}
