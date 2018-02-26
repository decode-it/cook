#ifndef HEADER_cook_staff_Context_hpp_ALREADY_INCLUDED
#define HEADER_cook_staff_Context_hpp_ALREADY_INCLUDED

#include "cook/model/Recipe.hpp"
#include "cook/model/Environment.hpp"

namespace cook { namespace staff {

struct Context
{
    model::Recipe * recipe;
    std::vector<model::Recipe *> dependencies;
    std::vector<model::Recipe *> topological_order;

    model::Environment * environment;
};

} }

#endif
