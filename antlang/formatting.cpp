#include "formatting.hpp"

#include <sstream>

std::string quote(std::string const& x)
{
    std::stringstream result;
    result << "'" << x << "'";
    return result.str();
}

std::ostream&
operator<<(std::ostream& stream, ant::left_parenthesis_token const&)
{
    return stream << '(';
}

std::ostream&
operator<<(std::ostream& stream, ant::right_parenthesis_token const&)
{
    return stream << ')';
}

std::ostream&
operator<<(std::ostream& stream, ant::function_token const& x)
{
    return stream << "function";
}

std::ostream&
operator<<(std::ostream& stream, ant::structure_token const& x)
{
    return stream << "structure";
}

std::ostream&
operator<<(std::ostream& stream, ant::protocol_token const& x)
{
    return stream << "protocol";
}

std::ostream&
operator<<(std::ostream& stream, ant::module_token const& x)
{
    return stream << "module";
}

std::ostream&
operator<<(std::ostream& stream, ant::import_token const& x)
{
    return stream << "import";
}

std::ostream&
operator<<(std::ostream& stream, ant::export_token const& x)
{
    return stream << "export";
}

std::ostream&
operator<<(std::ostream& stream, ant::identifier_token const& x)
{
    return stream << "identifier " << quote(x.name);
}

std::ostream&
operator<<(std::ostream& stream, ant::integer_literal_token const& x)
{
    return stream << "integer " << quote(x.value);
}

std::ostream&
operator<<(std::ostream& stream, ant::floating_point_literal_token const& x)
{
    return stream << "floating " << quote(x.value);
}

std::ostream&
operator<<(std::ostream& stream, ant::token_variant const& variant)
{
    return std::visit([&stream](auto&& arg) -> std::ostream& { return stream << arg; }, variant);
}
