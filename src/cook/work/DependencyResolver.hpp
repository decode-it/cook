#ifndef HEADER_cook_work_DependencyResolver_hpp_ALREADY_INCLUDED
#define HEADER_cook_work_DependencyResolver_hpp_ALREADY_INCLUDED

#include "cook/work/TopologicalOrder.hpp"
#include <vector>

namespace cook { namespace work {
    
    struct DependencyResolver 
    {
        using Book       = structure::Book;
        using Recipe     = structure::Recipe;
        using Element    = structure::Element;
        using Tag        = structure::Tag;
        using Uri        = structure::Uri;
        using TargetType = structure::TargetType;
        using Target     = structure::Target;
        using Config     = structure::Config;
        
        using recipe_iterator = std::vector<Recipe *>::const_reverse_iterator;
        
        using unresolved_dependency = std::pair<Recipe *, Uri>;
        
        bool resolve(Book & root, const Config & config);
        bool resolve(Book & root, const Config & config, std::list<unresolved_dependency> & lst);
        
        const TopologicalOrder & order() const { return order_; }
                
    private:
        using RecipeMap = std::unordered_map<Uri, Recipe *>;
        
        Recipe * find_dependency_(const Recipe & recipe, const Uri & dependency, const RecipeMap & map) const;
        bool set_recipe_target_(const Config & config) const;
        bool knows_type_(TargetType type) const;
        
        bool construct_topological_order_(Book & root, const RecipeMap & map);
        TopologicalOrder order_;
    };

} }

#endif
