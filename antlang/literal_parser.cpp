#include "literal_parser.hpp"

namespace ant
{

template <>
void set_stream_locale<bool>(std::istream& in)
{
    in >> std::boolalpha;
};

template <>
void set_stream_locale<bool>(std::ostream& out)
{
    out << std::boolalpha;
};

}  // namespace ant
