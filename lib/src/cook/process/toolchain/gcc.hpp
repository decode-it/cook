#ifndef HEADER_cook_process_toolchain_gcc_hpp_ALREADY_INCLUDED
#define HEADER_cook_process_toolchain_gcc_hpp_ALREADY_INCLUDED

#include "cook/Language.hpp"
#include <string>

namespace cook { namespace process { namespace toolchain {

    inline std::string standard_executable(Language language, const std::string & subbrand)
    {
        if (false) {}
        else if (subbrand == "gcc")
        {
            switch(language)
            {
                case Language::C: 
                    return "gcc";
                case Language::CXX: 
                    return "g++";
                default:
                    return "";
            }
        }
        else if (subbrand == "clang")
        {
            switch(language)
            {
                case Language::C: 
                    return "clang";
                case Language::CXX: 
                    return "clang++";
                default:
                    return "";
            }
        }
        else
        {
            switch(language)
            {
                case Language::C:
                    return "CC";
                case Language::CXX:
                    return "CXX";
                default:
                    return "";
            }
        }
    }

} } }

#endif


