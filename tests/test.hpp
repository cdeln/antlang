#pragma once

#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include <typeinfo>

std::ostream&
operator<< (std::ostream& stream, std::type_info const& type);
