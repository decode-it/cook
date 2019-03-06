#ifndef HEADER_cook_process_souschef_Archiver_hpp_ALREADY_INCLUDED
#define HEADER_cook_process_souschef_Archiver_hpp_ALREADY_INCLUDED

#include "cook/process/souschef/Interface.hpp"
#include "cook/process/command/Interface.hpp"
#include <set>

namespace cook { namespace process { namespace souschef {

class Archiver : public Interface
{
public:
    std::string description() const override { return "Archiver"; }
    Result process(model::Recipe & recipe, RecipeFilteredGraph & file_command_graph, const Context & context) const override;

private:
    Result archive_command_(command::Ptr &, model::Recipe & recipe, const Context & context) const;
};

} } }



#endif
