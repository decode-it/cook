#include "cook/process/toolchain/Element.hpp"

namespace cook { namespace process { namespace toolchain {

        Element::Element(Type element_type, Language language, TargetType target_type)
            : trans_()
            , key(element_type, language, target_type)
            , process_file_([](const auto &, const auto & , const auto & ){return false; })
            , process_key_value_([](const auto &, const auto & , const auto & ){return false; })
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
        return key.language; 
    }

    Element::Type Element::element_type() const 
    { 
        return key.element_type; 
    }

    TargetType Element::target_type() const
    {
        return key.target_type;
    }
            
    bool Element::process_ingredient(const LanguageTypePair & ltp, const ingredient::File & file)
    {
        return process_file_(*this, ltp, file);
    }

    bool Element::process_ingredient(const LanguageTypePair& ltp, const ingredient::KeyValue& key_value)
    {
        return process_key_value_(*this, ltp, key_value);
    }

    std::ostream & operator<<(std::ostream &os, Element::Type type)
    {
        switch(type)
        {
#define L_CASE(VAL) case Element::VAL: return os << #VAL
            L_CASE(Undefined);
            L_CASE(Archive);
            L_CASE(Compile);
            L_CASE(Link);
            L_CASE(UserDefined);
#undef L_CASE
            default:
            return os << "<unknown>";
        }
    }

    std::ostream & operator<<(std::ostream &os, Element::Key key)
    {
        os << "[Element](type:" << key.element_type << ")(language:" << key.language << ")(target_type:" << key.target_type << ")";
        return os;
    }

    bool Element::Key::operator<(const Key & rhs) const
    {
        if (element_type < rhs.element_type)
            return true;
        if (element_type > rhs.element_type)
            return false;

        if (language < rhs.language)
            return true;
        if (language > rhs.language)
            return false;

        return target_type < rhs.target_type;
    }

} } } 

#
