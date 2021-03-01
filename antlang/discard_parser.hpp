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
        return {none{}, parser.parse(pos,end).position};
    }
};

} // namespace ant
