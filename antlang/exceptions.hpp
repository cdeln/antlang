#pragma once

#include <stdexcept>

namespace ant
{

class token_pattern_mismatch_error : public std::runtime_error
{
    using std::runtime_error::runtime_error;
};

} // namespace ant
