#ifndef HEADER_cook_staff_LinkOrder_hpp_ALREADY_INCLUDED
#define HEADER_cook_staff_LinkOrder_hpp_ALREADY_INCLUDED

#include "cook/staff/Interface.hpp"

namespace cook { namespace staff {

class LinkLibrarySorter : public Interface
{
public:
    Result process(const Context & context, model::Snapshot & snapshot) const override;

private:
    Result process_(const Context & context, model::Snapshot & snapshot, ingredient::Collection<ingredient::File> & libraries) const;
};

} }

#endif
