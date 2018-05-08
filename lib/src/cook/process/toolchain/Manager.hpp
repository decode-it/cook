#ifndef HEADER_cook_process_toolchain_Manager_hpp_ALREADY_INCLUDED
#define HEADER_cook_process_toolchain_Manager_hpp_ALREADY_INCLUDED

#include "cook/Language.hpp"
#include "cook/Result.hpp"
#include <map>

namespace cook { namespace process { namespace toolchain { 

    class Compiler;
    class Archiver;
    class Linker;
    class Interface;

    class Manager
    {
    public:
        Manager();

        const Compiler * compiler(Language) const;
        const Archiver & archiver() const;
        const Linker & linker() const;
        Compiler * compiler(Language language);
        Archiver & archiver();
        Linker & linker();

        bool temp_set_brand(std::string brand);

        Result configure(const std::string &value);
        Result configure(const std::string & key, const std::string & value);
        Result initialize();

        bool has_config(const std::string & key) const;
        std::string config_value(const std::string & key) const;

    private:
        bool configure_(Interface & itf) const;
        using ConfigValue = std::pair<std::string, std::string>;
        using ConfigValues = std::list<ConfigValue>;

        ConfigValues config_values_;
        mutable std::map<Language, std::shared_ptr<Compiler>> compilers_;
        std::shared_ptr<Linker> linker_;
        std::shared_ptr<Archiver> archiver_;
 
        std::string name_;
        bool initialized_ = false;
    };

    void temp_initialize(Manager & manager, const std::string & brand);

} } } 

#endif
