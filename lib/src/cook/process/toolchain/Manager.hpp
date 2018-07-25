#ifndef HEADER_cook_process_toolchain_Manager_hpp_ALREADY_INCLUDED
#define HEADER_cook_process_toolchain_Manager_hpp_ALREADY_INCLUDED

#include "cook/Language.hpp"
#include "cook/process/toolchain/Element.hpp"
#include "cook/process/toolchain/Configuration.hpp"
#include "cook/TargetType.hpp"
#include "cook/Result.hpp"
#include "cook/model/Recipe.hpp"
#include <map>

namespace cook { namespace process { namespace toolchain { 

    class Manager
    {
    public:
        using NameFunctor = std::function<std::filesystem::path (const model::Recipe &)>;
        
        Manager();

        bool is_initialized() const;

        Element::Ptr element(Element::Type element_type, Language language, TargetType target_type) const;
        Element::Ptr goc_element(Element::Type type, Language language, TargetType target_type);

        void add_config(const std::string & value);
        void add_config(const std::string & key, const std::string & value);
        void add_configuration_callback(Configuration && cb);
        void add_configuration_callback(const Configuration & cb);

        void each_config(const std::function<void (const std::string &, const std::string &)> &cb) const;
        void each_config(const std::function<void (const std::string &, const std::string &, bool resolved)> &cb) const;
        void each_config(const std::string &wanted_key, const std::function<void (const std::string &)> &cb) const;

        Result initialize();

        bool has_config(const std::string & key, const std::string & value) const;
        bool has_config(const std::string & key) const;

        bool remove_config(const std::string & key, const std::string & value);
        bool remove_config(const std::string & key);

        std::list<std::string> config_values(const std::string & key) const { return board_.config_values(key); }
        std::list<std::pair<std::string, std::string>> all_config_values() const;

        const NameFunctor & primary_target_functor() const;
        void set_primary_target_functor(const NameFunctor & functor);
        std::filesystem::path get_primary_target(const model::Recipe & recipe) const;


    private:
        struct Key
        {
            Key() : 
                element_type(Element::Undefined), 
                language(Language::Undefined), 
                target_type(TargetType::Undefined) 
            {}

            Key(Element::Type etype, Language lang, TargetType ttype) : 
                element_type(etype),
                language(lang),
                target_type(ttype)
            {
            }

            Element::Type element_type;
            Language language;
            TargetType target_type;

            bool operator<(const Key & rhs) const;
        };
        Result resolve_();

        bool configure_(Element & element);
        bool initialized_ = false;

        using ElementDesc = std::pair<Element::Type, Language>;
        std::map<Key, Element::Ptr> elements_;
        ConfigurationBoard board_;
        NameFunctor primary_target_functor_;
    };

} } } 

#endif
