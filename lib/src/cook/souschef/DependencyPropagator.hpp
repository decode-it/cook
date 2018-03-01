#ifndef HEADER_cook_staff_souschef_DependentPropagator_hpp_ALREADY_INCLUDED
#define HEADER_cook_staff_souschef_DependentPropagator_hpp_ALREADY_INCLUDED

#include "cook/souschef/Interface.hpp"
#include <set>

namespace cook { namespace souschef {

class DependentPropagator : public Interface
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

} }



#endif
