#ifndef HEADER_cook_chef_DependentPropagation_hpp_ALREADY_INCLUDED
#define HEADER_cook_chef_DependentPropagation_hpp_ALREADY_INCLUDED

#include "cook/chef/Interface.hpp"
#include <set>

namespace cook { namespace chef {

class DependentPropagation : public Interface
{
public:
    using SelectionFunction = std::function<bool (const LanguageTypePair &)>;

    DependentPropagation();
    explicit DependentPropagation(const SelectionFunction & function);

    bool process(const Context &context, model::Snapshot &snapshot, model::Snapshot & post) const override;

private:
    std::function<bool (const LanguageTypePair &)> selection_;
};

} }



#endif
