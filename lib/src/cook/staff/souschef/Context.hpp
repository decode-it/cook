#ifndef HEADER_cook_staff_Context_hpp_ALREADY_INCLUDED
#define HEADER_cook_staff_Context_hpp_ALREADY_INCLUDED

#include "cook/model/Recipe.hpp"
#include "cook/model/Dirs.hpp"
#include "cook/process/DependencyDAG.hpp"
#include <vector>
#include <list>

namespace cook { namespace staff { namespace souschef {

struct Context
{
    model::Recipe * recipe;
    std::vector<model::Recipe *> dependencies;
    std::list<model::Recipe *> topological_order;
    const model::Dirs * dirs;

    process::DependencyDAG * execution_graph;
};

} } }

#endif