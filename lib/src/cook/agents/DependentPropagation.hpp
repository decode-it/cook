#ifndef HEADER_cook_agents_DependentPropagation_hpp_ALREADY_INCLUDED
#define HEADER_cook_agents_DependentPropagation_hpp_ALREADY_INCLUDED

#include "cook/agents/Interface.hpp"
#include <set>

namespace cook { namespace agents {

class DependentPropagation : public Interface
{
public:
    using SelectionFunction = std::function<bool (const LanguageTypePair &)>;

    DependentPropagation(const SelectionFunction & function = SelectionFunction());

    bool process(const Context &context, model::Snapshot &snapshot, model::Snapshot & post) const override;

private:
    std::function<bool (const LanguageTypePair &)> selection_;
};

} }



#endif
