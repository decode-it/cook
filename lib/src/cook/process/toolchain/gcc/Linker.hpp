#ifndef HEADER_cook_process_toolchain_gcc_Linker_hpp_ALREADY_INCLUDED
#define HEADER_cook_process_toolchain_gcc_Linker_hpp_ALREADY_INCLUDED

#include "cook/process/toolchain/Linker.hpp"

namespace cook { namespace process { namespace toolchain { namespace gcc {

    inline bool set_linker(Linker & linker, const std::string & executable)
    {
        MSS_BEGIN(bool);

        auto & trans = linker.translator_map();
        auto & kvm = linker.key_values_map();

        trans[Part::Cli] = [=](const std::string &, const std::string &){ return executable; };
        trans[Part::Pre] = [](const std::string &k, const std::string &v)
        {
            if (v.empty())
                return k;
            return k+"="+v;
        };
        trans[Part::Output] = [](const std::string &k, const std::string &v){return std::string{"-o "}+k;};
        trans[Part::Input] = [](const std::string &k, const std::string &v){return k;};
        trans[Part::Library] = [](const std::string &k, const std::string &v){return std::string{"-l"}+k;};
        trans[Part::LibraryPath] = [](const std::string &k, const std::string &v){return std::string{"-L"}+(k.empty() ? "./" : k);};

        kvm[Part::Pre].clear();

        auto configure = [&](const std::string &key, const std::string &value, const Configuration & conf)
        {
            MSS_BEGIN(bool);
            if (false) {}
            else if (key == "arch" && value == "x86")
                kvm[Part::Pre].emplace_back("-m32", "");
            else if (key == "arch" && value == "x64")
                kvm[Part::Pre].emplace_back("-m64", "");
            else if (key == "config" && value == "rtc")
            {
                kvm[Part::Pre].emplace_back("-fsanitize", "address");
            }
            else if (key == "config" && value == "profile")
                kvm[Part::Pre].emplace_back("-pg", "");
            else if (key == "c++-standard")
                kvm[Part::Pre].emplace_back("-std", "c++" + value);
            else if (key == "c-standard")
                kvm[Part::Pre].emplace_back("-std", "c" + value);
            else
            {
                //We do not fail on unknown config values
            }
            MSS_END();
        };
        linker.add_config(100, configure);

        MSS_END();
    }

} } } }

#endif
