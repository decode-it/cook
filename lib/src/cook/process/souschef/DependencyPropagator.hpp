#ifndef HEADER_cook_process_souschef_DependentPropagator_hpp_ALREADY_INCLUDED
#define HEADER_cook_process_souschef_DependentPropagator_hpp_ALREADY_INCLUDED

#include "cook/process/souschef/Interface.hpp"
#include <set>

namespace cook { namespace process { namespace souschef {

class DependentPropagator : public Interface
{
public:
    using SelectionFunction = std::function<bool (const LanguageTypePair &)>;

    DependentPropagator();
    explicit DependentPropagator(const SelectionFunction & function);

    std::string description() const override { return "Dependent ingredient propagation"; }
    Result process(const Context &, model::Recipe &) const override;

private:
    std::function<bool (const LanguageTypePair &)> selection_;
};

} } }



#endif
