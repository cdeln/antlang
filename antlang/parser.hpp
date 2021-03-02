#pragma once

#include "exceptions.hpp"
#include "rules.hpp"

namespace ant
{

template <class Rule>
struct parser;

template <typename T>
auto make_parser()
{
    return parser<rule_of_t<T>>();
}

} // namespace ant

#include "parser_result.hpp"
#include "ast_parser.hpp"
#include "alternative_parser.hpp"
#include "discard_parser.hpp"
#include "literal_parser.hpp"
#include "match_parser.hpp"
#include "repetition_parser.hpp"
#include "sequence_parser.hpp"
#include "token_parsers.hpp"
