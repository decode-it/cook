#include "cook/process/toolchain/Manager.hpp"
#include "cook/process/toolchain/Compiler.hpp"
#include "cook/process/toolchain/Archiver.hpp"
#include "cook/process/toolchain/Linker.hpp"
#include "cook/OS.hpp"
#include "cook/log/Scope.hpp"
#include "gubg/Strange.hpp"

namespace cook { namespace process { namespace toolchain { 

Manager::Manager()
    : archiver_ptr_(std::make_shared<Archiver>()),
      linker_ptr_(std::make_shared<Linker>())
{
    //Create compilers for common languages
    compiler(Language::C);
    compiler(Language::CXX);
    compiler(Language::ASM);
}

const Interface &Manager::compiler(Language language) const
{
    auto it = compilers_.find(language);
    if (it == compilers_.end())
    {
        compilers_[language].reset(new Compiler(language));
        configure_(*compilers_[language], RANGE(config_values_));
        it = compilers_.find(language);
    }
    return *it->second;
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
        //The brand can only be set once
        MSS(std::find_if(RANGE(config_values_), [](const auto &cv){return cv.first == "brand";}) == config_values_.end());
        //Make sure the brand comes first
        config_values_.emplace_front("brand", value);
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

    if (std::find_if(RANGE(config_values_), [](const auto &cv){return cv.first == "brand";}) == config_values_.end())
    {
        std::string default_brand;
        switch (get_os())
        {
        case OS::Linux: default_brand = "gcc"; break;
        case OS::Windows: default_brand = "msvc"; break;
        case OS::MacOS: default_brand = "clang"; break;
        default: default_brand = "gcc"; break;
        }
        config_values_.emplace_front("brand", default_brand);
    }

    for (auto &p: compilers_)
        MSS(configure_(*p.second, RANGE(config_values_)));

    MSS(configure_(archiver_(), RANGE(config_values_)));
    MSS(configure_(linker_(), RANGE(config_values_)));

    MSS_END();
}

Interface &Manager::archiver_() const
{
    if (!archiver_ptr_)
        archiver_ptr_.reset(new Archiver);

    return *archiver_ptr_;
}

Interface &Manager::linker_() const
{
    if (!linker_ptr_)
        linker_ptr_.reset(new Linker);

    return *linker_ptr_;
}

bool Manager::configure_(Interface &itf, const ConfigValue &cv)
{
    return itf.configure(cv.first, cv.second);
}
bool Manager::configure_(Interface &itf, ConfigValues::const_iterator begin, ConfigValues::const_iterator end)
{
    MSS_BEGIN(bool);
    for (; begin != end; ++begin)
    {
        MSS(configure_(itf, *begin));
    }
    MSS_END();
}

bool  Manager::has_config(const std::string & key) const
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
