#pragma once

#include <string>
#include <vector>

namespace ant
{
namespace ast
{

struct parameter
{
    std::string type;
    std::string name;
};

struct function
{
    std::string name;
    std::string return_type;
    std::vector<parameter> parameters;
};

} // namespace ast
} // namespace ant
