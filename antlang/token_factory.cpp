#include "token_factory.hpp"

#include <algorithm>
#include <cassert>
#include <sstream>
#include <utility>

namespace ant
{

namespace detail
{

std::string
make_alternation_pattern(std::vector<std::string> const& sub_patterns)
{
    std::stringstream pattern;
    if (!sub_patterns.empty())
    {
        pattern << '(' << sub_patterns.front() << ')';
        for (size_t i = 1; i < sub_patterns.size(); ++i)
        {
            pattern << '|' << '(' << sub_patterns[i] << ')';
        }
    }
    return pattern.str();
}

} // namespace detail

token_factory::token_factory(std::vector<std::unique_ptr<token_builder>>&& builders)
    : builders(std::move(builders))
{
}

size_t
token_factory::size() const
{
    return builders.size();
}

token_variant
token_factory::create(size_t index, std::string const& data) const
{
    assert(index < size());
    auto const & builder = builders.at(index);
    return builder->build(data);
}

std::string
token_factory::pattern() const
{
    std::vector<std::string> sub_patterns;
    std::transform(builders.begin(), builders.end(),
                   std::back_inserter(sub_patterns),
                   [](auto const & builder) { return builder->pattern(); });
    return detail::make_alternation_pattern(sub_patterns);
}

} // namespace ant
