#pragma once

#include "tokens.hpp"

#include <string>

namespace ant
{

std::string quote(std::string const& x);

std::string token_string(token_variant const& x);

} // namespace ant
