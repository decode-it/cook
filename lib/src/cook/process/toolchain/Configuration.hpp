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
            New, Resolving, Resolved,
        };
        using KeyValue = std::pair<std::string, std::string>;

        bool add_callback(const ConfigurationCallback & config_callback);

        void add_configuration(const std::string & key, const std::string & value);
        void add_configuration(const std::string & key);
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

        bool process(const Elements &);

    private:
        std::map<KeyValue, State> kv__state_;
        bool has_new_kvs_ = false;

        std::map<ConfigurationCallback::Key, ConfigurationCallback> callbacks_;
    };

} } }

#endif

