#include "formatting.hpp"
#include "string_tokenizer.hpp"

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <fstream>
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
                  << token.variant
                  << "\n";
    }
    return 0;
}
