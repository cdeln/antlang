#pragma once

#include "exceptions.hpp"
#include "rules.hpp"

namespace ant
{

template <class Rule>
struct rule_parser;

template <class Rule>
struct parser : rule_parser<rule_of_t<Rule>> {};

} // namespace ant

#include "parser_result.hpp"
#include "repetition_parser.hpp"
#include "sequence_parser.hpp"
#include "token_parsers.hpp"
