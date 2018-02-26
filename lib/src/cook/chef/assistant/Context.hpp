#ifndef HEADER_cook_chef_assistant_Context_hpp_ALREADY_INCLUDED
#define HEADER_cook_chef_assistant_Context_hpp_ALREADY_INCLUDED

#include "cook/model/Recipe.hpp"
#include "cook/model/Environment.hpp"

namespace cook { namespace chef { namespace assistant {

struct Context
{
    model::Recipe * recipe;
    std::vector<model::Recipe *> dependencies;
    std::list<model::Recipe *> topological_order;

    model::Environment * environment;
};

} } }

#endif
