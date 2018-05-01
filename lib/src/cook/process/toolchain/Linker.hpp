#ifndef HEADER_cook_process_toolchain_Linker_hpp_ALREADY_INCLUDED
#define HEADER_cook_process_toolchain_Linker_hpp_ALREADY_INCLUDED

#include "cook/process/toolchain/Interface.hpp"

namespace cook { namespace process { namespace toolchain { 

    class Linker: public Interface
    {
    public:
        Linker()
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
                            trans[Part::Cli] = [](const std::string &, const std::string &){return "g++";};
                        else if (brand == "clang")
                            trans[Part::Cli] = [](const std::string &, const std::string &){return "clang++";};

                        trans[Part::Pre] = [](const std::string &k, const std::string &v)
                        {
                            if (v.empty())
                                return k;
                            return k+"="+v;
                        };
                        trans[Part::Output] = [](const std::string &k, const std::string &v){return std::string{"-o "}+k;};
                        trans[Part::Input] = [](const std::string &k, const std::string &v){return k;};
                        trans[Part::Library] = [](const std::string &k, const std::string &v){return std::string{"-l"}+k;};
                        trans[Part::LibraryPath] = [](const std::string &k, const std::string &v){return std::string{"-L"}+k;};

                        kvm[Part::Pre].clear();
                        kvm[Part::Pre].emplace_back("-std", "c++17");

                        configure_.add(configure_gcclike_);
                    }
                    else if (brand == "msvc")
                    {
                        trans[Part::Cli] = [](const std::string &, const std::string &){return "link";};
                        trans[Part::Output] = [](const std::string &k, const std::string &v){return std::string{"/OUT:"}+k;};
                        trans[Part::Input] = [](const std::string &k, const std::string &v){return k;};
                        trans[Part::Library] = [](const std::string &k, const std::string &v){return k;};
                        trans[Part::LibraryPath] = [](const std::string &k, const std::string &v){return std::string{"/LIBPATH:"}+k;};

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
                    else MSS_Q(false);
                    MSS_END();
                };
                configure_.add(configure_generic);
            }

            configure_gcclike_ = [&](const std::string &key, const std::string &value, KeyValuesMap &kvm, TranslatorMap &trans)
            {
                MSS_BEGIN(bool);
                if (false) {}
                else if (key == "arch" && value == "x86")
                    kvm[Part::Pre].emplace_back("-m32", "");
                else if (key == "arch" && value == "x64")
                    kvm[Part::Pre].emplace_back("-m64", "");
                else if (key == "config" && value == "rtc")
                    kvm[Part::Pre].emplace_back("-fsanitize", "address");
                else if (key == "config" && value == "profile")
                    kvm[Part::Pre].emplace_back("-pg", "");
                else
                {
                    //We do not fail on unknown config values
                }
                MSS_END();
            };
            configure_msvc_ = [&](const std::string &key, const std::string &value, KeyValuesMap &kvm, TranslatorMap &trans)
            {
                MSS_BEGIN(bool);
                MSS_END();
            };
        }

        bool create(command::Link::Ptr &ptr) const override
        {
            MSS_BEGIN(bool);
            ptr.reset(new command::Link(kvm_, trans_));
            MSS_END();
        }

    private:
        ConfigureFunction configure_gcclike_;
        ConfigureFunction configure_msvc_;
    };

} } } 

#endif
