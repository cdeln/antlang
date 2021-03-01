#pragma once

#include "tokens.hpp"

#include <stdexcept>

namespace ant
{

struct token_pattern_mismatch_error : public std::runtime_error
{
    using std::runtime_error::runtime_error;
};

struct unexpected_token_error : public std::runtime_error
{
    token_context context;

    explicit unexpected_token_error(
            std::string message,
            token_context context = {-1, -1})
        : runtime_error(message)
        , context(context)
    {
    }
};

struct unexpected_end_of_input_error : public std::runtime_error
{
    token_context context;

    explicit unexpected_end_of_input_error(
            std::string message,
            token_context context = {-1, -1})
        : runtime_error(message)
        , context(context)
    {
    }
};

struct alternative_parser_error : public std::runtime_error
{
    token_context context;

    explicit alternative_parser_error(
            std::string message,
            token_context context = {-1, -1})
        : runtime_error(message)
        , context(context)
    {
    }
};

struct literal_mismatch_error  : public std::runtime_error
{
    token_context context;

    explicit literal_mismatch_error(
            std::string message,
            token_context context = {-1, -1})
        : runtime_error(message)
        , context(context)
    {
    }
};

} // namespace ant
