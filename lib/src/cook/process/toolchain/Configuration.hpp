#ifndef HEADER_cook_process_toolchain_Configuration_hpp_ALREADY_INCLUDED
#define HEADER_cook_process_toolchain_Configuration_hpp_ALREADY_INCLUDED

#include "cook/process/toolchain/Element.hpp"
#include "cook/Result.hpp"
#include "gubg/Range.hpp"

#include <set>
#include <map>
#include <string>
#include <algorithm>
#include <functional>
#include <queue>
#include <list>

namespace cook { namespace process { namespace toolchain {

    class ConfigurationBoard;
    class Element;

    class ConfigurationCallback
    {
    public:
        using Callback = std::function<bool (Element::Ptr, const std::string &, const std::string &, ConfigurationBoard &)>;
        using Priority = unsigned int;

        ConfigurationCallback(Priority priority, const std::string & uuid);

        bool operator<(const ConfigurationCallback & rhs) const { return key < rhs.key; }

        Callback callback;

        struct Key
        {
            Priority priority;
            std::string uuid;

            Key(Priority priority, const std::string &uuid): uuid(uuid), priority(priority) {}

            bool operator<(const Key & rhs) const;
        };

        const Key key;
    };

    inline std::ostream &operator<<(std::ostream &os, const ConfigurationCallback::Key &key)
    {
        os << "[Callback](priority:" << key.priority << ")(uuid:" << key.uuid << ")";
        return os;
    }

    class ConfigurationBoard
    {
    public:
        enum class State
        {
            New, Resolving, Resolved, Unresolved,
        };
        using KeyValue = std::pair<std::string, std::string>;

        void add_configuration(const std::string & key, const std::string & value);
        void add_configuration(const std::string & key);

        // Finds the configuration key-value that requires processing with the lowest priority configuration callback. This callback will be processed for all elements.
        // If one element can handle it, the key-value will remain there as `Resolved`, else, it will move to the next configuration callback as `New`.
        template <typename It>
        bool process(It element_begin, It element_end)
        {
            MSS_BEGIN(bool);

            using CB_EL_KV = std::tuple<ConfigurationCallback::Key, Element::Key, KeyValue>;
            using PriorityQueue = std::priority_queue<CB_EL_KV, std::vector<CB_EL_KV>, std::greater<CB_EL_KV>>;
            PriorityQueue priority_queue;

            std::map<Element::Key, Element::Ptr> elements;
            for (auto el = element_begin; el != element_end; ++el)
                elements[(*el)->key] = *el;

            while (true)
            {
                // Insert new key-values into the priority_queue
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

                if (priority_queue.empty())
                    // Nothing to process anymore
                    break;

                // Get the highest prio callback-element-keyvalue combination to process
                CB_EL_KV cb_el_kv = priority_queue.top();
                priority_queue.pop();

                auto cb_it = callbacks_.find(std::get<0>(cb_el_kv));
                MSS(cb_it != callbacks_.end());
                auto &cb = cb_it->second;
                auto element = elements[std::get<1>(cb_el_kv)];
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
                        kv__state_[kv] = State::Resolved;
                    }
                    else
                    {
                        // The callback could not handle the key-value. Try the next, if any.
                        ++cb_it;
                        if (cb_it == callbacks_.end())
                        {
                            kv__state_[kv] = State::Unresolved;
                        }
                        else
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

        bool add_callback(const ConfigurationCallback & config_callback);
        bool has_config(const std::string & key, const std::string & value) const;
        bool has_config(const std::string & key) const;
        bool remove_config(const std::string & key);
        bool remove_config(const std::string & key, const std::string & value);
        std::list<std::string> config_values(const std::string & key) const;

        template <typename Functor>
        Result each_config(Functor && functor) const
        {
            MSS_BEGIN(Result);

            for (const auto &[kv, state]: kv__state_)
            {
                MSS(functor(kv.first, kv.second, state == State::Resolved));
            }

            MSS_END();
        }

    private:
        std::map<KeyValue, State> kv__state_;
        std::map<ConfigurationCallback::Key, ConfigurationCallback> callbacks_;
    };

} } }

#endif

