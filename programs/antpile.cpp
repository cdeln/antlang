#include "formatting.hpp"
#include "tokenizer.hpp"
#include "parser.hpp"
#include "token_rules.hpp"

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

struct failure_handler
{
    std::vector<std::string> lines;

    failure_handler(std::vector<std::string> const& lines)
        : lines(lines)
    {
    }

    void handle(ant::parser_failure const& failure) const
    {
        const int line_index = failure.context.line - 1;
        const std::string line = lines.at(line_index);
        const int line_length = line.size();
        const int pad_left = failure.context.offset - 1;
        const int pad_right = line_length - pad_left - 1;
        std::cout << failure.message
                  << ", at line " << failure.context.line
                  << ", as seen in context here\n"
                  << line << "\n"
                  << std::string(pad_left, '~')
                  << '^'
                  << std::string(pad_right, '~') << "\n\n";
        if (failure.previous)
        {
            handle(*failure.previous);
        }
    }
};

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
    ant::tokenizer tokenizer;

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

    // for (auto token : tokens)
    // {
    //     std::cout << std::setw(4) << token.context.line << " "
    //               << std::setw(4) << token.context.offset << " "
    //               << token_string(token.variant)
    //               << "\n";
    // }

    const auto parser =
        ant::make_parser<ant::ast::function>();


    auto result = parser.parse(tokens.cbegin(), tokens.cend());
    if (is_failure(result))
    {
        failure_handler(lines).handle(get_failure(result));
    }

    return 0;
}
