#ifndef HEADER_cook_process_souschef_Linker_hpp_ALREADY_INCLUDED
#define HEADER_cook_process_souschef_Linker_hpp_ALREADY_INCLUDED

#include "cook/process/souschef/Interface.hpp"
#include "cook/process/command/Interface.hpp"
#include <set>

namespace cook { namespace process { namespace souschef {

class Linker : public Interface
{
public:
    std::string description() const override { return "LibraryLinker"; }
    Result process(model::Recipe & recipe, RecipeFilteredGraph & file_command_graph, const Context & context) const override;

private:
    virtual ingredient::File construct_link_file(model::Recipe & recipe, const Context &context) const;
    Result link_command_(command::Ptr &, const model::Recipe &recipe, const model::Recipe::KeyValues::Range &libs, const Context & context) const;
};

} } }



#endif
