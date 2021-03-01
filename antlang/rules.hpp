#pragma once

namespace ant
{

template <class Rule, typename Attribute>
struct rule
{
    using rule_type = Rule;
    using attribute_type = Attribute;
};

template <class Rule>
struct rule_of
{
    using type = typename Rule::rule_type;
};

template <typename T>
using rule_of_t = typename rule_of<T>::type;

template <class Rule>
struct attribute_of
{
    using type = typename Rule::attribute_type;
};

template <class Rule>
using attribute_of_t = typename attribute_of<Rule>::type;

class none
{
};

template <class Token>
struct non_attributed_token_rule
    : public rule
        < non_attributed_token_rule<Token>
        , none >
{
};

template <class Token, typename Attribute>
struct attributed_token_rule
    : public rule
        < attributed_token_rule<Token, Attribute>
        , Attribute >
{
};

template <typename T>
struct discard
    : public rule
        < discard<T>
        , none >
{
};

} // namespace ant
