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
                case TargetType::Plugin:
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
                case TargetType::Plugin:
                    oss << name << ".dll";
                    break;

                default:
                    return std::filesystem::path();
            }

            return std::filesystem::path(oss.str());
        }

        std::filesystem::path intermediary_namer(const std::filesystem::path & file, const LanguageTypePair & src, const LanguageTypePair & dst, Element::Type type)
        {
            switch(type)
            {
                case Element::Compile:
                    if (src.language == Language::Resource && dst.type == Type::Object)
                        return file.string() + ".res";
                    else if (dst.type == Type::Object)
                        return file.string() + ".obj";
                    break;


                default:
                    break;
            }
            return file;
        }
    }


    Manager::Manager(): 
        primary_target_functor_(get_os() == OS::Windows ? windows_namer : linux_namer),
        intermediary_name_(intermediary_namer)
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
        
    Element::Ptr Manager::clone_(Element::Type element_type, Language language, TargetType target_type) const
    {
        Element::Ptr e = element(element_type, language, target_type);
        if (e)
            e = std::make_shared<Element>(*e);
        return e;
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
    bool Manager::has_config(const std::string & key) const
    {
        return board_.has_config(key);
    }

    bool Manager::remove_config(const std::string & key, const std::string & value)
    {
        return board_.remove_config(key, value);
    }
    bool Manager::remove_config(const std::string & key)
    {
        return board_.remove_config(key);
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

    void Manager::each_config(const std::function<void(const std::string &, const std::string &)> &cb) const
    {
        auto lambda = [&](const std::string &key, const std::string &value, ConfigurationBoard::State)
        {
            cb(key, value);
            return Result{};
        };
        board_.each_config(lambda);
    }
    void Manager::each_config(const std::function<void (const std::string &, const std::string &, bool resolved)> &cb) const
    {
        auto lambda = [&](const std::string & key, const std::string & value, ConfigurationBoard::State s)
        {
            cb(key, value, s == ConfigurationBoard::Resolved);
            return Result{};
        };
        board_.each_config(lambda);
    }
    void Manager::each_config(const std::string &wanted_key, const std::function<void(const std::string &)> &cb) const
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

    void Manager::set_intermediary_name_functor(const IntermediaryName & functor)
    {
        intermediary_name_ = functor;
    }
        
    void Manager::set_command_configuration_functor(const CommandConfigurationFunctor & functor)
    {
        configure_command_ = functor;
    }

    std::filesystem::path Manager::get_primary_target(const model::Recipe & recipe) const
    {
        if (!primary_target_functor_)
            return std::filesystem::path();

        return primary_target_functor_(recipe);
    }
        
    std::filesystem::path Manager::intermediary_name(const std::filesystem::path & file, const LanguageTypePair & src, const LanguageTypePair & dst, Element::Type element_type) const
    {
        return intermediary_name_(file, src, dst, element_type);
    }

} } } 
