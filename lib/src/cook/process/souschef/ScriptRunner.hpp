#ifndef HEADER_cook_process_souschef_ScriptRunner_hpp_ALREADY_INCLUDED
#define HEADER_cook_process_souschef_ScriptRunner_hpp_ALREADY_INCLUDED

#include "cook/process/souschef/Interface.hpp"

namespace cook { namespace process { namespace souschef {

class ScriptRunner : public Interface
{
public:
    ScriptRunner(bool do_execute): do_execute_(do_execute) {}

    Result process(model::Recipe & recipe, RecipeFilteredGraph & file_command_graph, const Context & context) const override;
    std::string description() const override { return "scriptrunner"; }

private:
    const bool do_execute_;
};

} } }

#endif
