#ifndef HEADER_cook_rules_Resolver_hpp_ALREADY_INCLUDED
#define HEADER_cook_rules_Resolver_hpp_ALREADY_INCLUDED

#include "cook/rules/RuleSet.hpp"
#include "cook/model/GlobInfo.hpp"
#include "cook/property/File.hpp"

namespace cook { namespace rules {

class Resolver
{
public:
    Resolver(const RuleSet * rule_set)
        : rule_set_(rule_set)
    {
    }

    bool operator()(model::Recipe & recipe, const model::GlobInfo & globber) const;

private:
    const RuleSet * rule_set_;
};

} }

#endif
