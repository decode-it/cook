#ifndef HEADER_cook_structure_TargetConfig_hpp_ALREADY_INCLUDED
#define HEADER_cook_structure_TargetConfig_hpp_ALREADY_INCLUDED

#include "gubg/debug.hpp"
#include "gubg/file/System.hpp"
#include <map>
#include <set>
#include <vector>

namespace cook { namespace structure {
    
    using IncludePaths      = std::set<std::filesystem::path>;
    using Defines           = std::map<std::string, std::string>;
    using LibraryPaths      = std::set<std::filesystem::path>;
    using Libraries         = std::vector<std::string>;     //Order is important: we cannot use std::set
    
    struct TargetConfig
    {
        IncludePaths include_paths;
        Defines defines;
        LibraryPaths library_paths;
        Libraries libraries;
    };
    
    inline void merge(TargetConfig & lhs, const TargetConfig & rhs)
    {
        lhs.include_paths.insert(rhs.include_paths.begin(), rhs.include_paths.end());
        lhs.defines.insert(rhs.defines.begin(), rhs.defines.end());
        lhs.library_paths.insert(rhs.library_paths.begin(), rhs.library_paths.end());
        
        auto add_if_non_existant = [&](auto & vct, const auto & lib) 
        {
            if (std::find(vct.begin(), vct.end(), lib) == vct.end())
                vct.push_back(lib);
        };
        
        for(const auto & lib : rhs.libraries)
            add_if_non_existant(lhs.libraries, lib);
    }
} }


#endif
