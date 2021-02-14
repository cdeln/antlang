#pragma once

#include "tokens.hpp"

#include <string>

namespace ant
{

class token_builder
{
public:
    virtual token_variant build(std::string const& data) const = 0;

    virtual std::string pattern() const = 0;
};

template <class Token>
class token_alternative_builder final : public token_builder
{
public:
    token_variant build(std::string const& data) const override;

    std::string pattern() const override;
};

} // namespace ant
