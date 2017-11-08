#ifndef HEADER_cook_work_TopologicalOrder_hpp_ALREADY_INCLUDED
#define HEADER_cook_work_TopologicalOrder_hpp_ALREADY_INCLUDED

#include "cook/structure/Book.hpp"
#include "cook/structure/Recipe.hpp"
#include "cook/util/Range.hpp"
#include <unordered_set>
#include <stack>

namespace cook { namespace work {
    
    using Recipes = std::vector<structure::Recipe *>;
    using Books = std::vector<structure::Book *>;
    
    template <typename OutputIterator, typename It>
    OutputIterator subset_order(OutputIterator out_it, const structure::Uri & uri, const util::Range<It> & ordered_recipes, bool add_self = false)
    {
        using Recipe = structure::Recipe;
        
        // find the root
        Recipe * root = nullptr;
        {
            auto it = std::find_if(RANGE(ordered_recipes), [&](auto * recipe) { return recipe->uri() == uri; });
            if(it == ordered_recipes.end())
                return out_it;        
            root = *it;
        }
        
        std::unordered_set<Recipe *> sub_dag;
        {
            std::stack<Recipe *> todo;           
            auto add_dependencies = [&](Recipe * cur) { for(auto * dep : cur->required_recipes()) todo.push(dep); };
            add_dependencies(root);
        
            while(!todo.empty())
            {
                Recipe * cur = todo.top();
                todo.pop();
            
                // only process new element
                if (!sub_dag.insert(cur).second)
                    continue;
            
                add_dependencies(cur);
            }
        }
        
        for(Recipe * recipe : ordered_recipes)
            if(sub_dag.find(recipe) != sub_dag.end())
                *out_it++ = recipe;
            
        if (add_self)
            *out_it++ = root;
        
        return out_it;
    }
} }


#endif
