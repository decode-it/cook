#ifndef HEADER_cook_process_toolchain_gcc_Archiver_hpp_ALREADY_INCLUDED
#define HEADER_cook_process_toolchain_gcc_Archiver_hpp_ALREADY_INCLUDED

#include "cook/process/toolchain/Archiver.hpp"

namespace cook { namespace process { namespace toolchain { namespace gcc {

    inline bool set_archiver(Archiver & archiver)
    {
        MSS_BEGIN(bool);
        auto & trans = archiver.translator_map();
        auto & kvm = archiver.key_values_map();
        trans[Part::Cli] = [](const std::string &, const std::string &){return "ar";};
        trans[Part::Pre] = [](const std::string &k, const std::string &v) {return k;};
        trans[Part::Output] = [](const std::string &k, const std::string &v){return k;};
        trans[Part::Input] = [](const std::string &k, const std::string &v){return k;};

        kvm[Part::Pre].clear();
        switch (get_os())
        {
            case OS::Linux: 
                kvm[Part::Pre].emplace_back("crf", ""); 
                break;
            case OS::MacOS: 
                kvm[Part::Pre].emplace_back("crs", ""); 
                break;
            default:
                MSS(false);
                break;
        }

        MSS_END();
    }

} } } }

#endif
