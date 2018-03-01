#ifndef HEADER_cook_staff_souschef_LinkOrder_hpp_ALREADY_INCLUDED
#define HEADER_cook_staff_souschef_LinkOrder_hpp_ALREADY_INCLUDED

#include "cook/souschef/Interface.hpp"

namespace cook { namespace souschef {

class LinkLibrarySorter : public Interface
{
public:
    std::string description() const override { return "Link library sorter"; }
    Result process(const Context & context, model::Snapshot & snapshot) const override;

private:
    Result process_(const Context & context, model::Snapshot & snapshot, ingredient::Collection<ingredient::File> & libraries) const;
};

} }

#endif
