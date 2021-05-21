#pragma once

#include "tokens.hpp"

#include <string>
#include <vector>

namespace ant
{

std::vector<token>
tokenize(const std::string& source);

} // namespace ant
