#ifndef HEADER_cook_process_toolchain_Manager_hpp_ALREADY_INCLUDED
#define HEADER_cook_process_toolchain_Manager_hpp_ALREADY_INCLUDED

#include "cook/Language.hpp"
#include "cook/process/toolchain/Element.hpp"
#include "cook/process/toolchain/Configuration.hpp"
#include "cook/Result.hpp"
#include <map>

namespace cook { namespace process { namespace toolchain { 

    class Manager
    {
    public:
        Manager();

        bool is_initialized() const;

        Element::Ptr element(Element::Type type, Language language) const;
        Element::Ptr goc_element(Element::Type type, Language language);

        void add_config(const std::string & value);
        void add_config(const std::string & key, const std::string & value);
        void add_configuration_callback(Configuration && cb);
        void add_configuration_callback(const Configuration & cb);

        Result initialize();

        bool has_config(const std::string & key, const std::string & value) const;
        std::list<std::string> config_values(const std::string & key) const { return board_.config_values(key); }
        std::list<std::pair<std::string, std::string>> all_config_values() const;

    private:
        bool configure_(Element & element);
        bool initialized_ = false;

        using ElementDesc = std::pair<Element::Type, Language>;
        std::map<std::pair<Element::Type, Language>, Element::Ptr> elements_;
        ConfigurationBoard board_;
    };

} } } 

#endif
