#include "test.hpp"

#include <boost/core/demangle.hpp>

std::ostream&
operator<< (std::ostream& stream, std::type_info const& type)
{
    return stream << boost::core::demangle(type.name());
}
