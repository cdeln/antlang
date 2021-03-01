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
    using result_type = parser_result<attribute_type>;

    result_type
    parse(std::vector<token>::const_iterator pos,
          std::vector<token>::const_iterator end) const
    {
        result_type result;
        while (pos != end)
        {
            try
            {
                auto sub_parser = make_parser<T>();
                auto sub_result = sub_parser.parse(pos, end);
                if constexpr (!std::is_same_v<attribute_type, none>)
                {
                    result.value.push_back(std::move(sub_result.value));
                }
                pos = sub_result.position;
            }
            catch (unexpected_token_error const&)
            {
                break;
            }
            catch (alternative_parser_error const&)
            {
                break;
            }
        }
        result.position = pos;
        return result;
    }
};

} // namespace ant
