#pragma once

#include "tokens.hpp"
#include "token_factory.hpp"

#include <regex>
#include <string>
#include <vector>

namespace ant
{

class tokenizer
{
public:

    explicit tokenizer(token_factory&& factory);

    tokenizer();

    std::vector<token> tokenize(std::string const& source) const;

private:
    std::regex pattern;
    token_factory factory;
};

} // namespace ant
