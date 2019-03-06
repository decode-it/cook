#ifndef HEADER_cook_process_toolchain_Element_hpp_ALREADY_INCLUDED
#define HEADER_cook_process_toolchain_Element_hpp_ALREADY_INCLUDED

#include "cook/process/toolchain/Types.hpp"
#include "cook/process/command/Compile.hpp"
#include "cook/process/command/Archive.hpp"
#include "cook/process/command/Link.hpp"
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

            Element(Type type, Language language, TargetType target_type);
            Element(const Element &) = default;
            Element(Element &&) = default;
            Element & operator=(const Element &) = default;
            Element & operator=(Element &&) = default;

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

        private:

            KeyValuesMap kvm_;
            TranslatorMap trans_;
            Type element_type_;
            Language language_;
            TargetType target_type_;
    };

    std::ostream & operator<<(std::ostream & str, Element::Type type);

} } } 

#endif
