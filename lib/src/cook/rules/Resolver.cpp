#include "cook/rules/Resolver.hpp"

namespace cook { namespace rules {

bool Resolver::operator()(LanguageTypePair & key, property::File & file) const
{
    auto adapt = [&](const Interface & interface) { return interface.adapt(key, file); };
    return rule_set_->find(adapt) != nullptr;
}

} }
