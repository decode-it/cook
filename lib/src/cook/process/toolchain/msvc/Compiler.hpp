#ifndef HEADER_cook_process_toolchain_msvc_Compiler_hpp_ALREADY_INCLUDED
#define HEADER_cook_process_toolchain_msvc_Compiler_hpp_ALREADY_INCLUDED

#include "cook/process/toolchain/Compiler.hpp"
#include "cook/Language.hpp"
#include "gubg/mss.hpp"
#include <optional>

namespace cook { namespace process { namespace toolchain { namespace msvc {

    inline bool set_compiler(Compiler & compiler)
    {
        MSS_BEGIN(bool);

        auto & trans = compiler.translator_map();
        auto & kvm = compiler.key_values_map();
        trans[Part::Cli] = [](const std::string &, const std::string &){return "cl";};
        trans[Part::Pre] = [](const std::string &k, const std::string &v)
        {
            if (v.empty())
                return k;
            return k+":"+v;
        };
        trans[Part::Output] = [](const std::string &k, const std::string &v){return std::string{"/Fo:"}+k;};
        trans[Part::Input] = [](const std::string &k, const std::string &v){return k;};
        trans[Part::Deps] = [](const std::string &k, const std::string &v){return "/showIncludes";};
        kvm[Part::Deps].emplace_back("msvc", "");
        trans[Part::Define] = [](const std::string &k, const std::string &v)
        {
            if (v.empty())
                return std::string{"/D"}+k;
            return std::string{"/D"}+k+"="+v;
        };
        trans[Part::IncludePath] = [](const std::string &k, const std::string &v){return std::string{"/I"}+(k.empty() ? "./" : k);};
        trans[Part::ForceInclude] = [](const std::string &k, const std::string &v){return std::string{"/FI"}+k;};

        const Language & language = compiler.language;

        if (language == Language::C)
            kvm[Part::Pre].emplace_back("/TC", "");

        kvm[Part::Pre].emplace_back("/nologo", "");
        kvm[Part::Pre].emplace_back("/EHsc", "");
        kvm[Part::Pre].emplace_back("/bigobj", "");
        kvm[Part::Pre].emplace_back("-c", "");

        auto configure = [&](const std::string &key, const std::string &value, const Configuration & config)
        {
            MSS_BEGIN(bool);
            if (false) {}
            else if (key == "debug_symbols" && value == "true")
                kvm[Part::Pre].emplace_back("/Zi", "");
            else if (key == "optimization" && value == "max_speed")
                kvm[Part::Pre].emplace_back("/O2", "");
            else if (key == "arch" && value == "x86") {}
            else if (key == "arch" && value == "x64") {}
            else if (key == "c++-standard" && language == Language::CXX && value == "17")
                kvm[Part::Pre].emplace_back("/std", "c++latest");
            else
            {

            }
            MSS_END();
        };
        compiler.add_config(100, configure);

        MSS_END();
    }

} } } }

#endif
