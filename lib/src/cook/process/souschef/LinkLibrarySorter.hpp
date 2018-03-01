#ifndef HEADER_cook_process_souschef_LinkOrder_hpp_ALREADY_INCLUDED
#define HEADER_cook_process_souschef_LinkOrder_hpp_ALREADY_INCLUDED

#include "cook/process/souschef/Interface.hpp"

namespace cook { namespace process { namespace souschef {

class LinkLibrarySorter : public Interface
{
public:
    std::string description() const override { return "Link library sorter"; }
    Result process(const Context &, model::Recipe &) const override;

private:
    Result process_(const Context &, model::Recipe &, ingredient::Collection<ingredient::File> & libraries) const;
};

} } }

#endif
