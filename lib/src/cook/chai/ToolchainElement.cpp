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

    ToolchainElement::ToolchainElement(Element::Ptr element)
    : element_(element)
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

    ElementType ToolchainElement::type() const
    {
        return element_->type();
    }

} }
