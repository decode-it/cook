#include "cook/work/DependencyResolver.hpp"
#include "gubg/mss.hpp"
#include <stack>
#include <cassert>

namespace cook { namespace work {
    
    bool DependencyResolver::resolve(Book & root, const Config & config)
    {
        MSS_BEGIN(bool);
        
        std::list<unresolved_dependency> lst;
        MSS(resolve(root, config, lst));
        
        for(const auto & p : lst)
            std::cerr << "Recipe '" << p.first->uri() << "': could not find dependency " << p.second << std::endl;
        
        MSS(lst.empty());
        
        MSS_END();
    }
    
    bool DependencyResolver::resolve(Book & root, const Config & config, std::list<unresolved_dependency> & lst )
    {
        MSS_BEGIN(bool);
        
        RecipeMap map;
        
        // initialize the book_order
        ordered_books_.clear();
        ordered_books_.push_back(&root);
        std::size_t pos = 0;
        
        for(std::size_t pos = 0; pos < ordered_books_.size(); ++pos)
        {
            Book * b = ordered_books_[pos];
            
            
            for (auto & p: b->elements())
            {
                // add all recipes to the map
                {
                    Recipe * e = dynamic_cast<Recipe *>(p.second);
                    if (e)
                        map[e->uri()] = e;
                }
                    
                // and add all books to the todo
                {
                    Book * e = dynamic_cast<Book *>(p.second);
                    if (e)
                        ordered_books_.push_back(e);
                }
            }
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
        
        MSS(set_recipe_target_(config));
        
        MSS_END();
    }

    bool DependencyResolver::set_recipe_target_(const Config & config) const
    {
        MSS_BEGIN(bool);
        
        for (Recipe * recipe : ordered_recipes_)
        {
            recipe->add_local_include_paths();
            
            MSS(knows_type_(recipe->target_type()), std::cerr << "Unknown target type for " << recipe->uri() << std::endl);
            recipe->set_target_identifier(recipe->propose_target_identifier());
            
            MSS(recipe->configure(config));
        }
        
        MSS_END();
    }
    
    bool DependencyResolver::knows_type_(TargetType type) const
    {
        switch(type)
        {
            case TargetType::StaticLibrary:
            case TargetType::Executable:
                return true;
                
            default:
                return false;
        }
    }
           
    bool DependencyResolver::construct_topological_order_(Book & root, const RecipeMap & map)
    {       
        MSS_BEGIN(bool);
        
        // prepare the order vector
        ordered_recipes_.assign(map.size(), nullptr);
        auto it = ordered_recipes_.rbegin();
        
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
            MSS(it != ordered_recipes_.rend());
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
        
        MSS(it == ordered_recipes_.rend());
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
