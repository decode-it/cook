#ifndef HEADER_cook_process_souschef_ScriptRunner_hpp_ALREADY_INCLUDED
#define HEADER_cook_process_souschef_ScriptRunner_hpp_ALREADY_INCLUDED

#include "cook/process/souschef/Interface.hpp"

namespace cook { namespace process { namespace souschef {

class ScriptRunner : public Interface
{
public:
    Result process(model::Recipe & recipe, RecipeFilteredGraph & file_command_graph, const Context & context) const override;
    std::string description() const { return "scriptrunner"; }
};

} } }

#endif
