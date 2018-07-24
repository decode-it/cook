#ifndef HEADER_cook_chai_Toolchain_hpp_ALREADY_INCLUDED
#define HEADER_cook_chai_Toolchain_hpp_ALREADY_INCLUDED

#include "cook/process/toolchain/Manager.hpp"
#include "cook/chai/ToolchainElement.hpp"
#include "cook/chai/Flags.hpp"
#include "cook/chai/Freezable.hpp"

namespace cook { namespace chai {

    class Recipe;
    class Context;

    class Toolchain : public Freezable
    {
        using Manager = process::toolchain::Manager;

    public:
        using ConfigurationBoard = process::toolchain::ConfigurationBoard;
        using ConfigurationCallback = std::function<bool (ToolchainElement, const std::string &, const std::string &, ConfigurationBoard &)>;
        using PrimaryNameFunctor = std::function<std::string (Recipe)>;

        Toolchain(Manager * manager, const Context * context);

        bool has_element(ElementType type, const Flags & language, TargetType target_type);
        ToolchainElement element(ElementType type, const Flags & language, TargetType target_type);      
        
        void add_config_1(const std::string & key);
        void add_config_2(const std::string & key, const std::string & value);
        void configure(unsigned int priority, const std::string & uuid, const ConfigurationCallback & cb);

        using IterationCallback1 = std::function<void (const std::string &key, const std::string &value)>;
        void each_config_1(const IterationCallback1 &);

        using IterationCallback2 = std::function<void (const std::string &value)>;
        void each_config_2(const std::string &key, const IterationCallback2 &);

        bool has_config_1(const std::string & key) const;
        bool has_config_2(const std::string & key, const std::string & value) const;

        void set_primary_name_functor(const PrimaryNameFunctor & functor);

    private:
        Result check_language_(const Flags & language);
        Manager * manager_; 
        const Context * context_;
    };

} }

#endif

