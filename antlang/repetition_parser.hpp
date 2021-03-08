#pragma once

#include "exceptions.hpp"
#include "parser.hpp"
#include "repetition.hpp"

#include <vector>

namespace ant
{

template <typename T>
struct parser<repetition<T>>
{
    using attribute_type = attribute_of_t<repetition<T>>;

    parser_result<attribute_type>
    parse(std::vector<token>::const_iterator pos,
          std::vector<token>::const_iterator end) const
    {
        parser_success<attribute_type> result;
        while (pos != end)
        {
            auto sub_parser = make_parser<T>();
            auto sub_result = sub_parser.parse(pos, end);
            if (is_success(sub_result))
            {
                auto [value, next] = get_success(sub_result);
                if constexpr (!std::is_same_v<attribute_type, none>)
                {
                    result.value.push_back(std::move(value));
                }
                else
                {
                    static_cast<void>(value);
                }
                pos = next;
            }
            else
            {
                break;
            }
        }
        result.position = pos;
        return result;
    }
};

} // namespace ant
