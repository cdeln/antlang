#pragma once

namespace ant
{

template <class Rule, typename Attribute>
struct rule_spec
{
    using rule_type = Rule;
    using attribute_type = Attribute;
};

template <class Spec>
struct rule_of
{
    using type = typename Spec::rule_type;
};

template <typename Spec>
using rule_of_t = typename rule_of<Spec>::type;

template <class Spec>
struct attribute_of
{
    using type = typename Spec::attribute_type;
};

template <class Spec>
using attribute_of_t = typename attribute_of<Spec>::type;

class none
{
};

template <class Token>
struct non_attributed_token_rule :
    rule_spec<
        non_attributed_token_rule<Token>,
        none
    >
{
};

template <class Token, typename Attribute>
struct attributed_token_rule :
    rule_spec<
        attributed_token_rule<Token, Attribute>,
        Attribute
    >
{
};

template <typename T>
struct discard :
    rule_spec<
        discard<T>,
        none
    >
{
};

} // namespace ant
