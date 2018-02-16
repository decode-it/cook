#include "cook/rules/Resolver.hpp"

namespace cook { namespace rules {

bool Resolver::operator()(LanguageTypePair & key, property::File & file) const
{
    auto accepts = [&](const Interface & interface) { return interface.accepts(key, file); };
    const Interface * ptr = rule_set_->find(accepts);

    if (ptr == nullptr)
        return false;
}

} }
