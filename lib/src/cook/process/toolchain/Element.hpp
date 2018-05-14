#ifndef HEADER_cook_process_toolchain_Element_hpp_ALREADY_INCLUDED
#define HEADER_cook_process_toolchain_Element_hpp_ALREADY_INCLUDED

#include "cook/process/toolchain/Types.hpp"
#include "cook/process/command/Compile.hpp"
#include "cook/process/command/Archive.hpp"
#include "cook/process/command/Link.hpp"
#include "cook/Language.hpp"
#include "gubg/mss.hpp"

namespace cook { namespace process { namespace toolchain { 

    class Element 
    {
        public:
            enum Type
            {
                Archive,
                Compile,
                LinkExe,
                LinkSharedLibrary,
                UserDefined,
            };

            using Ptr = std::shared_ptr<Element>;

            Element(Type type, Language language);

            template <typename T>
            std::shared_ptr<T> create() const
            {
                return std::make_shared<T>(key_values_map(), trans_);
            }

            KeyValuesMap & key_values_map();
            TranslatorMap & translator_map();
            const KeyValuesMap & key_values_map() const;
            const TranslatorMap & translator_map() const;
            Language language() const;
            Type type() const;

        private:
            Element(const Element &) = delete;
            Element(Element &&) = delete;
            Element & operator=(const Element &) = delete;
            Element & operator=(Element &&) = delete;

            KeyValuesMap kvm_;
            TranslatorMapPtr trans_;
            Type type_;
            Language language_;
    };

    std::ostream & operator<<(std::ostream & str, Element::Type type);

} } } 

#endif
