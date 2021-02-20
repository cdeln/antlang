#include "formatting.hpp"
#include "string_tokenizer.hpp"
#include "parser.hpp"

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <queue>
#include <streambuf>
#include <string>
#include <utility>
#include <vector>

std::string
read_file(std::string const& filename)
{
    std::ifstream file(filename);
    file.seekg(std::ios::end);
    auto size = file.tellg();
    file.seekg(std::ios::beg);
    std::string content;
    content.reserve(size);
    return std::string(
        std::istreambuf_iterator<char>(file),
        std::istreambuf_iterator<char>());
}

int main(int argc, char** argv)
{
    const int arg_count = argc - 1;
    if (arg_count != 1)
    {
        std::cerr << "\n\tInvalid number of arguments, usage: " << argv[0] << " input-files\n\n";
        return -1;
    }
    const std::string input_file(argv[1]);
    const std::string source_code = read_file(argv[1]);
    std::stringstream stream(source_code);
    std::vector<std::string> lines;
    std::vector<ant::token> tokens;
    std::string line;
    int line_number = 0;
    ant::string_tokenizer tokenizer;

    while (std::getline(stream, line))
    {
        line_number += 1;
        lines.push_back(line);
        auto t = tokenizer.tokenize(line);
        std::transform(
            std::move_iterator(t.begin()), std::move_iterator(t.end()),
            std::back_inserter(tokens),
            [line_number](ant::token&& token)
            {
                token.context.line = line_number;
                return std::move(token);
            });
    }

    for (auto token : tokens)
    {
        std::cout << std::setw(4) << token.context.line << " "
                  << std::setw(4) << token.context.offset << " "
                  << token_string(token.variant)
                  << "\n";
    }

    ant::parser<
        ant::sequence<
            ant::left_parenthesis_token,
            ant::structure_token,
            ant::repetition<ant::identifier_token>,
            ant::right_parenthesis_token
        >
    > parser;

    try
    {
        auto parsed = parser.parse(tokens.cbegin(), tokens.cend());
    }
    catch (ant::unexpected_token_error const& error)
    {
        const int line_index = error.context.line - 1;
        const std::string line = lines.at(line_index);
        const int line_length = line.size();
        const int pad_left = error.context.offset - 1;
        const int pad_right = line_length - pad_left - 1;
        const std::string indent(4, ' ');
        std::cout << error.what();
        std::cout << ", as seen in context here\n"
                  << indent << line << "\n"
                  << indent << std::string(pad_left, '~')
                  << '^'
                  << std::string(pad_right, '~') << '\n';
    }

    return 0;
}
