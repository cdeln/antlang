#include "string_tokenizer.hpp"

namespace ant
{

string_tokenizer::string_tokenizer(token_factory&& factory)
    : pattern(factory.pattern())
    , factory(std::move(factory))
{
}

string_tokenizer::string_tokenizer()
    : string_tokenizer(token_factory_builder<token_variant>::make())
{
}

std::vector<token>
string_tokenizer::tokenize(std::string const& source) const
{
    const auto matches_end = std::sregex_iterator();
    std::vector<token> tokens;
    for (auto matches = std::sregex_iterator(source.begin(), source.end(), pattern);
         matches != matches_end;
         ++matches)
    {
        for (auto sub_match = matches->begin() + 1; sub_match != matches->end(); ++sub_match)
        {
            if (sub_match->length() == 0)
                continue;
            const int sub_match_index = std::distance(matches->begin() + 1, sub_match);
            const int sub_match_position = matches->position(1 + sub_match_index);
            token_variant variant = factory.create(sub_match_index, sub_match->str());
            token_context context = {
                -1,
                static_cast<int>(sub_match_position) + 1
            };
            tokens.push_back({std::move(variant), std::move(context)});
        }
    }
    return tokens;
}

} // namespace ant
