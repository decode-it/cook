#ifndef HEADER_cook_algo_TopologicalOrder_hpp_ALREADY_INCLUDED
#define HEADER_cook_algo_TopologicalOrder_hpp_ALREADY_INCLUDED

#include "cook/model/Recipe.hpp"
#include "cook/Result.hpp"

namespace cook { namespace algo {

Result make_TopologicalOrder(const std::list<model::Recipe *> & roots, std::list<model::Recipe*> & make_TopologicalOrder);
Result make_TopologicalOrder(model::Recipe * root, std::list<model::Recipe*> & make_TopologicalOrder);


} }

#endif
