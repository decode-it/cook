#include "cook/process/toolchain/Manager.hpp"
#include "cook/util/ElementAt.hpp"
#include "cook/log/Scope.hpp"
#include "cook/OS.hpp"
#include "gubg/Strange.hpp"
#include "gubg/iterator/Transform.hpp"

namespace cook { namespace process { namespace toolchain { 

    bool Manager::Key::operator<(const Key & rhs) const
    {
        if (element_type < rhs.element_type)
            return true;
        if (element_type > rhs.element_type)
            return false;

        if (language < rhs.language)
            return true;
        if (language > rhs.language)
            return false;

        return target_type < rhs.target_type;
    }

    namespace {

        std::filesystem::path linux_namer(const model::Recipe & recipe)
        {
            std::ostringstream oss;
            const std::string & name = recipe.build_target().name;

            switch(recipe.build_target().type)
            {
                case TargetType::Archive:
                    oss << "lib" << name << ".a";
                    break;

                case TargetType::Executable:
                    oss << name;
                    break;
                case TargetType::SharedLibrary:
                    oss << "lib" << name << ".so";
                    break;

                default:
                    return std::filesystem::path();
            }

            return std::filesystem::path(oss.str());
        }

        std::filesystem::path windows_namer(const model::Recipe & recipe)
        {
            std::ostringstream oss;
            const std::string & name = recipe.build_target().name;

            switch(recipe.build_target().type)
            {
                case TargetType::Archive:
                    oss << name << ".lib";
                    break;

                case TargetType::Executable:
                    oss << name << ".exe";
                    break;
                case TargetType::SharedLibrary:
                    oss << name << ".dll";
                    break;

                default:
                    return std::filesystem::path();
            }

            return std::filesystem::path(oss.str());
        }
    }


    Manager::Manager()
    : primary_target_functor_(get_os() == OS::Windows ? windows_namer : linux_namer)
    {
    }

    bool Manager::is_initialized() const
    {
        return initialized_;
    }

    Element::Ptr Manager::element(Element::Type type, Language language, TargetType target_type) const
    {
        Key k(type, language, target_type);
        auto it = elements_.find(k);
        return it == elements_.end() ? Element::Ptr() : it->second;
    }

    Element::Ptr Manager::goc_element(Element::Type type, Language language, TargetType target_type)
    {
        Key k(type, language, target_type);
        auto p = elements_.insert(std::make_pair(k, Element::Ptr()));

        // a new element
        if(p.second)
            p.first->second = std::make_shared<Element>(type, language, target_type);
        
        return p.first->second;
    }

    void Manager::add_config(const std::string & value)
    {
        board_.add_configuration(value);
        if (is_initialized())
            resolve_();
    }

    void Manager::add_config(const std::string & key, const std::string & value)
    {
        board_.add_configuration(key, value);
        if (is_initialized())
            resolve_();
    }

    Result Manager::resolve_()
    {
        MSS_BEGIN(Result);

        {
            auto first = gubg::iterator::transform(elements_.begin(), util::ElementAt<1>());
            auto last = gubg::iterator::transform(elements_.end(), util::ElementAt<1>());
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

    Result Manager::initialize()
    {
        MSS_BEGIN(Result);

        MSG_MSS(!is_initialized(), InternalError, "Manager Initialize multiple calls");

        MSS(resolve_());
        initialized_ = true;

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

    void Manager::each_config(const std::function<void(const std::string &, const std::string &)> &cb)
    {
        auto lambda = [&](const std::string &key, const std::string &value, ConfigurationBoard::State)
        {
            cb(key, value);
            return Result{};
        };
        board_.each_config(lambda);
    }
    void Manager::each_config(const std::string &wanted_key, const std::function<void(const std::string &)> &cb)
    {
        auto lambda = [&](const std::string &key, const std::string &value, ConfigurationBoard::State)
        {
            if (key == wanted_key)
                cb(value);
            return Result{};
        };
        board_.each_config(lambda);
    }

    const Manager::NameFunctor & Manager::primary_target_functor() const
    {
        return primary_target_functor_;
    }

    void Manager::set_primary_target_functor(const NameFunctor & functor)
    {
        primary_target_functor_ = functor;
    }

    std::filesystem::path Manager::get_primary_target(const model::Recipe & recipe) const
    {
        if (!primary_target_functor_)
            return std::filesystem::path();

        return primary_target_functor_(recipe);
    }

} } } 
