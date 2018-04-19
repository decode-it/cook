#ifndef HEADER_cook_process_souschef_LinkOrder_hpp_ALREADY_INCLUDED
#define HEADER_cook_process_souschef_LinkOrder_hpp_ALREADY_INCLUDED

#include "cook/process/souschef/Interface.hpp"

namespace cook { namespace process { namespace souschef {

class LinkLibrarySorter : public Interface
{
public:
    std::string description() const override { return "Link library sorter"; }
    Result process(model::Recipe & recipe, RecipeFilteredGraph & file_command_graph, const Context & context) const override;

private:
    Result process_(model::Recipe &, ingredient::Collection<ingredient::KeyValue> &libraries, const process::Menu &menu) const;
};

} } }

#endif
