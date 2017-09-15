#ifndef HEADER_cook_work_TopologicalOrder_hpp_ALREADY_INCLUDED
#define HEADER_cook_work_TopologicalOrder_hpp_ALREADY_INCLUDED

#include "cook/structure/Book.hpp"
#include "cook/structure/Recipe.hpp"

namespace cook { namespace work {
    
    struct TopologicalOrder
    {
        std::vector<structure::Recipe *> recipes;
        std::vector<structure::Book *> books;
    };
    
    
    bool simplify_order(TopologicalOrder & tgt, const structure::Uri & root, const TopologicalOrder & original_order);
} }


#endif
