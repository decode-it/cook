#ifndef HEADER_cook_process_toolchain_Compiler_hpp_ALREADY_INCLUDED
#define HEADER_cook_process_toolchain_Compiler_hpp_ALREADY_INCLUDED

#include "cook/process/toolchain/Interface.hpp"
#include "cook/Language.hpp"
#include "gubg/mss.hpp"
#include <optional>

namespace cook { namespace process { namespace toolchain { 

class Compiler: public Interface
{
public:
    Compiler(Language language)
        : language(language)
    {
        auto & kvm = key_values_map();
        auto configure = [&](const std::string & key, const std::string & value, const Configuration & conf)
        {
            MSS_BEGIN(bool);

            if (false) {}
            else if (key == "config" && value == "debug")
            {
                MSS(conf.configure("debug_symbols", "true"));
                MSS(conf.configure("config", "rtc"));
            }
            else if (key == "config" && value == "release")
            {
                MSS(conf.configure("optimization", "max_speed"));
                kvm[Part::Define].emplace_back("NDEBUG", "");
            }
            else MSS_Q(false);
            MSS_END();
        };

        add_config(10, configure);
    }

    bool create(command::Compile::Ptr &ptr) const override
    {
        MSS_BEGIN(bool);
        ptr.reset(new command::Compile(key_values_map(), translator_map_ptr()));
        MSS_END();
    }

    const Language language;
};

} } } 

#endif
