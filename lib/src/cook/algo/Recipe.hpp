#ifndef HEADER_cook_algo_Recipe_hpp_ALREADY_INCLUDED
#define HEADER_cook_algo_Recipe_hpp_ALREADY_INCLUDED

#include "cook/model/Recipe.hpp"
#include "cook/Result.hpp"

namespace cook { namespace algo {

Result topological_order(const std::list<model::Recipe *> & roots, std::list<model::Recipe*> & topological_order);
Result topological_order(model::Recipe * root, std::list<model::Recipe*> & topological_order);


} }

#endif
