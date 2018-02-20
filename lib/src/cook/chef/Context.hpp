#ifndef HEADER_cook_chef_Context_hpp_ALREADY_INCLUDED
#define HEADER_cook_chef_Context_hpp_ALREADY_INCLUDED

#include "cook/model/Recipe.hpp"

namespace cook { namespace chef {

struct Context
{
    model::Recipe * recipe;
    std::vector<model::Recipe *> dependencies;
    std::vector<model::Recipe *> topological_order;
};

} }

#endif
