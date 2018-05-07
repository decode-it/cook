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
{
    //Create compilers for common languages
    compiler(Language::C);
    compiler(Language::CXX);
    compiler(Language::ASM);
}

const Compiler & Manager::compiler(Language language) const
{
    auto it = compilers_.find(language);
    if (it == compilers_.end())
    {
        it = compilers_.insert(std::make_pair(language, Compiler(language))).first;

        if (initialized_)
        {
            if (false) {}
            else if (name_ == "msvc")
                msvc::set_compiler(it->second);
            else
                gcc::set_compiler(it->second, standard_executable(language, name_));

            configure_(it->second);
        }
    }
    return it->second;
}

Result Manager::configure(const std::string &value)
{
    MSS_BEGIN(Result);

    gubg::Strange str(value);
    if (false) {}
    else if (value == "cl")
        MSS(configure("msvc"));
    else if (str.pop_if("gcc") || str.pop_if("clang") || str.pop_if("msvc"))
    {
        MSS(name_.empty());
        name_ = value;
    }
    else if (str.pop_if("x86") || str.pop_if("x64"))
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

    if (name_.empty())
    {
        switch (get_os())
        {
        case OS::Linux: name_ = "gcc"; break;
        case OS::Windows: name_ = "msvc"; break;
        case OS::MacOS: name_ = "clang"; break;
        default: name_ = "gcc"; break;
        }
    }

    if (false) {}
    else if (name_ == "msvc")
    {
        for(auto & p: compilers_)
            msvc::set_compiler(p.second);

        msvc::set_linker(linker_);
        msvc::set_archiver(archiver_);
    }
    else
    {
        for(auto & p: compilers_)
            gcc::set_compiler(p.second, standard_executable(p.first, name_));

        gcc::set_linker(linker_, standard_executable(Language::CXX, name_));
        gcc::set_archiver(archiver_);
    }

    for (auto &p: compilers_)
        MSS(configure_(p.second));

    MSS(configure_(archiver_));
    MSS(configure_(linker_));

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
