#ifndef HEADER_cook_process_toolchain_gcc_Compiler_hpp_ALREADY_INCLUDED
#define HEADER_cook_process_toolchain_gcc_Compiler_hpp_ALREADY_INCLUDED

#include "cook/process/toolchain/gcc.hpp"
#include "cook/process/toolchain/Compiler.hpp"
#include "cook/Language.hpp"
#include "gubg/mss.hpp"
#include <optional>

namespace cook { namespace process { namespace toolchain { namespace gcc {

    inline bool set_compiler(Compiler & compiler, const std::string & executable)
    {
        MSS_BEGIN(bool);

        auto & trans = compiler.translator_map();
        auto & kvm = compiler.key_values_map();

        trans[Part::Cli] = [=](const std::string &, const std::string &){return executable; }; 
        trans[Part::Pre] = [](const std::string &k, const std::string &v)
        {
            if (v.empty())
                return k;
            return k+"="+v;
        };
        trans[Part::Output] = [](const std::string &k, const std::string &v){return std::string{"-o "}+k;};
        trans[Part::Input] = [](const std::string &k, const std::string &v){return k;};
        trans[Part::DepFile] = [](const std::string &k, const std::string &v){return std::string{"-MMD -MF "}+k;};
        trans[Part::Define] = [](const std::string &k, const std::string &v)
        {
            if (v.empty())
                return std::string{"-D"}+k;
            return std::string{"-D"}+k+"="+v;
        };
        trans[Part::IncludePath] = [](const std::string &k, const std::string &v){return std::string{"-I "}+(k.empty() ? "./" : k);};
        trans[Part::ForceInclude] = [](const std::string &k, const std::string &v){return std::string{"-include "}+k;};

        kvm[Part::Pre].emplace_back("-c", "");

        const Language & language = compiler.language;
        
        auto configure = [&](const std::string &key, const std::string &value, const Configuration & conf)
        {
            if (false) {}
            else if (key == "debug_symbols" && value == "true")
                kvm[Part::Pre].emplace_back("-g", "");
            else if (key == "optimization" && value == "max_speed")
                kvm[Part::Pre].emplace_back("-O3", "");
            else if (key == "config" && value == "rtc")
            {
                kvm[Part::Pre].emplace_back("-fsanitize", "address");
                kvm[Part::Pre].emplace_back("-fno-omit-frame-pointer", "");
            }
            else if (key == "config" && value == "profile")
                kvm[Part::Pre].emplace_back("-g", "");
            else if (key == "arch" && value == "x86")
                kvm[Part::Pre].emplace_back("-m32", "");
            else if (key == "arch" && value == "x64")
                kvm[Part::Pre].emplace_back("-m64", "");
            else if (key == "c++-standard" && language == Language::CXX)
                kvm[Part::Pre].emplace_back("-std", "c++" + value);
            else if (key == "c-standard" && language == Language::C)
                kvm[Part::Pre].emplace_back("-std", "c" + value);
            else
            {
                return false;
            }
            return true;
        };

        compiler.add_config(100, configure);
        MSS_END();
    }

} } } }

#endif
