#ifndef HEADER_cook_chef_assistant_DependentPropagator_hpp_ALREADY_INCLUDED
#define HEADER_cook_chef_assistant_DependentPropagator_hpp_ALREADY_INCLUDED

#include "cook/chef/assistant/Interface.hpp"
#include <set>

namespace cook { namespace chef { namespace assistant {

class DependentPropagator : public Interface
{
public:
    using SelectionFunction = std::function<bool (const LanguageTypePair &)>;

    DependentPropagator();
    explicit DependentPropagator(const SelectionFunction & function);

    Result process(const Context &context, model::Snapshot &snapshot) const override;

private:
    std::function<bool (const LanguageTypePair &)> selection_;
};

} } }



#endif
