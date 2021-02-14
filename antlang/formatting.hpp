#pragma once

#include "tokens.hpp"

#include <string>
#include <ostream>

std::string
quote(std::string const& x);

std::ostream&
operator<<(std::ostream& stream, ant::left_parenthesis_token const&);

std::ostream&
operator<<(std::ostream& stream, ant::right_parenthesis_token const&);

std::ostream&
operator<<(std::ostream& stream, ant::function_token const& x);

std::ostream&
operator<<(std::ostream& stream, ant::structure_token const& x);

std::ostream&
operator<<(std::ostream& stream, ant::protocol_token const& x);

std::ostream&
operator<<(std::ostream& stream, ant::module_token const& x);

std::ostream&
operator<<(std::ostream& stream, ant::import_token const& x);

std::ostream&
operator<<(std::ostream& stream, ant::export_token const& x);

std::ostream&
operator<<(std::ostream& stream, ant::identifier_token const& x);

std::ostream&
operator<<(std::ostream& stream, ant::integer_literal_token const& x);

std::ostream&
operator<<(std::ostream& stream, ant::floating_point_literal_token const& x);

std::ostream&
operator<<(std::ostream& stream, ant::token_variant const& variant);
