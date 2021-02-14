#pragma once

#include "fundamental_types.hpp"
#include "token_builder.hpp"

#include <memory>
#include <string>
#include <vector>

namespace ant
{

class token_factory
{
private:
    std::vector<std::unique_ptr<token_builder>> builders;
public:
    token_factory(std::vector<std::unique_ptr<token_builder>>&& builders);

    size_t size() const;

    token_variant create(size_t index, std::string const& data) const;

    std::string pattern() const;
};

template <class... Tokens>
token_factory
make_token_factory()
{
    std::vector<std::unique_ptr<token_builder>> builders;
    builders.reserve(sizeof...(Tokens));
    (builders.push_back(std::make_unique<token_alternative_builder<Tokens>>()), ...);
    return token_factory(std::move(builders));
}

template <class... Tokens>
struct token_factory_builder
{
    static constexpr token_factory make()
    {
        return make_token_factory<Tokens...>();
    }
};

template <class... Tokens>
struct token_factory_builder<std::variant<Tokens...>> : token_factory_builder<Tokens...> {};

} // namespace ant
