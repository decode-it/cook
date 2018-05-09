#ifndef HEADER_cook_process_toolchain_Linker_hpp_ALREADY_INCLUDED
#define HEADER_cook_process_toolchain_Linker_hpp_ALREADY_INCLUDED

#include "cook/process/toolchain/Manager.hpp"

namespace cook { namespace process { namespace toolchain { 

    inline bool link_config_cb(Element::Ptr element, const std::string & key, const std::string & value, ConfigurationBoard & board)
    {
        if (element->type() != Element::Link)
            return false;

        if (false) {}
        else if (key == "config" && value == "debug")
            board.add_configuration("config", "rtc");
        else
            return false;

        return true;
    }

    inline bool configure_link(Manager & manager)
    {
        MSS_BEGIN(bool);

        manager.goc_element(Element::Link, Language::Binary);

        {
            Configuration cb(10, "linker configuration");
            cb.callback = link_config_cb;
            manager.add_configuration_callback(std::move(cb));
        }
 
        MSS_END();
    }

} } } 

#endif
