#ifndef HEADER_cook_model_toolchain_Config_hpp_ALREADY_INCLUDED
#define HEADER_cook_model_toolchain_Config_hpp_ALREADY_INCLUDED

#include <string>

namespace cook { namespace model { namespace toolchain { 

    struct Config
    {
        std::string name;
        std::string arch;
        std::string config;
    };

} } }

#endif
