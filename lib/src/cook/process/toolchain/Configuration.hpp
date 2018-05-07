#ifndef HEADER_cook_process_toolchain_Configuration_hpp_ALREADY_INCLUDED
#define HEADER_cook_process_toolchain_Configuration_hpp_ALREADY_INCLUDED

#include <map>
#include <list>
#include <string>
#include <functional>

namespace cook { namespace process { namespace toolchain {

    class Configuration
    {
    public:
        using priority_type = unsigned int;
        using ConfigurationFunctor = std::function<bool (const std::string &, const std::string &, const Configuration & )>;

        void add_config(priority_type priority, const ConfigurationFunctor & functor, bool at_back = true)
        {
            auto & lst = config_[priority];
            
            auto it = at_back ? lst.end() : lst.begin();
            lst.insert(it, functor);
        }

        void add_front_config(const ConfigurationFunctor & functor)
        {
            config_[0].push_front(functor);
        }

        bool configure(const std::string & key, const std::string & value) const
        {
            for(const auto & p : config_)
                for(const auto & f : p.second)
                    if(f(key, value, *this))
                        return true;
            return false;
        }

    private:
        std::map<priority_type, std::list<ConfigurationFunctor>> config_;
    };

} } }

#endif

