#ifndef HEADER_cook_work_NinjaWriter_hpp_ALREADY_INCLUDED
#define HEADER_cook_work_NinjaWriter_hpp_ALREADY_INCLUDED

#include "cook/structure/Recipe.hpp"
#include <list>
#include <unordered_map>

namespace cook { namespace work { 
    
    struct BuildOptions
    {
        std::filesystem::path build_dir;
        std::filesystem::path deploy_dir;
        std::string compiler;
        std::string linker;
        std::string archiver;
        std::string cflags;
        std::string additional_defines;
        std::string arch;
        std::string config;
    };
    
    struct NinjaWriter
    {
        using Recipe = structure::Recipe;
        
        bool operator()(std::ostream & ofs, const std::list<Recipe *> & order, const structure::Uri & default_uri);
        bool operator()(std::ostream & ofs, const std::list<Recipe *> & recipes);
        BuildOptions options;
    
    private:
        bool write_recipe_(std::ostream & ofs, const Recipe & recipe, const std::list<Recipe *> & order);
        bool write_header_(std::ostream & ofs);
    };

} }

#endif
