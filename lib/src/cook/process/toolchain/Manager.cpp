#include "cook/process/toolchain/Manager.hpp"
#include "cook/process/toolchain/Compiler.hpp"
#include "cook/process/toolchain/Archiver.hpp"
#include "cook/process/toolchain/Linker.hpp"
#include "cook/process/toolchain/gcc.hpp"
#include "cook/process/toolchain/gcc/Archiver.hpp"
#include "cook/process/toolchain/gcc/Compiler.hpp"
#include "cook/process/toolchain/gcc/Linker.hpp"
#include "cook/process/toolchain/msvc/Archiver.hpp"
#include "cook/process/toolchain/msvc/Compiler.hpp"
#include "cook/process/toolchain/msvc/Linker.hpp"
#include "cook/OS.hpp"
#include "cook/log/Scope.hpp"
#include "gubg/Strange.hpp"

namespace cook { namespace process { namespace toolchain { 

    Manager::Manager()
    : linker_(std::make_shared<Linker>()),
    archiver_(std::make_shared<Archiver>())
    {
        //Create compilers for common languages
        compiler(Language::C);
        compiler(Language::CXX);
        compiler(Language::ASM);
    }

    bool Manager::is_initialized() const
    {
        return initialized_;
    }

    bool temp_set_brand(Manager & manager, std::string brand)
    {
        MSS_BEGIN(bool);
        MSS(!manager.is_initialized());

        if (brand.empty())
        {
            switch (get_os())
            {
                case OS::Linux:     brand = "gcc";      break;
                case OS::Windows:   brand = "msvc";     break;
                case OS::MacOS:     brand = "clang";    break;
                default:            brand = "gcc";      break;
            }
        }

        if (false) {}
        else if (brand == "msvc" || brand == "cl")
        {
            for(Language lang : {Language::C, Language::CXX, Language::ASM})
            {
                auto * comp = manager.compiler(lang);
                MSS(!!comp);
                msvc::set_compiler(*comp);
            }

            {
                auto & link = manager.linker();
                msvc::set_linker(link);
            }

            {
                auto & ar = manager.archiver();
                msvc::set_archiver(ar);
            }
        }
        else
        {
            for(Language lang : {Language::C, Language::CXX, Language::ASM})
            {
                auto * comp = manager.compiler(lang);
                MSS(!!comp);
                gcc::set_compiler(*comp, standard_executable(lang, brand));
            }

            {
                auto & link = manager.linker();
                gcc::set_linker(link, standard_executable(Language::CXX, brand));
            }

            {
                auto & ar = manager.archiver();
                gcc::set_archiver(ar);
            }
        }

        MSS_END();
    }

    const Archiver & Manager::archiver() const 
    { 
        return *archiver_; 
    }
    const Linker & Manager::linker() const 
    { 
        return *linker_; 
    }

    Archiver & Manager::archiver() 
    { 
        return *archiver_; 
    }
    Linker & Manager::linker() 
    { 
        return *linker_; 
    }

    Compiler * Manager::compiler(Language language)
    {
        auto it = compilers_.find(language);
        if (it == compilers_.end())
        {
            if (initialized_)
                return nullptr;

            it = compilers_.insert(std::make_pair(language, std::make_shared<Compiler>(language))).first;
        }

        return &*it->second;
    }

    const Compiler * Manager::compiler(Language language) const
    {
        auto it = compilers_.find(language);
        return (it == compilers_.end() ? nullptr : &*it->second);
    }

    Result Manager::configure(const std::string &value)
    {
        MSS_BEGIN(Result);

        gubg::Strange str(value);
        if (false) {}
        else if (str.pop_if("x86") || str.pop_if("x64") || str.pop_if("armv7"))
            MSS(configure("arch", value));
        else if (str.pop_if("x32"))
            MSS(configure("x86"));
        else if (str.pop_if("debug") || str.pop_if("release") || str.pop_if("rtc") || str.pop_if("profile"))
            MSS(configure("config", value));
        else
        {
            if (str.contains('='))
            {
                // key values
                std::string key;
                MSS(str.pop_until(key, '='));

                if (false) {}
                else if (key == "c++")
                    MSS(configure("c++-standard", str.str()));
                else if (key == "c")
                    MSS(configure("c-standard", str.str()));
                else
                    MSS(configure(key, str.str()));
            }
            else
            {
                // only keys
                MSS(configure(value, "true"));
            }
        }
        L(C(config_values_.size()));

        MSS_END();
    }

    Result Manager::configure(const std::string & key, const std::string & value)
    {
        MSS_BEGIN(Result);
        config_values_.emplace_back(key, value);
        MSS_END();
    }

    Result Manager::initialize()
    {
        MSS_BEGIN(Result);
        MSS(initialized_ == false);
        initialized_ = true;

        for (auto &p: compilers_)
            MSS(configure_(*p.second));

        MSS(configure_(*archiver_));
        MSS(configure_(*linker_));

        MSS_END();
    }

    bool Manager::configure_(Interface & itf) const
    {
        for(const auto & p : config_values_)
            itf.configure(p.first, p.second);

        return true;
    }

    bool Manager::has_config(const std::string & key) const
    {
        for(const auto & p : config_values_)
            if (p.first == key)
                return true;

        return false;
    }
    std::string  Manager::config_value(const std::string & key) const
    {
        for(const auto & p : config_values_)
            if (p.first == key)
                return p.second;

        return std::string();
    }


} } } 
