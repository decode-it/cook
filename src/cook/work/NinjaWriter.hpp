#ifndef HEADER_cook_work_NinjaWriter_hpp_ALREADY_INCLUDED
#define HEADER_cook_work_NinjaWriter_hpp_ALREADY_INCLUDED

#include "cook/structure/Recipe.hpp"
#include "cook/work/TopologicalOrder.hpp"
#include <list>
#include <unordered_map>

namespace cook { namespace work { 
    
    struct BuildOptions
    {
        std::filesystem::path build_dir;
        std::string compiler;
        std::string linker;
        std::string archiver;
        std::string cflags;
        std::string lflags;
        std::string additional_defines;
        std::string arch;
        std::string config;
    };
    
    struct NinjaWriter
    {
        using Recipe = structure::Recipe;
        
        bool operator()(std::ostream & ofs, const Recipes &recipes, const structure::Uri & default_uri);
        BuildOptions options;
    
    private:
        bool write_recipe_(std::ostream & ofs, const Recipe & recipe, const Recipes & order);
        bool write_header_(std::ostream & ofs);
    };

} }

#endif
