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

    class Configuration
    {
    public:
        using Callback = std::function<bool (Element::Ptr, const std::string &, const std::string &, ConfigurationBoard &)>;
        using priority_type = unsigned int;

        Configuration(priority_type priority, const std::string & uuid);

        bool operator<(const Configuration & rhs) const;

        Callback callback;

        const std::string uuid;
        const priority_type priority;
    };

    class ConfigurationBoard
    {
    public:
        enum State
        {
            New, Resolved, Unresolved
        };
        using ConfigPair = std::pair<std::string, std::string>;

        void add_configuration(const std::string & key, const std::string & value);

        template <typename It>
        bool process(It first, It second)
        {
            auto find_new = [&]() 
            {
                return std::find_if(config_.begin(), config_.end(), [](std::map<ConfigPair, State>::value_type & p) { return p.second == New; });
            };

            // find first unresolved
            auto it = find_new();
            if(it == config_.end())
                return false;

            // try to resolve the configuration for this element
            auto & cfg = *it;
            {
                bool did_resolve = false;
                for(Element::Ptr element : gubg::make_range(first, second))
                    if (resolve_(element, cfg.first))
                        did_resolve = true;

                // we did
                cfg.second = (did_resolve ? Resolved : Unresolved);
            }

            // check whether we have still element to do
            return find_new() != config_.end();
        }

        bool add_callback(const Configuration & config);
        bool has_config(const std::string & key, const std::string & value) const;
        std::list<std::string> config_values(const std::string & key) const;

        template <typename Functor>
        Result each_config(Functor && functor) const
        {
            MSS_BEGIN(Result);

            for(const auto & el : config_)
                MSS(functor(el.first.first, el.first.second, el.second));

            MSS_END();
        }

    private:
        bool resolve_(Element::Ptr element, const ConfigPair & p);

        std::set<Configuration> config_cbs_;
        std::map<ConfigPair, State> config_;
    };

} } }

#endif

