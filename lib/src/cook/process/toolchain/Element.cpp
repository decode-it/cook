#include "cook/process/toolchain/Element.hpp"

namespace cook { namespace process { namespace toolchain {

        Element::Element(Type element_type, Language language, TargetType target_type)
            : trans_()
            , element_type_(element_type)
            , language_(language)
            , target_type_(target_type)
            , process_file_([](const auto & , const auto & ){return false; })
            , process_key_value_([](const auto & , const auto & ){return false; })
        {
            auto lambda = [&](toolchain::Part part) {
                trans_[part] = [](const std::string&, const std::string&) { return ""; };
            };
            toolchain::each_part(lambda);
    }

    KeyValuesMap & Element::key_values_map() 
    { 
        return kvm_; 
    }

    TranslatorMap & Element::translator_map() 
    { 
        return trans_; 
    }

    const KeyValuesMap & Element::key_values_map() const 
    { 
        return kvm_; 
    }

    const TranslatorMap & Element::translator_map() const 
    { 
        return trans_; 
    }

    Language Element::language() const 
    { 
        return language_; 
    }

    Element::Type Element::element_type() const 
    { 
        return element_type_; 
    }

    TargetType Element::target_type() const
    {
        return target_type_;
    }
            
    bool Element::process_ingredient(const LanguageTypePair & ltp, const ingredient::File & file)
    {
        return process_file_(ltp, file);
    }

    bool Element::process_ingredient(const LanguageTypePair& ltp, const ingredient::KeyValue& key_value)
    {
        return process_key_value_(ltp, key_value);
    }

    std::ostream & operator<<(std::ostream & str, Element::Type type)
    {
        switch(type)
        {
#define L_CASE(VAL) case Element::VAL: return str << #VAL
            L_CASE(Undefined);
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
