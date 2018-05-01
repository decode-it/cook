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
        MSS_BEGIN(Result, "");

        auto ss = log::scope("Toolchain configure", [&](auto &node){node.attr("value", value);});

        if (value == "gcc" || value == "clang" || value == "msvc")
        {
            MSS(config_values_.empty());
            config_values_.emplace_back("brand", value);
        }
        else
        {
            if (config_values_.empty())
            {
                std::string default_brand;
                switch (get_os())
                {
                    case OS::Linux: default_brand = "gcc"; break;
                    case OS::Windows: default_brand = "msvc"; break;
                    case OS::MacOS: default_brand = "clang"; break;
                    default: default_brand = "gcc"; break;
                }
                MSS(configure(default_brand));
            }

            if (false) {}
            else if (value == "x86" || value == "x64")
                config_values_.emplace_back("arch", value);
            else if (value == "debug" || value == "release")
                config_values_.emplace_back("config", value);
            else
                config_values_.emplace_back(value, "true");
        }

        const auto &cv = config_values_.back();

        for (auto &p: compilers_)
        {
            MSS(configure_(*p.second, cv));
        }
        MSS(configure_(archiver_(), cv));
        MSS(configure_(linker_(), cv));

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
