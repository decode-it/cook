#include "cook/chai/Toolchain.hpp"
#include "cook/chai/Recipe.hpp"
#include "cook/process/toolchain/Element.hpp"

namespace cook { namespace chai {

        Toolchain::Toolchain(Manager * manager, const Context* context)
        : manager_(manager),
          context_(context)
        {
        }

        bool Toolchain::has_element(ElementType type, const Flags & language, TargetType target_type)
        {
            CHAI_MSS_BEGIN();
            CHAI_MSS(check_language_(language));

            return !!manager_->element(type, language.language().first, target_type);
        }

        ToolchainElement Toolchain::element(ElementType type, const Flags & language, TargetType target_type)
        {
            CHAI_MSS_BEGIN();
            CHAI_MSS(check_language_(language));

            Language lang = language.language().first;

            if (is_frozen())
                CHAI_MSS_MSG(!!manager_->element(type, lang, target_type), Error, "No toolchain element with type " << type << " and language " << lang << " exists");
            
            ToolchainElement el(manager_->goc_element(type, lang, target_type), context_);
            el.set_freeze_flag(is_frozen());

            return el;
        }
        
        Result Toolchain::check_language_(const Flags & language)
        {
            MSS_BEGIN(Result);
            MSS(language.exactly({Flag::Language}));
            MSS_END();
        }

        void Toolchain::add_config_1(const std::string & key)
        {
            CHAI_MSS_BEGIN();
            CHAI_MSS_MSG(!is_frozen(), Error, "The toolchain is frozen, cannot add config values");

            manager_->add_config(key);
        }

        void Toolchain::add_config_2(const std::string & key, const std::string & value)
        {
            CHAI_MSS_BEGIN();
            CHAI_MSS_MSG(!is_frozen(), Error, "The toolchain is frozen, cannot add config values");

            manager_->add_config(key, value);
        }
        
        bool Toolchain::has_config_1(const std::string & key) const
        {
            return manager_->has_config(key);
        }
        bool Toolchain::has_config_2(const std::string & key, const std::string & value) const
        {
            return manager_->has_config(key, value);
        }

        bool Toolchain::remove_config_1(const std::string & key)
        {
            return manager_->remove_config(key);
        }
        bool Toolchain::remove_config_2(const std::string & key, const std::string & value)
        {
            return manager_->remove_config(key, value);
        }

        void Toolchain::configure(unsigned int priority, const std::string & uuid, ConfigurationCallback cb)
        {
            using CFG = process::toolchain::Configuration;

            CHAI_MSS_BEGIN();
            CHAI_MSS_MSG(!is_frozen(), Error, "The toolchain is frozen, cannot add configuration callbacks");

            auto ctx = context_;

            CFG cfg(priority, uuid);
            cfg.callback = [=](process::toolchain::Element::Ptr e, const std::string & k, const std::string & v, ConfigurationBoard & b)
            {
                return cb(ToolchainElement(e,ctx), k, v, b);
            };
            manager_->add_configuration_callback(std::move(cfg));

        }

        void Toolchain::each_config_1(const IterationCallback1 &cb)
        {
            CHAI_MSS_BEGIN();
            manager_->each_config(cb);
        }
        void Toolchain::each_config_2(const std::string &key, const IterationCallback2 &cb)
        {
            CHAI_MSS_BEGIN();
            manager_->each_config(key, cb);
        }

        void Toolchain::set_primary_name_functor(const PrimaryNameFunctor & functor)
        {
            const Context * context = context_;
            auto lambda = [=](const model::Recipe & recipe) -> std::filesystem::path
            {
                Recipe r(const_cast<model::Recipe *>(&recipe), context);
                return std::filesystem::path(functor(r)); 
            };
            manager_->set_primary_target_functor(lambda);
        }
        
        void Toolchain::set_intermediary_name_functor(const IntermediaryNameFunctor & functor)
        {
            auto lambda = [=](const std::filesystem::path & path, const LanguageTypePair & src, const LanguageTypePair & dst, ElementType type)
            {
                auto flags_src = Flags(src.language) | src.type;
                auto flags_dst = Flags(dst.language) | dst.type;

                return functor(path.string(), flags_src, flags_dst, type);
            };
            manager_->set_intermediary_name_functor(lambda);
        }
        
        void Toolchain::set_command_configuration_functor(const CommandConfigurationFunctor & functor)
        {
            const Context * ctx = context_;
            auto lambda = [=](process::toolchain::Element::Ptr element, model::Recipe * recipe)
            {
                auto te = ToolchainElement(element, context_);
                auto re = Recipe(recipe, ctx);
                functor(te, re);
            };
            manager_->set_command_configuration_functor(lambda);
        }

} }
