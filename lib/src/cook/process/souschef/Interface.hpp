#ifndef HEADER_cook_process_chef_Interface_hpp_ALREADY_DEFINED
#define HEADER_cook_process_chef_Interface_hpp_ALREADY_DEFINED

#include "cook/Result.hpp"
#include "cook/Context.hpp"
#include "cook/model/Recipe.hpp"
#include "cook/process/RecipeFilteredGraph.hpp"

namespace cook { namespace process { namespace souschef {

class Interface
{
public:
    virtual ~Interface() {}

    virtual Result process(model::Recipe & recipe, RecipeFilteredGraph & file_command_graph, const Context & context) const = 0;
    virtual std::string description() const  = 0;
};



} } }

#endif
