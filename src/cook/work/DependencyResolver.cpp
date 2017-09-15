#include "cook/work/DependencyResolver.hpp"
#include "gubg/mss.hpp"
#include <stack>
#include <cassert>

namespace cook { namespace work {
    
    bool DependencyResolver::resolve(Book & root)
    {
        MSS_BEGIN(bool);
        
        std::list<unresolved_dependency> lst;
        MSS(resolve(root, lst));
        
        for(const auto & p : lst)
            std::cerr << "Recipe '" << p.first->uri() << "': could not find dependency " << p.second << std::endl;
        
        MSS(lst.empty());
        
        MSS_END();
    }
    
    bool DependencyResolver::resolve(Book & root, std::list<unresolved_dependency> & lst )
    {
        MSS_BEGIN(bool);
        
        RecipeMap map;
        
        // create a map from namespace to recipe
        std::stack<Book *> to_visit_;
        to_visit_.push(&root);
        
        while(!to_visit_.empty())
        {
            Book * b = to_visit_.top();
            to_visit_.pop();
            
            // add all recipes to the map
            for(auto & p : b->recipes())
                map[p.second.uri()] = &p.second;

            // and add all subbooks
            for(auto & p : b->subbooks())
                to_visit_.push(&p.second);
        }
        
        // loop over every recipe
        for(const auto & p : map)
        {
            const auto & uri = p.first;
            Recipe * recipe = p.second;
            
            for(const Uri & dependency : recipe->dependencies())
            {
                Recipe * supplier = find_dependency_(*recipe, dependency, map);
                if (!supplier)
                    lst.push_back(std::make_pair(recipe, dependency));
                else
                    recipe->add_required_recipe(supplier);
            }
        }
        
        MSS(construct_topological_order_(root, map), std::cerr << "Cyclic dependency detected" << std::endl);
        
        propagate_merge_();
        
        MSS_END();
    }
    
    void DependencyResolver::propagate_merge_()
    {
        for(Recipe * recipe : order_)
            for(Recipe * dep : recipe->required_recipes())
                recipe->merge(*dep);
    }
    
    bool DependencyResolver::construct_topological_order_(Book & root, const RecipeMap & map)
    {       
        MSS_BEGIN(bool);
        
        // prepare the order vector
        order_.assign(map.size(), nullptr);
        auto it = order_.rbegin();
        
        std::unordered_map<Recipe *, std::size_t> count_map;
        
        // fill a count map
        for (const auto & p : map)
        {
            // make sure this recipe is added to the map
            Recipe * recipe = p.second;
            if (count_map.find(recipe) == count_map.end())
                count_map[recipe] = 0;
            
            // and increment for the dependent ones
            for (Recipe * dep : recipe->required_recipes() )
                count_map[dep] += 1;
        }
        
        // add all elements with zero dependencies to the count map
        std::stack<Recipe *> todo;
        {
            for(const auto & p : count_map)
                if(p.second == 0)
                    todo.push(p.first);
        }
         
        
        
        while(!todo.empty())
        {
            // get a recipe
            Recipe * cur = todo.top();
            todo.pop();

            
            assert(count_map[cur] == 0);
            MSS(it != order_.rend());
            *it++ = cur;
            
                        
            // process the children
            for(Recipe * dep : cur->required_recipes())
            {
                std::size_t & cnt = count_map[dep];
                assert(cnt > 0);

                // only process if all children are visited
                if(--cnt == 0)
                    todo.push(dep);
            }
        }
        
        MSS(it == order_.rend());
        MSS_END();
    }
    
    structure::Recipe * DependencyResolver::find_dependency_(const Recipe & recipe, const Uri & dependency, const RecipeMap & map) const
    {
        const Uri & uri = recipe.book().uri();
        
        auto rng = uri.tags();
        
        do
        {
            auto it = map.find(rng + dependency);
            if (it != map.end())
                return it->second;
        }
        while(rng.drop_back());
        
        return nullptr;
    }
        

} }
