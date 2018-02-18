#ifndef HEADER_cook_agents_Context_hpp_ALREADY_INCLUDED
#define HEADER_cook_agents_Context_hpp_ALREADY_INCLUDED

#include "cook/model/Recipe.hpp"

namespace cook { namespace agents {

struct Context
{
    model::Recipe * recipe;
    std::vector<model::Recipe *> dependencies;
    std::vector<model::Recipe *> topological_order;
};

} }

#endif
