#ifndef HEADER_cook_process_RecipeContext_hpp_ALREADY_INCLUDED
#define HEADER_cook_process_RecipeContext_hpp_ALREADY_INCLUDED

#include "cook/model/Recipe.hpp"
#include <list>

namespace cook { namespace process { namespace build {

struct LocalContext
{
    model::Recipe * recipe;
    std::list<const model::Recipe *> topological_order;
    std::list<const model::Snapshot *> dependent_post_shots;
};

} } }

#endif
