#include "cook/process/toolchain/Manager.hpp"
#include "cook/util/ElementAt.hpp"
#include "cook/log/Scope.hpp"
#include "cook/OS.hpp"
#include "gubg/Strange.hpp"
#include "boost/iterator/transform_iterator.hpp"

namespace cook { namespace process { namespace toolchain { 

    Manager::Manager()
    {
    }

    bool Manager::is_initialized() const
    {
        return initialized_;
    }

    Element::Ptr Manager::element(Element::Type type, Language language) const
        {
            for (const auto & p : elements_)
                if (p.first.first == type && p.first.second == language)
                    return p.second;

            return Element::Ptr();
        }

    Element::Ptr Manager::goc_element(Element::Type type, Language language)
        {
            for (const auto & p : elements_)
                if (p.first.first == type && p.first.second == language)
                    return p.second;

            return elements_.emplace(std::make_pair(type, language), std::make_shared<Element>(type, language)).first->second;
        }

        void Manager::add_config(const std::string & value)
        {
            board_.add_configuration(value);
        }

        void Manager::add_config(const std::string & key, const std::string & value)
        {
            board_.add_configuration(key, value);
        }

        Result Manager::initialize()
        {
            MSS_BEGIN(Result);

            {
                auto first = boost::make_transform_iterator(elements_.begin(), util::ElementAt<1>());
                auto last = boost::make_transform_iterator(elements_.end(), util::ElementAt<1>());
                while (board_.process(first, last))
                    ;;
            }

            auto check_all_resolved =[](const std::string & key, const std::string & value, ConfigurationBoard::State s)
            {
                MSS_BEGIN(Result);
                MSG_MSS(s == ConfigurationBoard::Resolved, Warning, "Unresolved configuration " << key << " = " << value);
                MSS_END();

            };
            MSS(board_.each_config(check_all_resolved));
            MSS_END();
        }


        bool Manager::has_config(const std::string & key, const std::string & value) const
        {
            return board_.has_config(key, value);
        }
        
        void Manager::add_configuration_callback(Configuration && cb)
        {
            board_.add_callback(std::move(cb));
        }

        void Manager::add_configuration_callback(const Configuration & cb)
        {
            board_.add_callback(cb);
        }
    
        std::list<std::pair<std::string, std::string>> Manager::all_config_values() const
        {
            std::list<std::pair<std::string, std::string>> lst;
            auto add = [&](const std::string & key, const std::string & value, ConfigurationBoard::State s)
            {
                lst.push_back(std::make_pair(key, value));
                return true;
            };
            board_.each_config(add);
            return lst;

        }

} } } 
