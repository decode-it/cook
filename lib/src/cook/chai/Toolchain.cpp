#include "cook/chai/Toolchain.hpp"

namespace cook { namespace chai {

        Toolchain::Toolchain(Manager * manager)
        : manager_(manager)
        {
        }

        bool Toolchain::has_element(ElementType type, const Flags & language)
        {
            CHAI_MSS_BEGIN();
            CHAI_MSS(check_language_(language));

            return !!manager_->element(type, language.language().first);
        }

        ToolchainElement Toolchain::element(ElementType type, const Flags & language)
        {
            CHAI_MSS_BEGIN();
            CHAI_MSS(check_language_(language));

            Language lang = language.language().first;

            if (is_frozen())
                CHAI_MSS_MSG(!!manager_->element(type, lang), Error, "No toolchain element with type " << type << " and language " << lang << " exists");
            
            ToolchainElement el(manager_->goc_element(type, lang));
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

        void Toolchain::configure(unsigned int priority, const std::string & uuid, const ConfigurationCallback & cb)
        {
            using CFG = process::toolchain::Configuration;

            CHAI_MSS_BEGIN();
            CHAI_MSS_MSG(!is_frozen(), Error, "The toolchain is frozen, cannot add configuration callbacks");

            CFG cfg(priority, uuid);
            cfg.callback = [=](process::toolchain::Element::Ptr e, const std::string & k, const std::string & v, ConfigurationBoard & b)
            {
                return cb(ToolchainElement(e), k, v, b);
            };
            manager_->add_configuration_callback(std::move(cfg));

        }
} }
