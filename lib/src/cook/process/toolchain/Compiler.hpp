#ifndef HEADER_cook_process_toolchain_Compiler_hpp_ALREADY_INCLUDED
#define HEADER_cook_process_toolchain_Compiler_hpp_ALREADY_INCLUDED

#include "cook/process/toolchain/Manager.hpp"

namespace cook { namespace process { namespace toolchain { 

    inline bool compile_config_cb(Element::Ptr element, const std::string & key, const std::string & value, ConfigurationBoard & board)
    {
        if (element->type() != Element::Compile)
            return false;

        if (false) {}
        else if (key == "config" && value == "debug")
        {            
            board.add_configuration("debug_symbols", "true");
            board.add_configuration("config", "rtc");
        }
        else if (key == "config" && value == "release")
        {
            board.add_configuration("optimization", "max_speed");
            element->key_values_map()[Part::Define].emplace_back("NDEBUG", "");
        }
        else
        {
            return false;
        }

        return true;
    }

    inline bool configure_compiler(Manager & manager, const Language & language)
    {
        MSS_BEGIN(bool);

        {
            Configuration cb(10, "compiler config");
            cb.callback = &compile_config_cb;
            manager.add_configuration_callback(std::move(cb));
        }

        manager.goc_element(Element::Compile, language);

        MSS_END();
    }

} } } 

#endif
