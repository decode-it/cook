#include "cook/process/toolchain/Element.hpp"

namespace cook { namespace process { namespace toolchain { 

    Element::Element(Type type, Language language) : 
        trans_(std::make_shared<TranslatorMap>()), type_(type), language_(language)
    {
        auto lambda = [&](toolchain::Part part)
        {
            (*trans_)[part] = [](const std::string &, const std::string &){return "";};
        };
        toolchain::each_part(lambda);
    }

    KeyValuesMap & Element::key_values_map() 
    { 
        return kvm_; 
    }

    TranslatorMap & Element::translator_map() 
    { 
        return *trans_; 
    }

    const KeyValuesMap & Element::key_values_map() const 
    { 
        return kvm_; 
    }

    const TranslatorMap & Element::translator_map() const 
    { 
        return *trans_; 
    }

    Language Element::language() const 
    { 
        return language_; 
    }

    Element::Type Element::type() const 
    { 
        return type_; 
    }

    std::ostream & operator<<(std::ostream & str, Element::Type type)
    {
        switch(type)
        {
#define L_CASE(VAL) case Element::VAL: return str << #VAL
            L_CASE(Archive);
            L_CASE(Compile);
            L_CASE(Link);
            L_CASE(UserDefined);
#undef L_CASE
            default:
            return str << "<unknown>";
        }
    }

} } } 

#
