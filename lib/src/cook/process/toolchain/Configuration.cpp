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


    void ConfigurationBoard::add_configuration(const std::string & key)
    {
        add_configuration(key, "true");
    }

    void ConfigurationBoard::add_configuration(const std::string & key, const std::string & value)
    {
        auto s = log::scope("Configuration value", [&](auto & n){
            n.attr("key", key).attr("value", value);
        });
        config_.insert(std::make_pair(KeyValue(key, value), New));
    }

    bool ConfigurationBoard::add_callback(const Configuration & config)
    {
        return config_cbs_.insert(config).second;
    } 

    bool ConfigurationBoard::has_config(const std::string & key, const std::string & value) const
    {
        return config_.find(KeyValue(key, value)) != config_.end();
    }
    bool ConfigurationBoard::has_config(const std::string & key) const
    {
        auto lambda = [&](const auto & p)
        {
            return p.first.first == key;
        };
        return std::find_if(config_.begin(), config_.end(), lambda) != config_.end();
    }

    bool ConfigurationBoard::remove_config(const std::string & key)
    {
        unsigned int nr = 0;
        for (auto it = config_.begin(); it != config_.end();)
        {
            if (it->first.first == key)
            {
                it = config_.erase(it);
                ++nr;
            }
            else
                ++it;
        }
        return nr > 0;
    }
    bool ConfigurationBoard::remove_config(const std::string & key, const std::string & value)
    {
        auto it = config_.find(KeyValue(key, value));
        if (it == config_.end())
            return false;
        config_.erase(it);
        return true;
    }

    std::list<std::string> ConfigurationBoard::config_values(const std::string & key) const
    {
        std::list<std::string> result;
        for(const auto & el : config_)
            if (el.first.first == key)
                result.push_back(el.first.second);

        return result;
    }

    bool ConfigurationBoard::resolve_(Element::Ptr element, const KeyValue & kv)
    {
        auto s = log::scope("Resolving", [&](auto & n) {
            n.attr("element_type", element->element_type()).attr("element_language", element->language()).attr("target_type", element->target_type()).attr("key", kv.first).attr("value", kv.second);
        });
        for(const Configuration & cfg : config_cbs_)
        {
            const bool res = cfg.callback(element, kv.first, kv.second, *this);
            auto ss = log::scope("callback", [&](auto & n) {
                n.attr("uuid", cfg.uuid).attr("result", res);
            });

            if (res)
                return true;
        }

        return false;
    }

} } }

