#include "tokenize.hpp"

#include "pre_processing.hpp"
#include "tokenizer.hpp"

#include <algorithm>

namespace ant
{

std::vector<token>
tokenize(const std::string& source)
{
    std::stringstream stream(source);
    std::vector<std::string> lines;
    std::vector<ant::token> tokens;
    std::string line;
    int line_number = 0;
    ant::tokenizer tokenizer;

    while (std::getline(stream, line))
    {
        line_number += 1;
        lines.push_back(line);
        auto t = tokenizer.tokenize(ant::remove_comments(line));
        std::transform(
            std::move_iterator(t.begin()), std::move_iterator(t.end()),
            std::back_inserter(tokens),
            [line_number](ant::token&& token)
            {
                token.context.line = line_number;
                return std::move(token);
            });
    }

    tokens.push_back({
        ant::end_of_input_token(), {
            !tokens.empty() ? tokens.back().context.line : 1,
            !tokens.empty() ? tokens.back().context.offset : 0
        }
    });

    return tokens;
}

} // namespace ant
