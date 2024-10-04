#ifndef HEADER_cook_chai_ToolchainElement_hpp_ALREADY_INCLUDED
#define HEADER_cook_chai_ToolchainElement_hpp_ALREADY_INCLUDED

#include "cook/chai/Freezable.hpp"
#include "cook/chai/File.hpp"
#include "cook/chai/Recipe.hpp"
#include "cook/chai/KeyValue.hpp"
#include "cook/process/toolchain/Element.hpp"

namespace cook { namespace chai {

    using Part = process::toolchain::Part;
    using ElementType = process::toolchain::Element::Type;

    class KeyValues : public Freezable
    {
        using key_values_type = process::toolchain::KeyValuesMap;

    public:
        KeyValues(key_values_type * map);

        void clear(Part part);
        void erase_1(Part part, const std::string & key);
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
        using FileProcessingFctr = std::function<bool (ToolchainElement, Recipe, const File &)>;
        using KeyValueProcessingFctr = std::function<bool (ToolchainElement, Recipe, const KeyValue &)>;
        
        ToolchainElement(Element::Ptr element, const Context * context);

        KeyValues key_values() const;
        Translators translators() const;

        Language language() const;
        ElementType element_type() const;
        TargetType target_type() const;

        void set_file_processing_function(const FileProcessingFctr & fctr);
        void set_key_value_processing_function(const KeyValueProcessingFctr & fctr);

    private:
        Element::Ptr element_;
        const Context * context_;
    };

} }

#endif

