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
        : language_(language)
    {
        {
            auto configure_brand = [&](const std::string &key, const std::string &value, KeyValuesMap &kvm, TranslatorMap &trans){
                MSS_BEGIN(bool);
                MSS_Q(key == "brand");
                const auto &brand = value;
                if (false) {}
                else if (brand == "gcc" || brand == "clang")
                {
                    if (false) {}
                    else if (brand == "gcc")
                        switch (language_)
                        {
                        case Language::C: trans[Part::Cli] = [](const std::string &, const std::string &){return "gcc";}; break;
                        case Language::CXX: trans[Part::Cli] = [](const std::string &, const std::string &){return "g++";}; break;
                        default:
                            break;
                        }
                    else if (brand == "clang")
                        switch (language_)
                        {
                        case Language::C: trans[Part::Cli] = [](const std::string &, const std::string &){return "clang";}; break;
                        case Language::CXX: trans[Part::Cli] = [](const std::string &, const std::string &){return "clang++";}; break;
                        default:
                            break;
                        }

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
                    kvm[Part::Pre].emplace_back("-fPIC", "");
                    configure_.add(configure_gcclike_);
                }
                else if (brand == "msvc")
                {
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

                    if (language_ == Language::C)
                        kvm[Part::Pre].emplace_back("/TC", "");

                    kvm[Part::Pre].emplace_back("/nologo", "");
                    kvm[Part::Pre].emplace_back("/EHsc", "");
                    kvm[Part::Pre].emplace_back("/bigobj", "");
                    kvm[Part::Pre].emplace_back("-c", "");

                    configure_.add(configure_msvc_);
                }
                MSS_END();
            };
            configure_.add(configure_brand);
        }
        {
            auto configure_generic = [&](const std::string &key, const std::string &value, KeyValuesMap &kvm, TranslatorMap &trans)
            {
                MSS_BEGIN(bool);
                if (false) {}
                else if (key == "config" && value == "debug")
                {
                    MSS(configure_("debug_symbols", "true", kvm, trans));
                    MSS(configure_("config", "rtc", kvm, trans));
                }
                else if (key == "config" && value == "release")
                {
                    MSS(configure_("optimization", "max_speed", kvm, trans));
                    kvm[Part::Define].emplace_back("NDEBUG", "");
                }
                else MSS_Q(false);
                MSS_END();
            };
            configure_.add(configure_generic);
        }

        configure_gcclike_ = [&](const std::string &key, const std::string &value, KeyValuesMap &kvm, TranslatorMap &trans)
        {
            MSS_BEGIN(bool);
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
            else if (key == "arch" && value == "armv7")
                kvm[Part::Pre].emplace_back("-arch armv7", "");
            else if (key == "c++-standard" && language_ == Language::CXX)
                kvm[Part::Pre].emplace_back("-std", "c++" + value);
            else if (key == "c-standard" && language_ == Language::C)
                kvm[Part::Pre].emplace_back("-std", "c" + value);
            else
            {

            }
            MSS_END();
        };
        configure_msvc_ = [&](const std::string &key, const std::string &value, KeyValuesMap &kvm, TranslatorMap &trans)
        {
            MSS_BEGIN(bool);
            if (false) {}
            else if (key == "debug_symbols" && value == "true")
                kvm[Part::Pre].emplace_back("/Zi", "");
            else if (key == "optimization" && value == "max_speed")
                kvm[Part::Pre].emplace_back("/O2", "");
            else if (key == "arch" && value == "x86") {}
            else if (key == "arch" && value == "x64") {}
            else if (key == "c++-standard" && language_ == Language::CXX && value == "17")
                kvm[Part::Pre].emplace_back("/std", "c++latest");
            else
            {

            }
            MSS_END();
        };
    }

    bool create(command::Compile::Ptr &ptr) const override
    {
        MSS_BEGIN(bool);
        ptr.reset(new command::Compile(kvm_, trans_));
        MSS_END();
    }

private:
    const Language language_;
    ConfigureFunction configure_gcclike_;
    ConfigureFunction configure_msvc_;
};

} } } 

#endif
