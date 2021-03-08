#pragma once

#include "rules.hpp"
#include "parser.hpp"
#include "tokens.hpp"

#include <vector>

namespace ant
{

template <typename T>
struct parser<discard<T>>
{
    parser_result<none>
    parse(std::vector<token>::const_iterator pos,
          std::vector<token>::const_iterator end) const
    {
        const auto parser = make_parser<T>();
        auto result = parser.parse(pos, end);
        if (is_success(result))
        {
            const auto [value, next] = get_success(result);
            static_cast<void>(value); // a.k.a. discard
            return parser_success<none>{none{}, next};
        }
        else
        {
            return std::move(get_failure(result));
        }
    }
};

} // namespace ant
