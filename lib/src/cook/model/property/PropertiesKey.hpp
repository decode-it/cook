#ifndef HEADER_cook_model_property_PropertiesKey_hpp_ALREADY_INCLUDED
#define HEADER_cook_model_property_PropertiesKey_hpp_ALREADY_INCLUDED

#include "cook/Language.hpp"
#include "cook/Type.hpp"

namespace cook { namespace model { namespace property {

struct PropertiesKey
{
    PropertiesKey()
        : language(Language::Undefined), type(Type::Undefined)
    {
    }

    PropertiesKey(Language language, Type type)
        : language(language), type(type)
    {
    }


    Language language;
    Type type;
};

bool operator<(const PropertiesKey & lhs, const PropertiesKey & rhs)
{
    return lhs.language < rhs.language || (lhs.language == rhs.language && lhs.type < rhs.type);
}

} } }

#endif
