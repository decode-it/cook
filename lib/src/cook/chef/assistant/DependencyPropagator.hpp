#ifndef HEADER_cook_chef_assistant_DependentPropagator_hpp_ALREADY_INCLUDED
#define HEADER_cook_chef_assistant_DependentPropagator_hpp_ALREADY_INCLUDED

#include "cook/chef/Assistant.hpp"
#include <set>

namespace cook { namespace chef { namespace assistant {

class DependentPropagator : public Assistant
{
public:
    using SelectionFunction = std::function<bool (const LanguageTypePair &)>;

    DependentPropagator();
    explicit DependentPropagator(const SelectionFunction & function);

    std::string description() const override { return "Dependent ingredient propagation"; }
    Result process(const Context &context, model::Snapshot &snapshot) const override;

private:
    std::function<bool (const LanguageTypePair &)> selection_;
};

} } }



#endif
