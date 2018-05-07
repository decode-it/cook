#ifndef HEADER_cook_chai_Toolchain_hpp_ALREADY_INCLUDED
#define HEADER_cook_chai_Toolchain_hpp_ALREADY_INCLUDED

#include "cook/process/toolchain/Manager.hpp"
#include <string>

namespace cook { namespace chai {

    class Toolchain
    {
    public:
        Toolchain(process::toolchain::Manager * manager);
        
        bool has_config(const std::string & key) const;
        std::string config_value(const std::string & key) const;
        void set_config_value(const std::string & key, const std::string & value);

    private:
        process::toolchain::Manager * manager_;

    };

} }

#endif

