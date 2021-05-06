#include "alternative_parser.hpp"

namespace ant
{

ptrdiff_t
get_longest_failure_offset(std::vector<token>::const_iterator position,
                           parser_failure const& failure)
{
    ptrdiff_t result = std::distance(position, failure.position);
    for (auto const& child : failure.children)
    {
        result = std::max(result, get_longest_failure_offset(position, child));
    }
    return result;
}

} // namespace ant
