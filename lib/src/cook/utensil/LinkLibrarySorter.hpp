#ifndef HEADER_cook_utensil_LinkOrder_hpp_ALREADY_INCLUDED
#define HEADER_cook_utensil_LinkOrder_hpp_ALREADY_INCLUDED

#include "cook/utensil/Interface.hpp"

namespace cook { namespace utensil {

class LinkLibrarySorter : public Interface
{
public:
    Result process(const Context & context, model::Snapshot & snapshot) const override;

private:
    Result process_(const Context & context, model::Snapshot & snapshot, ingredient::Collection<ingredient::File> & libraries) const;
};

} }

#endif
