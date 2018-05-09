#ifndef HEADER_cook_chai_ToolchainLoader_hpp_ALREADY_INCLUDED
#define HEADER_cook_chai_ToolchainLoader_hpp_ALREADY_INCLUDED

#include "cook/Result.hpp"
#include "gubg/std/filesystem.hpp"
#include <vector>
#include <map>
#include <ostream>
#include <functional>

namespace cook { namespace chai {

    class ToolchainLoader
    {
    public:
        ToolchainLoader();

        void add_include_path(const std::filesystem::path & path);        
        Result load(const std::string & name, std::filesystem::path & toolchain_filename) const;

    private:
        using SerializationFunction = std::function<void (std::ostream &)>;
        
        std::vector<std::filesystem::path> include_paths_;
        std::map<std::string, SerializationFunction> known_serializers_;
    };

} }

#endif

