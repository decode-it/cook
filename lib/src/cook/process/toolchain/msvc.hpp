#ifndef HEADER_cook_process_toolchain_msvc_hpp_ALREADY_INCLUDED
#define HEADER_cook_process_toolchain_msvc_hpp_ALREADY_INCLUDED

#include "cook/Language.hpp"
#include "cook/process/toolchain/Element.hpp"
#include "cook/process/toolchain/Archiver.hpp"
#include "cook/process/toolchain/Compiler.hpp"
#include "cook/process/toolchain/Linker.hpp"
#include <string>

namespace cook { namespace process { namespace toolchain { namespace msvc {

    inline bool configure_archiver(Manager & manager)
    {
        MSS_BEGIN(bool);
        MSS(toolchain::configure_archiver(manager));
        
        auto archiver = manager.goc_element(Element::Archive, Language::Binary);
        auto & kvm = archiver->key_values_map();
        auto & trans = archiver->translator_map();
        kvm[Part::Cli].emplace_back("lib", "");
        trans[Part::Cli] = [](const std::string & k, const std::string &){return k; };
        trans[Part::Pre] = [](const std::string &k, const std::string &v) {return k;};
        trans[Part::Output] = [](const std::string &k, const std::string &v){return std::string{"/OUT:"}+k;};
        trans[Part::Input] = [](const std::string &k, const std::string &v){return k;};

        MSS_END();
    }

    inline bool configure_compiler_cb(Element::Ptr element, const std::string &key, const std::string &value, ConfigurationBoard & board)
    {
        if (element->type() != Element::Compile)
            return false;

        auto & kvm = element->key_values_map();

        if (false) {}
        else if (key == "debug_symbols" && value == "true")
            kvm[Part::Pre].emplace_back("/Zi", "");
        else if (key == "optimization" && value == "max_speed")
            kvm[Part::Pre].emplace_back("/O2", "");
        else if (key == "arch" && value == "x86") {}
        else if (key == "arch" && value == "x64") {}
        else if (key == "c++-standard" && element->language() == Language::CXX && value == "17")
            kvm[Part::Pre].emplace_back("/std", "c++latest");
        else
            return false;

        return true;
    }

    inline bool configure_compiler(Manager & manager, Language language)
    {
        MSS_BEGIN(bool);

        MSS(toolchain::configure_compiler(manager, language));

        {        
            auto compiler = manager.goc_element(Element::Compile, language);
            auto & trans = compiler->translator_map();
            auto & kvm = compiler->key_values_map();
            kvm[Part::Cli].emplace_back("cl", "");
            trans[Part::Cli] = [](const std::string & k, const std::string &){return k;};
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

            Language language = compiler->language();

            if (language == Language::C)
                kvm[Part::Pre].emplace_back("/TC", "");

            kvm[Part::Pre].emplace_back("/nologo", "");
            kvm[Part::Pre].emplace_back("/EHsc", "");
            kvm[Part::Pre].emplace_back("/bigobj", "");
            kvm[Part::Pre].emplace_back("-c", "");
        }

        {
            Configuration cb(100, "msvc compiler configuration");
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
    

    inline bool configure_linker(Manager & manager)
    {
        MSS_BEGIN(bool);
        MSS(toolchain::configure_link(manager));

        {
            auto link = manager.goc_element(Element::Link, Language::Binary);
            auto & trans = link->translator_map();
            auto & kvm = link->key_values_map();

            kvm[Part::Cli].emplace_back("link", "");
            trans[Part::Cli] = [](const std::string & k, const std::string &){return k;};
            trans[Part::Output] = [](const std::string &k, const std::string &v){return std::string{"/OUT:"}+k;};
            trans[Part::Input] = [](const std::string &k, const std::string &v){return k;};
            trans[Part::Library] = [](const std::string &k, const std::string &v){return k+".lib";};
            trans[Part::LibraryPath] = [](const std::string &k, const std::string &v){return std::string{"/LIBPATH:"}+(k.empty() ? "./" : k);};

            kvm[Part::Pre].clear();
        }

        MSS_END();
    }
    
    inline bool configure(Manager & manager)
    {
        MSS_BEGIN(bool);

        MSS(msvc::configure_archiver(manager));
        MSS(msvc::configure_linker(manager));

        for(Language lang : {Language::C, Language::CXX, Language::ASM})
            MSS(msvc::configure_compiler(manager, lang));

        MSS_END();
    }

} } } }

#endif
