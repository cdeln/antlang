#include "compiler.hpp"
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
read_file(std::ifstream& file)
{
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
    std::string file_name;
    std::vector<std::string> lines;

    failure_handler(std::string const& file_name,
                    std::vector<std::string> const& lines)
        : file_name{file_name}
        , lines(lines)
    {
    }


    void show_context_info(ant::token_context context)
    {
        const int line_index = context.line - 1;
        const std::string line = lines.at(line_index);
        const int line_length = line.size();
        const int pad_left = context.offset - 1;
        const int pad_right = line_length - pad_left - 1;
        std::cout << '\n' << line << '\n'
                  << std::string(pad_left, '~')
                  << '^'
                  << std::string(pad_right, '~') << '\n';
    }
};

struct parser_failure_handler : failure_handler
{
    using failure_handler::failure_handler;

    void handle(ant::parser_failure const& failure)
    {
        std::cout << file_name << ":" << failure.context.line << ": " << failure.message;
        if (failure.children.empty())
        {
            show_context_info(failure.context);
        }
        std::cout << '\n';
        for (auto const& sub_failure : failure.children)
        {
            handle(sub_failure);
        }
    }
};

struct compiler_failure_handler : failure_handler
{
    using failure_handler::failure_handler;

    void handle(ant::compiler_failure const& failure)
    {
        std::cout << file_name << ":" << failure.context.line << ": " << failure.message;
        const ant::token_context context = failure.context;
        if (context.line)
        {
            show_context_info(context);
        }
        std::cout << '\n';
    }
};

template <typename T>
std::string format(T const& terminal);

std::string format(ant::runtime::structure const& structure);

std::string format(ant::runtime::value_variant const& value);

template <typename T>
std::string format(T const& terminal)
{
    std::stringstream result;
    result << terminal;
    return result.str();
}

std::string format(ant::runtime::structure const& structure)
{
    std::stringstream result;
    result << '{';
    for (auto const& field : structure.fields)
    {
        result << format(field);
    }
    result << '}';
    return result.str();
}

std::string format(ant::runtime::value_variant const& value)
{
    return visit([](auto const& x) { return format(x); },
                      static_cast<ant::runtime::value_variant_base const&>(value));
}

void print(ant::runtime::value_variant const& value)
{
    std::cout << format(value) << '\n';
}

int main(int argc, char** argv)
{
    const int arg_count = argc - 1;
    if (arg_count != 1)
    {
        std::cerr << "\n\tInvalid number of arguments, usage: " << argv[0] << " input-files\n\n";
        return -1;
    }
    const std::string input_file_path(argv[1]);
    std::ifstream input_file(input_file_path);
    if (!input_file)
    {
        std::cerr << "No such file " << ant::quote(input_file_path) << '\n';
        return -1;
    }
    const std::string source_code = read_file(input_file);
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

    tokens.push_back({ant::end_of_input_token(), {}});

    const auto parser = ant::make_parser<ant::ast::program>();

    const auto parsed = parser.parse(tokens.cbegin(), tokens.cend());

    if (is_failure(parsed))
    {
        parser_failure_handler(input_file_path, lines).handle(get_failure(parsed));
        return -1;
    }

    ant::ast::program const& statements = ant::get_success(parsed).value;

    auto [env, prog] = ant::setup_compiler();
    const std::vector<ant::compiler_status> compile_info = compile(prog, env, statements);
    for (auto const& status : compile_info)
    {
        if (is_failure(status))
        {
            compiler_failure_handler(input_file_path, lines).handle(get_failure(status));
            return -1;
        }
    }

    for (auto& eval : prog.evaluations)
    {
        ant::runtime::value_variant result = execute(eval);
        print(result);
    }

    return 0;
}
