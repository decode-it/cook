#ifndef HEADER_cook_algo_Recipe_hpp_ALREADY_INCLUDED
#define HEADER_cook_algo_Recipe_hpp_ALREADY_INCLUDED

#include "cook/model/Recipe.hpp"
#include "cook/Result.hpp"

namespace cook { namespace algo {

Result resolve_dependencies(model::Recipe *recipe, bool *all_resolved = nullptr);


} }

#endif
