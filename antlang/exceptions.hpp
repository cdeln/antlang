#pragma once

#include "tokens.hpp"

#include <stdexcept>

namespace ant
{

struct tokenizer_error : public std::runtime_error
{
    token_context context;

    tokenizer_error(std::string message, token_context context)
        : std::runtime_error(message)
        , context(context)
    {
    }
};

struct token_pattern_mismatch_error : public tokenizer_error
{
    using tokenizer_error::tokenizer_error;
};

struct unexpected_end_of_input_error : public tokenizer_error
{
    unexpected_end_of_input_error(std::string message)
        : tokenizer_error(message, {-1, -1})
    {
    }
};

} // namespace ant
