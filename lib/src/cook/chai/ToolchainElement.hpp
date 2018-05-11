#ifndef HEADER_cook_chai_ToolchainElement_hpp_ALREADY_INCLUDED
#define HEADER_cook_chai_ToolchainElement_hpp_ALREADY_INCLUDED

#include "cook/chai/Freezable.hpp"
#include "cook/process/toolchain/Element.hpp"

namespace cook { namespace chai {

    using Part = process::toolchain::Part;

    class KeyValues : public Freezable
    {
        using key_values_type = process::toolchain::KeyValuesMap;

    public:
        KeyValues(key_values_type * map);

        void clear(Part part);
        void append_1(Part part, const std::string & key);
        void append_2(Part part, const std::string & key, const std::string & value);

    private:
        key_values_type * map_;
    };

    class Translators : public Freezable
    {
        using translators_type = process::toolchain::TranslatorMap;
        using translator_type = process::toolchain::Translator;

    public:
        Translators(translators_type * map);

        translator_type & operator[](Part part);

    private:
        translators_type * map_;
    };

    class ToolchainElement : public Freezable
    {
        using Element = process::toolchain::Element;

    public:
        using key_values_type = process::toolchain::KeyValuesMap;
        using ElementType = Element::Type;
        
        ToolchainElement(Element::Ptr element);

        KeyValues key_values() const;
        Translators translators() const;

        Language language() const;
        ElementType type() const;

    private:
        Element::Ptr element_;
    };

} }

#endif

