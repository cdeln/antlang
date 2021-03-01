#pragma once

#include "tokens.hpp"

#include <stdexcept>

namespace ant
{

struct exception : public std::runtime_error
{
    using std::runtime_error::runtime_error;
};

struct token_pattern_mismatch_error : public exception
{
    using exception::exception;
};

struct unexpected_token_error : public exception
{
    token_context context;

    explicit unexpected_token_error(
            std::string message,
            token_context context = {-1, -1})
        : exception(message)
        , context(context)
    {
    }
};

struct unexpected_end_of_input_error : public exception
{
    token_context context;

    explicit unexpected_end_of_input_error(
            std::string message,
            token_context context = {-1, -1})
        : exception(message)
        , context(context)
    {
    }
};

struct alternative_parser_error : public exception
{
    token_context context;

    explicit alternative_parser_error(
            std::string message,
            token_context context = {-1, -1})
        : exception(message)
        , context(context)
    {
    }
};

struct literal_mismatch_error  : public exception
{
    token_context context;

    explicit literal_mismatch_error(
            std::string message,
            token_context context = {-1, -1})
        : exception(message)
        , context(context)
    {
    }
};

} // namespace ant
