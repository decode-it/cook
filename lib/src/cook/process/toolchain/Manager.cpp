#include "cook/process/toolchain/Manager.hpp"
#include "cook/process/toolchain/Compiler.hpp"
#include "cook/process/toolchain/Archiver.hpp"
#include "cook/process/toolchain/Linker.hpp"
#include "cook/OS.hpp"
#include "cook/log/Scope.hpp"

namespace cook { namespace process { namespace toolchain { 

    Manager::Manager()
    {
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

        if (false) {}
        else if (value == "gcc" || value == "clang" || value == "msvc")
        {
            //The brand can only be set once
            MSS(std::find_if(RANGE(config_values_), [](const auto &cv){return cv.first == "brand";}) == config_values_.end());
            //Make sure the brand comes first
            config_values_.emplace_front("brand", value);
        }
        else if (value == "x86" || value == "x64")
            config_values_.emplace_back("arch", value);
        else if (value == "x32")
            MSS(configure("x86"));
        else if (value == "debug" || value == "release" || value == "rtc" || value == "profile")
            config_values_.emplace_back("config", value);
        else
            config_values_.emplace_back(value, "true");

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
        {
            MSS(configure_(*p.second, RANGE(config_values_)));
        }
        MSS(configure_(archiver_(), RANGE(config_values_)));
        MSS(configure_(linker_(), RANGE(config_values_)));

        MSS_END();
    }

    Interface &Manager::archiver_() const
    {
        if (!archiver_ptr_)
        {
            archiver_ptr_.reset(new Archiver);
            configure_(*archiver_ptr_, RANGE(config_values_));
        }
        return *archiver_ptr_;
    }

    Interface &Manager::linker_() const
    {
        if (!linker_ptr_)
        {
            linker_ptr_.reset(new Linker);
            configure_(*linker_ptr_, RANGE(config_values_));
        }
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

} } } 
