#include "cook/process/toolchain/Configuration.hpp"
#include "cook/log/Scope.hpp"

namespace cook { namespace process { namespace toolchain {


    ConfigurationCallback::ConfigurationCallback(Priority priority, const std::string & uuid) : 
        uuid(uuid),
        priority(priority)
    {
    }

    bool ConfigurationCallback::operator<(const ConfigurationCallback & rhs) const
    {
        return (priority < rhs.priority) ||
        (priority == rhs.priority && uuid < rhs.uuid);
    }

    ConfigurationBoard::ConfigurationBoard()
    {
        {
            ConfigurationCallback first_cb{0u, ""};
            first_cb.callback = [](Element::Ptr, const std::string &, const std::string &, ConfigurationBoard &){
                return false;
            };
            cb__state__kvs_.emplace(first_cb, State__KeyValues{});
        }
        {
            ConfigurationCallback last_cb{std::numeric_limits<ConfigurationCallback::Priority>::max(), "~~~~~~~~~~~~~~~~"};
            last_cb.callback = [](Element::Ptr, const std::string &key, const std::string &value, ConfigurationBoard &){
                return true;
            };
            cb__state__kvs_.emplace(last_cb, State__KeyValues{});
        }
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

        const KeyValue kv{key, value};

        for (const auto &[cb, state__kvs]: cb__state__kvs_)
            for (const auto &[state, kvs]: state__kvs)
                if (kvs.count(kv))
                {
                    return;
                }

        if (cb__state__kvs_.empty())
        {
            return;
        }

        cb__state__kvs_.begin()->second[New].insert(kv);
    }

    bool ConfigurationBoard::add_callback(const ConfigurationCallback & config_callback)
    {
        return cb__state__kvs_.emplace(config_callback, State__KeyValues{}).second;
    } 

    bool ConfigurationBoard::has_config(const std::string & key, const std::string & value) const
    {
        const KeyValue needle{key, value};

        for (const auto &[cb, state__kvs]: cb__state__kvs_)
        {
            for (const auto &[state, kvs]: state__kvs)
            {
                if (kvs.count(needle))
                    return true;
            }
        }

        return false;
    }
    bool ConfigurationBoard::has_config(const std::string & key) const
    {
        for (const auto &[cb, state__kvs]: cb__state__kvs_)
        {
            for (const auto &[state, kvs]: state__kvs)
            {
                for (const auto &kv: kvs)
                {
                    if (kv.first == key)
                        return true;
                }
            }
        }

        return false;
    }

    bool ConfigurationBoard::remove_config(const std::string & key)
    {
        unsigned int nr = 0;
        for (auto &[cb, state__kvs]: cb__state__kvs_)
        {
            for (auto &[state, kvs]: state__kvs)
            {
                for (auto it = kvs.begin(); it != kvs.end();)
                {
                    if (it->first == key)
                    {
                        it = kvs.erase(it);
                        ++nr;
                    }
                    else
                        ++it;
                }
            }
        }
        return nr > 0;
    }
    bool ConfigurationBoard::remove_config(const std::string & key, const std::string & value)
    {
        const KeyValue needle{key, value};

        unsigned int nr = 0;
        for (auto &[cb, state__kvs]: cb__state__kvs_)
        {
            for (auto &[state, kvs]: state__kvs)
            {
                nr += kvs.erase(needle);
            }
        }

        return nr > 0;
    }

    std::list<std::string> ConfigurationBoard::config_values(const std::string & key) const
    {
        std::list<std::string> result;

        for (const auto &[cb, state__kvs]: cb__state__kvs_)
        {
            for (const auto &[state, kvs]: state__kvs)
            {
                for (const auto &kv: kvs)
                {
                    if (kv.first == key)
                        result.push_back(kv.second);
                }
            }
        }

        return result;
    }

} } }

