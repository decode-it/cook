#ifndef HEADER_cook_process_toolchain_Loader_hpp_ALREADY_INCLUDED
#define HEADER_cook_process_toolchain_Loader_hpp_ALREADY_INCLUDED

#include "cook/Result.hpp"
#include "gubg/std/filesystem.hpp"
#include <list>
#include <map>
#include <ostream>
#include <functional>

namespace cook { namespace process { namespace toolchain {

    class Loader
    {
    public:
        Loader();

        void append_include_path(const std::filesystem::path & path);        
        Result load(const std::string & name, std::filesystem::path & toolchain_filename) const;

    private:
        static std::filesystem::path get_cook_home_path();
        bool translate_(const std::string & base, std::string & translated) const;
        
        using SerializationFunction = std::function<void (std::ostream &)>;
        
        std::list<std::filesystem::path> include_paths_;
        std::map<std::string, SerializationFunction> known_serializers_;
    };

} } }

#endif

