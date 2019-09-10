#ifndef HEADER_cook_process_toolchain_Element_hpp_ALREADY_INCLUDED
#define HEADER_cook_process_toolchain_Element_hpp_ALREADY_INCLUDED

#include "cook/process/toolchain/Types.hpp"
#include "cook/model/Recipe.hpp"
#include "cook/Language.hpp"
#include "cook/TargetType.hpp"
#include "gubg/mss.hpp"

namespace cook { namespace process { namespace toolchain { 

    class Element 
    {
        public:
            enum Type
            {
                Undefined,
                Archive,
                Compile,
                Link,
                UserDefined,
            };

            using Ptr = std::shared_ptr<Element>;
            using FileProcessingFctr = std::function<bool(const LanguageTypePair&, const ingredient::File&)>;
            using KeyValueProcessingFctr = std::function<bool(const LanguageTypePair&, const ingredient::KeyValue&)>;

            Element(Type type, Language language, TargetType target_type);
            Element(const Element &) = default;
            Element(Element &&) = default;
            Element & operator=(const Element &) = default;
            Element & operator=(Element &&) = default;

            void set_recipe(model::Recipe * recipe) { recipe_ = recipe; }

            template <typename T>
            std::shared_ptr<T> create() const
            {
                return std::make_shared<T>(key_values_map(), trans_, language_);
            }

            KeyValuesMap & key_values_map();
            TranslatorMap & translator_map();
            const KeyValuesMap & key_values_map() const;
            const TranslatorMap & translator_map() const;
            Language language() const;
            Type element_type() const;
            TargetType target_type() const;
            const model::Recipe & recipe() const { return *recipe_; }
            
            void set_file_processing_function(const FileProcessingFctr & fctr) { process_file_ = fctr; }
            void set_key_value_processing_function(const KeyValueProcessingFctr & fctr) { process_key_value_ = fctr; }
            bool process_ingredient(const LanguageTypePair& ltp, const ingredient::File& file);
            bool process_ingredient(const LanguageTypePair& ltp, const ingredient::KeyValue& key_value);

        private:
            model::Recipe * recipe_;

        protected:
            KeyValuesMap kvm_;
            TranslatorMap trans_;
            Type element_type_;
            Language language_;
            TargetType target_type_;
            FileProcessingFctr process_file_;
            KeyValueProcessingFctr process_key_value_;
    };

    std::ostream & operator<<(std::ostream & str, Element::Type type);

} } } 

#endif
