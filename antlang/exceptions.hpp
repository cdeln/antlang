#pragma once

#include "tokens.hpp"

#include <stdexcept>

namespace ant
{

struct exception : public std::runtime_error
{
    token_context context;

    exception(std::string message, token_context context)
        : std::runtime_error(message)
        , context(context)
    {
    }
};

struct tokenizer_error : public std::runtime_error
{
    using std::runtime_error::runtime_error;
};

struct token_pattern_mismatch_error : public tokenizer_error
{
    using tokenizer_error::tokenizer_error;
};

struct unexpected_token_error : public exception
{
    using exception::exception;
};

struct unexpected_end_of_input_error : public exception
{
    unexpected_end_of_input_error(std::string message)
        : exception(message, {-1, -1})
    {
    }
};

struct alternative_parser_error : public exception
{
    using exception::exception;
};

struct literal_mismatch_error  : public exception
{
    using exception::exception;
};

} // namespace ant
