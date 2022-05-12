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
        has_new_kvs_ = true;
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

    bool ConfigurationBoard::process(const Elements &elements)
    {
        MSS_BEGIN(bool);

        using CB_EL_KV = std::tuple<ConfigurationCallback::Key, Element::Key, KeyValue>;
        using PriorityQueue = std::priority_queue<CB_EL_KV, std::vector<CB_EL_KV>, std::greater<CB_EL_KV>>;
        PriorityQueue priority_queue;

        while (true)
        {
            // Insert new key-values into the priority_queue, if any
            if (has_new_kvs_)
            {
                for (auto &[kv, state]: kv__state_)
                {
                    if (state != State::New)
                        continue;
                    for (const auto &[cb, _]: callbacks_)
                    {
                        for (const auto &[el, _]: elements)
                            priority_queue.emplace(cb, el, kv);

                        break;
                    }
                    state = State::Resolving;
                }
                has_new_kvs_ = false;
            }

            if (priority_queue.empty())
                // Nothing to process anymore
                break;

            // Get the highest prio callback-element-keyvalue combination to process
            CB_EL_KV cb_el_kv = priority_queue.top();
            priority_queue.pop();

            auto cb_it = callbacks_.find(std::get<0>(cb_el_kv));
            MSS(cb_it != callbacks_.end());
            auto &cb = cb_it->second;

            auto el_it = elements.find(std::get<1>(cb_el_kv));
            MSS(el_it != elements.end());
            auto &element = el_it->second;

            const auto kv = std::get<2>(cb_el_kv);

            {
                auto s = log::scope("Resolving", [&](auto & n) {
                    n.attr("element_type", element->element_type()).attr("element_language", element->language()).attr("target_type", element->target_type()).attr("key", kv.first).attr("value", kv.second);
                });
                const bool res = cb.callback(element, kv.first, kv.second, *this);
                auto ss = log::scope("callback", [&](auto & n) {
                    n.attr("uuid", cb.key.uuid).attr("result", res);
                });
                if (res)
                {
                    // The callback could handle the key-value for the given element
                    // Indicate that at least one callback could resolve it.
                    kv__state_[kv] = State::Resolved;
                }
                else
                {
                    // The callback could not handle the key-value. Try the next, if any.
                    ++cb_it;
                    if (cb_it != callbacks_.end())
                    {
                        std::get<0>(cb_el_kv) = cb_it->first;
                        priority_queue.push(cb_el_kv);
                    }
                }
            }
        }

        auto check_all_resolved =[](const std::string & key, const std::string & value, bool resolved)
        {
            MSS_BEGIN(Result);
            MSG_MSS(resolved, Warning, "Unresolved configuration " << key << " = " << value);
            MSS_END();
        };
        MSS(each_config(check_all_resolved));

        MSS_END();
    }

} } }

