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

namespace cook { namespace process { namespace toolchain {

    class ConfigurationBoard;
    class Element;

    class ConfigurationCallback
    {
    public:
        using Callback = std::function<bool (Element::Ptr, const std::string &, const std::string &, ConfigurationBoard &)>;
        using Priority = unsigned int;

        ConfigurationCallback(Priority priority, const std::string & uuid);

        bool operator<(const ConfigurationCallback & rhs) const;

        Callback callback;

        const std::string uuid;
        const Priority priority;
    };

    class ConfigurationBoard
    {
    public:
        enum State
        {
            New, Resolved
        };
        using KeyValue = std::pair<std::string, std::string>;

        void add_configuration(const std::string & key, const std::string & value);
        void add_configuration(const std::string & key);

        // Finds the configuration key-value that requires processing with the lowest priority configuration callback. This callback will be processed for all elements.
        // If one element can handle it, the key-value will remain there as `Resolved`, else, it will move to the next configuration callback as `New`.
        template <typename It>
        bool process(It element_begin, It element_end)
        {
            // Find new key-value with lowest callback priority
            auto cb_it = cb__state__kvs_.end();
            KeyValue kv;
            for (auto it = cb__state__kvs_.begin(); it != cb__state__kvs_.end(); ++it)
            {
                if (auto p2 = it->second.find(New); p2 != it->second.end())
                {
                    if (auto &kvs = p2->second; !kvs.empty())
                    {
                        cb_it = it;
                        kv = *kvs.begin();
                        kvs.erase(kv);
                        break;
                    }
                }
            }

            if (cb_it == cb__state__kvs_.end())
                // All key-values are processed
                return false;

            const auto &cb = cb_it->first;

            // Process the key-value with this callback for all elements
            // Question: Shouldn't we keep track of the resolved state _per element_?
            bool did_resolve = false;
            for (Element::Ptr element : gubg::make_range(element_begin, element_end))
            {
                auto s = log::scope("Resolving", [&](auto & n) {
                    n.attr("element_type", element->element_type()).attr("element_language", element->language()).attr("target_type", element->target_type()).attr("key", kv.first).attr("value", kv.second);
                });
                const bool res = cb.callback(element, kv.first, kv.second, *this);
                auto ss = log::scope("callback", [&](auto & n) {
                    n.attr("uuid", cb.uuid).attr("result", res);
                });
                if (res)
                    did_resolve = true;
            }

            if (did_resolve)
            {
                // The callback could handle the key-value: keep it here as `Resolved`
                cb_it->second[Resolved].insert(kv);
            }
            else
            {
                // The callback could not handle it: move as `New` to the next callback
                ++cb_it;
                if (cb_it != cb__state__kvs_.end())
                    cb_it->second[New].insert(kv);
            }

            return true;
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

            for (const auto &[cb, state__kvs]: cb__state__kvs_)
            {
                for (const auto &[state, kvs]: state__kvs)
                {
                    for (const auto &kv: kvs)
                    {
                        MSS(functor(kv.first, kv.second, state));
                    }
                }
            }

            MSS_END();
        }

    private:
        using KeyValues = std::set<KeyValue>;
        using State__KeyValues = std::map<State, KeyValues>;
        using Callback__State__KeyValues = std::map<ConfigurationCallback, State__KeyValues>;
        Callback__State__KeyValues cb__state__kvs_;
    };

} } }

#endif

