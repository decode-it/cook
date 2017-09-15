#include "cook/work/TopologicalOrder.hpp"
#include <unordered_set>

namespace cook { namespace work {
    
    bool simplify_order(TopologicalOrder & tgt, const structure::Uri & root, const TopologicalOrder & original_order)
    {
        tgt.recipes.clear();
        tgt.recipes.reserve(original_order.recipes.size());
        
        tgt.books = original_order.books;
        
        std::unordered_set<structure::Uri> to_locate;
        to_locate.insert(root);
                
        for(auto it = original_order.recipes.rbegin(); it != original_order.recipes.rend(); ++it)
        {
            auto * recipe = *it;
            if (to_locate.find(recipe->uri()) != to_locate.end())
            {
                for (auto * dep : recipe->required_recipes())
                    to_locate.insert(dep->uri());
                
                tgt.recipes.push_back(recipe);
            }
        }
        
        std::reverse(tgt.recipes.begin(), tgt.recipes.end());
        
        
        return !tgt.recipes.empty();
    }

} }
