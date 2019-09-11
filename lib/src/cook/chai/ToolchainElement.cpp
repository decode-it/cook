#include "cook/chai/ToolchainElement.hpp"

namespace cook { namespace chai {

    KeyValues::KeyValues(key_values_type * map)
    : map_(map)
    {
    }

    void KeyValues::clear(Part part)
    {
        (*map_)[part].clear();
    }

    void KeyValues::append_1(Part part, const std::string & key)
    {
        (*map_)[part].emplace_back(key, "");
    }

    void KeyValues::append_2(Part part, const std::string & key, const std::string & value)
    {
        (*map_)[part].emplace_back(key, value);
    }

    Translators::Translators(translators_type * map)
    : map_(map)
    {
    }

    Translators::translator_type & Translators::operator[](Part part) 
    {
        return (*map_)[part];
    }

    ToolchainElement::ToolchainElement(Element::Ptr element, const Context* context)
        : element_(element)
        , context_(context)
    {
    }

    KeyValues ToolchainElement::key_values() const
    {
        KeyValues kv(&element_->key_values_map());
        kv.set_freeze_flag(is_frozen());
        return kv;
    }

    Translators ToolchainElement::translators() const
    {
        Translators tr(&element_->translator_map());
        tr.set_freeze_flag(is_frozen());
        return tr;
    }

    Language ToolchainElement::language() const
    {
        return element_->language();
    }

    ElementType ToolchainElement::element_type() const
    {
        return element_->element_type();
    }

    TargetType ToolchainElement::target_type() const
    {
        return element_->target_type();
    }
        
    void ToolchainElement::set_file_processing_function(const FileProcessingFctr & fctr)
    {
        const Context * context = context_;
        
        auto fct = [=](process::toolchain::Element & e, const LanguageTypePair & ltp, const ingredient::File & file)
        {

            File f(ltp, file, context);
            ToolchainElement el(e.shared_from_this(), context);
            Recipe r(&e.recipe(), context);
            return fctr(el, r, f);
        };
        element_->set_file_processing_function(fct);
    }
    void ToolchainElement::set_key_value_processing_function(const KeyValueProcessingFctr & fctr)
    {
        const Context * context = context_;
        auto fct = [=](process::toolchain::Element & e, const LanguageTypePair & ltp, const ingredient::KeyValue& kv)
        {
            KeyValue k(ltp, kv, context);
            ToolchainElement el(e.shared_from_this(), context);
            Recipe r(&e.recipe(), context);
            return fctr(el, r, k);
        };
        element_->set_key_value_processing_function(fct);
    }

} }
