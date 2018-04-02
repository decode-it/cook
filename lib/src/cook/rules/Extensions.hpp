#ifndef HEADER_cook_rules_Extensions_hpp_ALREADY_INCLUDED
#define HEADER_cook_rules_Extensions_hpp_ALREADY_INCLUDED

#include "cook/Language.hpp"
#include "cook/Type.hpp"
#include "gubg/std/filesystem.hpp"
#include <map>
#include <string>

namespace cook { namespace rules { 

    namespace details { 
        using TypeMap = std::map<std::string, Type>;
        bool fill(TypeMap &tm, Language lang);
    } 

    class Extensions
    {
    public:
        Extensions(Language lang) {details::fill(type_per_extension_, lang);}

        bool is_known(const std::string &ext)           const { return type_per_extension_.find(ext) != type_per_extension_.end(); }
        bool is_known(const std::filesystem::path &ext) const { return is_known(ext.string()); }

        bool get_type(Type &out, const std::string &extension, Type in = Type::Undefined) const
        {
            if (in == Type::Undefined)
            {
                auto it = type_per_extension_.find(extension);
                if (it == type_per_extension_.end())
                    return false;
                in = it->second;
            }
            out = in;
            return true;
        }
        bool get_type(Type &out, const std::filesystem::path &extension, Type in = Type::Undefined) const {return get_type(out, extension.string(), in);}

    private:
        details::TypeMap type_per_extension_;
    };

} } 

#endif
