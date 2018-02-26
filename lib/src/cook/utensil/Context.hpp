#ifndef HEADER_cook_utensil_Context_hpp_ALREADY_INCLUDED
#define HEADER_cook_utensil_Context_hpp_ALREADY_INCLUDED

#include "cook/model/Recipe.hpp"
#include "cook/model/Environment.hpp"

namespace cook { namespace utensil {

struct Context
{
    model::Recipe * recipe;
    std::vector<model::Recipe *> dependencies;
    std::vector<model::Recipe *> topological_order;

    model::Environment * environment;
};

} }

#endif
