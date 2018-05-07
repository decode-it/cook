#ifndef HEADER_cook_process_toolchain_Linker_hpp_ALREADY_INCLUDED
#define HEADER_cook_process_toolchain_Linker_hpp_ALREADY_INCLUDED

#include "cook/process/toolchain/Interface.hpp"

namespace cook { namespace process { namespace toolchain { 

    class Linker: public Interface
    {
    public:
        Linker()
        {
            auto configure = [&](const std::string &key, const std::string &value, const Configuration & conf)
            {
                MSS_BEGIN(bool);
                if (false) {}
                else if (key == "config" && value == "debug")
                {
                    MSS(conf.configure("config", "rtc"));
                }
                else MSS_Q(false);
                MSS_END();
            };
            add_config(1, configure);
        }

        bool create(command::Link::Ptr &ptr) const override
        {
            MSS_BEGIN(bool);
            ptr.reset(new command::Link(key_values_map(), translator_map_ptr()));
            MSS_END();
        }
    };

} } } 

#endif
