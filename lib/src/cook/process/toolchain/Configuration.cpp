#include "cook/process/toolchain/Configuration.hpp"
#include "cook/log/Scope.hpp"

namespace cook { namespace process { namespace toolchain {


    Configuration::Configuration(priority_type priority, const std::string & uuid) : 
        uuid(uuid),
        priority(priority)
    {
    }

    bool Configuration::operator<(const Configuration & rhs) const
    {
        return (priority < rhs.priority) ||
        (priority == rhs.priority && uuid < rhs.uuid);
    }

    void ConfigurationBoard::add_configuration(const std::string & key, const std::string & value)
    {
        auto s = log::scope("Configuration value", [&](auto & n){
            n.attr("key", key).attr("value", value);
        });
        config_.insert(std::make_pair(ConfigPair(key, value), New));
    }

    bool ConfigurationBoard::add_callback(const Configuration & config)
    {
        return config_cbs_.insert(config).second;
    } 

    bool ConfigurationBoard::has_config(const std::string & key, const std::string & value) const
    {
        return config_.find(ConfigPair(key, value)) != config_.end();
    }

    std::list<std::string> ConfigurationBoard::config_values(const std::string & key) const
    {
        std::list<std::string> result;
        for(const auto & el : config_)
            if (el.first.first == key)
                result.push_back(el.first.second);

        return result;
    }

    bool ConfigurationBoard::resolve_(Element::Ptr element, const ConfigPair & p)
    {
        auto s = log::scope("Resolving", [&](auto & n) {
            n.attr("element_type", element->type()).attr("element_language", element->language()).attr("key", p.first).attr("value", p.second);
        });
        for(const Configuration & cfg : config_cbs_)
        {
            bool res = cfg.callback(element, p.first, p.second, *this);
            auto ss = log::scope("callback", [&](auto & n) {
                n.attr("uuid", cfg.uuid).attr("result", res);
            });

            if (res)
                return true;
        }

        return false;
    }

} } }

