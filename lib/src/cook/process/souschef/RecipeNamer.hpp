#ifndef HEADER_cook_process_souschef_RecipeNamer_hpp_ALREADY_INCLUDED
#define HEADER_cook_process_souschef_RecipeNamer_hpp_ALREADY_INCLUDED

#include "cook/process/souschef/Interface.hpp"
#include "gubg/stream.hpp"

namespace cook { namespace process { namespace souschef {

class RecipeNamer : public Interface
{
public:
    std::string description() const override { return "Recipe name"; }
    Result process(model::Recipe & recipe, RecipeFilteredGraph & file_command_graph, const Context & context) const override;
};

} } }



#endif
