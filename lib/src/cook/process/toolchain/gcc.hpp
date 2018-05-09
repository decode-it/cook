#ifndef HEADER_cook_process_toolchain_gcc_hpp_ALREADY_INCLUDED
#define HEADER_cook_process_toolchain_gcc_hpp_ALREADY_INCLUDED

#include "cook/Language.hpp"
#include "cook/process/toolchain/Element.hpp"
#include "cook/process/toolchain/Archiver.hpp"
#include "cook/process/toolchain/Compiler.hpp"
#include "cook/process/toolchain/Linker.hpp"
#include <string>

namespace cook { namespace process { namespace toolchain { namespace gcc {

    inline std::string standard_executable(Language language, const std::string & subbrand)
    {
        if (false) {}
        else if (subbrand == "gcc")
        {
            switch(language)
            {
                case Language::C: 
                    return "gcc";
                case Language::CXX: 
                    return "g++";
                default:
                    return "";
            }
        }
        else if (subbrand == "clang")
        {
            switch(language)
            {
                case Language::C: 
                    return "clang";
                case Language::CXX: 
                    return "clang++";
                default:
                    return "";
            }
        }
        else
        {
            switch(language)
            {
                case Language::C:
                    return "CC";
                case Language::CXX:
                    return "CXX";
                default:
                    return "";
            }
        }
    }

    inline bool configure_archiver(Manager & manager)
    {
        MSS_BEGIN(bool);

        MSS(toolchain::configure_archiver(manager));


        auto archiver = manager.goc_element(Element::Archive, Language::Binary);
        auto & trans = archiver->translator_map();
        auto & kvm = archiver->key_values_map();
        kvm[Part::Cli].emplace_back("ar", "");
        trans[Part::Cli] = [](const std::string & k, const std::string &){return k; };
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

    inline bool configure_compiler_cb(Element::Ptr element, const std::string &key, const std::string &value, ConfigurationBoard & board)
    {
        if (element->type() != Element::Compile)
            return false;

        auto & kvm = element->key_values_map();

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
        {
            kvm[Part::Cli].clear();
            kvm[Part::Cli].emplace_back("/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/clang", "");
            if (element->language() == Language::CXX)
            {
                kvm[Part::Pre].emplace_back("-x c++", "");
                kvm[Part::Pre].emplace_back("-stdlib=libc++ -Wno-c++11-extensions", "");
            }
            kvm[Part::Pre].emplace_back("-arch armv7", "");
            kvm[Part::Pre].emplace_back("-fmessage-length=0 -fdiagnostics-show-note-include-stack -fmacro-backtrace-limit=0 -Wno-trigraphs -fpascal-strings -Wno-missing-field-initializers -Wno-missing-prototypes -Wno-return-type -Wno-non-virtual-dtor -Wno-overloaded-virtual -Wno-exit-time-destructors -Wno-missing-braces -Wparentheses -Wswitch -Wno-unused-function -Wno-unused-label -Wno-unused-parameter -Wno-unused-variable -Wunused-value -Wno-empty-body -Wno-uninitialized -Wno-unknown-pragmas -Wno-shadow -Wno-four-char-constants -Wno-conversion -Wno-constant-conversion -Wno-int-conversion -Wno-bool-conversion -Wno-enum-conversion -Wno-float-conversion -Wno-non-literal-null-conversion -Wno-objc-literal-conversion -Wno-shorten-64-to-32 -Wnewline-eof -isysroot /Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS11.3.sdk -fstrict-aliasing -Wdeprecated-declarations -Winvalid-offsetof -miphoneos-version-min=9.3 -g -Wno-sign-conversion -Wno-infinite-recursion -Wno-move -Wno-comma -Wno-block-capture-autoreleasing -Wno-strict-prototypes -Wno-range-loop-analysis -fembed-bitcode-marker -Wall -Wextra", "");
            kvm[Part::Define].emplace_back("MAC", "1");
        }
        else if (key == "c++-standard" && element->language() == Language::CXX)
            kvm[Part::Pre].emplace_back("-std", "c++" + value);
        else if (key == "c-standard" && element->language() == Language::C)
            kvm[Part::Pre].emplace_back("-std", "c" + value);
        else
            return false;

        return true;
    }

    inline bool configure_compiler(Manager & manager, Language language, const std::string & subbrand)
    {
        MSS_BEGIN(bool);

        MSS(toolchain::configure_compiler(manager, language));

        {        
            std::string executable = standard_executable(language, subbrand);
            auto compiler = manager.goc_element(Element::Compile, language);
            auto & trans = compiler->translator_map();
            auto & kvm = compiler->key_values_map();

        }

        {
            Configuration cb(100, "gcc compiler configuration");
            cb.callback = configure_compiler_cb;
            manager.add_configuration_callback(std::move(cb));
        }

        MSS_END();
    }

    inline bool configure_linker_cb(Element::Ptr element, const std::string &key, const std::string &value, ConfigurationBoard & board)
    {
        if (element->type() != Element::Link)
            return false;

        auto & kvm = element->key_values_map();

        if (false) {}
        else if (key == "arch" && value == "x86")
            kvm[Part::Pre].emplace_back("-m32", "");
        else if (key == "arch" && value == "x64")
            kvm[Part::Pre].emplace_back("-m64", "");
        else if (key == "arch" && value == "armv7")
            kvm[Part::Pre].emplace_back("-arch armv7", "");
        else if (key == "config" && value == "rtc")
            kvm[Part::Pre].emplace_back("-fsanitize", "address");
        else if (key == "config" && value == "profile")
            kvm[Part::Pre].emplace_back("-pg", "");
        else if (key == "c++-standard")
            kvm[Part::Pre].emplace_back("-std", "c++" + value);
        else if (key == "c-standard")
            kvm[Part::Pre].emplace_back("-std", "c" + value);
        else
            return false;

        return true;
    }
    

    inline bool configure_linker(Manager & manager, const std::string & subbrand)
    {
        MSS_BEGIN(bool);
        MSS(toolchain::configure_link(manager));

        {
            std::string executable = standard_executable(Language::CXX, subbrand);
            auto link = manager.goc_element(Element::Link, Language::Binary);
            auto & trans = link->translator_map();
            auto & kvm = link->key_values_map();

            kvm[Part::Cli].emplace_back(executable, "");
            trans[Part::Cli] = [=](const std::string & k, const std::string &){ return k; };
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
        }

        {
            Configuration cb(100,  "gcc link configuration");
            cb.callback = configure_linker_cb;
            manager.add_configuration_callback(std::move(cb));
        }

        MSS_END();
    }
    
    inline bool configure(Manager & manager, const std::string & subbrand)
    {
        MSS_BEGIN(bool);

        MSS(gcc::configure_archiver(manager));
        MSS(gcc::configure_linker(manager, subbrand));

        for(Language lang : {Language::C, Language::CXX, Language::ASM})
            MSS(gcc::configure_compiler(manager, lang, subbrand));

        MSS_END();
    }

} } } }

#endif
