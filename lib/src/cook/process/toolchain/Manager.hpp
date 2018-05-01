#ifndef HEADER_cook_process_toolchain_Manager_hpp_ALREADY_INCLUDED
#define HEADER_cook_process_toolchain_Manager_hpp_ALREADY_INCLUDED

#include "cook/process/toolchain/Interface.hpp"
#include "cook/Language.hpp"
#include <map>

namespace cook { namespace process { namespace toolchain { 

    class Manager
    {
    public:
        Manager();

        const Interface &compiler(Language) const;
        const Interface &archiver() const {return archiver_();}
        const Interface &linker() const {return linker_();}

        Result configure(const std::string &value);
        Result initialize();

    private:
        using ConfigValue = std::pair<std::string, std::string>;
        using ConfigValues = std::list<ConfigValue>;
        ConfigValues config_values_;
        mutable std::map<Language, Interface::Ptr> compilers_;
        mutable Interface::Ptr archiver_ptr_;
        mutable Interface::Ptr linker_ptr_;

        Interface &archiver_() const;
        Interface &linker_() const;

        static bool configure_(Interface &, const ConfigValue &);
        static bool configure_(Interface &, ConfigValues::const_iterator begin, ConfigValues::const_iterator end);
    };

} } } 

#endif
