#include "cook/process/toolchain/Configuration.hpp"
#include "cook/log/Scope.hpp"

namespace cook { namespace process { namespace toolchain {


    ConfigurationCallback::ConfigurationCallback(Priority priority, const std::string & uuid) : key(priority, uuid)
    {
    }

    bool ConfigurationCallback::Key::operator<(const ConfigurationCallback::Key & rhs) const
    {
        return std::make_pair(priority, uuid) < std::make_pair(rhs.priority, rhs.uuid);
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

        kv__state_.emplace(KeyValue{key, value}, State::New);
    }

    bool ConfigurationBoard::add_callback(const ConfigurationCallback & config_callback)
    {
        return callbacks_.emplace(config_callback.key, config_callback).second;
    } 

    bool ConfigurationBoard::has_config(const std::string & key, const std::string & value) const
    {
        return kv__state_.count(KeyValue{key, value}) > 0;
    }
    bool ConfigurationBoard::has_config(const std::string & key) const
    {
        for (const auto &[kv, _]: kv__state_)
        {
            if (kv.first == key)
                return true;
        }
        return false;
    }

    bool ConfigurationBoard::remove_config(const std::string & key)
    {
        unsigned int nr = 0;
        for (auto it = kv__state_.begin(); it != kv__state_.end(); ++it)
        {
            if (it->first.first == key)
            {
                it = kv__state_.erase(it);
                ++nr;
            }
            else
                ++it;
        }
        return nr > 0;
    }
    bool ConfigurationBoard::remove_config(const std::string & key, const std::string & value)
    {
        return kv__state_.erase(KeyValue{key, value}) > 0;
    }

    std::list<std::string> ConfigurationBoard::config_values(const std::string & key) const
    {
        std::list<std::string> result;

        for (const auto &[kv, _]: kv__state_)
        {
            if (kv.first == key)
                result.push_back(kv.second);
        }

        return result;
    }

} } }

