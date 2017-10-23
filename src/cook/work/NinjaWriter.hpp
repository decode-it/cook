#ifndef HEADER_cook_work_NinjaWriter_hpp_ALREADY_INCLUDED
#define HEADER_cook_work_NinjaWriter_hpp_ALREADY_INCLUDED

#include "cook/structure/Recipe.hpp"
#include "cook/Toolchain.hpp"
#include "cook/work/TopologicalOrder.hpp"
#include <list>
#include <unordered_map>

namespace cook { namespace work { 
    
    struct BuildOptions
    {
        std::filesystem::path build_dir;
    };
    
    struct NinjaWriter
    {
        using Recipe = structure::Recipe;
        
        bool operator()(std::ostream & os, const Recipes &recipes, const structure::Uri & default_uri);
        Toolchain toolchain;
        BuildOptions options;
    
    private:
        bool write_header_(std::ostream & os);
        bool write_global_vars_(std::ostream & os);
        bool write_rules_(std::ostream & os);
        bool write_recipe_(std::ostream & os, const Recipe & recipe, const Recipes & order);
    };

} }

#endif
