#include "pre_processing.hpp"

namespace ant
{

std::string remove_comments(std::string const& source)
{
    auto i = source.find(";");
    return source.substr(0, i);
}

} // namespace ant
