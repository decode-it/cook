#ifndef HEADER_cook_work_NinjaWriter_hpp_ALREADY_INCLUDED
#define HEADER_cook_work_NinjaWriter_hpp_ALREADY_INCLUDED

#include "cook/work/TopologicalOrder.hpp"

namespace cook { namespace work { 
    
    struct BuildOptions
    {
        std::filesystem::path build_dir;
        std::filesystem::path output_dir;
        std::string compiler;
        std::string linker;
        std::string archiver;
        std::string cflags;
        std::string additional_defines;
        std::string arch;
        std::string config;
    };
    
    struct NinjaTarget
    {
        std::filesystem::path location;
        structure::TargetType type;
        
        std::set<std::filesystem::path> extra_objects;
        std::set<std::filesystem::path> extra_libraries;
        std::set<std::filesystem::path> extra_library_paths;
    };

    struct NinjaWriter
    {
        bool operator()(std::ostream & ofs, const TopologicalOrder & order, const structure::Uri & default_uri);
        BuildOptions options;
    
    private:
        bool write_recipe_(std::ostream & ofs, const structure::Recipe & recipe);
        bool write_header_(std::ostream & ofs);
        
        bool decide_targets_(const TopologicalOrder & order);
        std::map<structure::Uri, NinjaTarget> targets_;
    };

} }

#endif
