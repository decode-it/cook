#ifndef HEADER_cook_staff_Context_hpp_ALREADY_INCLUDED
#define HEADER_cook_staff_Context_hpp_ALREADY_INCLUDED

#include "cook/model/Recipe.hpp"
#include "cook/model/Environment.hpp"
#include "cook/process/FileDependencyGraph.hpp"
#include <vector>
#include <list>

namespace cook { namespace staff { namespace souschef {

struct Context
{
    model::Recipe * recipe;
    std::vector<model::Recipe *> dependencies;
    std::list<model::Recipe *> topological_order;
    model::Environment * environment;

    process::FileDependencyGraph * execution_graph;
};

} } }

#endif
